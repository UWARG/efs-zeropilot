#ifndef ALTIMETER_H
#define ALTIMETER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"

HAL_StatusTypeDef M5637_Init();


float getRawPressure();
float getAltitude();
float getRawTemperature();

#ifdef __cplusplus
}
#endif

#endif /* ALTIMETER_H */
