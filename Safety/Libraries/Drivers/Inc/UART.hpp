/**
 * Implements UART driver for the available ports on the chip
 * By default configures for both tx and rx, with an 8-bit data size
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */

#pragma once

#include "GPIO.hpp"
#include <stdint.h>
#include <stdlib.h>

//only ports 1 and 2 supported on safety chip
//only 1, 2, 3, 4 supported on autopilot
typedef enum UARTPortNum {
	UART_PORT1 = 0,
	UART_PORT2,
	UART_PORT3,
	UART_PORT4,
	UART_PORT5,
	UART_PORT6
} UARTPortNum;

typedef enum UARTParity {
	UART_NO_PARITY,
	UART_EVEN_PARITY,
	UART_ODD_PARITY
} UARTParity;

typedef struct UARTSettings {
	uint32_t baudrate = 9600;
	uint8_t stop_bits = 1;
	UARTParity parity;
	bool cts_rts = false; //weather to enable cts/rts (ie. hardware flow control)
} UARTSettings;

class UARTPort {
 public:
	UARTPort(UARTPortNum port, UARTSettings settings);

	/**
	 * Re-configures the baudrate to be the specified value
	 * @param baudrate
	 * @return
	 */
	StatusCode adjustBaudrate(uint32_t baudrate);

	/**
	 * Configures the GPIO ports, timers, etc
	 * @return
	 */
	StatusCode setup();

	/**
	 * Sets all pins to their default state
	 * @return
	 */
	StatusCode reset();

	/**
	 * Get a byte received on the buffer
	 * @param data The byte read will be written to here
	 * @return STATUS_CODE_EMPTY if nothing in rx buffer.
	 */
	StatusCode read_byte(uint8_t &data);

	/**
	 * Retrieve a request number of bytes
	 * @param data
	 * @param len
	 * @param bytes_read Returns the actual number of bytes read from the buffer
	 * @return STATUS_CODE_EMPTY if nothing in buffer.
	 */
	StatusCode read_bytes(uint8_t *data, size_t len, size_t &bytes_read);

	/**
	 * Transmit a set of data
	 * @param data
	 * @param len
	 * @return If not enough space in buffer, will return a STATUS_CODE_RESOURCE_EXAUSTED and not transmit
	 */
	StatusCode transmit(uint8_t *data, size_t len);

 private:
	GPIOPin rx_pin;
	GPIOPin tx_pin;
	UARTPortNum port;
	UARTSettings settings;
	bool is_setup = false;
};
