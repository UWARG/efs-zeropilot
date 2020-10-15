/**
 * IMU Sensor
 * Author: Dhruv Rawat
 */

#include "IMU.hpp"
#include "spi.h"
#include "SPI.hpp"
#include "Status.hpp"
 
//Address for ICM20602
#define ICM20602_SPI hspi1 //SPI Port 1
 
/*** REGISTER DEFINITION BEGINS ***/

//Configuration
#define REG_CONFIG 0x1A
#define REG_GYRO_CONFIG 0x1B
#define REG_ACCEL_CONFIG 0x1C
#define REG_ACCEL_CONFIG_2 0x1D
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

#define REG_USER_CTRL 0x6A
//Power management
#define REG_PWR_MGMT_1 0x6B
#define REG_PWR_MGMT_2 0x6C
//For I2C management
#define REG_I2C_IF 0x70
//Device idenitifier
#define REG_WHO_AM_I 0x75

/*** REGISTER DEFINITION ENDS ***/
 
bool ICM20602::isSPIBusDefined = false;
bool ICM20602::dataIsNew = false;
 
static SPI_HandleTypeDef* hspi1;

float measuredAccX, measuredAccY, measuredAccZ; //Acceleration readings
float measuredGyroX, measuredGyroY, measuredGyroZ; //Gyroscope readings 
float measuredTemp; //Temperature reading

static SPIPort *spi_port;
static SPISettings hspi_1;
uint8_t *buffer;
StatusCode sensorSuccess;
 
/*** MAIN BLOCK OF CODE BEGINS ***/
 
ICM20602::ICM20602() {
   if(!isSPIBusDefined) {
      // Configures the SPI Settings
      hspi_1.port = SPI_PORT1;

      /*
         Data is latched on the rising edge of SPC
         Data should be transitioned on the falling edge of SPC

         From the above, we know that CPHA must be 0. 
         CPOL is 0.
      */

      hspi_1.mode = SPI_MODE_0;
      hspi_1.master = false;
      hspi_1.frequency = 10;                                           
      hspi_1.word_size = 2 ; 

      // Creates SPI Object
      spi_port = new SPIPort(hspi_1);

      uint8_t *setup = 0;

      //Full reset of chip 
      *setup = 0x80;
      *buffer = REG_PWR_MGMT_1 | 0x00; //0x80 means we are reading; 0x00 means we are writing
      spi_port->exchange_data(buffer, setup, 1);

      //Verify chip is working properly
      *buffer = REG_WHO_AM_I | 0x80;
      sensorSuccess = spi_port->exchange_data(buffer, setup, 1);

      if(sensorSuccess != STATUS_CODE_OK) {
         //Debug code here
      }

      //Place accelerometer and gyroscopoe on standby
      *setup = 0x3F;
      *buffer = REG_PWR_MGMT_2 | 0x00;
      spi_port->exchange_data(buffer, setup, 1);

      //Disable FIFO
      *setup = 0x00;
      *buffer = REG_USER_CTRL | 0x00;
      spi_port->exchange_data(buffer, setup, 1);

      //Disable I2C
      *setup = 0x40;
      *buffer = REG_I2C_IF | 0x00;
      spi_port->exchange_data(buffer, setup, 1);

      //Set up gyroscope
      *setup = 0x10; //Setting to 500 dps max for gyroscope; setting to 4G max for accelerometer
      *buffer = REG_GYRO_CONFIG | 0x00;
      spi_port->exchange_data(buffer, setup, 1);

      //set up accelerometer
      *buffer = REG_ACCEL_CONFIG | 0x00; 
      spi_port->exchange_data(buffer, setup, 1);

      *setup = 0x08;
      *buffer = REG_ACCEL_CONFIG_2 | 0x00;
      spi_port->exchange_data(buffer, setup, 1);

      //Turn on gyroscope and accelerometer
      *setup = 0x00;
      *buffer = REG_PWR_MGMT_2 | 0x00;
      spi_port->exchange_data(buffer, setup, 1);  

      delete setup;
   }

   accelConversionFactor = ACCEL_SENSITIVITY_4G;
   gyroConversionFactor = GRYO_SENSITIVITY_500;
   tempConversionFactor = TEMPERATURE_SENSITIVITY;
}

void ICM20602::get_accel_temp_gyro_reading(float *accx, float *accy, float *accz, float *gyrx, float *gyry, float *gyrz, float *temp) {
   uint8_t raw_data[14];
   //Store both High and Low Byte values
   int16_t shiftedSensorAccX, shiftedSensorAccY, shiftedSensorAccZ, shiftedSensorTemp, shiftedSensorGyroX, shiftedSensorGyroY, shiftedSensorGyroZ; 

   //Uses the sensor registers to get raw data for all sensors
   *buffer = REG_ACCEL_XOUT_H | 0x80; 
   sensorSuccess = spi_port->exchange_data(buffer, raw_data, 14); 
   
   shiftedSensorAccX = (raw_data[0] << 8) + raw_data[1];
   shiftedSensorAccY = (raw_data[2] << 8) + raw_data[3];
   shiftedSensorAccZ = (raw_data[4] << 8) + raw_data[5];

   shiftedSensorTemp = (raw_data[6] << 8) + raw_data[7];

   shiftedSensorGyroX = (raw_data[8] << 8) + raw_data[9];
   shiftedSensorGyroY = (raw_data[10] << 8) + raw_data[11];
   shiftedSensorGyroZ = (raw_data[12] << 8) + raw_data[13];


   //Converts 16-bit integers to a float. These are the actual measurements
   *accx = ((float) shiftedSensorAccX)/accelConversionFactor; 
   *accy = ((float) shiftedSensorAccY)/accelConversionFactor; 
   *accz = ((float) shiftedSensorAccZ)/accelConversionFactor; 

   *temp = ((float) shiftedSensorTemp)/tempConversionFactor;

   *gyrx = ((float) shiftedSensorGyroX)/gyroConversionFactor;
   *gyry = ((float) shiftedSensorGyroY)/gyroConversionFactor;
   *gyrz = ((float) shiftedSensorGyroZ)/gyroConversionFactor;
}

uint32_t ICM20602::get_current_time() {
   std::time_t currentTime = std::time(0);
   return static_cast<int>(currentTime);
}
 
void ICM20602::Begin_Measuring() {
   get_accel_temp_gyro_reading(&measuredAccX, &measuredAccY, &measuredAccZ, &measuredGyroX, &measuredGyroY, &measuredGyroZ, &measuredTemp);
   dataIsNew = true;
   timeOfResult = get_current_time();
}
 
void ICM20602::GetResult(IMUData_t *Data) {
   dataIsNew = false;
   if (sensorSuccess == STATUS_CODE_OK) {
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
      Data->sensorStatus = 0;
   } else { //In case data was not received. Stick with old data
      Data->isDataNew = false;
      Data->sensorStatus = -1; 
   }
  
}
 
/* MAIN BLOCK OF CODE ENDS */
