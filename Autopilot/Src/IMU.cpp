/**
 * IMU Sensor
 * Author: Dhruv Rawat
 */

#include "IMU.hpp"
#include "spi.h"
#include "SPI.hpp"
#include "Status.hpp"
 
//Address for ICM20602 (PLACEHOLDERS FOR NOW)
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
//For I2C management
#define REG_I2C_IF 0x70
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

static SPIPort *spi_port;
static SPISettings hspi_1;
uint8_t *buffer;
 
/*** MAIN BLOCK OF CODE BEGINS ***/
 
ICM20602::ICM20602() {
   if(!isSPIBusDefined) {
      // Configures the SPI Settings
      hspi_1.port = SPI_PORT1;
      hspi_1.mode = SPI_MODE_0;                        // SET LATER
      hspi_1.master = false;
      hspi_1.frequency = 10;                           // SET LATER
      hspi_1.word_size = 2 ; 

      // Creates SPI Object
      spi_port = new SPIPort(hspi_1);

      uint8_t *setup = 0;

      //Full reset of chip (0x80 means we are reading; 0x00 means we are writing)
      *setup = 0x80;
      *buffer = REG_PWR_MGMT_1 | 0x00; 
      send_dummy_byte(buffer);
      spi_port->exchange_data(buffer, setup, 1);

      //Verify chip is working properly
      *buffer = REG_WHO_AM_I | 0x80;
      send_dummy_byte(buffer);
      spi_port->exchange_data(buffer, setup, 1);

      //Place accelerometer and gyroscopoe on standby
      *setup = 0x3F;
      *buffer = REG_PWR_MGMT_2 | 0x00;
      send_dummy_byte(buffer);
      spi_port->exchange_data(buffer, setup, 1);

      //Disable FIFO
      *setup = 0x00;
      *buffer = REG_USER_CTRL | 0x00;
      send_dummy_byte(buffer);
      spi_port->exchange_data(buffer, setup, 1);

      //Disable I2C
      *setup = 0x40;
      *buffer = REG_I2C_IF | 0x00;
      send_dummy_byte(buffer);
      spi_port->exchange_data(buffer, setup, 1);

      //Set up gyroscope
      *setup = 0x10; // setting to 500 dps max (For accelerometer it is setting it to 4G max)
      *buffer = REG_GYRO_CONFIG | 0x00;
      send_dummy_byte(buffer);
      spi_port->exchange_data(buffer, setup, 1);

      //set up  accelerometer
      *buffer = REG_ACCEL_CONFIG | 0x00; 
      send_dummy_byte(buffer);
      spi_port->exchange_data(buffer, setup, 1);

      *setup = (1 << 3);
      *buffer = REG_ACCEL_CONFIG_2 | 0x00;
      send_dummy_byte(buffer);
      spi_port->exchange_data(buffer, setup, 1);

      //Turn on gyroscope and accelerometer
      *setup = 0x00;
      *buffer = REG_PWR_MGMT_2 | 0x00;
      send_dummy_byte(buffer);
      spi_port->exchange_data(buffer, setup, 1);  

      delete setup;
   }

   accelConversionFactor = ACCEL_SENSITIVITY_4G;
   gyroConversionFactor = GRYO_SENSITIVITY_500;
   tempConversionFactor = 326.8;                            //VERIFY
}

//Sends dummy byte to SPI register
void ICM20602::send_dummy_byte(uint8_t *reg) { 
   uint8_t * dummyByte = 0x00;
   spi_port->exchange_data(reg, dummyByte, 1);

   // ADD CODE THAT ENSURES THE MESSAGE WAS RECEIVED. IF NOT, RESEND THE DUMMYBYTE 

}

