#include "PWM.hpp"
#include "Interchip.h"
#include "interchip_S.c"
#include "stdlib.h"
#include "stm32f0xx_hal_iwdg.h"


static Interchip_AtoS_Packet *dataRX;
static Interchip_StoA_Packet *dataTX;

PWMManager &manager = PWMManager::getInstance();

void safety_controller_init()
{
    
    dataTX = (Interchip_StoA_Packet*) malloc(sizeof(Interchip_StoA_Packet));
    dataRX = (Interchip_AtoS_Packet*) malloc(sizeof(Interchip_AtoS_Packet));
    //Interchip_Init(dataTX, dataRX);
}

void safety_run(IWDG_HandleTypeDef &hiwdg)
{
    HAL_IWDG_Refresh(&hiwdg);
}

void setPWMChannel(PWMManager &manager, int channel, int percentage)
{
    manager.channel(channel).set(percentage);
}
