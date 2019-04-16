#include "I2C.hpp"
#include "Status.hpp"
#include "stm32f0xx_hal.h"

const uint32_t I2C_SYNCHRONOUS_TIMEOUT = 50; //50ms timeout for synchronous implementation

//safety chip specific constants
static const GPIOPort I2C1_SDA_PORT = GPIO_PORT_B;
static const GPIOPort I2C1_SCL_PORT = GPIO_PORT_B;
static const GPIOPinNum I2C1_SDA_PIN_NUM = 9;
static const GPIOPinNum I2C1_SCL_PIN_NUM = 8;

//retrieved from the stm i2c configurator excel (in repo)
//the i2c clock should be at the system clock freq (48Mhz)
static const uint32_t STD_MODE_MASTER_TIMING = 0x10805E89;
static const uint32_t STD_MODE_SLAVE_TIMING = 0x10800000;
static const uint32_t FAST_MODE_MASTER_TIMING = 0x00901850;
static const uint32_t FAST_MODE_SLAVE_TIMING = 0x00900000;

//only i2c1 valid for safety chip
static I2C_HandleTypeDef hi2c1;

static I2C_HandleTypeDef *get_i2c_handle_from_port(I2CPortNum num);
extern StatusCode get_status_code(HAL_StatusTypeDef status);

I2CPort::I2CPort(I2CPortNum port_num, I2CSpeed speed) {
	port = port_num;

	//only i2c1 is valid
	sda = GPIOPin(I2C1_SDA_PORT,
				  I2C1_SDA_PIN_NUM,
				  GPIO_ALT_OD,
				  GPIO_STATE_LOW,
				  GPIO_RES_PULLUP,
				  GPIO_SPEED_HIGH,
				  GPIO_AF1_I2C1);
	scl = GPIOPin(I2C1_SCL_PORT,
				  I2C1_SCL_PIN_NUM,
				  GPIO_ALT_OD,
				  GPIO_STATE_LOW,
				  GPIO_RES_PULLUP,
				  GPIO_SPEED_HIGH,
				  GPIO_AF1_I2C1);
}

//this is called after the children (I2CSlave or I2CMaster)
//have setup the h12c* instance timers and addressing
StatusCode I2CPort::setup() {
	if (port == I2C_PORT1) {

		sda.setup();
		scl.setup();

		//start hardware clock
		__HAL_RCC_I2C1_CLK_ENABLE();

		//the following was generated using stm cube
		hi2c1.Instance = I2C1;
		hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		hi2c1.Init.OwnAddress2 = 0;
		hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
		hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

		if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
			return STATUS_CODE_INTERNAL_ERROR;
		}

		/**Configure Analogue filter
		*/
		if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
			return STATUS_CODE_INTERNAL_ERROR;
		}

		/**Configure Digital filter
		*/
		if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
			return STATUS_CODE_INTERNAL_ERROR;
		}

		return STATUS_CODE_OK;
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CPort::reset() {
	if (port == I2C_PORT1) {
		__HAL_RCC_I2C1_CLK_DISABLE();
		StatusCode code1 = scl.reset();
		sda.reset();
		return code1;
	}
	return STATUS_CODE_INVALID_ARGS;
}

I2CSlavePort::I2CSlavePort(I2CPortNum port_num, I2CSpeed speed, I2CAddress address) : I2CPort(port_num, speed) {
	this->address = static_cast<I2CAddress>(address & (0xFF >> 1)); //force 7-bit mask
}

StatusCode I2CSlavePort::setup() {
	if (port == I2C_PORT1) {
		if (speed == I2C_SPEED_FAST) {
			hi2c1.Init.Timing = FAST_MODE_SLAVE_TIMING;
		} else if (speed == I2C_SPEED_STANDARD) {
			hi2c1.Init.Timing = STD_MODE_SLAVE_TIMING;
		}

		hi2c1.Init.OwnAddress1 = address;

		return I2CPort::setup();
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CSlavePort::read_bytes(uint8_t *rx_data, size_t rx_len) {
	if (port == I2C_PORT1) {
		HAL_StatusTypeDef status =
			HAL_I2C_Slave_Receive(get_i2c_handle_from_port(port), rx_data, (uint16_t) rx_len, I2C_SYNCHRONOUS_TIMEOUT);
		return get_status_code(status);
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CSlavePort::write_bytes(uint8_t *tx_data, size_t tx_len) {
	if (port == I2C_PORT1) {
		HAL_StatusTypeDef status =
			HAL_I2C_Slave_Transmit(get_i2c_handle_from_port(port), tx_data, (uint16_t) tx_len, I2C_SYNCHRONOUS_TIMEOUT);
		return get_status_code(status);
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CMasterPort::setup() {
	if (port == I2C_PORT1) {
		if (speed == I2C_SPEED_FAST) {
			hi2c1.Init.Timing = FAST_MODE_MASTER_TIMING;
		} else if (speed == I2C_SPEED_STANDARD) {
			hi2c1.Init.Timing = STD_MODE_MASTER_TIMING;
		}

		hi2c1.Init.OwnAddress1 = 0;

		return I2CPort::setup();
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CMasterPort::read_bytes(I2CAddress addr, uint8_t *rx_data, size_t rx_len) {
	if (port == I2C_PORT1) {
		HAL_StatusTypeDef status = HAL_I2C_Master_Receive(get_i2c_handle_from_port(port),
														  addr,
														  rx_data,
														  (uint16_t) rx_len,
														  I2C_SYNCHRONOUS_TIMEOUT);
		return get_status_code(status);
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CMasterPort::write_bytes(I2CAddress addr, uint8_t *tx_data, size_t tx_len) {
	if (port == I2C_PORT1) {
		HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(get_i2c_handle_from_port(port),
														   addr,
														   tx_data,
														   (uint16_t) tx_len,
														   I2C_SYNCHRONOUS_TIMEOUT);
		return get_status_code(status);
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CMasterPort::read_register(I2CAddress addr,
										uint16_t register_address,
										size_t register_address_size,
										uint8_t *rx_data,
										size_t rx_len) {
	if (port == I2C_PORT1) {
		HAL_StatusTypeDef status = HAL_I2C_Mem_Read(get_i2c_handle_from_port(port),
													addr,
													register_address,
													register_address,
													rx_data,
													(uint16_t) rx_len,
													I2C_SYNCHRONOUS_TIMEOUT);
		return get_status_code(status);
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CMasterPort::write_register(I2CAddress addr,
										 uint16_t register_address,
										 size_t register_address_size,
										 uint8_t *tx_data,
										 size_t tx_len) {
	if (port == I2C_PORT1) {
		HAL_StatusTypeDef status = HAL_I2C_Mem_Write(get_i2c_handle_from_port(port),
													 addr,
													 register_address,
													 register_address,
													 tx_data,
													 (uint16_t) tx_len,
													 I2C_SYNCHRONOUS_TIMEOUT);
		return get_status_code(status);
	}
	return STATUS_CODE_INVALID_ARGS;
}

static I2C_HandleTypeDef *get_i2c_handle_from_port(I2CPortNum num) {
	if (num == I2C_PORT1) {
		return &hi2c1;
	}
	return nullptr;
}

//this function is called by the stm32 hal when we call HAL_I2C_Init
//It configures the GPIOS
//It was generated by STM32CUBE, don't touch
void HAL_I2C_MspInit(I2C_HandleTypeDef *i2cHandle) {
	//do nothing. We configured the gpio pins beforehand
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *i2cHandle) {

	//do nothing. This is implemented in I2C::reset_state()
}