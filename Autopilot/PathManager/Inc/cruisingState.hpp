#include "pathDatatypes.hpp"
#include "waypointManager.hpp"

#ifndef CRUISING_STATE
#define CRUISING_STATE

int editFlightPath(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager);

int pathFollow(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager, _WaypointManager_Data_In input, _WaypointManager_Data_Out * output, bool& goingHome, bool& inHold);

void setReturnValues(_CruisingState_Telemetry_Return * _returnToGround, WaypointManager& cruisingStateManager, int editErrorCode, int pathErrorCode);

#endif

