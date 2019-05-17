/**
 * DMA implementation uses UART Idle line detection to support variable size reads. More info can be found
 * on: https://github.com/akospasztor/stm32-dma-uart
 * Which is what this code is based off
 */

#include "../../../../Common/Inc/UART.hpp"
#include "../../../../Common/Inc/GPIO.hpp"
#include "../../../../Common/Inc/Status.hpp"
#include "Debug.hpp"
#include "stm32f0xx_hal.h"
#include <stdlib.h>
#include <queue>
#include "DMA.hpp"

static const GPIOPort UART1_RX_PORT = GPIO_PORT_B;
static const GPIOPort UART1_TX_PORT = GPIO_PORT_B;
static const GPIOPinNum UART1_RX_PIN = 7;
static const GPIOPinNum UART1_TX_PIN = 6;

static const GPIOPort UART2_RX_PORT = GPIO_PORT_A;
static const GPIOPort UART2_TX_PORT = GPIO_PORT_A;
static const GPIOPinNum UART2_RX_PIN = 3;
static const GPIOPinNum UART2_TX_PIN = 2;

static UART_HandleTypeDef huart1;

//stm32f0xx_it.c needs this, so don't make static
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
static std::deque<uint8_t> uart2_rx_queue;
DMAConfig uart2_dma_config;

extern StatusCode get_status_code(HAL_StatusTypeDef status);
void USART2_DMA_ErrorCallback(DMA_HandleTypeDef *dma);

UARTPort::UARTPort(UARTPortNum port, UARTSettings settings) {
	this->port = port;
	this->settings = settings;

	switch (port) {
		case UART_PORT1:
			rx_pin = GPIOPin(UART1_RX_PORT,
							 UART1_RX_PIN,
							 GPIO_ALT_PP,
							 GPIO_STATE_LOW,
							 GPIO_RES_PULLUP,
							 GPIO_SPEED_HIGH,
							 GPIO_AF0_USART1);
			tx_pin = GPIOPin(UART1_TX_PORT,
							 UART1_TX_PIN,
							 GPIO_ALT_PP,
							 GPIO_STATE_LOW,
							 GPIO_RES_PULLUP,
							 GPIO_SPEED_HIGH,
							 GPIO_AF0_USART1);
			break;
		case UART_PORT2:
			rx_pin = GPIOPin(UART2_RX_PORT,
							 UART2_RX_PIN,
							 GPIO_ALT_PP,
							 GPIO_STATE_LOW,
							 GPIO_RES_PULLUP,
							 GPIO_SPEED_HIGH,
							 GPIO_AF1_USART2);
			tx_pin = GPIOPin(UART2_TX_PORT,
							 UART2_TX_PIN,
							 GPIO_ALT_PP,
							 GPIO_STATE_LOW,
							 GPIO_RES_PULLUP,
							 GPIO_SPEED_HIGH,
							 GPIO_AF1_USART2);
			break;
		default: rx_pin = GPIOPin();
			tx_pin = GPIOPin();
			break;
	}
}

StatusCode UARTPort::setup() {
	StatusCode status;
	UART_HandleTypeDef *uart;

	if (port == UART_PORT1) {
		uart = &huart1;
		uart->Instance = USART1;
		__HAL_RCC_USART1_CLK_ENABLE();
	} else if (port == UART_PORT2) {
		uart = &huart2;
		uart->Instance = USART2;
		__HAL_RCC_USART2_CLK_ENABLE();
	} else {
		return STATUS_CODE_INVALID_ARGS;
	}

	status = rx_pin.setup();
	if (status != STATUS_CODE_OK) return status;

	status = tx_pin.setup();
	if (status != STATUS_CODE_OK) return status;

	uart->Init.BaudRate = settings.baudrate;
	uart->Init.WordLength = UART_WORDLENGTH_8B;

	if (settings.stop_bits == 2) {
		uart->Init.StopBits = UART_STOPBITS_2;
	} else {
		uart->Init.StopBits = UART_STOPBITS_1;
	}

	if (settings.parity == UART_EVEN_PARITY) {
		uart->Init.Parity = UART_PARITY_EVEN;
	} else if (settings.parity == UART_ODD_PARITY) {
		uart->Init.Parity = UART_PARITY_ODD;
	} else {
		uart->Init.Parity = UART_PARITY_NONE;
	}

	uart->Init.Mode = UART_MODE_TX_RX;

	if (settings.rx_inverted) {
		uart->AdvancedInit.AdvFeatureInit |= UART_ADVFEATURE_RXINV_ENABLE;
		uart->AdvancedInit.RxPinLevelInvert = UART_ADVFEATURE_RXINV_ENABLE;
	}

	if (settings.tx_inverted) {
		uart->AdvancedInit.AdvFeatureInit |= UART_ADVFEATURE_TXINV_ENABLE;
		uart->AdvancedInit.TxPinLevelInvert = UART_ADVFEATURE_TXINV_ENABLE;
	}

	if (settings.cts_rts) {
		uart->Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
	} else {
		uart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	}

	uart->Init.OverSampling = UART_OVERSAMPLING_16;
	uart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;

	if (settings.flip_tx_rx) {
		uart->AdvancedInit.AdvFeatureInit |= UART_ADVFEATURE_SWAP_INIT;
		uart->AdvancedInit.Swap = UART_ADVFEATURE_SWAP_ENABLE;
	}

	status = get_status_code(HAL_UART_Init(uart));
	is_setup = true;
	return status;
}

