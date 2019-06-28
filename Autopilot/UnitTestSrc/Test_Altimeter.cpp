#include <gtest/gtest.h>
#include "fff.h"
#include "i2c.h"
#include "altimeter.h"

/***********************************************************************************************************************
 * Mocks
 **********************************************************************************************************************/


FAKE_VOID_FUNC(MX_I2C1_Init);
FAKE_VOID_FUNC(MX_I2C2_Init);
FAKE_VOID_FUNC(MX_I2C4_Init);
FAKE_VALUE_FUNC(I2C_HandleTypeDef*, I2C_GetHandle, I2C_TypeDef*);
FAKE_VOID_FUNC(I2C_WriteByte, I2C_HandleTypeDef *, uint16_t, uint8_t, uint8_t);
FAKE_VOID_FUNC(I2C_WriteBytes, I2C_HandleTypeDef *, uint16_t, uint8_t, uint8_t *, uint16_t);
FAKE_VALUE_FUNC(uint8_t, I2C_ReadByte, I2C_HandleTypeDef *, uint16_t, uint8_t);
FAKE_VOID_FUNC(I2C_ReadBytes, I2C_HandleTypeDef *, uint16_t, uint8_t, uint8_t *, uint16_t);
FAKE_VALUE_FUNC(HAL_StatusTypeDef, HAL_I2C_IsDeviceReady, I2C_HandleTypeDef *, uint16_t, uint32_t, uint32_t);

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

I2C_HandleTypeDef hi2c2; // this var is global in i2c.c and extern in i2c.h. Since the mocks have no knoweledge of the .c file, it must be defined here for test purposes.extern I2C_HandleTypeDef hi2c2;

