#include "IMUInterface.h"
#include "imu.hpp"

IMU& imuObj = BMX160::getInstance();

void IMUInterfaceExecute(void) {
    imuObj.Begin_Measuring();
}
