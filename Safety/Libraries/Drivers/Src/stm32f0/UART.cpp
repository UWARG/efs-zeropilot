#include "UART.hpp"
#include "GPIO.hpp"
#include "stm32f0xx_hal.h"
#include "Status.hpp"

static uint32_t UART_TIMEOUT = 50;

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
		default: return;
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

	StatusCode status;

	switch (port) {
		case UART_PORT1: status = get_status_code(HAL_UART_Receive(&huart1, data, (uint16_t) len, UART_TIMEOUT));
			break;
		case UART_PORT2: status = get_status_code(HAL_UART_Receive(&huart2, data, (uint16_t) len, UART_TIMEOUT));
			break;
		default: return STATUS_CODE_INVALID_ARGS;

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