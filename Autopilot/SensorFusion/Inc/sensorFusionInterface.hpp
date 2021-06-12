#ifndef SENSOR_FUSION_INTERFACE_H
#define SENSOR_FUSION_INTERFACE_H

#include "SensorFusion.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void SensorFusionInterfaceInit(void);
SFError_t SensorFusionInterfaceExecute(void);

#ifdef __cplusplus
}
#endif 

#endif
