#pragma once

#include <stddef.h>
#include <stdint.h>
#include "Status.hpp"

typedef uint8_t I2CAddress;

typedef enum {
  I2C_SPEED_STANDARD = 0,  // 100kHz
  I2C_SPEED_FAST,          // 400 kHz
  NUM_I2C_SPEEDS,
} I2CSpeed;

typedef enum {
  I2C_PORT1 = 0,
  I2C_PORT2 = 1,
  I2C_PORT3 = 2,
  I2C_PORT4 = 3
} I2CPort;

//define settings struct, in case we want to expand in number of settings
typedef struct {
  I2CSpeed speed;
} I2CSettings;

/**
 * Initialize the specified I2C port. Note that if the port doesn't exist in the hardware,
 * it will return a STATUS_CODE_INVALID_ARGS status code
 * @param port
 * @param speed
 * @return STATUS_CODE_INVALID_ARGS if port doesn't exist
 */
StatusCode i2c_init(I2CPort port, I2CSettings speed);

/**
 * Read a specified number of bytes from the i2c address. Looks like:
 * START | ADDR READ ACK | DATA ACK | ... | STOP
 * @param i2c
 * @param addr
 * @param rx_data Pointer to pre-allocated buffer to write data into
 * @param rx_len Number of bytes to read
 * @return STATUS_CODE_INVALID_ARGS if port doesn't exist
 */
StatusCode i2c_read(I2CPort i2c, I2CAddress addr, uint8_t *rx_data, size_t rx_len);

/**
 * Write a specified number of bytes from the i2c address. Looks like:
 * START | ADDR WRITE ACK | DATA ACK | ... | STOP
 * @param i2c
 * @param addr
 * @param rx_data Pointer to tx data to write
 * @param rx_len Number of bytes to write
 * @return STATUS_CODE_INVALID_ARGS if port doesn't exist
 */
StatusCode i2c_write(I2CPort i2c, I2CAddress addr, uint8_t *tx_data, size_t tx_len);

/**
 * Read a specified register on a specified device. Looks like:
 * START | ADDR WRITE ACK | REG ACK | START | ADDR READ ACK | DATA ACK | ... | STOP
 * @param i2c
 * @param addr
 * @param rx_data Pointer to pre-allocated buffer to read data into
 * @param rx_len Number of bytes to read
 * @return STATUS_CODE_INVALID_ARGS if port doesn't exist
 */
StatusCode i2c_read_reg(I2CPort i2c, I2CAddress addr, uint8_t reg, uint8_t *rx_data, size_t rx_len);

/**
 * Write to a specified register on a specified device. Looks like:
 * START | ADDR WRITE ACK | REG ACK | START | ADDR WRITE ACK | DATA ACK | ... | STOP
 * @param i2c
 * @param addr
 * @param rx_data Data to write
 * @param rx_len Number of bytes to write
 * @return STATUS_CODE_INVALID_ARGS if port doesn't exist
 */
StatusCode i2c_write_reg(I2CPort i2c, I2CAddress addr, uint8_t reg, uint8_t *tx_data,
                         size_t tx_len);