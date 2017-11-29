#include "interchip_comm_A.h"
#include "interchip_comm.h"
#include "debug.h"
#include "spi.h"

Interchip_AtoS_Packet dataTX;
Interchip_StoA_Packet dataRX;

HAL_StatusTypeDef transmit_status;
int hello=0;
void interchipInit(){

    //put random values to test interchip
    for(int i=0; i<12; i++){
        dataTX.PWM[i] = i*2;
    }
    dataTX.autonomous_level = 27;
    debug("sizeof: %d", sizeof(Interchip_StoA_Packet)/sizeof(uint16_t));
    //hspi1.State = RESET;
    //HAL_SPI_Init(&hspi1);
    //HAL_SPI_Transmit(&hspi1, (uint8_t*)&dataTX,  sizeof(Interchip_StoA_Packet)/sizeof(uint16_t),100);

    
}

HAL_StatusTypeDef interchipUpdate(){

    //send data
    
    debug("status: %d, %d", (hspi1.State), HAL_SPI_Transmit_IT(&hspi1, (uint8_t*)&dataTX, /*(uint8_t*)&dataRX,*/ sizeof(Interchip_StoA_Packet)/sizeof(uint16_t)));
    //debug("%d",hello);
    dataTX.PWM[0]++;
    if(transmit_status == HAL_OK){
        /*printf("SPI Receive: ");
        int sum =0;
        for(int i =0; i<12; i++){
            sum+=dataRX.PWM[i]
            printf(", %d",dataRX.PWM[i]);
        }
        debug("\n%d\n", sum);*/
      }else{
           Error_Handler();
        }

    return transmit_status;
}

void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{ 
    hello++;
}