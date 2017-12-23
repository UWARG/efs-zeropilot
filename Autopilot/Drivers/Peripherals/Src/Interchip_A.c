#include "Interchip_A.h"
#include "Common.h"
#include "debug.h"
#include "spi.h"
#include "cmsis_os.h"

static Interchip_AtoS_Packet *dataTX;
static Interchip_StoA_Packet *dataRX;
osMutexId Interchip_MutexHandle;

void Interchip_Run(void const * argument) {
  dataRX = malloc(sizeof(Interchip_StoA_Packet));
  dataTX = malloc(sizeof(Interchip_AtoS_Packet));

  osMutexDef(Interchip_Mutex);
  Interchip_MutexHandle = osMutexCreate(osMutex(Interchip_Mutex));

  while (1) {
    osMutexWait(&Interchip_MutexHandle, INTERCHIP_TRANSMIT_DELAY);

    HAL_StatusTypeDef transmit_status = HAL_SPI_TransmitReceive_IT(
        &hspi1, (uint8_t *)dataTX, (uint8_t *)dataRX,
        sizeof(Interchip_StoA_Packet) / sizeof(uint16_t));

    osMutexRelease(&Interchip_MutexHandle);

    if (transmit_status != HAL_OK) {
      Error_Handler();
    }
    osDelay(INTERCHIP_TRANSMIT_DELAY);
  }
}

// Public Functions to get and set data

Interchip_StoA_Packet Interchip_GetData() { return *dataRX; }

void Interchip_SetData(Interchip_AtoS_Packet data) {
  osMutexWait(&Interchip_MutexHandle, 2*INTERCHIP_TRANSMIT_DELAY);
  *dataTX = data;
  osMutexRelease(&Interchip_MutexHandle);
}

int16_t *Interchip_GetPWM(){
  return dataRX->PWM;
}
void Interchip_SetPWM(int16_t *data){
  for(uint8_t i=0; i<PWM_NUM_CHANNELS;i++){
    dataTX->PWM[i] = data[i];
  }
}
uint16_t Interchip_GetSafetyLevel(){
  return dataRX->safety_level;
}
void Interchip_SetAutonomousLevel(uint16_t data){
  dataTX->autonomous_level = data;
}