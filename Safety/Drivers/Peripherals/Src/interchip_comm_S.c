#include "interchip_comm_S.h"
#include "debug.h"
#include <stdio.h>
#include "spi.h"


Interchip_StoA_Packet* dataTX;
Interchip_AtoS_Packet* dataRX;

HAL_StatusTypeDef transmit_status;

void interchipInit(Interchip_StoA_Packet* ptrTX, Interchip_AtoS_Packet* ptrRX){

    dataTX = ptrTX;
    dataRX = ptrRX;

    //put random values to test interchip
    for(int i=0; i<12; i++){
        dataTX->PWM[i] = i*2;
    }
    dataTX->safety_level = 27;
    HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  printf("SPI Receive: \n");
  HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)dataTX,(uint8_t *)dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t));
}