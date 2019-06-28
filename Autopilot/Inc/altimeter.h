#include "i2c.h"


#ifndef ALTIMETER_H
#define ALTIMETER_H

#ifdef __cplusplus
extern "C" {
#endif

HAL_StatusTypeDef MPL3115A2_Init(void);

float getAltitude(void);

float getTemperature(void);

#ifdef __cplusplus
}
#endif

#endif /* ALTIMETER_H */
