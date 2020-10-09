/**
 * IMU Sensor
 * Author: Dhruv Rawat
 */

#include "IMU.hpp"
#include "spi.h"
 
//Address for ICM20602 (PLACEHOLDERS FOR NOW)
#define ICM20602_ADDR 0
#define ICM20602_WRITE_ADDR 0
#define ICM20602_READ_ADDR 0
#define ICM20602_SPI hspi1 //SPI Port 1
 
/*** REGISTER DEFINITION BEGINS ***/
 
// Gyroscope low noise to low power offset shift and gyroscope offset temperature compensation (TC) register
#define REG_XG_OFFS_TC_H 0x04
#define REG_XG_OFFS_TC_L 0x05
#define REG_YG_OFFS_TC_H 0x07
#define REG_YG_OFFS_TC_L 0x08
#define REG_ZG_OFFS_TC_H 0x0A
#define REG_ZG_OFFS_TC_L 0x0B
//Accelerometer self-test registers
#define REG_SELF_TEST_X_ACCEL 0x0D
#define REG_SELF_TEST_Y_ACCEL 0x0E
#define REG_SELF_TEST_Z_ACCEL 0x0F
//Axis specific gyroscope offset adjustment registers (Last character: H = high; L = low)
#define REG_XG_OFFS_USRH 0x13
#define REG_XG_OFFS_USRL 0x14
#define REG_YG_OFFS_USRH 0x15
#define REG_YG_OFFS_USRL 0x16
#define REG_ZG_OFFS_USRH 0x17
#define REG_ZG_OFFS_USRL 0x18
//Sample rate divider
#define REG_SMPLRT_DIV 0x19
//Configuration
#define REG_CONFIG 0x1A
#define REG_GYRO_CONFIG 0x1B
#define REG_ACCEL_CONFIG 0x1C
#define REG_ACCEL_CONFIG_2 0x1D
#define REG_LP_MODE_CFG 0x1E //Low power mode
//Wake-on motion threshold for accelerometer (by axis)
#define REG_ACCEL_WOM_X_THR 0x20
#define REG_ACCEL_WOM_Y_THR 0x21
#define REG_ACCEL_WOM_Z_THR 0x22
//FIFO (First in, first out) reisters
#define REG_FIFO_EN 0x23  //Enable
//Inturrupt statusses
#define REG_FSYNC_INT 0x36
#define REG_INT_PIN_CFG 0x37 //INT/DRDY Pin/ Bypass enable config
#define REG_INT_ENABLE 0x38
#define REG_FIFO_WM_INT_STATUS 0x39
#define REG_INT_STATUS 0x3A
//Accelerometer measurements (Last character: H = high; L = low)
#define REG_ACCEL_XOUT_H 0x3B
#define REG_ACCEL_XOUT_L 0x3C
#define REG_ACCEL_YOUT_H 0x3D
#define REG_ACCEL_YOUT_L 0x3E
#define REG_ACCEL_ZOUT_H 0x3F
#define REG_ACCEL_ZOUT_L 0x40
//Temperature measurements (Last character: H = high; L = low)
#define REG_TEMP_OUT_H 0x41
#define REG_TEMP_OUT_L 0x42
//Gryroscope measurements (Last character: H = high; L = low)
#define REG_GYRO_XOUT_H 0x43
#define REG_GYRO_XOUT_L 0x44
#define REG_GYRO_YOUT_H 0x45
#define REG_GYRO_YOUT_L 0x46
#define REG_GYRO_ZOUT_H 0x47
#define REG_GYRO_ZOUT_L 0x48
//Gyroscope self-test registers
#define REG_SELF_TEST_X_GYRO 0x50
#define REG_SELF_TEST_Y_GYRO 0x51
#define REG_SELF_TEST_Z_GYRO 0x52
//FIFO watermark threshold
#define REG_FIFO_WM_TH1 0x60
#define REG_FIFO_WM_TH2 0x61
 
