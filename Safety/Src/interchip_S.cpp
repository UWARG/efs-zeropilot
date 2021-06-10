#include "spi.h"
#include "Interchip_S.hpp"

static volatile Interchip_Packet rxData;
static volatile Interchip_Packet txData;
static volatile bool dataNew;

// Get the pwm signal that has been recieved.
volatile int16_t* getPWM() {
	dataNew = false;
	return rxData.PWM;
}

// Get the safety level that is currently being sent out.
uint16_t getSafetyLevel() {
	return txData.safetyLevel;
}

// Set the safety level that is currently being sent out.
void setSafetyLevel(uint16_t level) {
	txData.safetyLevel = level;
}

// return whether the data is new.
bool isDataNew() {
	return dataNew;
}

// Starts interchip interrupt. Call this once, not in a loop, each interchip callback
// sets up another interrupt.
void interchipInit() {
	HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)&txData,(uint8_t *)&rxData, sizeof(Interchip_Packet));
}

// used to populate the interchip packet with fake data.
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

// spi callback that starts up another interrupt.
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
	// Start Listening after the previous exchange.
	HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)&txData,(uint8_t *)&rxData, sizeof(Interchip_Packet));
	dataNew = true;
}
