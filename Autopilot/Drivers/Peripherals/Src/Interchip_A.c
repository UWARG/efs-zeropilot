#include "Interchip_A.h"
#include "debug.h"
#include "spi.h"
#include "freertos.h"

static Interchip_AtoS_Packet *dataTX;
static Interchip_StoA_Packet *dataRX;

void Interchip_Run(){

  dataRX = malloc(sizeof(Interchip_StoA_Packet));
  dataTX = malloc(sizeof(Interchip_AtoS_Packet));

  while(1){
    HAL_StatusTypeDef transmit_status = HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t*)dataTX, (uint8_t*)dataRX, sizeof(Interchip_StoA_Packet)/sizeof(uint16_t));
    if(transmit_status != HAL_OK){
      Error_Handler();
    }
    osDelay(INTERCHIP_TRANSMIT_DELAY);
  }
}

//Public Functions to get and set data

Interchip_StoA_Packet Interchip_GetData(){
  return *dataRX;
}

void Interchip_SetData(Interchip_AtoS_Packet data){
  *dataTX = data;
}