#include "stm32f7xx_hal.h"
#include "Interchip.h"

#define INTERCHIP_TRANSMIT_DELAY 50

Interchip_StoA_Packet Interchip_GetData();
void Interchip_SetData(Interchip_AtoS_Packet data);