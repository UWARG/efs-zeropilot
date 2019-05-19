#include "I2C.hpp"
#include "Status.hpp"
#include "Debug.hpp"
#include "DMA.hpp"
#include <stdlib.h>

#if STM32F030xC
#include "stm32f0xx_hal.h"
#elif STM32F7xx
#include "stm32f7xx_hal.h"
#endif

extern StatusCode get_status_code(HAL_StatusTypeDef status);

const uint32_t I2C_SYNCHRONOUS_TIMEOUT = 50; //50ms timeout for synchronous implementation

extern uint32_t STD_MODE_MASTER_TIMING;
extern uint32_t STD_MODE_SLAVE_TIMING;
extern uint32_t FAST_MODE_MASTER_TIMING;
extern uint32_t FAST_MODE_SLAVE_TIMING;

//this is called after the children (I2CSlave or I2CMaster)
//have setup the h12c* instance timers and addressing
StatusCode I2CPort::setup() {
	if (is_setup || !is_valid_port()) return STATUS_CODE_INVALID_ARGS;

	auto handle = (I2C_HandleTypeDef *) interface_handle;

	sda.setup();
	scl.setup();

	//the following was generated using stm cube
	handle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	handle->Init.OwnAddress2 = 0;
	handle->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	handle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	if (HAL_I2C_Init(handle) != HAL_OK) {
		return STATUS_CODE_INTERNAL_ERROR;
	}

	/**Configure Analogue filter
	*/
	if (HAL_I2CEx_ConfigAnalogFilter(handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
		return STATUS_CODE_INTERNAL_ERROR;
	}

	/**Configure Digital filter
	*/
	if (HAL_I2CEx_ConfigDigitalFilter(handle, 0) != HAL_OK) {
		return STATUS_CODE_INTERNAL_ERROR;
	}

	is_setup = true;
	return STATUS_CODE_OK;
}

I2CSlavePort::I2CSlavePort(I2CPortNum port_num, I2CSettings settings, I2CAddress address) : I2CPort(port_num,
																									settings) {
	this->address = static_cast<I2CAddress>(address & (0xFF >> 1)); //force 7-bit mask
}

StatusCode I2CSlavePort::setup() {
	if (is_setup || !is_valid_port()) return STATUS_CODE_INVALID_ARGS;

	auto handle = (I2C_HandleTypeDef *) interface_handle;

	if (settings.speed == I2C_SPEED_FAST) {
		handle->Init.Timing = FAST_MODE_SLAVE_TIMING;
	} else if (settings.speed == I2C_SPEED_STANDARD) {
		handle->Init.Timing = STD_MODE_SLAVE_TIMING;
	}

	handle->Init.OwnAddress1 = address;

	return I2CPort::setup();
}

StatusCode I2CSlavePort::read_bytes(uint8_t *rx_data, size_t rx_len, size_t &bytes_read) {
	if (!is_setup || !is_valid_port()) return STATUS_CODE_INVALID_ARGS;

	StatusCode status = STATUS_CODE_OK;

	if (dma_setup_rx) {
		if (dma_config->reset) {
			reallocate_dma_buffer = false;
			reset();
			setup();
			setupDMA(0, dma_config->dma_buffer_len);
			reallocate_dma_buffer = true;
			dma_config->reset = false;
		}

		if (rx_queue == nullptr) abort("RX QUEUE IS NULL!", __FILE__, __LINE__);

		bytes_read = 0;

		while (bytes_read < rx_len && !rx_queue->empty()) {
			rx_data[bytes_read] = rx_queue->front();
			rx_queue->pop_front();
			bytes_read++;
		}
	} else {
		auto handle = (I2C_HandleTypeDef *) interface_handle;

		status =
			get_status_code(HAL_I2C_Slave_Receive(handle,
												  rx_data,
												  (uint16_t) rx_len,
												  I2C_SYNCHRONOUS_TIMEOUT));

		status == STATUS_CODE_OK ? bytes_read = rx_len : bytes_read = 0;
	}

	if (status == STATUS_CODE_OK && bytes_read == 0) return STATUS_CODE_EMPTY;
	return status;
}

StatusCode I2CSlavePort::write_bytes(uint8_t *tx_data, size_t tx_len) {
	if (!is_setup || !is_valid_port()) return STATUS_CODE_INVALID_ARGS;

	auto handle = (I2C_HandleTypeDef *) interface_handle;

	HAL_StatusTypeDef status =
		HAL_I2C_Slave_Transmit(handle, tx_data, (uint16_t) tx_len, I2C_SYNCHRONOUS_TIMEOUT);
	return get_status_code(status);
}

StatusCode I2CSlavePort::registerDMAReceiveCallback(void (*f)()) {
	dma_config->complete_callback = f;
	return STATUS_CODE_OK;
}

StatusCode I2CSlavePort::clearDMAReceiveCallback() {
	dma_config->complete_callback = nullptr;
	return STATUS_CODE_OK;
}

StatusCode I2CMasterPort::setup() {
	if (is_setup || !is_valid_port()) return STATUS_CODE_INVALID_ARGS;

	auto handle = (I2C_HandleTypeDef *) interface_handle;

	if (settings.speed == I2C_SPEED_FAST) {
		handle->Init.Timing = FAST_MODE_MASTER_TIMING;
	} else if (settings.speed == I2C_SPEED_STANDARD) {
		handle->Init.Timing = STD_MODE_MASTER_TIMING;
	}

	handle->Init.OwnAddress1 = 0;

	return I2CPort::setup();
}

StatusCode I2CMasterPort::read_bytes(I2CAddress addr, uint8_t *rx_data, size_t rx_len) {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

	auto handle = (I2C_HandleTypeDef *) interface_handle;

	HAL_StatusTypeDef status = HAL_I2C_Master_Receive(handle,
													  addr,
													  rx_data,
													  (uint16_t) rx_len,
													  I2C_SYNCHRONOUS_TIMEOUT);
	return get_status_code(status);
}

StatusCode I2CMasterPort::write_bytes(I2CAddress addr, uint8_t *tx_data, size_t tx_len) {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

	auto handle = (I2C_HandleTypeDef *) interface_handle;

	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(handle,
													   addr,
													   tx_data,
													   (uint16_t) tx_len,
													   I2C_SYNCHRONOUS_TIMEOUT);
	return get_status_code(status);
}

StatusCode I2CMasterPort::read_register(I2CAddress addr,
										uint16_t register_address,
										size_t register_address_size,
										uint8_t *rx_data,
										size_t rx_len) {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

	auto handle = (I2C_HandleTypeDef *) interface_handle;

	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(handle,
												addr,
												register_address,
												register_address,
												rx_data,
												(uint16_t) rx_len,
												I2C_SYNCHRONOUS_TIMEOUT);
	return get_status_code(status);
}

StatusCode I2CMasterPort::write_register(I2CAddress addr,
										 uint16_t register_address,
										 size_t register_address_size,
										 uint8_t *tx_data,
										 size_t tx_len) {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

	auto handle = (I2C_HandleTypeDef *) interface_handle;

	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(handle,
												 addr,
												 register_address,
												 register_address,
												 tx_data,
												 (uint16_t) tx_len,
												 I2C_SYNCHRONOUS_TIMEOUT);
	return get_status_code(status);
}