#define REG_SIGNAL_PATH_RESET 0x68
#define REG_ACCEL_INTEL_CTRL 0x69 //accelerometer intelligence control
#define REG_USER_CTRL 0x6A
//Power management
#define REG_PWR_MGMT_1 0x6B
#define REG_PWR_MGMT_2 0x6C
//FIFO count registers (Last character: H = high; L = low)
#define REG_FIFO_COUNTH 0x72
#define REG_FIFO_COUNTL 0x73
//FIFO read write
#define REG_FIFO_R_W 0x74
//Device idenitifier
#define REG_WHO_AM_I 0x75
#define REG_WHO_AM_I_CONST 0X12
//Accelerometer offset registers
#define REG_XA_OFFSET_H 0x77
#define REG_XA_OFFSET_L 0x78
#define REG_YA_OFFSET_H 0x7A
#define REG_YA_OFFSET_L 0x7B
#define REG_ZA_OFFSET_H 0x7D
#define REG_ZA_OFFSET_L 0x7E
 
/*** REGISTER DEFINITION ENDS ***/
 
bool ICM20602::isSPIBusDefined = false;
bool ICM20602::dataIsNew = false;
 
static SPI_HandleTypeDef* hspi1;

float measuredAccX, measuredAccY, measuredAccZ; //Acceleration readings
float measuredGyroX, measuredGyroY, measuredGyroZ; //Gyroscope readings 
float measuredTemp;
 
/*** MAIN BLOCK OF CODE BEGINS ***/
 
ICM20602::ICM20602() {
   if(!isSPIBusDefined) {
 
   }

   //Default sensitivities. SHOULD BE ABLE TO CHANGE, BUT IDK IF THAT IS A HARDWARE OR A SOFTWARE THING
   accelSensitivity = 2;
   gyroSensitivity = 2;
}

//Gives conversion factor to convert raw values to g-force (denoted by: G)
float ICM20602::getAccelerationSensitivity() { 
   
   //Will need to change values later
   
   if(accelSensitivity == ACCEL_SENSITIVITY_2G) {
      return 10.0;
   } else if(accelSensitivity == ACCEL_SENSITIVITY_4G) {
      return 10.0;
   } else if(accelSensitivity == ACCEL_SENSITIVITY_8G) {
      return 10.0;
   } else if(accelSensitivity == ACCEL_SENSITIVITY_16G) { 
      return 10.0;
   }

   return -1.0; //If parameter value is incorrect
}

//Gives conversion factor to convert raw values to degrees per second (denoted by: dps)
float ICM20602::getGyroscopeSensitivity() {
   
   //Will need to change values later
   
   if(gyroSensitivity == GRYO_SENSITIVITY_250) {
      return 10.0;
   } else if(gyroSensitivity == GRYO_SENSITIVITY_500) {
      return 10.0;
   } else if(gyroSensitivity == GRYO_SENSITIVITY_1000) {
      return 10.0;
   } else if(gyroSensitivity == GRYO_SENSITIVITY_2000) {
      return 10.0;
   }

   return -1.0; //If parameter value is incorrect
}

void ICM20602::getAccelerationReading() {
   
}

void ICM20602::getGyroscopeReading() {

}

void ICM20602::getTempReading() {

}

uint8_t ICM20602::readRawAcceleration() {

}

uint8_t ICM20602::readRawGyroscope() {

}

uint8_t ICM20602::readTempRaw() {

}

//Sets sensitivity of accelerometer
void ICM20602::setAccelSensitivity(uint8_t desiredSensitivity) {
   accelSensitivity = desiredSensitivity; 
}
//Sets sensitivity of gyroscope
void ICM20602::setGyroSensitivity(uint8_t desiredSensitivity) {
   gyroSensitivity = desiredSensitivity; 
}

uint32_t ICM20602::getCurrentTime() {
   std::time_t currentTime = std::time(0);
   return static_cast<int>(currentTime);
}
 
void ICM20602::Begin_Measuring() {
   dataIsNew = true;
   timeOfResult = getCurrentTime();
}
 
void ICM20602::GetResult(IMUData_t *Data) {
   Data->isDataNew = true;
   dataIsNew = false;
   Data->accx = measuredAccX;
   Data->accy = measuredAccY;
   Data->accz = measuredAccZ;
   Data->gyrx = measuredGyroX;
   Data->gyry = measuredGyroY;
   Data->gyrz = measuredGyroZ;
   Data->magx = NAN;
   Data->magy = NAN;
   Data->magz = NAN;
   Data->utcTime = timeOfResult;
   Data->temp = measuredTemp;
}
 
/* MAIN BLOCK OF CODE ENDS */
