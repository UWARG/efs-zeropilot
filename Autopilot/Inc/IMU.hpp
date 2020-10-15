/**
 * IMU Sensor Functions and Part Number Selection
 * Author: Lucy Gong, Dhruv Rawat
 */

#ifndef IMU_HPP
#define IMU_HPP

#include <stdlib.h>
#include <ctime>
#include <cstdint>
#include <stdint.h>

#define ICM_20602 0
#define MPU9255 1

//Different acceleration sensitivities
// #define ACCEL_SENSITIVITY_2G 16384.0f
#define ACCEL_SENSITIVITY_4G 8192.0f
// #define ACCEL_SENSITIVITY_8G 4096.0f
// #define ACCEL_SENSITIVITY_16G 2048.0f

//Different gyroscope sensitivities
// #define GRYO_SENSITIVITY_250 131.0f
#define GRYO_SENSITIVITY_500 65.5
// #define GRYO_SENSITIVITY_1000 32.8
// #define GRYO_SENSITIVITY_2000 16.4

#define TEMPERATURE_SENSITIVITY 328.6

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
