/* Includes ------------------------------------------------------------------*/
#include "imu.hpp"
#include "PPM.hpp"
#include "gpio.h"
#include "i2c.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_i2c.h"
#include <cstdint>

/* Private define ------------------------------------------------------------*/

/* BMX160 Relevant Registers
   Taken from datasheet: https://www.mouser.com/pdfdocs/BST-BMX160-DS000-11.pdf
 */
 
#define BMX160_READ_BIT 0x01
#define BMX160_WRITE_BIT 0x00
#define BMX160_DUMMY_BYTE 0x00

#define BMX160_I2C_ADDR (0x68 << 1) // Left-aligned slave address

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
#define MAG_REFRESH_200_HZ 0x09  
#define MAG_SLEEP_MODE 0x01

#define MAX_BUFF_SIZE 21

constexpr float GYRO_RANGE_1000_FACTOR = 1879.3f; // lsb/rad/s
constexpr float ACC_RANGE_8_FACTOR = 4.096f; // lsb/mg
constexpr float MAG_ADJUST_FACTOR = 16.0f; // lsb/uT

/* Arrays for data collection and calibration */
static int16_t accXLog[2000];
static int16_t accYLog[2000];
static int16_t accZLog[2000];

static int16_t gyrXLog[2000];
static int16_t gyrYLog[2000];
static int16_t gyrZLog[2000];

static uint16_t index;
static uint8_t cnter;


// /* Private Methods ---------------------------------------------------------*/


void BMX160::configAcc() {
	// Configure acceleration sampling rate as 800 Hz and every four are averaged
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, ACC_CONF_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) ACC_ODR_800_OSR4, 1, HAL_MAX_DELAY);

	// Configure accelerometer to have range of +- 8g
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, ACC_RANGE_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) ACC_RANGE_8G, 1, HAL_MAX_DELAY);
}

void BMX160::configGyro() {
	// Configure gyro sampling rate as 800 Hz and every four samples are averaged
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, GYR_CONF_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) GYRO_ODR_800_OSR4, 1, HAL_MAX_DELAY);

	// Configure gyroscope to have a range of +- 1000 deg/s
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, GYR_RANGE_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) GYRO_RANGE_1000, 1, HAL_MAX_DELAY);
}

void BMX160::configMag() {

	/*
	Steps for Magnetometer Configuration:
	1.) Put Mag into normal power mode (Already there since that was called before)
	2.) Wait 650 us
	3.) Write 0x80 to 0x4C (MAG_IF[0]) - set Mag into setup mode - done
	4.) Write 0x01 to 0x4F (MAG_IF[3]) and 0x4B to 0x4E (MAG_IF[2]) - put mag into sleep mode - done
	5.) Write 0x04 to 0x4F (MAG_IF[3]) and 0x51 to 0x4E (MAG_IF[2]) - put mag in regular preset mode - done
	6.) Write 0x0E to 0x4F (MAG_IF[3]) and 0x52 to 0x4E (MAG_IF[2]) - done
	7.) Write 0x02 to 0x4F, 0x4C to 0x4E, and 0x42 to 0x4D - prepare MAG_IF[0:3] for mag_if_data mode - done
	8.) Write 0x__ to MAG_CONF_REG to set ODR to 800 Hz - done
	9.) Write 0x00 to 0x4C
	*/

	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_IF_0_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_SETUP_EN, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_IF_3_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_SLEEP_MODE, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_IF_2_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_MODE_REG, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_IF_3_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) REP_XY_REGULAR_PRESET, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_IF_2_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_REPXY_REG, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_IF_3_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*)REP_Z_REGULAR_PRESET, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_IF_2_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_REPZ_REG, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_IF_3_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_IF_3_DATA_MODE, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_IF_2_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_IF_2_DATA_MODE, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_IF_1_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_IF_1_DATA_MODE, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, MAG_CONF_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_REFRESH_200_HZ, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR | BMX160_WRITE_BIT, CMD_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_SETUP_DIS, 1, HAL_MAX_DELAY);

}

bool BMX160::scan() {
	if (HAL_I2C_IsDeviceReady(&hi2c1, BMX160_I2C_ADDR, 50, HAL_MAX_DELAY) == HAL_OK) {
		return true;
	}
	return false;
}

// Calibrates IMU so that any drift or offset is accounted for
void BMX160::calibrate(void) {
	const int nSamplesForReliableAverage = 100;
    IMUData_t TempImuData;

    IMUCalibration.accel_x = 0.0f;
    IMUCalibration.accel_y = 0.0f;
    IMUCalibration.accel_z = 0.0f;
    IMUCalibration.gyro_x = 0.0f;
    IMUCalibration.gyro_y = 0.0f;
    IMUCalibration.gyro_z = 0.0f;
    IMUCalibration.mag_x = 0.0f;
    IMUCalibration.mag_y = 0.0f;
    IMUCalibration.mag_z = 0.0f;

    for (int i = 0; i < nSamplesForReliableAverage; i++)
    {
        this->updateData();

        HAL_Delay(7);

        this->GetResult(TempImuData);

        IMUCalibration.gyro_x += TempImuData.gyro_x;
        IMUCalibration.gyro_y += TempImuData.gyro_y;
        IMUCalibration.gyro_z += TempImuData.gyro_z;
        IMUCalibration.accel_x += TempImuData.accel_x;
        IMUCalibration.accel_y += TempImuData.accel_y;
        IMUCalibration.accel_z += TempImuData.accel_z;

    }

    IMUCalibration.gyro_x /= (float) nSamplesForReliableAverage;
    IMUCalibration.gyro_y /= (float) nSamplesForReliableAverage;
    IMUCalibration.gyro_z /= (float) nSamplesForReliableAverage;
    IMUCalibration.accel_x /= (float) nSamplesForReliableAverage;
    IMUCalibration.accel_y /= (float) nSamplesForReliableAverage;
    IMUCalibration.accel_z /= (float) nSamplesForReliableAverage;

    IMUCalibration.accel_z -= 1000.0f;    // at calibration, Z needs to read -1g.
}


