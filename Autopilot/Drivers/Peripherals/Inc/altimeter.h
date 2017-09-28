#ifndef ALTIMETER_H
#define ALTIMETER_H

#include "stm32f7xx_hal.h"
#include "i2c.h"

HAL_StatusTypeDef MPL3115A2_Init();

float getAltitude();

float getTemperature();

#endif
