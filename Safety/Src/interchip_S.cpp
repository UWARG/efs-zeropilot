#if 0

/*
#include "interchip_S.h"
//#include "debug.h"
#include <stdio.h>
#include "spi.c"


static Interchip_StoA_Packet* dataTX;
static Interchip_AtoS_Packet* dataRX;
static uint16_t errorCount=0;
//

uint16_t getErrorCount(){
  return errorCount;
}

int16_t HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){
  //Once packet recieved, listen for next packet
  if(hspi->Instance == SPI1){
    HAL_StatusTypeDef transmit_status = HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));
    if (transmit_status != HAL_OK) {
        return 1;
    }
    else
    {
        return 0;
    }
  }
  return 1;
  //debug("%d", dataRX->PWM[0]);
}

I have no clue what this function is for - Aadi
/* //I don't think this code is required
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi){
  if(hspi->Instance == SPI1){
    HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));
    errorCount++;
  }
}
*/

#include "interchip_S.hpp"
#include <stdio.h>

static Interchip_AtoS_Packet *dataRX;
static Interchip_StoA_Packet *dataTX;

void Interchip_SetAutonomousLevel(uint16_t data) {
  dataTX->autonomous_level = data;
}
int16_t *Interchip_GetPWM(void)
{
  return dataRX->PWM;
}

int16_t Interchip_Init(Interchip_StoA_Packet* ptrTX, Interchip_AtoS_Packet* ptrRX){
    dataTX = ptrTX;
    dataRX = ptrRX;
    //start TxRx
    HAL_StatusTypeDef transmit_status = HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));

    if (transmit_status != HAL_OK) {
        return 1;
    }
    else
    {
        return 0;
    }
}

//so this function was called HAL_SPI_TxRxCpltCallback, but that was interefering with another function called in the HAL library code.
//the HAL version was void, but this one was int16_t, so I renamed it rather than change the return type
int16_t Interchip_TxRx(SPI_HandleTypeDef *hspi){
  //Once packet recieved, listen for next packet
  if(hspi->Instance == SPI1){
    HAL_StatusTypeDef transmit_status = HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));
    if (transmit_status != HAL_OK) {
        return 1;
    }
    else
    {
        return 0;
    }
  }
  return 1;
  //debug("%d", dataRX->PWM[0]);
}

#endif

