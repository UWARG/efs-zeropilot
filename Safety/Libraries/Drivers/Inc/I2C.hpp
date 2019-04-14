/**
 * Implements a synchronous, polling I2C driver. Calls the underlying stm32HAL, and should be used in place of it
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "Status.hpp"
#include "GPIO.hpp"

typedef uint8_t I2CAddress;

typedef enum {
  I2C_SPEED_STANDARD = 0,  // 100kHz
  I2C_SPEED_FAST          // 400 kHz
} I2CSpeed;

typedef enum {
  I2C_PORT1 = 0, //only port 1 is valid for stm32f0
  I2C_PORT2 = 1,
  I2C_PORT3 = 2,
  I2C_PORT4 = 3
} I2CPortNum;

class I2CPort {
 public:
	/**
	 * Creates the object, but does not initialize the connection
	 * @param port_num
	 * @param speed
	 * @param address The address of this device (use if in slave mode). Use 7-bit address
	 */
	I2CPort(I2CPortNum port_num, I2CSpeed speed, I2CAddress address = 0x08);

	/**
	 * Initializes the connection. Sets up the sda and scl pins to be pulled-up.
	 * @return STATUS_CODE_INVALID_ARGS if invalid port number was provided
	 * @return STATUS_CODE_INTERNAL_ERROR if something else went wrong
	 */
	StatusCode setup();

	/**
	 * De-initializes the I2C connection. Sets the sda and scl pins to their default state
	 * @return
	 */
	StatusCode reset();

	/**
	 * Read bytes as a master (need address of slave)
	 * @param addr
	 * @param rx_data Buffer to write data to
	 * @param rx_len
	 */
	StatusCode read_bytes(I2CAddress addr, uint8_t *rx_data, size_t rx_len);

	/**
	 * Read bytes as a slave
	 * @param rx_data Buffer to write data to
	 * @param rx_len
	 */
	StatusCode read_bytes(uint8_t *rx_data, size_t rx_len);

	/**
	 * Write bytes as a master (need address of slave)
	 * @param addr
	 * @param tx_data
	 * @param tx_len
	 */
	StatusCode write_bytes(I2CAddress addr, uint8_t *tx_data, size_t tx_len);

	/**
	 * Write bytes as a slave
	 * @param tx_data
	 * @param tx_len
	 */
	StatusCode write_bytes(uint8_t *tx_data, size_t tx_len);

	/**
	 * Read a specific register on the i2c slave device
	 * @param addr
	 * @param register_address Address of the register. Read datasheet of device for info
	 * @param register_address_size Size of register in bytes. Usually 1 or 2. Read datasheet of slave device for this info
	 * @param rx_data
	 * @param rx_len
	 * @return
	 */
	StatusCode read_register(I2CAddress addr, uint16_t register_address, size_t register_address_size, uint8_t *rx_data, size_t rx_len);

	/**
	 * Write to a specific register on the slave device
	 * @param addr
	 * @param register_address Address of the register. Read datasheet of device for info
	 * @param register_address_size Size of register in bytes. Usually 1 or 2. Read datasheet of slave device for this info
	 * @param tx_data
	 * @param tx_len
	 * @return
	 */
	StatusCode write_register(I2CAddress addr, uint16_t register_address, size_t register_address_size, uint8_t *tx_data, size_t tx_len);

 private:
	I2CPortNum port;
	GPIOPin sda;
	GPIOPin scl;
	I2CAddress address;
};