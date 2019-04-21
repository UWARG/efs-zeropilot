#include "UART.hpp"
#include "GPIO.hpp"
#include "Status.hpp"
#include <stdlib.h>
#include <queue>
#include "stm32f0xx_hal.h"

static uint32_t UART_TIMEOUT = 50;
static size_t UART_RECEIVE_BUFFER_SIZE = 512;

static const GPIOPort UART1_RX_PORT = GPIO_PORT_B;
static const GPIOPort UART1_TX_PORT = GPIO_PORT_B;
static const GPIOPinNum UART1_RX_PIN = 7;
static const GPIOPinNum UART1_TX_PIN = 6;

static const GPIOPort UART2_RX_PORT = GPIO_PORT_A;
static const GPIOPort UART2_TX_PORT = GPIO_PORT_A;
static const GPIOPinNum UART2_RX_PIN = 2;
static const GPIOPinNum UART2_TX_PIN = 3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

static uint8_t* uart2_rx_dma_buffer;
static size_t uart2_rx_dma_buffer_len;
std::deque<uint8_t> uart2_rx_queue;

extern StatusCode get_status_code(HAL_StatusTypeDef status);

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
		default:
			rx_pin = GPIOPin();
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

	if (settings.cts_rts) {
		uart->Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
	} else {
		uart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	}

	uart->Init.OverSampling = UART_OVERSAMPLING_16;
	uart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	uart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_SWAP_INIT;
	uart->AdvancedInit.Swap = UART_ADVFEATURE_SWAP_ENABLE;

	status = get_status_code(HAL_UART_Init(uart));
	is_setup = true;
	return status;
}

StatusCode UARTPort::reset() {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

	switch (port) {
		case UART_PORT1: __HAL_RCC_USART1_CLK_DISABLE();
			break;
		case UART_PORT2: __HAL_RCC_USART2_CLK_DISABLE();
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
	if (rx_buffer_size == 0){
		return STATUS_CODE_INVALID_ARGS;
	}

	if (port == UART_PORT2){
		hdma_usart2_rx.Instance = DMA1_Channel1;
		hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
		hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;

		StatusCode status = get_status_code(HAL_DMA_Init(&hdma_usart2_rx));
		if (status != STATUS_CODE_OK) return status;

		__HAL_DMA1_REMAP(HAL_DMA1_CH1_USART2_RX);

		__HAL_LINKDMA(&huart2,hdmarx,hdma_usart2_rx);

		HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

		uart2_rx_dma_buffer = (uint8_t*)malloc(rx_buffer_size*sizeof(uint8_t));

		uart2_rx_queue.resize(UART_RECEIVE_BUFFER_SIZE);
		rx_queue = &uart2_rx_queue;

		if (uart2_rx_dma_buffer == nullptr){
			return STATUS_CODE_RESOURCE_EXHAUSTED;
		}
		uart2_rx_dma_buffer_len = rx_buffer_size;

		//init circular dma transfer
		HAL_UART_Receive_DMA(&huart2, uart2_rx_dma_buffer, (uint16_t)uart2_rx_dma_buffer_len);

		dma_setup_rx = true;

		return STATUS_CODE_OK;
	}

	return STATUS_CODE_UNIMPLEMENTED; //only port 2 supported for stm32f0
}

StatusCode UARTPort::resetDMA() {
	if (port != UART_PORT2) return STATUS_CODE_UNIMPLEMENTED;
	if (!dma_setup_rx) return STATUS_CODE_INVALID_ARGS;

	StatusCode status;
	status = get_status_code(HAL_DMA_DeInit(&hdma_usart2_rx));
	if (status != STATUS_CODE_OK) return status;

	free(uart2_rx_dma_buffer);

	HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);

	dma_setup_rx = false;

	return STATUS_CODE_OK;
}

StatusCode UARTPort::adjustBaudrate(uint32_t baudrate) {
	reset();
	settings.baudrate = baudrate;
	return setup();
}

StatusCode UARTPort::read_byte(uint8_t &data) {
	size_t read;

	return read_bytes(&data, 1, read);
}

//synchronous implementation for now. Use DMA later
StatusCode UARTPort::read_bytes(uint8_t *data, size_t len, size_t &bytes_read) {
	if (is_setup) return STATUS_CODE_UNINITIALIZED;

	StatusCode status = STATUS_CODE_OK;

	if (dma_setup_rx){
		bytes_read = 0;

		if (rx_queue == nullptr) abort("RX QUEUE IS NULL!", __FILE__, __LINE__);

		while (bytes_read < len && !rx_queue->empty()){
			data[bytes_read] = rx_queue->front();
			rx_queue->pop_front();
			bytes_read++;
		}

	} else {
		switch (port) {
			case UART_PORT1: status = get_status_code(HAL_UART_Receive(&huart1, data, (uint16_t) len, UART_TIMEOUT));
				break;
			case UART_PORT2: status = get_status_code(HAL_UART_Receive(&huart2, data, (uint16_t) len, UART_TIMEOUT));
				break;
			default: return STATUS_CODE_INVALID_ARGS;

		}
	}

	bytes_read = len;

	return status;
}

StatusCode UARTPort::transmit(uint8_t *data, size_t len) {
	if (is_setup) return STATUS_CODE_UNINITIALIZED;

	StatusCode status;

	switch (port) {
		case UART_PORT1: status = get_status_code(HAL_UART_Transmit(&huart1, data, (uint16_t) len, UART_TIMEOUT));
			break;
		case UART_PORT2: status = get_status_code(HAL_UART_Transmit(&huart2, data, (uint16_t) len, UART_TIMEOUT));
			break;
		default: return STATUS_CODE_INVALID_ARGS;

	}

	return status;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if (huart->Instance == USART2) {
		for (size_t i = uart2_rx_dma_buffer_len/2; i < uart2_rx_dma_buffer_len; i++){
			uart2_rx_queue.push_back(uart2_rx_dma_buffer[i]);
		}
	}
}
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart){
	if (huart->Instance == USART2) {
		for (size_t i = 0; i < uart2_rx_dma_buffer_len/2; i++){
			uart2_rx_queue.push_back(uart2_rx_dma_buffer[i]);
		}
	}
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	//todo: handle diff error types here
}
void HAL_UART_AbortCpltCallback (UART_HandleTypeDef *huart){

}

void DMA1_Channel1_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

	/* USER CODE END DMA1_Channel1_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_usart2_rx);
	/* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

	/* USER CODE END DMA1_Channel1_IRQn 1 */
}