StatusCode UARTPort::reset() {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

	if (dma_setup_rx || dma_setup_tx) {
		resetDMA();
	}

	switch (port) {
		case UART_PORT1: __HAL_RCC_USART1_CLK_DISABLE();
			HAL_UART_DeInit(&huart1);
			break;
		case UART_PORT2: __HAL_RCC_USART2_CLK_DISABLE();
			HAL_UART_DeInit(&huart2);
			break;
		default: return STATUS_CODE_INVALID_ARGS;
	}

	StatusCode status = rx_pin.reset();
	if (status != STATUS_CODE_OK) return status;

	status = tx_pin.reset();
	if (status != STATUS_CODE_OK) return status;

	is_setup = false;
	return STATUS_CODE_OK;
}

StatusCode UARTPort::setupDMA(size_t tx_buffer_size, size_t rx_buffer_size) {
	if (!is_setup) return STATUS_CODE_UNINITIALIZED;
	if (dma_setup_rx) return STATUS_CODE_INVALID_ARGS; //should call this without calling reset() first
	if (rx_buffer_size == 0) {
		return STATUS_CODE_INVALID_ARGS;
	}

	if (port == UART_PORT2) {

		__HAL_RCC_DMA1_CLK_ENABLE();
		hdma_usart2_rx.Instance = DMA1_Channel5;
		hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
		hdma_usart2_rx.Init.Priority = DMA_PRIORITY_HIGH;

		StatusCode status = get_status_code(HAL_DMA_Init(&hdma_usart2_rx));
		if (status != STATUS_CODE_OK) return status;

		__HAL_DMA1_REMAP(HAL_DMA1_CH5_USART2_RX);
		__HAL_LINKDMA(&huart2, hdmarx, hdma_usart2_rx);

		resetDMAConfig(&uart2_dma_config, rx_buffer_size);
		uart2_dma_config.dma_handle = (void *) &hdma_usart2_rx;
		uart2_dma_config.queue = (void *) &uart2_rx_queue;
		uart2_dma_config.timeout = 50;

		//don't allocate memory if we hadn't free'd it
		if (uart2_dma_config.dma_buffer == nullptr) {
			uart2_dma_config.dma_buffer = (uint8_t *) malloc(uart2_dma_config.dma_buffer_len * sizeof(uint8_t));
		}

		if (uart2_dma_config.dma_buffer == nullptr) {
			return STATUS_CODE_RESOURCE_EXHAUSTED;
		}

		rx_queue = &uart2_rx_queue;

		//init circular dma transfer
		status =
			get_status_code(HAL_UART_Receive_DMA(&huart2,
												 uart2_dma_config.dma_buffer,
												 (uint16_t) uart2_dma_config.dma_buffer_len));
		if (status != STATUS_CODE_OK) return status;

		//disable the DMA half interrupt since we're using idle line detection instead
		__HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
		HAL_DMA_RegisterCallback(&hdma_usart2_rx, HAL_DMA_XFER_ERROR_CB_ID, USART2_DMA_ErrorCallback);

		//enable uart idle line interrupt
		SET_BIT(huart2.Instance->CR1, USART_CR1_IDLEIE);

		HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
		HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
		HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);

		dma_setup_rx = true;

		return STATUS_CODE_OK;
	}

	return STATUS_CODE_UNIMPLEMENTED; //only port 2 supported for stm32f0
}

