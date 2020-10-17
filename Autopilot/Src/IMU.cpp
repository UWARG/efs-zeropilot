/**
 * IMU Sensor
 * Author(s): Dhruv Rawat
 */

#include "IMU.hpp"
#include "GPIO.hpp"
#include "Status.hpp"
#include "SPI.hpp"

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
 
bool ICM20602::isSPIBusDefined = false;
bool ICM20602::dataIsNew = false;

float measuredAccX, measuredAccY, measuredAccZ; //Acceleration readings
float measuredGyroX, measuredGyroY, measuredGyroZ; //Gyroscope readings 
float measuredTemp; //Temperature reading
static SPIPort *spi_port;
static SPISettings hspi_1;
uint8_t *buffer;
StatusCode sensorSuccess;

/*** MAIN BLOCK OF CODE BEGINS ***/
 
void ICM20602::Init() {
   if(!isSPIBusDefined) {
      // Configures the SPI Settings
      hspi_1.port = SPI_PORT1;
      hspi_1.mode = SPI_MODE_0;
      hspi_1.master = true;
      hspi_1.frequency = 10;                                            
      
      // Creates SPI Object
      spi_port = new SPIPort(hspi_1);
      GPIOPin pin(GPIO_PORT_A, 1, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE, GPIO_FREQ_HIGH, 0);                             //GET VALUES LATER 
      slaveIdentifier = spi_port->add_slave(pin); //Adds slave pin to the spi_port class object

      uint8_t *setup = 0;

      //Full reset of chip 
      *setup = 0x80;
      *buffer = REG_PWR_MGMT_1 | 0x00; //0x80 means we are reading; 0x00 means we are writing
      uint8_t writeData[2] = {*buffer, *setup}; // writeData[0] -> register address; writeData[1] -> write instructions
      write_data(writeData);

      //Verify chip is working properly
      *buffer = REG_WHO_AM_I | 0x80;
      uint8_t dataFromSensor[2] = {0x00, 0x00}; // dataFromSensor[0] -> dummy byte (contains SPI address); dataFromSensor[1] -> data byte
      spi_port->set_slave(slaveIdentifier);
      sensorSuccess = spi_port->exchange_data(buffer, dataFromSensor, 1);

      if(sensorSuccess != STATUS_CODE_OK) {
         //Debug code here
      }

      //Place accelerometer and gyroscopoe on standby
      *setup = 0x3F;
      *buffer = REG_PWR_MGMT_2 | 0x00;
      writeData[0] = *buffer; 
      writeData[1] = *setup; 
      write_data(writeData); 

      //Disable FIFO
      *setup = 0x00;
      *buffer = REG_USER_CTRL | 0x00;
      writeData[0] = *buffer;
      writeData[1] = *setup;
      write_data(writeData); 

      //Disable I2C
      *setup = 0x40;
      *buffer = REG_I2C_IF | 0x00;
      writeData[0] = *buffer;
      writeData[1] = *setup;
      write_data(writeData); 

      //Set up gyroscope
      *setup = 0x10; //Setting to 500 dps max for gyroscope; setting to 4G max for accelerometer
      *buffer = REG_GYRO_CONFIG | 0x00;
      writeData[0] = *buffer;
      writeData[1] = *setup;
      write_data(writeData); 

      //set up accelerometer
      *buffer = REG_ACCEL_CONFIG | 0x00; 
      writeData[0] = *buffer;
      write_data(writeData); 

      *setup = 0x08;
      *buffer = REG_ACCEL_CONFIG_2 | 0x00;
      writeData[0] = *buffer;
      writeData[1] = *setup;
      write_data(writeData); 
  
      //Turn on gyroscope and accelerometer
      *setup = 0x00;
      *buffer = REG_PWR_MGMT_2 | 0x00;
      writeData[0] = *buffer;
      writeData[1] = *setup;
      write_data(writeData); 
   }

   accelConversionFactor = ACCEL_SENSITIVITY_4G;
   gyroConversionFactor = GRYO_SENSITIVITY_500;
   tempConversionFactor = TEMPERATURE_SENSITIVITY;
}

void ICM20602::write_data(uint8_t *writeData) { 
   uint8_t *placeholderByte;
   spi_port->set_slave(slaveIdentifier);
   spi_port->exchange_data(writeData, placeholderByte, 2);                                                                    //WRITEDATA ARRAY HAS TWO ELEMENTS, THUS THE BUFFER SIZE IS TWO HERE!!!!
}

void ICM20602::get_accel_temp_gyro_reading(float *accx, float *accy, float *accz, float *gyrx, float *gyry, float *gyrz, float *temp) {
   uint8_t raw_data[15];
   raw_data[0] = 0x00;
   //Store both High and Low Byte values
   int16_t shiftedSensorAccX, shiftedSensorAccY, shiftedSensorAccZ, shiftedSensorTemp, shiftedSensorGyroX, shiftedSensorGyroY, shiftedSensorGyroZ; 
   //Uses the sensor registers to get raw data for all sensors
   *buffer = REG_ACCEL_XOUT_H | 0x80; 
   
   spi_port->set_slave(slaveIdentifier);
   sensorSuccess = spi_port->exchange_data(buffer, raw_data, 1); //Using burst read, 14 data points for both sensors will be collected. 
   
   shiftedSensorAccX = (raw_data[1] << 8) + raw_data[2];
   shiftedSensorAccY = (raw_data[3] << 8) + raw_data[4];
   shiftedSensorAccZ = (raw_data[5] << 8) + raw_data[6];

   shiftedSensorTemp = (raw_data[7] << 8) + raw_data[8];

   shiftedSensorGyroX = (raw_data[9] << 8) + raw_data[10];
   shiftedSensorGyroY = (raw_data[11] << 8) + raw_data[12];
   shiftedSensorGyroZ = (raw_data[13] << 8) + raw_data[14];

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
   dataIsNew = false;
   if (sensorSuccess == STATUS_CODE_OK) {
      Data.isDataNew = true;
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
