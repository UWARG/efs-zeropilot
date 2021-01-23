#include "Interchip_A.h"
#include "cmsis_os.h"
#include "spi.h"

static Interchip_AtoS_Packet *dataTX;
static Interchip_StoA_Packet *dataRX;
osMutexId Interchip_MutexHandle;

void Interchip_Run(void const *argument) {
  dataRX = malloc(sizeof(Interchip_StoA_Packet));
  dataTX = malloc(sizeof(Interchip_AtoS_Packet));

  osMutexDef(Interchip_Mutex);
  Interchip_MutexHandle = osMutexCreate(osMutex(Interchip_Mutex));

  while (1) {
    osMutexWait(Interchip_MutexHandle, 0);

    HAL_StatusTypeDef transmit_status = HAL_SPI_TransmitReceive_IT(
        &hspi1, (uint8_t *)dataTX, (uint8_t *)dataRX,
        sizeof(Interchip_StoA_Packet) / sizeof(uint16_t));

    osMutexRelease(Interchip_MutexHandle);

    if (transmit_status != HAL_OK) {
      Error_Handler();
    }
    osDelay(INTERCHIP_TRANSMIT_DELAY);
  }
}

// Public Functions to get and set data

int16_t *Interchip_GetPWM(void) 
{ 
  return dataRX->PWM; 
}


void Interchip_SetPWM(int16_t data[]) {
  osMutexWait(Interchip_MutexHandle, 0);
  for (uint8_t i = 0; i < 12; i++) {
    dataTX->PWM[i] = data[i];
  }
  osMutexRelease(Interchip_MutexHandle);
}


uint16_t Interchip_GetAutonomousLevel(void) { return dataRX->autonomous_level; }

void Interchip_SetAutonomousLevel(uint16_t data) {
  osMutexWait(Interchip_MutexHandle, 0);
  dataTX->autonomous_level = data;
  osMutexRelease(&Interchip_MutexHandle);
}