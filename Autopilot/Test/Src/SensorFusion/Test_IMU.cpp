//Provides skeleton IMU definitions for testing sensor fusion

#include "imu.hpp"
#include "CommonDataTypes.hpp"

TestIMU* TestIMU::GetInstance()
{
    TestIMU * imu = new TestIMU;
    return imu;
}

void TestIMU::Begin_Measuring()
{
}

void TestIMU::GetResult(IMUData_t &Data)
{
    Data.gyrx = 0; //roll speed
    Data.accx = 0;
    Data.magx = 0;
    Data.gyry = 0; //pitch speed
    Data.accy = 0;
    Data.magy = 0;
    Data.gyrz = 0; //yaw speed
    Data.accz = 0;
    Data.magz = 0;

    Data.temp = 0;

    Data.isDataNew = true;
    Data.sensorStatus = SENSOR_SUCCESS;
}
