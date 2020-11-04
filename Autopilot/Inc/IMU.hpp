












/* DELETE THIS FILE WHEN MERGING */ 













#ifndef IMU_HPP
#define IMU_HPP

#include <stdlib.h>
#include <ctime>
#include <cstdint>
#include <math.h>

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
        void Init(); 

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
        //Variables
        uint32_t timeOfResult;
        static bool isSPIBusDefined;
        static bool dataIsNew;
        float accelConversionFactor, gyroConversionFactor, tempConversionFactor; //Conversion Factors
        uint32_t imuSlaveIdentifier;

        //Methods
        void get_accel_temp_gyro_reading(float *, float *, float *, float *, float *, float *, float *);
        void write_data(uint8_t *);
};

#endif