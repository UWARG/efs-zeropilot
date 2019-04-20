/**
 * Implements a synchronous SPI driver. Calls the underlying stm32HAL, and should be used in place of it
 * Note this driver is for 6-wire, full duplex SPI.
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */

#pragma once

#include <stdint.h>
#include "Status.hpp"
#include "GPIO.hpp"

const int MAX_SPI_SLAVES = 5;

typedef enum SPIPortNum {
	SPI_PORT1 = 0,
	SPI_PORT2,
	SPI_PORT3,
	SPI_PORT4,
	SPI_PORT5,
	SPI_PORT6
} SPIPortNum;

typedef enum SPIMode {
	SPI_MODE_0 = 0,  // CPOL: 0 CPHA: 0
	SPI_MODE_1,      // CPOL: 0 CPHA: 1
	SPI_MODE_2,      // CPOL: 1 CPHA: 0
	SPI_MODE_3      // CPOL: 1 CPHA: 1
} SPIMode;

typedef struct SPISettings {
	SPIPortNum port;
	SPIMode mode;
	bool master; //wether we're a master or a slave
	uint8_t frequency; //desired master clock frequency in Mhz. Will be rounded down to nearest possible one
	uint8_t word_size = 2; //valid values are 1 (8-bit) or 2 (16-bit)
	bool first_bit_msb = true;
} SPISettings;

class SPIPort {
 public:

	/**
	 * Initializes the object, but not the port
	 * @param settings
	 */
	SPIPort(SPISettings settings);

	/**
	 * Initializes the port, including any necessary GPIOs
	 * @return
	 */
	StatusCode setup();

	/**
	 * Only valid in master mode
	 * Add a slave pin to the port. Then you can use the slave functions to control which slave to communicate with
	 * @param slave_pin The pin. Note setup() must have already been called on it! Will copy the class over,
	 * destroy your reference to it afterwards
	 * @return The number to reference the slave from. Increases linearly with more slaves. -1 if can't add more
	 */
	int32_t add_slave(GPIOPin slave_pin);

	/**
	 * Only valid in master mode
	 * Sets the specified slave pin on, turns the others to off
	 * @param slave_num The slave you want to talk to
	 * @return
	 */
	StatusCode set_slave(uint32_t slave_num);

	/**
	 * Send/receives some bytes over. Requires both a tx and rx buffer since SPI is duplex.
	 * @param tx_data
	 * @param rx_data
	 * @param length
	 * @return STATUS_CODE_INVALID_ARGS if both tx and rx buffers are null
	 */
	StatusCode exchange_data(uint8_t *tx_data, uint8_t *rx_data, size_t length);

	/**
	 * Resets the port to its default state, resetting all the GPIOs
	 * @return
	 */
	StatusCode reset();

 private:
	bool has_setup = false;
	SPISettings settings;
	GPIOPin nss;
	GPIOPin mosi;
	GPIOPin miso;
	GPIOPin sck;
	uint32_t num_slaves = 0;
	uint32_t curr_slave = 0;
	GPIOPin slaves[MAX_SPI_SLAVES]; //support up to 5 slaves
};