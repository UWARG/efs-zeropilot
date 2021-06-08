/*
 * Interchip.cpp
 *
 *  Created on: Jun 4, 2021
 *      Author: aadi
 */

#include "spi.h"
#include "Interchip.hpp"

static volatile Interchip_Packet rxData;
static volatile Interchip_Packet txData;
static volatile bool dataNew;

// Set the pwm signal that is currently being sent out.
bool setPWM(int8_t index,int16_t value) {
	if (index >= 12 || index < 0) return false;
	txData.PWM[index] = value;
	return true;
}

// Get the pwm signal that is currently being sent out.
int16_t getPWM(int8_t index) {
	if (index >= 12 || index < 0) return -1;
	return txData.PWM[index];
}

uint16_t getSafetyLevel() {
	dataNew = false;
	return rxData.safetyLevel;
}

bool isDataNew() {
	return dataNew;
}

bool sendRecieveData() {
	//do some interrupt shit idk
		txData.PWM[0] = 14;
		txData.PWM[1] = 15;
		txData.PWM[2] = 16;
		txData.PWM[3] = 17;
		txData.PWM[4] = 18;
		txData.PWM[5] = 19;
		txData.PWM[6] = 20;
		txData.PWM[7] = 21;
		txData.PWM[8] = 22;
		txData.PWM[9] = 23;
		txData.PWM[10] = 24;
		txData.PWM[11] = 25;
		txData.safetyLevel = 26;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_StatusTypeDef result = HAL_SPI_TransmitReceive_IT(&hspi6,(uint8_t *)&txData,(uint8_t *)&rxData, (sizeof(Interchip_Packet)/sizeof(int8_t)));
	if(result != HAL_OK) {
		return false;
	}
	return true;

//	uint8_t x = 2;
//	uint8_t * i = &x;
//		//HAL_StatusTypeDef result = HAL_SPI_Transmit(&hspi6,i, sizeof(int8_t), 100);
//	HAL_StatusTypeDef result = HAL_SPI_Transmit_IT(&hspi6,i, sizeof(int8_t));
//	if(result != HAL_OK) {
//				return false;
//			}
//			return true;
}

void init() {

	MX_SPI6_Init();
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
	  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	dataNew = true;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi) {
	  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	dataNew = true;
}



