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
  txData.PWM[0] = 0;
	txData.PWM[1] = 0;
	txData.PWM[2] = 0;
	txData.PWM[3] = 0;
	txData.PWM[4] = 0;
	txData.PWM[5] = 0;
	txData.PWM[6] = 0;
	txData.PWM[7] = 0;
	txData.PWM[8] = 0;
	txData.PWM[9] = 0;
	txData.PWM[10] = 0;
	txData.PWM[11] = 0;
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

// called during the SPI TxRx interrupt
void InterchipTxRxCallback() {
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
