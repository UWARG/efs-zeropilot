#ifndef INTERCHIP_A_H
#define INTERCHIP_A_H

#include "stm32f7xx_hal.h"
#include "Interchip.h"
#include <stdint.h>

#define INTERCHIP_TRANSMIT_DELAY 5

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint16_t safetyLevel;
	int16_t* PWM;
} Interchip_Packet;   

void Interchip_Init();
void Interchip_Run();
int16_t *Interchip_GetPWM(void);
void Interchip_SetPWM(int16_t *data);
uint16_t Interchip_GetAutonomousLevel(void);
void Interchip_SetAutonomousLevel(uint16_t data);
void InterchipTxRxInterrupt();
uint8_t InterchipIsDataNew();

#ifdef __cplusplus
}
#endif

#endif
