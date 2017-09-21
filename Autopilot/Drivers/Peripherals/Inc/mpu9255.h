#ifndef MPU9255_H
#define MPU9255_H

#include "stm32f7xx_hal.h"
#include "i2c.h"


typedef struct {
    float Ax, Ay, Az;
    float Gx, Gy, Gz;
    float Mx, My, Mz;



} MPU9255_t;

HAL_StatusTypeDef MPU9255_Init(MPU9255_t* mpu);


#endif
