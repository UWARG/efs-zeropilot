#ifndef MPU9255_H
#define MPU9255_H

#include "stm32f7xx_hal.h"
#include "i2c.h"


typedef struct {
    float Ax, Ay, Az;
    float Gx, Gy, Gz;
    float Mx, My, Mz;

    float A_res, G_res, M_res;

    float Mx_adj, My_adj, Mx_adj;
    
} MPU9255_t;

HAL_StatusTypeDef MPU9255_Init(MPU9255_t* mpu);


#endif
