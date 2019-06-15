#include "Status.hpp"
#include "stm32f7xx_hal.h"
#include "SPI.hpp"
#include "GPIO.hpp"
#include "Clock.hpp"

const uint32_t SPI_SYNCHRONOUS_TIMEOUT = 50; //50ms timeout for synchronous implementation

SPI_HandleTypeDef hspi1; //we only have a single spi port enabled on the stm32f0

//configure our pin mappings
static const GPIOPort NSS_PORT_SPI1 = GPIO_PORT_A;
static const GPIOPort MISO_PORT_SPI1 = GPIO_PORT_B;
static const GPIOPort MOSI_PORT_SPI1 = GPIO_PORT_B;
static const GPIOPort SCK_PORT_SPI1 = GPIO_PORT_B;
static const GPIOPinNum NSS_PIN_SPI1 = 15;
static const GPIOPinNum MISO_PIN_SPI1 = 4;
static const GPIOPinNum MOSI_PIN_SPI1 = 5;
static const GPIOPinNum SCK_PIN_SPI1 = 3;

static StatusCode configure_spi_port(SPI_HandleTypeDef *handle, SPISettings settings, uint32_t clock);
extern StatusCode get_status_code(HAL_StatusTypeDef status);

SPIPort::SPIPort(SPISettings settings) {
	this->settings = settings;
}

StatusCode SPIPort::setup() {
//	if (has_setup) { //if the interface was already setup
//		return STATUS_CODE_INVALID_ARGS;
//	}
//	if (settings.word_size != 1 || settings.word_size != 2) { //we only support 8-bit or 16-bit spi
//		return STATUS_CODE_INVALID_ARGS;
//	}
//
//	if (settings.port == SPI_PORT1) { //only port 1 valid for stm32f0
//		hspi1.Instance = SPI1;
//
//		StatusCode status;
//
//		__HAL_RCC_SPI1_CLK_ENABLE();
//
//		//spi1 is on apb1 bus for spi1
//		uint32_t clock = get_peripheral_clock_apb1();
//
//		//only configure nss in case we're in slave mode
//		if (!settings.master) {
//			nss = GPIOPin(NSS_PORT_SPI1,
//						  NSS_PIN_SPI1,
//						  GPIO_ALT_PP,
//						  GPIO_STATE_LOW,
//						  GPIO_RES_NONE,
//						  GPIO_SPEED_HIGH,
//						  GPIO_AF0_SPI1);
//			status = nss.setup();
//			if (status != STATUS_CODE_OK) {
//				return status;
//			}
//		}
//
//		miso = GPIOPin(MISO_PORT_SPI1,
//					   MISO_PIN_SPI1,
//					   GPIO_ALT_PP,
//					   GPIO_STATE_LOW,
//					   GPIO_RES_NONE,
//					   GPIO_SPEED_HIGH,
//					   GPIO_AF0_SPI1);
//		mosi = GPIOPin(MOSI_PORT_SPI1,
//					   MOSI_PIN_SPI1,
//					   GPIO_ALT_PP,
//					   GPIO_STATE_LOW,
//					   GPIO_RES_NONE,
//					   GPIO_SPEED_HIGH,
//					   GPIO_AF0_SPI1);
//		sck = GPIOPin(SCK_PORT_SPI1,
//					  SCK_PIN_SPI1,
//					  GPIO_ALT_PP,
//					  GPIO_STATE_LOW,
//					  GPIO_RES_NONE,
//					  GPIO_SPEED_HIGH,
//					  GPIO_AF0_SPI1);
//
//		status = miso.setup();
//		if (status != STATUS_CODE_OK) {
//			return status;
//		}
//		status = mosi.setup();
//		if (status != STATUS_CODE_OK) {
//			return status;
//		}
//		status = sck.setup();
//		if (status != STATUS_CODE_OK) {
//			return status;
//		}
//
//		status = configure_spi_port(&hspi1, this->settings, clock);
//
//		//configure SPI1 interrupts
//		HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
//		HAL_NVIC_EnableIRQ(SPI1_IRQn);
//
//		return status;
//	}
	return STATUS_CODE_INVALID_ARGS;
}

