
/* Includes ------------------------------------------------------------------*/
#include "imu.hpp"
#include "PPM.hpp"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_i2c.h"
#include <cstdint>

/* Private define ------------------------------------------------------------*/

/* BMX160 Relevant Registers
   Taken from datasheet: https://www.mouser.com/pdfdocs/BST-BMX160-DS000-11.pdf
 */
 
#define BMX160_READ_BIT 0x80
#define BMX160_WRITE_BIT 0x00
#define BMX160_DUMMY_BYTE 0x00

#define BMX160_I2C_ADDR 0x68

#define CHIP_ID_REG 0x00
#define DATA_REG 0x04
#define STATUS_REG 0x1B
#define CMD_REG 0x7E
#define PMU_STATUS_REG 0x03
#define ACC_CONF_REG 0x40
#define ACC_RANGE_REG 0x41
#define GYR_CONF_REG 0x42
#define GYR_RANGE_REG 0x43
#define MAG_CONF_REG 0x44
#define MAG_IF_0_REG 0x4C
#define MAG_IF_1_REG 0x4D
#define MAG_IF_2_REG 0x4E
#define MAG_IF_3_REG 0x4F
#define MAG_REPZ_REG 0x52
#define MAG_REPXY_REG 0x51
#define MAG_MODE_REG 0x4B

// Registers for setting power mode to normal 
#define ACC_NORMAL_MODE_CMD 0x11
#define GYRO_NORMAL_MODE_CMD 0x15
#define MAG_NORMAL_MODE_CMD 0x19

#define ACC_RANGE_8G 0x08

// From datasheet - last four bits (1011 indicates a sampling rate of 800 Hz)
// The first four bits indicate OSR4 - data is sampled at an oversampling rate of 4
#define ACC_ODR_800_OSR4 0x0B // 0b00001011 


#define GYRO_RANGE_1000  0x01

// From datasheet - last four bits (1011 indicates a sampling rate of 800 Hz)
// The first four bits indicate OSR4 - data is sampled at an oversampling rate of 4
#define GYRO_ODR_800_OSR4 0x0B // 0b00001011
#define MAG_SETUP_EN 0x80
#define MAG_SETUP_DIS 0x00
#define REP_XY_REGULAR_PRESET 0x04 // TODO there is also a high accuracy preset. Not too sure what that's about.
#define REP_Z_REGULAR_PRESET 0x0E
#define MAG_IF_3_DATA_MODE 0x02
#define MAG_IF_2_DATA_MODE 0x4C
#define MAG_IF_1_DATA_MODE 0x42
#define MAG_REFRESH_50_HZ 0x07  // TODO mag should refresh at 200 Hz as well, but I cant seem to get that to work.
#define MAG_SLEEP_MODE 0x01

#define MAX_BUFF_SIZE 21
// // constants

// const uint8_t kImuAddr = 104;
// const double kGryoCorrector = 131.0;
// const double kAccelCorrector = 16384.0;


// /* Private variables ---------------------------------------------------------*/


// /* Public Methods  ---------------------------------------------------------*/

IMU& BMX160::getInstance() {
	static BMX160 singleton;
	return singleton;
}

BMX160::BMX160() {
	HAL_I2C_Init(&hi2c1);
	IMUInit();
}

void BMX160::IMUInit(void) {
	scan();
	setAllPowerModesToNormal();
	configAcc();
	configGyro();
	configMag();
}

void BMX160::setAllPowerModesToNormal(){
	// Set gyro to normal mode
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR, CMD_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) GYRO_NORMAL_MODE_CMD, 1, HAL_MAX_DELAY);

	// Set accelerometer to normal mode
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR, CMD_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) ACC_NORMAL_MODE_CMD, 1, HAL_MAX_DELAY);

	// Set magnetometer to normal mode
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR, CMD_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_NORMAL_MODE_CMD, 1, HAL_MAX_DELAY);

}

