#ifndef ALTIMETER_H
#define ALTIMETER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"

HAL_StatusTypeDef MS5637_Init();

/*
* Returns pressure in mbar
*/
float getPressure();

/*
* Returns temp in degrees C
*/
float getTemperature();

/*
* Returns altitude in meters
*/
float getAltitude();

#ifdef __cplusplus
}
#endif

#endif /* ALTIMETER_H */