void ICM20602::get_acceleration_reading(float *accx, float *accy, float *accz) { 

   uint8_t *sensorAccX, *sensorAccY, *sensorAccZ; //Store the raw sensor values
   int16_t shiftedSensorAccX, shiftedSensorAccY, shiftedSensorAccZ; //Store the converted raw sensor values

   //Uses the sensor registers to get raw data
   *buffer = REG_ACCEL_XOUT_H | 0x80; 
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorAccX, 1); //Get high-byte value 
   shiftedSensorAccX = *sensorAccX << 8;

   *buffer = REG_ACCEL_XOUT_L | 0x80; 
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorAccX, 1); //Get low-btye value
   shiftedSensorAccX = shiftedSensorAccX | *sensorAccX;  


   *buffer = REG_ACCEL_YOUT_H | 0x80;
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorAccY, 1);
   shiftedSensorAccY = *sensorAccY << 8;

   *buffer = REG_ACCEL_YOUT_L | 0x80;
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorAccY, 1); 
   shiftedSensorAccY = shiftedSensorAccY | *sensorAccY;  


   *buffer = REG_ACCEL_ZOUT_H | 0x80;
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorAccZ, 1); 
   shiftedSensorAccZ = *sensorAccZ << 8;

   *buffer = REG_ACCEL_ZOUT_L | 0x80;
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorAccZ, 1); 
   shiftedSensorAccZ = shiftedSensorAccZ | *sensorAccZ;  

   //Converts 16-bit integer to a float (this is the actual measurement in Gs)
   *accx = ((float) shiftedSensorAccX)/accelConversionFactor; 
   *accy = ((float) shiftedSensorAccY)/accelConversionFactor; 
   *accz = ((float) shiftedSensorAccZ)/accelConversionFactor; 

   delete sensorAccX, sensorAccY, sensorAccZ;
}

void ICM20602::get_gyroscope_reading(float *gyrx, float *gyry, float *gyrz) {
   uint8_t *sensorGyroX, *sensorGyroY, *sensorGyroZ; //Store the raw sensor values
   int16_t shiftedSensorGyroX, shiftedSensorGyroY, shiftedSensorGyroZ; //Store the converted raw sensor values

   //Uses the sensor registers to get raw data
   *buffer = REG_GYRO_XOUT_H | 0x80; 
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorGyroX, 1); //Get high-byte value 
   shiftedSensorGyroX = *sensorGyroX << 8;

   *buffer = REG_GYRO_XOUT_L | 0x80; 
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorGyroX, 1); //Get low-btye value
   shiftedSensorGyroX = shiftedSensorGyroX | *sensorGyroX;  


   *buffer = REG_GYRO_YOUT_H | 0x80;
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorGyroY, 1);
   shiftedSensorGyroY = *sensorGyroY << 8;

   *buffer = REG_GYRO_YOUT_L | 0x80;
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorGyroY, 1); 
   shiftedSensorGyroY = shiftedSensorGyroY | *sensorGyroY;  


   *buffer = REG_GYRO_ZOUT_H | 0x80;
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorGyroZ, 1); 
   shiftedSensorGyroZ = *sensorGyroZ << 8;

   *buffer = REG_GYRO_ZOUT_L | 0x80;
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorGyroZ, 1); 
   shiftedSensorGyroZ = shiftedSensorGyroZ | *sensorGyroZ;    

   //Converts 16-bit integer to a float (this is the actual measurement in dps)
   *gyrx = ((float) shiftedSensorGyroX)/gyroConversionFactor;
   *gyry = ((float) shiftedSensorGyroY)/gyroConversionFactor;
   *gyrz = ((float) shiftedSensorGyroZ)/gyroConversionFactor;

   delete sensorGyroX, sensorGyroY, sensorGyroZ;
}

void ICM20602::get_temp_reading(float *temp) {                       // NEED TO READ MORE ABOUT THIS ONE
   uint8_t *sensorTemp;
   int16_t shiftedSensorTemp;

   *buffer = REG_TEMP_OUT_H | 0x80;
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorTemp, 1);
   shiftedSensorTemp = *sensorTemp << 8;
   
   *buffer = REG_TEMP_OUT_L | 0x80;
   send_dummy_byte(buffer);
   spi_port->exchange_data(buffer, sensorTemp, 1);
   shiftedSensorTemp = shiftedSensorTemp | *sensorTemp;

   *temp = ((float) shiftedSensorTemp)/tempConversionFactor;
   
   delete sensorTemp;
}

uint32_t ICM20602::get_current_time() {
   std::time_t currentTime = std::time(0);
   return static_cast<int>(currentTime);
}
 
void ICM20602::Begin_Measuring() {
   get_acceleration_reading(&measuredAccX, &measuredAccY, &measuredAccZ);
   get_gyroscope_reading(&measuredGyroX, &measuredGyroY, &measuredGyroZ);
   get_temp_reading(&measuredTemp);
   dataIsNew = true;
   timeOfResult = get_current_time();
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
