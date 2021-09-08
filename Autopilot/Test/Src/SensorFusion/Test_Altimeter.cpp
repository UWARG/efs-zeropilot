//Provides skeleton altimeter definitions for testing sensor fusion

#include "altimeter.hpp"

TestAltimeter* TestAltimeter::GetInstance()
{
    TestAltimeter * altimeter = new TestAltimeter;
    return altimeter;
}

void TestAltimeter::Begin_Measuring()
{
}

void TestAltimeter::GetResult(AltimeterData_t &Data)
{
    Data.pressure = 0;
    Data.altitude = 0;
    Data.temp = 0;

    Data.isDataNew = true;
    Data.status = 0;
    Data.utcTime = 0;
}
