#include "altimeter.h"
#include "debug.h"

#define MPL3115A2_ADDR 0x60 << 1
#define MPL3115A2_I2C I2C2
//registers
#define STATUS 0x00
#define PRESSURE_OUT_MSB 0x01
#define PRESSURE_OUT_CSB 0x02
#define PRESSURE_OUT_LSB 0x03
#define TEMP_OUT_MSB 0x04
#define TEMP_OUT_LSB 0x05
#define WHO_AM_I_MPL3115A2 0x0C
#define PT_DATA_CFG 0x13
#define CTRL_REG1 0x26

static I2C_HandleTypeDef* hi2c;

 HAL_StatusTypeDef MPL3115A2_Init() {
     hi2c = I2C_GetHandle(MPL3115A2_I2C);

     if (HAL_I2C_IsDeviceReady(hi2c, MPL3115A2_ADDR, 2, 5) != HAL_OK) {
         debug("no altimeter");
         // error
     }

     // who am i
     if (I2C_ReadByte(hi2c, MPL3115A2_ADDR, WHO_AM_I_MPL3115A2) != 0xC4) {
         debug("altimeter who am i failed");
         // error
     }

     //initializing altimeter (see flowchart in documentation for better description)
     I2C_WriteByte(hi2c, MPL3115A2_ADDR, CTRL_REG1, 0xB8);
     I2C_WriteByte(hi2c, MPL3115A2_ADDR, PT_DATA_CFG, 0x07);
     I2C_WriteByte(hi2c, MPL3115A2_ADDR, CTRL_REG1, 0xB9); //enable

     return HAL_OK;
}


 float getAltitude(){
   //20 bit number for altitude 18 + 2 decimal places
   uint8_t data[3];
   I2C_ReadBytes(hi2c, MPL3115A2_ADDR, PRESSURE_OUT_MSB, data, 3);
   int16_t msb = data[0] << 8;
   msb |= data[1];
   //msb *= 4;
   float lsb = data[2] >> 4;
   lsb /= 4;
   return ((float)msb + lsb);
 }

 float getTemperature(){
   //msb is temp in degrees C and lsb is fractional part
   uint8_t data[2];
   I2C_ReadBytes(hi2c, MPL3115A2_ADDR, TEMP_OUT_MSB, data, 2);
   uint8_t temp = data[0];
   float tempDec = data[1] >> 4;
   tempDec /= 16;
   return((float)temp + tempDec);
 }
