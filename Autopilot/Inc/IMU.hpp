/**
 * IMU Sensor Functions and Part Number Selection
 * Author(s): Lucy Gong, Dhruv Rawat
 */

#ifndef IMU_HPP
#define IMU_HPP

#include <stdlib.h>
#include <ctime>
#include <cstdint>
#include <math.h>
#include "main.h"

#define ICM_20602 0
#define MPU9255 1

#define USE_IMU ICM_20602

struct IMUData_t {

    float magx, magy, magz; //ICM20602 does not have a magnetometer. Thus, these readings will be left as NAN
    float accx, accy, accz;
    float gyrx, gyry, gyrz; 
    float temp; 

    bool isDataNew;
    int sensorStatus; //TBD but probably 0 = SUCCESS, -1 = FAIL, 1 = BUSY 
    uint32_t utcTime; //Last time GetResult was called
};

class IMU {
    public:
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
        ICM20602(const ICM20602*) = delete;
        static ICM20602* GetInstance();

        /**
         * Triggers interrupt for new IMU measurement - stores raw data in variables and returns right away
         * */
        void Begin_Measuring(); 

        /**GetResult should:
         * 1. Reset dataIsNew flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * */
        void GetResult(IMUData_t &Data); //
             
        
    private:
        //Constructor 
        ICM20602();

        //Object Instance
        static ICM20602* imu_Instance;

        //Variables
        bool dataIsNew = false;
        float accelConversionFactor, gyroConversionFactor, tempConversionFactor; //Conversion Factors
        uint32_t imuSlaveIdentifier;
        float measuredAccX = 0, measuredAccY = 0, measuredAccZ = 0; //Acceleration readings
        float measuredGyroX = 0, measuredGyroY = 0, measuredGyroZ = 0; //Gyroscope readings 
        float measuredTemp = 0; //Temperature reading

        //Methods
        void get_accel_temp_gyro_reading();
        void write_data(uint8_t *);
};

#ifdef UNIT_TESTING
#include "IMU_Mock.hpp"
#endif

#ifdef SIMULATION

class SimulatedIMU : public IMU {
    public:
        void Begin_Measuring();
        void GetResult(IMUData_t &Data);
};

#endif

#endif
