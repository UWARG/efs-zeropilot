#ifndef INTERCHIP_A_H
#define INTERCHIP_A_H

#include "stm32f7xx_hal.h"
#include "Interchip.h"
#include <stdint.h>

#define INTERCHIP_TRANSMIT_DELAY 5

#ifdef __cplusplus
extern "C" {
#endif
 

void Interchip_Init();
void Interchip_Run();
void Interchip_SetPWM(int index,int data);
uint16_t Interchip_GetAutonomousLevel(void);
void Interchip_SetAutonomousLevel(uint16_t data);
void InterchipTxRxInterrupt();
uint8_t InterchipIsDataNew();

#ifdef __cplusplus
}
#endif

#endif
