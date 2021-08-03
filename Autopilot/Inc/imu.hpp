/**
 * IMU Sensor Functions and Part Number Selection.
 * Available IMU driver(s): BMX160
 * Author(s): Lucy Gong, Dhruv Rawat, Anthony Berbari
 */

#ifndef IMU_HPP
#define IMU_HPP

#include <cstdint>

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

struct IMUData_t {

    float magx, magy, magz; // TODO although the BMX 160 has a magnetometer, it seems to produce bizarre results.More investigation needs to be done. Figuring out what the RHall register does is likely a part of that.
    float accx, accy, accz;
    float gyrx, gyry, gyrz; 
    float temp; 

    bool isDataNew;
    int sensorStatus; // 0 = SUCCESS, -1 = FAIL, 1 = BUSY
};

/***********************************************************************************************************************
 * Prototypes
 *********************************************************************************************************************/

class IMU {
    public:
        /**
         * Begins a transaction with the IMU.
         * This function is non blocking and returns right away, data will be stored inside the module as it arrives.
         * To achieve synchronous data, this function must be called synchronously.
         * */
        virtual void Begin_Measuring() = 0; 

        /**
         * Retrieves any data already received by the imu.
         * If no new data is available, the appropriate flag will be set in the return struct.
         * All contents of the return struct, apart from the isDataNew flag, are undefined unless isDataNew is set to 1.
         * This function is non blocking and returns right away.
         * @param[in]       Data        reference to the results struct.
         * */
        virtual void GetResult(IMUData_t& Data) = 0;
};

class BMX160: public IMU{
    public:

        /**
         * This module is built as a singleton. Thus to access a BMX160 object, this function must be called.
         * Only a single BMX160 object will ever be created and will be shared by all callers of this function.
         * @return      IMU        reference to the singleton object.
         * */
        static IMU& getInstance();

        /**
         * Deletes the constructor to disallow users to instantiate objects.
         * */
        BMX160(const BMX160*) = delete;

        void Begin_Measuring();
        void GetResult(IMUData_t& Data);

    private:

        BMX160();

        void SetAllPowerModesToNormal(void);
        void ConfigAcc(void);
        void ConfigGyro(void);
        void ConfigMag(void);
        void SetMagConfig(uint8_t regAddr, uint8_t data);
        void PrepareMagForDataMode(void);

        void Bmx160WriteReg(uint8_t reg, uint8_t val);
        void Bmx160ReadReg(uint8_t const regAddr, uint8_t *pData, uint8_t len);

        //Variables
        uint8_t rawImuData[21];

};

/**
* Callback for exclusive use by the SPI ISR.
* Users of this module should never call this function.
* Any manipulation of this function by the higher level voids all guarantees made by this module.
* */
void ImuTxRxCallback();

#ifdef UNIT_TESTING

class TestIMU : public IMU {
    public:
        static TestIMU* GetInstance();

        void Begin_Measuring();

        void GetResult(IMUData_t& Data);

};

#endif

#endif
