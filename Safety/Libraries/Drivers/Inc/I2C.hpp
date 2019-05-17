/**
 * Implements a synchronous, polling I2C driver. Calls the underlying stm32HAL, and should be used in place of it
 * Don't initialize this class more than once for a single i2c port, unless you also called
 * reset()
 *
 * Receives are always blocking, however DMA for i2c receives when a slave have been enabled.
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "Status.hpp"
#include "GPIO.hpp"
#include <deque>

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

typedef struct I2CSettings {
	I2CSpeed speed;
} I2CSettings;

/**
 * Don't use this class directly. Use either I2CSlavePort or I2CMasterPort, based on your needs
 */
class I2CPort {
 public:
	I2CPort(I2CPortNum port_num, I2CSettings settings);

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

 protected:
	I2CPortNum port;
	I2CSettings settings;
	GPIOPin sda;
	GPIOPin scl;
	bool is_setup = false;
};

class I2CSlavePort : public I2CPort {
 public:
	/**
	 * Creates the object, but does not initialize the connection
	 * @param port_num
	 * @param speed
	 * @param address The address of this device (use if in slave mode). Use 7-bit address
	 */
	I2CSlavePort(I2CPortNum port_num, I2CSettings settings, I2CAddress address = 0x08);

	StatusCode setup();

	/**
	 * Sets up DMA transfers transparently, if the hardware can support it. If possible, attempts to do both tx and RX
	 * using DMA. If not, may only do RX. If either the parameters are 0, will only enable either the RX/TX buffers respectively
	 * @param tx_buffer_size Size of DMA buffer when transmitting (used if possible). Try to make this as large as the largest
	 * 	packet you could possible send
	 * @param rx_buffer_size Size of DMA buffer when receiving. Make this the size of the largest packet you're expecting
	 * to receive
	 * @return
	 */
	StatusCode setupDMA(size_t tx_buffer_size, size_t rx_buffer_size);

	/**
	 * If DMA was setup for this channel, resets it so that synchronous transfers are done instead
	 */
	StatusCode resetDMA();

	/**
	 * Read bytes as a slave
	 * @param rx_data Buffer to write data to
	 * @param rx_len
	 */
	StatusCode read_bytes(uint8_t *rx_data, size_t rx_len, size_t &bytes_read);

	/**
	 * Write bytes as a slave
	 * @param tx_data
	 * @param tx_len
	 */
	StatusCode write_bytes(uint8_t *tx_data, size_t tx_len);

	/**
	 * Gets called when the slave receieves a complete DMA packet
	 * Call read inside the callback if you want to process it
	 * If you don't register a callback whilst in DMA, you'll have to poll the
	 * read_bytes() function
	 * @param f Callback function
	 * @return
	 */
	StatusCode registerDMAReceiveCallback(void (*f)());

	/**
	 * Note the callback isn't automatically cleared when calling resetDMA()
	 * If you don't want a callback being fired anymore, make sure to call this
	 * function
	 * @return
	 */
	StatusCode clearDMAReceiveCallback();

 private:
	I2CAddress address; //the devices i2c address
	std::deque<uint8_t> *rx_queue;
	bool dma_setup_tx = false;
	bool dma_setup_rx = false;
	bool reallocate_dma_buffer = true;
};

class I2CMasterPort : public I2CPort {
 public:
	StatusCode setup();

	/**
	 * Read bytes as a master (need address of slave)
	 * @param addr
	 * @param rx_data Buffer to write data to
	 * @param rx_len
	 */
	StatusCode read_bytes(I2CAddress addr, uint8_t *rx_data, size_t rx_len);

	/**
	 * Write bytes as a master (need address of slave)
	 * @param addr
	 * @param tx_data
	 * @param tx_len
	 */
	StatusCode write_bytes(I2CAddress addr, uint8_t *tx_data, size_t tx_len);

	/**
	 * Read a specific register on the i2c slave device
	 * @param addr
	 * @param register_address Address of the register. Read datasheet of device for info
	 * @param register_address_size Size of register in bytes. Usually 1 or 2. Read datasheet of slave device for this info
	 * @param rx_data
	 * @param rx_len
	 * @return
	 */
	StatusCode read_register(I2CAddress addr,
							 uint16_t register_address,
							 size_t register_address_size,
							 uint8_t *rx_data,
							 size_t rx_len);

	/**
	 * Write to a specific register on the slave device
	 * @param addr
	 * @param register_address Address of the register. Read datasheet of device for info
	 * @param register_address_size Size of register in bytes. Usually 1 or 2. Read datasheet of slave device for this info
	 * @param tx_data
	 * @param tx_len
	 * @return
	 */
	StatusCode write_register(I2CAddress addr,
							  uint16_t register_address,
							  size_t register_address_size,
							  uint8_t *tx_data,
							  size_t tx_len);
};