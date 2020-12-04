#include "stm32f0xx_hal.h"
#include "Interchip.h"
#include "spi.hpp"

#ifndef INTERCHIP_S_HPP
#define INTERCHIP_S_HPP

int16_t Interchip_Init(Interchip_StoA_Packet*, Interchip_AtoS_Packet*);
int16_t Interchip_TxRx(SPI_HandleTypeDef *hspi);

int16_t *Interchip_GetPWM(void);
void Interchip_SetAutonomousLevel(uint16_t data);

#endif
