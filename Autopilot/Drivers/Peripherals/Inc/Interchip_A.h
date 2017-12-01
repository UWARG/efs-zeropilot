#include "stm32f7xx_hal.h"
#include "Interchip.h"

void interchipInit(Interchip_AtoS_Packet*, Interchip_StoA_Packet*);
HAL_StatusTypeDef Interchip_Update();