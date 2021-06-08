/*
 * Interchip.cpp
 *
 *  Created on: Jun 7, 2021
 *      Author: blueb
 */

#include "spi.h"
#include "Interchip.hpp"

static volatile Interchip_Packet rxData;
static volatile Interchip_Packet txData;
static volatile bool dataNew;

// Get the pwm signal that is currently being sent out.
// what do i return if the index is bad
int16_t getPWM(int8_t index) {
	if (index >= 12 || index < 0) return -1;
	dataNew = false;
	return rxData.PWM[index];
}

uint16_t getSafetyLevel() {
	return txData.safetyLevel;
}

void setSafetyLevel(uint16_t level) {
	txData.safetyLevel = level;
}

bool isDataNew() {
	return dataNew;
}

bool sendReceiveData() {
	//for (int i = 0; i < 12; i++) txData.PWM[i] = i+1;
	txData.PWM[0] = 1;
	txData.PWM[1] = 2;
	txData.PWM[2] = 3;
	txData.PWM[3] = 4;
	txData.PWM[4] = 5;
	txData.PWM[5] = 6;
	txData.PWM[6] = 7;
	txData.PWM[7] = 8;
	txData.PWM[8] = 9;
	txData.PWM[9] = 10;
	txData.PWM[10] = 11;
	txData.PWM[11] = 12;
	txData.safetyLevel = 13;
	HAL_StatusTypeDef result = HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)&txData,(uint8_t *)&rxData, (sizeof(Interchip_Packet)/sizeof(int8_t)));
	if(result != HAL_OK) {
		return false;
	}
	return true;
//		HAL_StatusTypeDef result = HAL_SPI_Receive_IT(&hspi1,(uint8_t *)&rxData, sizeof(Interchip_Packet)/sizeof(int8_t));
//		if(result != HAL_OK) {
//			return false;
//		}
//		return true;

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
	MX_SPI1_Init();
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);

	dataNew = true;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi) {
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
	dataNew = true;
}



