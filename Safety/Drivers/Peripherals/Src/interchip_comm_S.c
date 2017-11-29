#include "interchip_comm_S.h"
#include "debug.h"
#include <stdio.h>
#include "spi.h"


Interchip_StoA_Packet* dataTX;
Interchip_AtoS_Packet* dataRX;

void interchipInit(Interchip_StoA_Packet* ptrTX, Interchip_AtoS_Packet* ptrRX){
    dataTX = ptrTX;
    dataRX = ptrRX;
    //start TxRx
    HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  //Once packet recieved, listen for next packet
  if(hspi->Instance == SPI1){
    HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));
  }

}