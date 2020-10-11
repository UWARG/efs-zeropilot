/**
 * IMU Sensor Functions and Part Number Selection
 * Author: Lucy Gong, Dhruv Rawat
 */

#ifndef IMU_HPP
#define IMU_HPP

#include <stdlib.h>
#include <ctime>
#include <cstdint>

#define ICM_20602 0
#define MPU9255 1

//Different acceleration sensitivities
#define ACCEL_SENSITIVITY_2G 16384.0f
#define ACCEL_SENSITIVITY_4G 8192.0f
#define ACCEL_SENSITIVITY_8G 4096.0f
#define ACCEL_SENSITIVITY_16G 2048.0f

//Different gyroscope sensitivities
#define GRYO_SENSITIVITY_250 131.0f
#define GRYO_SENSITIVITY_500 65.5
#define GRYO_SENSITIVITY_1000 32.8
#define GRYO_SENSITIVITY_2000 16.4


#define USE_IMU ICM_20602

struct IMUData_t {

    float magx, magy, magz; //Sensor does not have a magnetometer. Thus, these readings will be left as NaN
    float accx, accy, accz;
    float gyrx, gyry, gyrz; 
    float temp; 

    bool isDataNew; 
    int sensorStatus; //TBD but probably 0 = SUCCESS, -1 = FAIL, 1 = BUSY 
    float utcTime; //Last time GetResult was called
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

        void setAccelSensitivity(uint8_t); //1 = 2G; 2 = 4G; 3 = 8G; 4 = 16G
        void setGyroSensitivity(uint8_t); //1 = 250 dps; 2 = 500 dps; 3 = 1000 dps; 4 = 2000 dps
    private:
        //Variables
        uint32_t timeOfResult;
        static bool isSPIBusDefined;
        static bool dataIsNew;
        float accelConversionFactor;
        float gyroConversionFactor; 

        /*
            Method structure:
                - Generic idea:
                    - Read all of the data at the same time:
                        - Accel
                        - Gyro
                        - Temp
                - Process:
                    - Begin_Measuiring() will call each of the get___Reading() methods in order. Pass the global variable in as a pointer
                    - get___Reading() methods will collect data for its designated sensor and assign them to the global variable pointer in IMU.cpp
                        - Each method will call a method named IMU20602Read() which will use the register to get a raw reading.
                    - Begin_Measuring will terminate
        */

        //Methods
        void getAccelerationReading(float *, float *, float *);
        void getGyroscopeReading(float *, float *, float *);
        void getTempReading(float *); 
        uint16_t convertInt8ToInt16(uint8_t *);
        uint32_t getCurrentTime();
};

#endif
