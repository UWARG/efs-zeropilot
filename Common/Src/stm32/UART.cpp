#include "UART.hpp"
#include "Debug.hpp"

#if STM32F030xC
#include "stm32f0xx_hal.h"
#elif STM32F7xx
#include "stm32f7xx_hal.h"
#endif

extern StatusCode get_status_code(HAL_StatusTypeDef status);

StatusCode UARTPort::setup() {
	if (!is_valid_port()) return STATUS_CODE_INVALID_ARGS;

	StatusCode status;
	auto uart = (UART_HandleTypeDef *) this->interface_handle;

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

		status = rts_pin.setup();
		if (status != STATUS_CODE_OK) return status;

		status = cts_pin.setup();
		if (status != STATUS_CODE_OK) return status;
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
		if (dma_config->reset) {
			reallocate_dma_buffer = false;
			reset();
			setup();
			setupDMA(0, dma_config->dma_buffer_len);
			reallocate_dma_buffer = true;
			dma_config->reset = false;
			return STATUS_CODE_INTERNAL_ERROR; //let user know we're reconfiguring
		}

		if (rx_queue == nullptr) abort("RX QUEUE IS NULL!", __FILE__, __LINE__);

		while (bytes_read < len && !rx_queue->empty()) {
			data[bytes_read] = rx_queue->front();
			rx_queue->pop_front();
			bytes_read++;
		}
	} else {
		auto uart = (UART_HandleTypeDef *) this->interface_handle;
		status = get_status_code(HAL_UART_Receive(uart, data, (uint16_t) len, settings.timeout));

		status == STATUS_CODE_OK ? bytes_read = len : bytes_read = 0;
	}

	if (status == STATUS_CODE_OK && bytes_read == 0) return STATUS_CODE_EMPTY;
	return status;
}

StatusCode UARTPort::transmit(uint8_t *data, size_t len) {
	if (!is_setup) return STATUS_CODE_UNINITIALIZED;

	StatusCode status;
	auto uart = (UART_HandleTypeDef *) this->interface_handle;

	status = get_status_code(HAL_UART_Transmit(uart, data, (uint16_t) len, settings.timeout));

	return status;
}



