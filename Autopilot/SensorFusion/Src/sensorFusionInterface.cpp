#include "sensorFusionInterface.hpp"
#include "SensorFusion.hpp"

void SensorFusionInterfaceInit(void) {
    SF_Init();
}

SFError_t SensorFusionInterfaceExecute(void) {
    return SF_GenerateNewResult();
}
