//Provides skeleton GPS definitions for testing sensor fusion

#include "gps.hpp"
#include "CommonDataTypes.hpp"

TestGps* TestGps::GetInstance()
{
    TestGps * gps = new TestGps;
    return gps;
}

void TestGps::Begin_Measuring()
{
}

void TestGps::GetResult(GpsData_t& Data)
{
    Data.altitude = 0;
    Data.latitude = 0;
    Data.longitude = 0;
    Data.groundSpeed = 0;
    Data.heading = 0;

    Data.numSatellites = 3;
    Data.dataIsNew = true;
    Data.timeIsNew = true;
    Data.ggaDataIsNew = true;
    Data.vtgDataIsNew = true;
    Data.sensorStatus = SENSOR_SUCCESS;
}
