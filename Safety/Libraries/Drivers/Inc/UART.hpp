/**
 * Implements UART driver for the available ports on the chip
 * By default configures for both tx and rx, with an 8-bit data size
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 *
 *  Notes on DMA:
 *  - You should only setup DMA on the interface if you're expecting to receive constant size packets. Variable size
 *  packets may work, however then you should pick a common multiple of the different sizes of packets. This is because
 *  a DMA interrupt only triggers when the buffer is half full or full, so if the device has sent data that doesn't fill
 *  the buffer, you'll have to wait until enough bytes are received before being able to read it!
 *
 *  Note that this isn't an issue when sending TX through DMA (just make sure that the dma send buffer is large enough to
 *  fill your largest packet
 */

#pragma once

#include "GPIO.hpp"
#include <stdint.h>
#include <stdlib.h>
#include <queue>

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
	bool rx_inverted = false; //useful for inverted UART protocols like S.BUS
	bool tx_inverted = false;
	uint32_t timeout = 50; //when we're in blocking mode (no DMA), how long to wait before we abort a read/transmit in ms
	bool flip_tx_rx = false; //wether to flip the tx/rx pins. Useful in case of wiring(or routing) issues
} UARTSettings;

class UARTPort {
 public:
	UARTPort() = default;
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
	 * Sets up DMA transfers transparently, if the hardware can support it. If possible, attempts to do both tx and RX
	 * using DMA. If not, may only do RX. If either the parameters are 0, will only enable either the RX/TX buffers respectively
	 * @param tx_buffer_size Size of DMA buffer when transmitting (used if possible). Try to make this as large as the largest
	 * 	packet you could possible send
	 * @param rx_buffer_size Size of DMA buffer when receiving. Make this the size of the smallest packet you're expected to receive
	 * @return
	 */
	StatusCode setupDMA(size_t tx_buffer_size, size_t rx_buffer_size);

	/**
	 * If DMA was setup for this channel, resets it so that synchronous transfers are done instead
	 */
	StatusCode resetDMA();

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
	bool dma_setup_rx = false;
	bool dma_setup_tx = false;
	std::deque<uint8_t>* rx_queue;
};
