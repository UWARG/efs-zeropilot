
/* Includes ------------------------------------------------------------------*/
#include "imu.hpp"
#include "stm32f0xx_hal.h"

/* Private define ------------------------------------------------------------*/


#define WHO_AM_I 0x75 // Use to check if device is there

#define IMU_ADDRESS 0x68 <<1 // 7-bit address

#define PWR_MGMT_1 0x6B

#define SMPLRT_DIV 0x19

//Config
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C

//Gryo registers
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x44
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

//Accelerometer register
#define ACCEL_XOUT_H 0x3B

// constants

const uint8_t kImuAddr = 104;
const double kGryoCorrector = 131.0;
const double kAccelCorrector = 16384.0;


/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;


/* Public Methods  ---------------------------------------------------------*/

Imu& MPU6050::getInstance(){
  static MPU6050 singleton; 
  return singleton; 

}

void MPU6050::getResult(ImuData& Data){
	uint8_t data[6];
	int16_t raw_x = 0, raw_y = 0, raw_z = 0;
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Read(&hi2c1, IMU_ADDRESS, GYRO_XOUT_H, 1, data, 6, HAL_MAX_DELAY);

	raw_x = (int16_t)(data[0] << 8 | data[1]);
	raw_y = (int16_t)(data[2] << 8 | data[3]);
	raw_z = (int16_t)(data[4] << 8 | data[5]);

	/* Converting raw values into degrees per second.
	* Scaling by the sensitivity for the configured scale range.*/

	Data.gyro_x = raw_x / kGryoCorrector;
	Data.gyro_y = raw_y / kGryoCorrector;
	Data.gyro_z = raw_z / kGryoCorrector;

	status = HAL_I2C_Mem_Read(&hi2c1, IMU_ADDRESS, ACCEL_XOUT_H, 1, data, 6, HAL_MAX_DELAY);

	raw_x = (int16_t)(data[0] << 8 | data[1]);
	raw_y = (int16_t)(data[2] << 8 | data[3]);
	raw_z = (int16_t)(data[4] << 8 | data[5]);

	/* Converting raw values into acceleration in terms of g.
	*  Scaling by the sensitivity for the configured scale range.*/

	Data.accel_x = raw_x / kAccelCorrector;
	Data.accel_y = raw_y / kAccelCorrector;
	Data.accel_z = raw_z / kAccelCorrector;

}

/* Private methods  ---------------------------------------------------------*/

MPU6050::MPU6050(){
  HAL_I2C_Init(&hi2c1);
}

void MPU6050::ImuInit (void){
	//Use this for IMU set up
	uint8_t imu_check = 0, data_check = 0;
	HAL_StatusTypeDef wake = HAL_OK;


	//Check if device is present and the IMU is the expected device
	
		if (HAL_I2C_Mem_Read(&hi2c1, IMU_ADDRESS, WHO_AM_I, 1, &imu_check, 1, HAL_MAX_DELAY) == HAL_OK && imu_check == kImuAddr){

				//Wake the sensor up
				wake = HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, PWR_MGMT_1, 1, &data_check, 1, HAL_MAX_DELAY);

				data_check = 0x07;
				HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, SMPLRT_DIV, 1, &data_check, 1, HAL_MAX_DELAY);

				//Configure gyro to have range of +/- 250 dps
				data_check = 0x00;
				HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, GYRO_CONFIG, 1, &data_check, 1, HAL_MAX_DELAY);

				//Configure Accelerometer to have range of +/- 2g
				data_check = 0x00;
				HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, ACCEL_CONFIG, 1, &data_check, 1, HAL_MAX_DELAY);
			}

	

}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
