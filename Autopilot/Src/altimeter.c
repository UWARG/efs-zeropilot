#include "altimeter.h"

#define MS5637_ADDR 0x76 //Address for the MS5637. Needs to be shifted for write/read (ref:datasheet)
#define MS5637_I2C I2C2 //Port... I think?
//registers
//Hex codes for polling, ref: Datasheet pg.9

#define MS5637_RESET_CMD = 0x1E
#define MS5637_START_PRESSURE_CONVERSION = 0x40
#define MS5637_START_TEMP_CONVERSION = 0x50
#define MS5637_READ_ADC = 0x00
#define MS5637_INTERN_MEM_ADDRESS = 0x00 //There isn't actually one. Datasheet and stack exchange confirms

static I2C_HandleTypeDef* hi2c;

 HAL_StatusTypeDef M5637_Init() {
     hi2c = I2C_GetHandle(MS5637_I2C);

     if (HAL_I2C_IsDeviceReady(hi2c, MS5637_ADDR, 2, 5) != HAL_OK) {
//         debug("no altimeter");
         // error
     }

     
     //I2C_WriteByte(hi2c, MS5637_ADDR, MS5637_INTERN_MEM_ADDRESS, MS5637_START_TEMP_CONVERSION);
     return HAL_OK;
}


 float getRawPressure() {
   //24 bit number for altitude 18 + 2 decimal places
   I2C_WriteByte(hi2c, MS5637_ADDR, MS5637_INTERN_MEM_ADDRESS, MS5637_START_PRESSURE_CONVERSION);
   uint32_t data;
   I2C_ReadBytes(hi2c, MS5637_ADDR, MS5637_READ_ADC, data, 3);
   return (data);
 }

 float getRawTemperature() {
   I2C_WriteByte(hi2c, MS5637_ADDR, MS5637_INTERN_MEM_ADDRESS, MS5637_START_TEMP_CONVERSION);
   uint32_t data;
   I2C_ReadBytes(hi2c, MS5637_ADDR, MS5637_READ_ADC, data, 3);
   return (data);

 }

 float getAdjustedPressure() {
   
 }