// /* Public Methods  ---------------------------------------------------------*/

IMU& BMX160::getInstance() {
	static BMX160 singleton;
	return singleton;
}

void BMX160::updateData(void) {
	// Just updates the rawIMUData and conducts some processing on it
	HAL_I2C_Mem_Read(&hi2c1, BMX160_I2C_ADDR, DATA_REG, I2C_MEMADD_SIZE_8BIT, rawImuData, 20, HAL_MAX_DELAY);
}

void BMX160::GetResult(IMUData_t &Data) {
	// HAL_I2C_Mem_Read(&hi2c1, BMX160_I2C_ADDR, DATA_REG, I2C_MEMADD_SIZE_8BIT, rawImuData, 20, HAL_MAX_DELAY); // test update data first
	uint8_t * intImuDataPtr = &rawImuData[1]; // First byte is garbage, data starts from second byte

	// The 15:8 and 7:0 bits are in different registers. The bitmasking below joins them into one 16 bit integer
	int16_t magx = (intImuDataPtr[1] << 8) | intImuDataPtr[0];
	int16_t magy = (intImuDataPtr[3] << 8) | intImuDataPtr[2];
	int16_t magz = (intImuDataPtr[5] << 8) | intImuDataPtr[4];
	int16_t rhall = (intImuDataPtr[7] << 8) | intImuDataPtr[6];
	int16_t gyrx = (intImuDataPtr[9] << 8) | intImuDataPtr[8];
	int16_t gyry = (intImuDataPtr[11] << 8) | intImuDataPtr[10];
	int16_t gyrz = (intImuDataPtr[13] << 8) | intImuDataPtr[12];
	int16_t accx = (intImuDataPtr[15] << 8) | intImuDataPtr[14];
	int16_t accy = (intImuDataPtr[17] << 8) | intImuDataPtr[16];
	int16_t accz = (intImuDataPtr[19] << 8) | intImuDataPtr[18];

	Data.mag_x = static_cast<float> (magx) / MAG_ADJUST_FACTOR;
	Data.mag_y = static_cast<float> (magy) / MAG_ADJUST_FACTOR;
	Data.mag_z = static_cast<float> (magz) / MAG_ADJUST_FACTOR;

	Data.accel_x = (static_cast<float>(accx) / ACC_RANGE_8_FACTOR) - IMUCalibration.accel_x;
	Data.accel_y = (static_cast<float>(accy) / ACC_RANGE_8_FACTOR) - IMUCalibration.accel_y;
	Data.accel_z = (static_cast<float>(accz) / ACC_RANGE_8_FACTOR) - IMUCalibration.accel_z;

	Data.gyro_x = (static_cast<float>(gyrx) / GYRO_RANGE_1000_FACTOR) - IMUCalibration.gyro_x;
	Data.gyro_y = (static_cast<float>(gyry) / GYRO_RANGE_1000_FACTOR) - IMUCalibration.gyro_y;
	Data.gyro_z = (static_cast<float>(gyry) / GYRO_RANGE_1000_FACTOR) - IMUCalibration.gyro_z;
}

BMX160::BMX160() {
	HAL_I2C_Init(&hi2c1);
	IMUInit();
	this -> calibrate(); // Calibrate IMU and populate callibration struct for use
}

void BMX160::IMUInit(void) {
	if (scan()) {
		setAllPowerModesToNormal();
		configAcc();
		configGyro();
		configMag();
		HAL_I2C_Mem_Read(&hi2c1, BMX160_I2C_ADDR, PMU_STATUS_REG, I2C_MEMADD_SIZE_8BIT, &powerStatus, 1, HAL_MAX_DELAY);
	}
	else {
		// The device doesn't exist or isn't responding - there is some error
		// TODO: Implement error protocol
	}
}

void BMX160::setAllPowerModesToNormal(){
	HAL_Delay(100);
	// Set gyro to normal mode
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR, CMD_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) GYRO_NORMAL_MODE_CMD, 1, HAL_MAX_DELAY);
	HAL_Delay(100);
	// Set accelerometer to normal mode
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR, CMD_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) ACC_NORMAL_MODE_CMD, 1, HAL_MAX_DELAY);
	HAL_Delay(100);
	// Set magnetometer to normal mode
	HAL_I2C_Mem_Write(&hi2c1, BMX160_I2C_ADDR, CMD_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) MAG_NORMAL_MODE_CMD, 1, HAL_MAX_DELAY);
	HAL_Delay(100);
}
