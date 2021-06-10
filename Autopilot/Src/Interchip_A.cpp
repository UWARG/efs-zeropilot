#include "Interchip_A.h"
#include "cmsis_os.h"
#include "spi.h"

static volatile Interchip_Packet rxData;
static volatile Interchip_Packet txData;
static volatile bool dataNew;

// freertos run function
void Interchip_Run() {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_StatusTypeDef result = HAL_SPI_TransmitReceive_IT(&hspi6,(uint8_t *)&txData,(uint8_t *)&rxData, 26);
}

void Interchip_Init() {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  // the rest of this program is just for like,
  txData.safetyLevel = 14;
  txData.PWM[0] = 15;
	txData.PWM[1] = 16;
	txData.PWM[2] = 17;
	txData.PWM[3] = 18;
	txData.PWM[4] = 19;
	txData.PWM[5] = 20;
	txData.PWM[6] = 21;
	txData.PWM[7] = 22;
	txData.PWM[8] = 23;
	txData.PWM[9] = 24;
	txData.PWM[10] = 25;
	txData.PWM[11] = 26;
}


// Sets a PWM channel
void Interchip_SetPWM(int index,int data) {
	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
	txData.PWM[index] = (int16_t) data;
}

// returns the safetyLevel
uint16_t Interchip_GetAutonomousLevel(void) {
  dataNew = false;
  return rxData.safetyLevel; 
}

// set the safetyLevel
void Interchip_SetAutonomousLevel(uint16_t data) {
  //this shouldnt exist but it was defined so I'll support it
  rxData.safetyLevel = data;
}

// called during the SPI TxRx interrupt
void InterchipTxRxInterrupt() {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	dataNew = true;
}

// returns the dataNew variables state
uint8_t InterchipIsDataNew() {
	if (dataNew) {
		return 1;
	} 
	return 0;
}