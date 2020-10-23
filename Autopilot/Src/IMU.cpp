/**
 * IMU Sensor
 * Author(s): Dhruv Rawat
 */

#include "IMU.hpp"
#include "GPIO.hpp"
#include "Status.hpp"
#include "SPI.hpp"
#include <stddef.h>

//Address for ICM20602
#define ICM20602_SPI hspi1 //SPI Port 1

//Sensor Sensitivities
#define ACCEL_SENSITIVITY_4G 8192.0f
#define GRYO_SENSITIVITY_500 65.5
#define TEMPERATURE_SENSITIVITY 328.6

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
ICM20602* ICM20602::imu_Instance = NULL;

static SPIPort *spi_port;
static SPISettings hspi_1;
StatusCode sensorSuccess;

/*** MAIN BLOCK OF CODE BEGINS ***/

ICM20602* ICM20602::GetInstance() {
   if(!imu_Instance) {
      imu_Instance = new ICM20602;
   }
   return imu_Instance;
}
 
ICM20602::ICM20602() {
   // Configures the SPI Settings
   hspi_1.port = SPI_PORT1;
   hspi_1.mode = SPI_MODE_0;
   hspi_1.master = true;
   hspi_1.frequency = 10;                                            
   
   // Creates SPI Object
   spi_port = new SPIPort(hspi_1);

   /*
      SPI1_MISO - PG9
      SPI1_NSS - PG10
      SPI1_SCK - PG11
      SPI1_MOSI - PD7

      No option for GPIO_PORT_G, so I assume that GPIO_PORT_D is correct. 

   */
   GPIOPin pin(GPIO_PORT_D, 7, GPIO_ALT_PP, GPIO_STATE_LOW, GPIO_RES_NONE, GPIO_FREQ_HIGH, GPIO_AF5_SPI1);                         
   imuSlaveIdentifier = spi_port->add_slave(pin); //Adds slave pin to the spi_port class object

   uint8_t *setup = 0;
   uint8_t *imuBuffer_write = 0x00; //tx buffer when writing one byte
   uint8_t imuBufferRead_OneByte[2] = {0x00, 0x00}; 

   //Full reset of chip 
   *setup = 0x80;
   *imuBuffer_write = REG_PWR_MGMT_1 | 0x00; //0x80 means we are reading; 0x00 means we are writing
   uint8_t writeData[2] = {*imuBuffer_write, *setup}; // writeData[0] -> register address; writeData[1] -> write instructions
   write_data(writeData);

   //Verify chip is working properly
   imuBufferRead_OneByte[0] = REG_WHO_AM_I | 0x80;
   imuBufferRead_OneByte[1] = 0x00;
   uint8_t dataFromIMUSensor[2] = {0x00, 0x00}; // dataFromIMUSensor[0] -> dummy byte; dataFromIMUSensor[1] -> data byte
   spi_port->set_slave(imuSlaveIdentifier);
   sensorSuccess = spi_port->exchange_data(imuBufferRead_OneByte, dataFromIMUSensor, 2); //Sending in two bytes to tx_data (One register byte and one dummy byte)

   //Place accelerometer and gyroscopoe on standby
   *setup = 0x3F;
   *imuBuffer_write = REG_PWR_MGMT_2 | 0x00;
   writeData[0] = *imuBuffer_write; 
   writeData[1] = *setup; 
   write_data(writeData); 

   //Disable FIFO
   *setup = 0x00;
   *imuBuffer_write = REG_USER_CTRL | 0x00;
   writeData[0] = *imuBuffer_write;
   writeData[1] = *setup;
   write_data(writeData); 

   //Disable I2C
   *setup = 0x40;
   *imuBuffer_write = REG_I2C_IF | 0x00;
   writeData[0] = *imuBuffer_write;
   writeData[1] = *setup;
   write_data(writeData); 

   //Set up gyroscope
   *setup = 0x10; //Setting to 500 dps max for gyroscope; setting to 4G max for accelerometer
   *imuBuffer_write = REG_GYRO_CONFIG | 0x00;
   writeData[0] = *imuBuffer_write;
   writeData[1] = *setup;
   write_data(writeData); 

   //set up accelerometer
   *imuBuffer_write = REG_ACCEL_CONFIG | 0x00; 
   writeData[0] = *imuBuffer_write;
   write_data(writeData); 

   *setup = 0x08;
   *imuBuffer_write = REG_ACCEL_CONFIG_2 | 0x00;
   writeData[0] = *imuBuffer_write;
   writeData[1] = *setup;
   write_data(writeData); 

   //Turn on gyroscope and accelerometer
   *setup = 0x00;
   *imuBuffer_write = REG_PWR_MGMT_2 | 0x00;
   writeData[0] = *imuBuffer_write;
   writeData[1] = *setup;
   write_data(writeData); 
   
   accelConversionFactor = ACCEL_SENSITIVITY_4G;
   gyroConversionFactor = GRYO_SENSITIVITY_500;
   tempConversionFactor = TEMPERATURE_SENSITIVITY;
}

