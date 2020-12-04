#include "safety_controller.hpp"
#include "PWM.hpp"
#include "Interchip.h"
#include "interchip_S.hpp"
#include "stdlib.h"
#include "stm32f0xx_hal_iwdg.h"



static Interchip_AtoS_Packet *dataRX;
static Interchip_StoA_Packet *dataTX;

PWMManager &manager = PWMManager::getInstance();

void safety_controller_init()
{
    
    dataTX = (Interchip_StoA_Packet*) malloc(sizeof(Interchip_StoA_Packet));
    dataRX = (Interchip_AtoS_Packet*) malloc(sizeof(Interchip_AtoS_Packet));
    Interchip_Init(dataTX, dataRX);
}

void safety_run(IWDG_HandleTypeDef &hiwdg, PPMChannel &ppm)
{
    HAL_IWDG_Refresh(&hiwdg);
    if(isSafetyManual())
    {
        for(int channel = 0; channel < 12; channel++)
        {
            setPWMChannel(manager, channel, dataRX->PWM[channel]);
        }   
    }
    else
    {
        for(int channel = 0; channel < 12; channel++)
        {
            setPWMChannel(manager, channel, getPPM(ppm, channel));
        }
    }
    
}


/*
* Sets an individual PWM channel to the desired output.
*/
void setPWMChannel(PWMManager &manager, int channel, int percentage)
{
    //Input protection
    if(percentage > 100) {percentage = 100;}
    if(percentage < 0) {percentage = 0;}
    manager.channel(channel).set(percentage);
}

bool isSafetyManual()
{
    /*
    *   TODO: ADD SYSTEM TO MAKE SAFETY MANUAL!
    */
    //Not sure what the implementation is for now...?
    return false;
}

int getPPM(PPMChannel &ppm, int channel)
{
    return ppm.get(channel);
}
