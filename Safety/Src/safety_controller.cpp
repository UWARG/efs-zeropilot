#include "PWM.hpp"
#include "Interchip.h"
#include "stdlib.h"


static Interchip_AtoS_Packet *dataRX;
static Interchip_StoA_Packet *dataTX;

PWMManager &manager = PWMManager::getInstance();

void safety_controller_init()
{
    dataTX = (Interchip_StoA_Packet*) malloc(sizeof(Interchip_StoA_Packet));
    dataRX = (Interchip_AtoS_Packet*) malloc(sizeof(Interchip_AtoS_Packet));
}