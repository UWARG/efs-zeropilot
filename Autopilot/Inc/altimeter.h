#ifndef ALTIMETER_H
#define ALTIMETER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"

HAL_StatusTypeDef MPL3115A2_Init();

float getAltitude();

float getTemperature();

#ifdef __cplusplus
}
#endif

#endif /* ALTIMETER_H */
