#include "I2C.hpp"
#include "Status.hpp"
#include "stm32f0xx_hal.h"
#include "Debug.hpp"
#include <stdlib.h>
#include "DMA.hpp"

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
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
DMAConfig i2c1_dma_config;
static std::deque<uint8_t> i2c1_rx_queue;

static I2C_HandleTypeDef *get_i2c_handle_from_port(I2CPortNum num);
extern StatusCode get_status_code(HAL_StatusTypeDef status);
void I2C1_DMA_ErrorCallback(DMA_HandleTypeDef *dma);

I2CPort::I2CPort(I2CPortNum port_num, I2CSettings settings) {
	port = port_num;
	this->settings = settings;

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
	if (is_setup) return STATUS_CODE_INVALID_ARGS;

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

		is_setup = true;
		return STATUS_CODE_OK;
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CPort::reset() {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

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
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;
	if (dma_setup_rx) return STATUS_CODE_INVALID_ARGS; //should call this without calling reset() first
	if (rx_buffer_size == 0) { //invalid rx buffer size
		return STATUS_CODE_INVALID_ARGS;
	}

	__HAL_RCC_DMA1_CLK_ENABLE();

	hdma_i2c1_rx.Instance = DMA1_Channel3;
	hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_i2c1_rx.Init.Mode = DMA_CIRCULAR;
	hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;

	StatusCode status = get_status_code(HAL_DMA_Init(&hdma_i2c1_rx));
	if (status != STATUS_CODE_OK) return status;

	__HAL_DMA1_REMAP(HAL_DMA1_CH3_I2C1_RX);
	__HAL_LINKDMA(&hi2c1, hdmarx, hdma_i2c1_rx);

	resetDMAConfig(&i2c1_dma_config, rx_buffer_size);
	i2c1_dma_config.dma_handle = (void *) &hdma_i2c1_rx;
	i2c1_dma_config.queue = (void *) &i2c1_rx_queue;

	//don't allocate memory if we hadn't free'd it
	if (i2c1_dma_config.dma_buffer == nullptr) {
		i2c1_dma_config.dma_buffer = (uint8_t *) malloc(i2c1_dma_config.dma_buffer_len * sizeof(uint8_t));
	}

	if (i2c1_dma_config.dma_buffer == nullptr) {
		return STATUS_CODE_RESOURCE_EXHAUSTED;
	}

	rx_queue = &i2c1_rx_queue;

	//enable stop bit detection and error interrupts
	SET_BIT(hi2c1.Instance->CR1, I2C_CR1_ERRIE);
	SET_BIT(hi2c1.Instance->CR1, I2C_CR1_STOPIE);

	//init circular dma transfer
	status =
		get_status_code(HAL_I2C_Slave_Receive_DMA(&hi2c1,
												  i2c1_dma_config.dma_buffer,
												  (uint16_t) i2c1_dma_config.dma_buffer_len));
	if (status != STATUS_CODE_OK) return status;

	HAL_DMA_RegisterCallback(&hdma_i2c1_rx, HAL_DMA_XFER_ERROR_CB_ID, I2C1_DMA_ErrorCallback);

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

	//clear interrupts
	CLEAR_BIT(hi2c1.Instance->CR1, I2C_CR1_ERRIE);
	CLEAR_BIT(hi2c1.Instance->CR1, I2C_CR1_STOPIE);
	HAL_NVIC_DisableIRQ(I2C1_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);

	StatusCode status;
	status = get_status_code(HAL_DMA_DeInit(&hdma_i2c1_rx));
	if (status != STATUS_CODE_OK) return status;

	//avoid uneccessary re-allocations in case of dma error
	if (reallocate_dma_buffer) {
		if (i2c1_dma_config.dma_buffer == nullptr) abort("i2c dma buffer null!", __FILE__, __LINE__);
		free(i2c1_dma_config.dma_buffer);
		i2c1_dma_config.dma_buffer = nullptr;
		i2c1_rx_queue.erase(i2c1_rx_queue.begin(), i2c1_rx_queue.end());
	}

	dma_setup_rx = false;
	return STATUS_CODE_OK;
}

I2CSlavePort::I2CSlavePort(I2CPortNum port_num, I2CSettings settings, I2CAddress address) : I2CPort(port_num,
																									settings) {
	this->address = static_cast<I2CAddress>(address & (0xFF >> 1)); //force 7-bit mask
}

StatusCode I2CSlavePort::setup() {
	if (is_setup) return STATUS_CODE_INVALID_ARGS;

	if (port == I2C_PORT1) {
		if (settings.speed == I2C_SPEED_FAST) {
			hi2c1.Init.Timing = FAST_MODE_SLAVE_TIMING;
		} else if (settings.speed == I2C_SPEED_STANDARD) {
			hi2c1.Init.Timing = STD_MODE_SLAVE_TIMING;
		}

		hi2c1.Init.OwnAddress1 = address;

		return I2CPort::setup();
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CSlavePort::read_bytes(uint8_t *rx_data, size_t rx_len, size_t &bytes_read) {
	if (!is_setup || port != I2C_PORT1) return STATUS_CODE_INVALID_ARGS;

	StatusCode status = STATUS_CODE_OK;

	if (dma_setup_rx) {
		if (i2c1_dma_config.reset) {
			reallocate_dma_buffer = false;
			reset();
			setup();
			setupDMA(0, i2c1_dma_config.dma_buffer_len);
			reallocate_dma_buffer = true;
			i2c1_dma_config.reset = false;
		}

		if (rx_queue == nullptr) abort("RX QUEUE IS NULL!", __FILE__, __LINE__);

		while (bytes_read < rx_len && !rx_queue->empty()) {
			rx_data[bytes_read] = rx_queue->front();
			rx_queue->pop_front();
			bytes_read++;
		}
	} else {
		status =
			get_status_code(HAL_I2C_Slave_Receive(get_i2c_handle_from_port(port),
												  rx_data,
												  (uint16_t) rx_len,
												  I2C_SYNCHRONOUS_TIMEOUT));

		status == STATUS_CODE_OK ? bytes_read = rx_len : bytes_read = 0;
	}

	if (status == STATUS_CODE_OK && bytes_read == 0) return STATUS_CODE_EMPTY;
	return status;
}

StatusCode I2CSlavePort::write_bytes(uint8_t *tx_data, size_t tx_len) {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

	if (port == I2C_PORT1) {
		HAL_StatusTypeDef status =
			HAL_I2C_Slave_Transmit(get_i2c_handle_from_port(port), tx_data, (uint16_t) tx_len, I2C_SYNCHRONOUS_TIMEOUT);
		return get_status_code(status);
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CSlavePort::registerDMAReceiveCallback(void (*f)()) {
	i2c1_dma_config.complete_callback = f;
	return STATUS_CODE_OK;
}

StatusCode I2CSlavePort::clearDMAReceiveCallback() {
	i2c1_dma_config.complete_callback = nullptr;
	return STATUS_CODE_OK;
}

StatusCode I2CMasterPort::setup() {
	if (is_setup) return STATUS_CODE_INVALID_ARGS;

	if (port == I2C_PORT1) {
		if (settings.speed == I2C_SPEED_FAST) {
			hi2c1.Init.Timing = FAST_MODE_MASTER_TIMING;
		} else if (settings.speed == I2C_SPEED_STANDARD) {
			hi2c1.Init.Timing = STD_MODE_MASTER_TIMING;
		}

		hi2c1.Init.OwnAddress1 = 0;

		return I2CPort::setup();
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode I2CMasterPort::read_bytes(I2CAddress addr, uint8_t *rx_data, size_t rx_len) {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

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
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

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
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

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
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

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

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {
		processDMARXCompleteEvent(&i2c1_dma_config);

		//notify user we received new packet
		if (i2c1_dma_config.complete_callback) {
			i2c1_dma_config.complete_callback();
		}
	}
}

static I2C_HandleTypeDef *get_i2c_handle_from_port(I2CPortNum num) {
	if (num == I2C_PORT1) {
		return &hi2c1;
	}
	return nullptr;
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