int32_t SPIPort::add_slave(GPIOPin slave_pin) {
	if (!has_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}
	if (num_slaves == MAX_SPI_SLAVES) {
		return -1;
	}

	slaves[num_slaves] = slave_pin;
	num_slaves++;
	return num_slaves - 1;
}

StatusCode SPIPort::set_slave(uint32_t slave_num) {
	if (!has_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}
	if (num_slaves > 0 && slave_num < num_slaves) {
		slaves[curr_slave].set_state(GPIO_STATE_LOW);
		slaves[slave_num].set_state(GPIO_STATE_HIGH);
		curr_slave = slave_num;
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode SPIPort::exchange_data(uint8_t *tx_data, uint8_t *rx_data, size_t length) {
	if (!has_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}

	if (settings.port == SPI_PORT1) {
		return get_status_code(HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, (uint16_t) length,
													   SPI_SYNCHRONOUS_TIMEOUT));
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode SPIPort::reset() {
	if (!has_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}
	miso.reset();
	mosi.reset();
	nss.reset();
	sck.reset();

	if (settings.port == SPI_PORT1) {
		__HAL_RCC_SPI1_CLK_DISABLE();
		HAL_NVIC_DisableIRQ(SPI1_IRQn);
	}
	has_setup = false;
	return STATUS_CODE_OK;
}

StatusCode configure_spi_port(SPI_HandleTypeDef *handle, SPISettings settings, uint32_t clock) {
	if (settings.master) {
		handle->Init.Mode = SPI_MODE_MASTER;

		uint32_t prescaler = clock / settings.frequency;

		if (prescaler <= 2) {
			handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
		} else if (prescaler <= 4) {
			handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
		} else if (prescaler <= 8) {
			handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
		} else if (prescaler <= 16) {
			handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
		} else if (prescaler <= 32) {
			handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
		} else if (prescaler <= 64) {
			handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
		} else if (prescaler <= 128) {
			handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
		} else {
			handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
		}
	} else {
		handle->Init.Mode = SPI_MODE_SLAVE;
	}

	handle->Init.Direction = SPI_DIRECTION_2LINES;

	if (settings.word_size == 1) {
		handle->Init.DataSize = SPI_DATASIZE_8BIT;
	} else {
		handle->Init.DataSize = SPI_DATASIZE_16BIT;
	}

	switch (settings.mode) {
		case SPI_MODE_0: handle->Init.CLKPolarity = SPI_POLARITY_LOW;
			handle->Init.CLKPhase = SPI_PHASE_1EDGE;
			break;
		case SPI_MODE_1: handle->Init.CLKPolarity = SPI_POLARITY_LOW;
			handle->Init.CLKPhase = SPI_PHASE_2EDGE;
			break;
		case SPI_MODE_2: handle->Init.CLKPolarity = SPI_POLARITY_HIGH;
			handle->Init.CLKPhase = SPI_PHASE_1EDGE;
			break;
		case SPI_MODE_3: handle->Init.CLKPolarity = SPI_POLARITY_HIGH;
			handle->Init.CLKPhase = SPI_PHASE_2EDGE;
			break;
	}

	handle->Init.NSS = SPI_NSS_HARD_INPUT; //hardware slave pin

	if (settings.first_bit_msb) {
		handle->Init.FirstBit = SPI_FIRSTBIT_MSB;
	} else {
		handle->Init.FirstBit = SPI_FIRSTBIT_LSB;
	}

	handle->Init.TIMode = SPI_TIMODE_DISABLE;
	handle->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	handle->Init.CRCPolynomial = 7;
	handle->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	handle->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

	return get_status_code(HAL_SPI_Init(handle));
}