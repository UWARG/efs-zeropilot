//Provides skeleton airspeed definitions for testing sensor fusion

#include "airspeed.hpp"

TestAirspeed* TestAirspeed::GetInstance()
{
    TestAirspeed * airspeed = new TestAirspeed;
    return airspeed;
}

void TestAirspeed::Begin_Measuring()
{
}

void TestAirspeed::GetResult(airspeedData_t& Data)
{
    Data.airspeed = 0;

    Data.isDataNew = true;
    Data.sensorStatus = SENSOR_SUCCESS;
    Data.utcTime = 0;
}
