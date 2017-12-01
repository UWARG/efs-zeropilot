#include <stdlib.h>
#include "safety_control.h"
#include "interchip_comm_S.h"

#include "PWM.h"
#include "buzzer.h"
#include "debug.h"

#define SAFETY_CHANNEL 4

static Interchip_StoA_Packet* dataTX;
static Interchip_AtoS_Packet* dataRX;
static int16_t PPM[12];

uint16_t Safety_isManual(int16_t PWM_Value);

void Safety_Init(){
    
    PWM_Init();
    //Buzzer_init();

    dataTX = calloc(1,sizeof(Interchip_StoA_Packet));
    dataRX = calloc(1,sizeof(Interchip_AtoS_Packet));
    interchipInit(dataTX, dataRX);

}

void Safety_Run(){
    while(1){
        volatile int16_t *PPM_input = PPM_Get();
        for(uint8_t i=0; i<PPM_NUM_CHANNELS; i++){
            PPM[i] = PPM_input[i];
        }
        

        uint16_t isManual = Safety_isManual(PPM[SAFETY_CHANNEL]);

        //send to Autopilot
        //TODO: block spi during this?
        //dataTX->PWM = PPM;
        dataTX->safety_level = isManual;

        //if not manual, change values from input PPM to Autopilot PWM values
        if(!isManual){
            uint8_t i;
            uint16_t offset =1;
            for(i=0; i<PPM_NUM_CHANNELS;i++){
                if( (dataRX->autonomous_level & offset) !=0){
                    PPM[i] = dataRX->PWM[i];
                }
                offset = offset << 1;
            }
        }
        PWM_SetAll(PPM);
    }
}

uint16_t Safety_isManual(int16_t PWM_Value){
    return PWM_Value>0; //TODO improve
}