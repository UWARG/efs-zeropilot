#include "I2C.hpp"
#include "Status.hpp"
#include "Debug.hpp"
#include "DMA.hpp"
#include "stm32f0xx_hal.h"
#include <stdlib.h>

//safety chip specific constants
static const GPIOPort I2C1_SDA_PORT = GPIO_PORT_B;
static const GPIOPort I2C1_SCL_PORT = GPIO_PORT_B;
static const GPIOPinNum I2C1_SDA_PIN_NUM = 9;
static const GPIOPinNum I2C1_SCL_PIN_NUM = 8;

//retrieved from the stm i2c configurator excel (in repo)
//the i2c clock should be at the system clock freq (48Mhz)
uint32_t STD_MODE_MASTER_TIMING = 0x10805E89;
uint32_t STD_MODE_SLAVE_TIMING = 0x10800000;
uint32_t FAST_MODE_MASTER_TIMING = 0x00901850;
uint32_t FAST_MODE_SLAVE_TIMING = 0x00900000;

//only i2c1 valid for safety chip
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
DMAConfig i2c1_dma_config;
static std::deque<uint8_t> i2c1_rx_queue;

extern StatusCode get_status_code(HAL_StatusTypeDef status);
void I2C1_DMA_ErrorCallback(DMA_HandleTypeDef *dma);

bool I2CPort::is_valid_port() {
	return port == I2C_PORT1;
}

I2CPort::I2CPort(I2CPortNum port_num, I2CSettings settings) {
	port = port_num;
	this->settings = settings;

	if (!is_valid_port()) return;

	//only i2c1 is valid
	sda = GPIOPin(I2C1_SDA_PORT,
				  I2C1_SDA_PIN_NUM,
				  GPIO_ALT_OD,
				  GPIO_STATE_LOW,
				  GPIO_RES_PULLUP,
				  GPIO_SPEED_FAST,
				  GPIO_AF1_I2C1);
	scl = GPIOPin(I2C1_SCL_PORT,
				  I2C1_SCL_PIN_NUM,
				  GPIO_ALT_OD,
				  GPIO_STATE_LOW,
				  GPIO_RES_PULLUP,
				  GPIO_SPEED_FAST,
				  GPIO_AF1_I2C1);

	interface_handle = (void *) &hi2c1;
	hi2c1.Instance = I2C1;

	//start hardware clock
	__HAL_RCC_I2C1_CLK_ENABLE();
}

StatusCode I2CPort::reset() {
	if (!is_setup || !is_valid_port()) return STATUS_CODE_INVALID_ARGS;

	if (port == I2C_PORT1) {
		__HAL_RCC_I2C1_CLK_DISABLE();
		StatusCode code1 = scl.reset();
		sda.reset();
		return code1;
	}
	return STATUS_CODE_INVALID_ARGS;
}

//we're only gonna support DMA on I2C1 RX for this device
StatusCode I2CSlavePort::setupDMA(size_t tx_buffer_size, size_t rx_buffer_size) {
	if (port != I2C_PORT1) return STATUS_CODE_UNIMPLEMENTED;
	if (dma_setup_rx || !is_setup || rx_buffer_size == 0) return STATUS_CODE_INVALID_ARGS;

	__HAL_RCC_DMA1_CLK_ENABLE();

	dma_config = &i2c1_dma_config;
	resetDMAConfig(dma_config, rx_buffer_size);
	dma_config->dma_handle = (void *) &hdma_i2c1_rx;
	dma_config->queue = (void *) &i2c1_rx_queue;
	rx_queue = &i2c1_rx_queue;

	auto handle = (I2C_HandleTypeDef *) interface_handle;
	auto dma_handle = (DMA_HandleTypeDef *) dma_config->dma_handle;

	dma_handle->Instance = DMA1_Channel3;
	dma_handle->Init.Direction = DMA_PERIPH_TO_MEMORY;
	dma_handle->Init.PeriphInc = DMA_PINC_DISABLE;
	dma_handle->Init.MemInc = DMA_MINC_ENABLE;
	dma_handle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	dma_handle->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	dma_handle->Init.Mode = DMA_CIRCULAR;
	dma_handle->Init.Priority = DMA_PRIORITY_MEDIUM;

	StatusCode status = get_status_code(HAL_DMA_Init(dma_handle));
	if (status != STATUS_CODE_OK) return status;

	__HAL_DMA1_REMAP(HAL_DMA1_CH3_I2C1_RX);
	__HAL_LINKDMA(handle, hdmarx, *dma_handle);

	//don't allocate memory if we hadn't free'd it
	if (dma_config->dma_buffer == nullptr) {
		dma_config->dma_buffer = (uint8_t *) malloc(dma_config->dma_buffer_len * sizeof(uint8_t));
	}

	if (dma_config->dma_buffer == nullptr) {
		return STATUS_CODE_RESOURCE_EXHAUSTED;
	}

	//enable stop bit detection and error interrupts
	SET_BIT(handle->Instance->CR1, I2C_CR1_ERRIE);
	SET_BIT(handle->Instance->CR1, I2C_CR1_STOPIE);

	//init circular dma transfer
	status =
		get_status_code(HAL_I2C_Slave_Receive_DMA(handle,
												  dma_config->dma_buffer,
												  (uint16_t) dma_config->dma_buffer_len));
	if (status != STATUS_CODE_OK) return status;

	HAL_DMA_RegisterCallback(dma_handle, HAL_DMA_XFER_ERROR_CB_ID, I2C1_DMA_ErrorCallback);

	HAL_NVIC_SetPriority(I2C1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(I2C1_IRQn);

	//enable dma interrupts
	HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

	dma_setup_rx = true;

	return STATUS_CODE_OK;
}

StatusCode I2CSlavePort::resetDMA() {
	if (port != I2C_PORT1) return STATUS_CODE_UNIMPLEMENTED;
	if (!dma_setup_rx) return STATUS_CODE_INVALID_ARGS;

	auto handle = (I2C_HandleTypeDef *) interface_handle;
	auto handle_dma = (DMA_HandleTypeDef *) dma_config->dma_handle;

	//clear interrupts
	CLEAR_BIT(handle->Instance->CR1, I2C_CR1_ERRIE);
	CLEAR_BIT(handle->Instance->CR1, I2C_CR1_STOPIE);
	HAL_NVIC_DisableIRQ(I2C1_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);

	StatusCode status;
	status = get_status_code(HAL_DMA_DeInit(handle_dma));
	if (status != STATUS_CODE_OK) return status;

	//avoid uneccessary re-allocations in case of dma error
	if (reallocate_dma_buffer) {
		if (dma_config->dma_buffer == nullptr) abort("i2c dma buffer null!", __FILE__, __LINE__);
		free(dma_config->dma_buffer);
		dma_config->dma_buffer = nullptr;
		rx_queue->erase(rx_queue->begin(), rx_queue->end());
	}

	dma_setup_rx = false;
	return STATUS_CODE_OK;
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {
		processDMARXCompleteEvent(&i2c1_dma_config);

		//notify user we received new packet
		if (i2c1_dma_config.complete_callback) {
			i2c1_dma_config.complete_callback();
		}
	}
}

void I2C1_DMA_ErrorCallback(DMA_HandleTypeDef *dma) {
	error("I2C1 DMA Error received!", dma->ErrorCode);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {
		error("USART1 Error received", hi2c->ErrorCode);
		i2c1_dma_config.reset = true;
	} else {
		error("Unknown USART port got an error callback");
	}
}