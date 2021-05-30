#include "sensorFusionInterface.hpp"
#include "SensorFusion.hpp"

struct SFError_t SensorFusionInterfaceExecute(void) {
    SFError_t err = SF_GenerateNewResult();
    return err;
}