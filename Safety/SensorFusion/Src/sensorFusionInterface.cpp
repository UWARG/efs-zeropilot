#include "sensorFusionInterface.hpp"
#include "SensorFusion.hpp"

void SensorFusionInterfaceInit(void) {
    SF_Init();
}

SFError_t SensorFusionInterfaceExecute(void) {
    SFError_t err = SF_GenerateNewResult();
    return err;
}
