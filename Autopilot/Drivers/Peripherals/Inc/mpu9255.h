#ifndef MPU9255_H
#define MPU9255_H

#include "stm32f7xx_hal.h"
#include "i2c.h"


typedef struct {
    float Ax, Ay, Az; // Units: g
    float Gx, Gy, Gz; // Units: rad/s
    float Mx, My, Mz; // units: uT

    float A_res, G_res, M_res;
    float Mx_adj, My_adj, Mz_adj;

    int16_t Ax_raw, Ay_raw, Az_raw;
    int16_t Gx_raw, Gy_raw, Gz_raw;
    int16_t Mx_raw, My_raw, Mz_raw;
} MPU9255_t;

HAL_StatusTypeDef MPU9255_Init(MPU9255_t* mpu);

void MPU9255_ReadAccel(MPU9255_t* mpu);
void MPU9255_ReadGyro(MPU9255_t* mpu);
void MPU9255_ReadMag(MPU9255_t* mpu);

int16_t MPU9255_ReadTemp(MPU9255_t* mpu);

#endif
