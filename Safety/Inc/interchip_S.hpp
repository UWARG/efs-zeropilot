#include "stm32f0xx_hal.h"
#include "Interchip.h"
#include "spi.hpp"
//
int16_t Interchip_Init(Interchip_StoA_Packet*, Interchip_AtoS_Packet*);
int16_t Interchip_TxRx(SPI_HandleTypeDef *hspi);
//uint16_t getErrorCount();

int16_t *Interchip_GetPWM(void);
void Interchip_SetPWM(int16_t *data);
void Interchip_SetAutonomousLevel(uint16_t data);


