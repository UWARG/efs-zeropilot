#include "IMUThreadInterface.h"
#include "imu.hpp"

void IMUThreadInterfaceInit(void) {
    static IMU& imuobj = BMX160::getInstance();
    imuobj.Begin_Measuring();
}

void IMUThreadInterfaceExecute() {
    static IMU& imuobj = BMX160::getInstance();
    imuobj.Begin_Measuring();
}
