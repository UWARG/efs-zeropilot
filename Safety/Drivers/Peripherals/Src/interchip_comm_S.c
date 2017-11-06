#include "interchip_comm_S.h"
#include "interchip_comm.h"
#include "debug.h"
#include <stdio.h>
#include "spi.h"


Interchip_StoA_Packet dataTX;
Interchip_AtoS_Packet dataRX;

HAL_StatusTypeDef transmit_status;

void interchipInit(){

    //put random values to test interchip
    for(int i=0; i<12; i++){
        dataTX.PWM[i] = i*2;
    }
    dataTX.safety_level = 27;
    debug("sizeof: %d", sizeof(Interchip_StoA_Packet)/sizeof(uint16_t));
    
}

void pollInterchip(){
    if(HAL_SPI_Receive(&hspi1,(uint8_t *)&dataRX, sizeof(Interchip_AtoS_Packet)/sizeof(uint16_t),500) == HAL_OK){
        printf("SPI Receive: ");
        for(int i =0; i<12; i++){
            printf(", %d",dataRX.PWM[i]);
        }
        debug("\n");
        HAL_SPI_Transmit(&hspi1, (uint8_t *)&dataTX, sizeof(Interchip_StoA_Packet)/sizeof(uint16_t),500);
      }
}