#include "stm32f7xx_hal.h"
#include "Interchip.h"

#define INTERCHIP_TRANSMIT_DELAY 5

int16_t *Interchip_GetPWM();
void Interchip_SetPWM(int16_t *data);
uint16_t Interchip_GetSafetyLevel();
void Interchip_SetAutonomousLevel(uint16_t data);