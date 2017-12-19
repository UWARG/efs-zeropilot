#include "safety_control.h"
#include <stdbool.h>
#include "debug.h"
#include "iwdg.h"

static Interchip_StoA_Packet* dataTX;
static Interchip_AtoS_Packet* dataRX;
static int16_t PPM[PPM_NUM_CHANNELS];

bool Safety_isManual(int16_t PWM_Value);

void Safety_Init(){
    PWM_Init();

    dataTX = malloc(sizeof(Interchip_StoA_Packet));
    dataRX = malloc(sizeof(Interchip_AtoS_Packet));
    Interchip_Init(dataTX, dataRX);

}

void Safety_Run(){
    while(1){
         HAL_IWDG_Refresh(&hiwdg);

        volatile int16_t *PPM_input = PPM_Get();
        for(uint8_t i=0; i<PPM_NUM_CHANNELS; i++){
            PPM[i] = PPM_input[i];
        }


        bool isManual = Safety_isManual(PPM[SAFETY_CHANNEL]);

        //send to Autopilot
        for(uint8_t i=0; i<PPM_NUM_CHANNELS;i++){
            dataTX->PWM[i] = PPM[i];
        }
        dataTX->safety_level = isManual;

        //if not manual, change values from input PPM to Autopilot PWM values
        if(!isManual){
            uint8_t i;
            uint16_t offset =1;
            for(i=0; i<PPM_NUM_CHANNELS;i++){
                if( dataRX->autonomous_level & offset ){
                    PPM[i] = dataRX->PWM[i];
                    if(PPM[i]<PWM_MIN) PPM[i] = PWM_MIN;
                    if(PPM[i]>PWM_MAX) PPM[i] = PWM_MAX;
                }
                offset = offset << 1;
            }
        }
        PWM_SetAll(PPM);
    }
}

bool Safety_isManual(int16_t PWM_Value){
    return (PWM_Value>-1500 && PWM_Value>-1000);
}