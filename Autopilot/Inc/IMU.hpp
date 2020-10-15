/**
 * IMU Sensor Functions and Part Number Selection
 * Author(s): Lucy Gong, Dhruv Rawat
 */

#ifndef IMU_HPP
#define IMU_HPP

#include <stdlib.h>
#include <ctime>
#include <cstdint>
#include <stdint.h>

#define ICM_20602 0
#define MPU9255 1

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

#define USE_IMU ICM_20602

struct IMUData_t {

    float magx, magy, magz; //Sensor does not have a magnetometer. Thus, these readings will be left as NaN
    float accx, accy, accz;
    float gyrx, gyry, gyrz; 
    float temp; 

    bool isDataNew; 
    int sensorStatus; //TBD but probably 0 = SUCCESS, -1 = FAIL, 1 = BUSY 
    uint32_t utcTime; //Last time GetResult was called
};

class IMU{
    public:
        /**
         * Initializes IMU
         * */
        virtual void Init() = 0; 

        /**
         * Triggers interrupt for new IMU measurement - stores raw data in variables and returns right away
         * */
        virtual void Begin_Measuring() = 0; 

        /**GetResult should:
         * 1. Reset dataIsNew flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * */
        virtual void GetResult(IMUData_t &Data) = 0; //
};

//To be replaced with implementation of actual sensor
class ICM20602: public IMU{
    public:
        /**
         * Initializes IMU
         * */
        ICM20602(); 

        /**
         * Triggers interrupt for new IMU measurement - stores raw data in variables and returns right away
         * */
        void Begin_Measuring(){}; 

        /**GetResult should:
         * 1. Reset dataIsNew flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * */
        void GetResult(IMUData_t *Data){}; //

    private:
        //Variables
        uint32_t timeOfResult;
        static bool isSPIBusDefined;
        static bool dataIsNew;
        float accelConversionFactor;
        float gyroConversionFactor; 
        float tempConversionFactor;
        //Methods
        void get_accel_temp_gyro_reading(float *, float *, float *, float *, float *, float *, float *);
        void write_data(uint8_t *);
        uint32_t get_current_time();
};

#endif
