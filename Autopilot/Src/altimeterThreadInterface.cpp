#include "altimeterThreadInterface.h"
#include "imu.hpp"

void AltimeterThreadInterfaceInit(void) {
    static IMU &imuobj = BMX160::getInstance();
    imuobj.Begin_Measuring();
}

void AltimeterThreadInterfaceExecute() {
    static IMU &imuobj = BMX160::getInstance();
    imuobj.Begin_Measuring();
}
