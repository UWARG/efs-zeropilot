#include "interchip_S.h"
#include "debug.h"
#include <stdio.h>
#include "spi.h"


static Interchip_StoA_Packet* dataTX;
static Interchip_AtoS_Packet* dataRX;
static uint16_t errorCount=0;

void Interchip_Init(Interchip_StoA_Packet* ptrTX, Interchip_AtoS_Packet* ptrRX){
    dataTX = ptrTX;
    dataRX = ptrRX;

    //start TxRx
    HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));
}

uint16_t getErrorCount(){
  return errorCount;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){
  //Once packet recieved, listen for next packet
  if(hspi->Instance == SPI1){
    HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));
  }
  debug("%d", dataRX->PWM[0]);
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi){
  if(hspi->Instance == SPI1){
    HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));
    errorCount++;
  }
}