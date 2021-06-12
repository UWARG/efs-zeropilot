#include "PathManagerInterface.h"
#include "imu.hpp"

void PathManagerInterfaceInit(void) 
{
    static IMU &imuobj = BMX160::getInstance();
    imuobj.Begin_Measuring();

}

bool PathManagerInterfaceExecute(void) 
{
    static IMU &imuobj = BMX160::getInstance();

    imuobj.Begin_Measuring();

    return true;
}