void ICM20602::write_data(uint8_t *writeData) { 
   uint8_t imuPlaceholderByte[2] = {0x00, 0x00};                                                                            
   spi_port->set_slave(imuSlaveIdentifier);
   spi_port->exchange_data(writeData, imuPlaceholderByte, 2);                                                                  
}

void ICM20602::get_accel_temp_gyro_reading(float *accx, float *accy, float *accz, float *gyrx, float *gyry, float *gyrz, float *temp) {
   uint8_t imuBufferRead_FourteenBytes[15];
   imuBufferRead_FourteenBytes[0] = REG_ACCEL_XOUT_H | 0x80;

   uint8_t imu_raw_data[15];
   imu_raw_data[0] = 0x00; //Dummy byte for MOSI
   
   //Store both High and Low Byte values
   int16_t shiftedSensorAccX, shiftedSensorAccY, shiftedSensorAccZ, shiftedSensorTemp, shiftedSensorGyroX, shiftedSensorGyroY, shiftedSensorGyroZ; 
   
   for(int i = 1; i < 15; i++) {  
      //0x00 is the reset value for all registers (Page 31). Thus, 0xFF will be used for the sensor_read (read only) registers
      imuBufferRead_FourteenBytes[i] = 0xFF; //Initializes all dummy bytes in the MISO  
      imu_raw_data[i] = 0x00; 
   }
   
   //Uses the sensor registers to get raw data for all sensors
   spi_port->set_slave(imuSlaveIdentifier);
   sensorSuccess = spi_port->exchange_data(imuBufferRead_FourteenBytes, imu_raw_data, 15); //Sending in 15 bytes to tx_data (One register byte and fourteen dummy bytes)
   
   shiftedSensorAccX = (imu_raw_data[1] << 8) + imu_raw_data[2];
   shiftedSensorAccY = (imu_raw_data[3] << 8) + imu_raw_data[4];
   shiftedSensorAccZ = (imu_raw_data[5] << 8) + imu_raw_data[6];

   shiftedSensorTemp = (imu_raw_data[7] << 8) + imu_raw_data[8];

   shiftedSensorGyroX = (imu_raw_data[9] << 8) + imu_raw_data[10];
   shiftedSensorGyroY = (imu_raw_data[11] << 8) + imu_raw_data[12];
   shiftedSensorGyroZ = (imu_raw_data[13] << 8) + imu_raw_data[14];

   //Converts 16-bit integers to a float. These are the actual measurements
   *accx = ((float) shiftedSensorAccX)/accelConversionFactor; 
   *accy = ((float) shiftedSensorAccY)/accelConversionFactor; 
   *accz = ((float) shiftedSensorAccZ)/accelConversionFactor; 

   *temp = ((float) shiftedSensorTemp)/tempConversionFactor;

   *gyrx = ((float) shiftedSensorGyroX)/gyroConversionFactor;
   *gyry = ((float) shiftedSensorGyroY)/gyroConversionFactor;
   *gyrz = ((float) shiftedSensorGyroZ)/gyroConversionFactor;
}
 
void ICM20602::Begin_Measuring() {
   get_accel_temp_gyro_reading(&measuredAccX, &measuredAccY, &measuredAccZ, &measuredGyroX, &measuredGyroY, &measuredGyroZ, &measuredTemp);
   dataIsNew = true;
}
 
void ICM20602::GetResult(IMUData_t &Data) {
   if (sensorSuccess == STATUS_CODE_OK) {
      Data.isDataNew = dataIsNew;
      dataIsNew = false;
      Data.accx = measuredAccX;
      Data.accy = measuredAccY;
      Data.accz = measuredAccZ;
      Data.gyrx = measuredGyroX;
      Data.gyry = measuredGyroY;
      Data.gyrz = measuredGyroZ;
      Data.magx = NAN;
      Data.magy = NAN;
      Data.magz = NAN;
      Data.temp = measuredTemp;
      Data.sensorStatus = 0;
   } else { //In case data was not received. Stick with old data
      Data.isDataNew = false;
      Data.sensorStatus = -1; 
   }
}
 
/* MAIN BLOCK OF CODE ENDS */