void configAcc() {
	// Configure acceleration sampling rate as 800 Hz and every four are averaged
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR, ACC_CONF_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) ACC_ODR_800_OSR4, 1, HAL_MAX_DELAY);

	// Configure accelerometer to have range of +- 8g
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR, ACC_RANGE_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) ACC_RANGE_8G, 1, HAL_MAX_DELAY);
}

void configGyro() {
	// Configure gyro sampling rate as 800 Hz and every four samples are averaged
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR, GYR_CONF_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) GYRO_ODR_800_OSR4, 1, HAL_MAX_DELAY);

	// Configure gyroscope to have a range of +- 1000 deg/s
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR, GYR_RANGE_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) GYRO_RANGE_1000, 1, HAL_MAX_DELAY);
}

void configMag() {
	// Configure magnetometer
}
// IMU& MPU6050::getInstance(){
//   static MPU6050 singleton; 
//   return singleton; 
// }

// void MPU6050::GetResult(IMUData_t& Data){
// 	uint8_t data[6];
// 	int16_t raw_x = 0, raw_y = 0, raw_z = 0;
// 	HAL_StatusTypeDef status = HAL_OK;

// 	status = HAL_I2C_Mem_Read(&hi2c1, IMU_ADDRESS, GYRO_XOUT_H, 1, data, 6, HAL_MAX_DELAY);

// 	raw_x = (int16_t)(data[0] << 8 | data[1]);
// 	raw_y = (int16_t)(data[2] << 8 | data[3]);
// 	raw_z = (int16_t)(data[4] << 8 | data[5]);

// 	/* Converting raw values into degrees per second.
// 	* Scaling by the sensitivity for the configured scale range.*/

// 	Data.gyro_x = raw_x / kGryoCorrector;
// 	Data.gyro_y = raw_y / kGryoCorrector;
// 	Data.gyro_z = raw_z / kGryoCorrector;

// 	status = HAL_I2C_Mem_Read(&hi2c1, IMU_ADDRESS, ACCEL_XOUT_H, 1, data, 6, HAL_MAX_DELAY);

// 	raw_x = (int16_t)(data[0] << 8 | data[1]);
// 	raw_y = (int16_t)(data[2] << 8 | data[3]);
// 	raw_z = (int16_t)(data[4] << 8 | data[5]);

// 	/* Converting raw values into acceleration in terms of g.
// 	*  Scaling by the sensitivity for the configured scale range.*/

// 	Data.accel_x = raw_x / kAccelCorrector;
// 	Data.accel_y = raw_y / kAccelCorrector;
// 	Data.accel_z = raw_z / kAccelCorrector;

// }

// /* Private methods  ---------------------------------------------------------*/

// MPU6050::MPU6050(){
//   HAL_I2C_Init(&hi2c1);
//   ImuInit();
// }

// void MPU6050::ImuInit (void){
// 	//Use this for IMU set up
// 	uint8_t imu_check = 0, data_check = 0;
// 	HAL_StatusTypeDef wake = HAL_OK;


// 	//Check if device is present and the IMU is the expected device
	
// 		if (HAL_I2C_Mem_Read(&hi2c1, IMU_ADDRESS, WHO_AM_I, 1, &imu_check, 1, HAL_MAX_DELAY) == HAL_OK && imu_check == kImuAddr){

// 				//Wake the sensor up
// 				wake = HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, PWR_MGMT_1, 1, &data_check, 1, HAL_MAX_DELAY);

// 				data_check = 0x07;
// 				HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, SMPLRT_DIV, 1, &data_check, 1, HAL_MAX_DELAY);

// 				//Configure gyro to have range of +/- 250 dps
// 				data_check = 0x00;
// 				HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, GYRO_CONFIG, 1, &data_check, 1, HAL_MAX_DELAY);

// 				//Configure Accelerometer to have range of +/- 2g
// 				data_check = 0x00;
// 				HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, ACCEL_CONFIG, 1, &data_check, 1, HAL_MAX_DELAY);
// 			}

	

// }


// /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
