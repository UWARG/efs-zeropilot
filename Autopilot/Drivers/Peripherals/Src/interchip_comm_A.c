#include "interchip_comm_A.h"
#include "interchip_comm.h"
#include "debug.h"
#include "spi.h"

Interchip_AtoS_Packet dataTX;
Interchip_StoA_Packet dataRX;

HAL_StatusTypeDef transmit_status;

void interchipInit(){

    //put random values to test interchip
    for(int i=0; i<12; i++){
        dataTX.PWM[i] = i*2;
    }
    dataTX.autonomous_level = 27;
    debug("sizeof: %d", sizeof(Interchip_StoA_Packet)/sizeof(uint16_t));
    
}

HAL_StatusTypeDef interchipUpdate(){

    //send data
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&dataTX, (uint8_t*)&dataRX, sizeof(Interchip_StoA_Packet)/sizeof(uint16_t), 500);

    if(transmit_status == HAL_OK){
        //debug("TxRx %d, %d, %d, %d", dataRx[0], dataRx[1], dataRx[2],dataRx[3]);
      }
    return transmit_status;
}