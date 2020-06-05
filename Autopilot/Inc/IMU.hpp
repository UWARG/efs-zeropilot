/**
 * IMU Sensor Functions and Part Number Selection
 * Author: Lucy Gong
 */

#ifndef IMU_HPP
#define IMU_HPP

#define ICM-20602 0
#define MPU9255 1

#define USE_IMU ICM-20602

struct IMUData_t {

    float magx, magy, magz;
    float accx, accy, accz;
    float gyrx, gyry, gyrz; 

    bool isDataNew; 
    int status; //TBD but probably 0 = SUCCESS, -1 = FAIL, 1 = BUSY 
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
        virtual void GetResult(IMUData_t *Data) = 0; //
};

#endif
