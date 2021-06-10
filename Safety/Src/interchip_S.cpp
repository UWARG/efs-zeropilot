#include "spi.h"
#include "Interchip_S.hpp"

static volatile Interchip_Packet rxData;
static volatile Interchip_Packet txData;
static volatile bool dataNew;

// Get the pwm signal that has been .
int16_t getPWM(int8_t index) {
	dataNew = false;
	return rxData.PWM[index];
}

// Get the safety level that is currently being sent out.
uint16_t getSafetyLevel() {
	return txData.safetyLevel;
}

// Set the safety level that is currently being sent out.
void setSafetyLevel(uint16_t level) {
	txData.safetyLevel = level;
}

bool isDataNew() {
	return dataNew;
}

void sendReceiveData() {
	HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)&txData,(uint8_t *)&rxData, sizeof(Interchip_Packet));
}

void testSetup() {
	txData.PWM[0] = 1;
	txData.PWM[1] = 2;
	txData.PWM[2] = 3;
	txData.PWM[3] = 7;
	txData.PWM[4] = 5;
	txData.PWM[5] = 6;
	txData.PWM[6] = 7;
	txData.PWM[7] = 8;
	txData.PWM[8] = 9;
	txData.PWM[9] = 10;
	txData.PWM[10] = 11;
	txData.PWM[11] = 12;
	txData.safetyLevel = 13; 
}


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
	// Start Listening after the previous exchange.
	HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)&txData,(uint8_t *)&rxData, sizeof(Interchip_Packet));
	dataNew = true;
}
