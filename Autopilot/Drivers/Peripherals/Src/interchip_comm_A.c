#include "interchip_comm_A.h"
#include "debug.h"
#include "spi.h"

static Interchip_AtoS_Packet *dataTX;
static Interchip_StoA_Packet *dataRX;

void interchipInit(Interchip_AtoS_Packet* ptrTX, Interchip_StoA_Packet* ptrRX){
    dataTX = ptrTX;
    dataRX = ptrRX;
}

HAL_StatusTypeDef interchipUpdate(){
    //send data
    HAL_StatusTypeDef transmit_status = HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t*)dataTX, (uint8_t*)dataRX, sizeof(Interchip_StoA_Packet)/sizeof(uint16_t));

    if(transmit_status != HAL_OK){
           Error_Handler();
    }

    return transmit_status;
}