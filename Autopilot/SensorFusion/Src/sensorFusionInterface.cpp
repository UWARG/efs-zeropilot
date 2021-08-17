/**
 * Interface implementation from SensorFusion algorithm (written in C++) to freertos (C).
 * Author(s): Dhruv Upadhyay
 */


#include "sensorFusionInterface.hpp"
#include "SensorFusion.hpp"

void SensorFusionInterfaceInit(void) {
    SF_Init();
}

SFError_t SensorFusionInterfaceExecute(void) {
    SFError_t err = SF_GenerateNewResult();
    return err;
}