StatusCode UARTPort::resetDMA() {
	if (port != UART_PORT2) return STATUS_CODE_UNIMPLEMENTED;
	if (!dma_setup_rx) return STATUS_CODE_INVALID_ARGS;

	//disable dma interrupts
	CLEAR_BIT(huart2.Instance->CR1, USART_CR1_IDLEIE);
	HAL_NVIC_DisableIRQ(USART2_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Channel4_5_IRQn);

	StatusCode status;
	status = get_status_code(HAL_DMA_DeInit(&hdma_usart2_rx));
	if (status != STATUS_CODE_OK) return status;

	//avoid uneccessary re-allocations in case of dma error
	if (reallocate_dma_buffer) {
		if (uart2_dma_config.dma_buffer == nullptr) abort("i2c dma buffer null!", __FILE__, __LINE__);
		free(uart2_dma_config.dma_buffer);
		uart2_dma_config.dma_buffer = nullptr;
		uart2_rx_queue.erase(uart2_rx_queue.begin(), uart2_rx_queue.end());
	}

	dma_setup_rx = false;

	return STATUS_CODE_OK;
}

StatusCode UARTPort::adjustBaudrate(uint32_t baudrate) {
	bool was_setup = is_setup;
	if (was_setup) {
		reset();
	}
	settings.baudrate = baudrate;

	if (was_setup) {
		return setup();
	}
	return STATUS_CODE_OK;
}

StatusCode UARTPort::read_byte(uint8_t &data) {
	size_t read;

	return read_bytes(&data, 1, read);
}

StatusCode UARTPort::read_bytes(uint8_t *data, size_t len, size_t &bytes_read) {
	if (!is_setup) return STATUS_CODE_UNINITIALIZED;

	StatusCode status = STATUS_CODE_OK;

	if (dma_setup_rx) {
		bytes_read = 0;

		//by default if something as simple as a frame error occurs, the HAL aborts all transfers
		//in this case, re-setup the DMA connection
		if (uart2_dma_config.reset) {
			reallocate_dma_buffer = false;
			reset();
			setup();
			setupDMA(0, uart2_dma_config.dma_buffer_len);
			reallocate_dma_buffer = true;
			uart2_dma_config.reset = false;
			return STATUS_CODE_INTERNAL_ERROR; //let user know we're reconfiguring
		}

		if (rx_queue == nullptr) abort("RX QUEUE IS NULL!", __FILE__, __LINE__);

		while (bytes_read < len && !rx_queue->empty()) {
			data[bytes_read] = rx_queue->front();
			rx_queue->pop_front();
			bytes_read++;
		}
	} else {
		switch (port) {
			case UART_PORT1:
				status = get_status_code(HAL_UART_Receive(&huart1, data, (uint16_t) len, settings.timeout));
				break;
			case UART_PORT2:
				status = get_status_code(HAL_UART_Receive(&huart2, data, (uint16_t) len, settings.timeout));
				break;
			default: return STATUS_CODE_INVALID_ARGS;

		}

		status == STATUS_CODE_OK ? bytes_read = len : bytes_read = 0;
	}

	if (status == STATUS_CODE_OK && bytes_read == 0) return STATUS_CODE_EMPTY;
	return status;
}

StatusCode UARTPort::transmit(uint8_t *data, size_t len) {
	if (!is_setup) return STATUS_CODE_UNINITIALIZED;

	StatusCode status;

	switch (port) {
		case UART_PORT1: status = get_status_code(HAL_UART_Transmit(&huart1, data, (uint16_t) len, settings.timeout));
			break;
		case UART_PORT2: status = get_status_code(HAL_UART_Transmit(&huart2, data, (uint16_t) len, settings.timeout));
			break;
		default: return STATUS_CODE_INVALID_ARGS;

	}

	return status;
}

//Below code based off: https://github.com/akospasztor/stm32-dma-uart/blob/master/Src/main.c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		processDMARXCompleteEvent(&uart2_dma_config);
	}
}
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
	error("half called");
	UNUSED(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		error("USART1 Error received", huart->ErrorCode);
	} else if (huart->Instance == USART2) {
		error("USART2 Error received", huart->ErrorCode);
		uart2_dma_config.reset = true;
	} else {
		error("Unknown USART port got an error callback");
	}
}

void USART2_DMA_ErrorCallback(DMA_HandleTypeDef *dma) {
	error("USART 2 DMA Error received!", dma->ErrorCode);
}

void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		error("USART1 Abort received");
	}
	if (huart->Instance == USART2) {
		error("USART2 Abort received");
	}
}

