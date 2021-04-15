/*
* Declares the functions that interact with the Waypoint Manager (on behalf of the cruisingState state)
* Author: Dhruv Rawat
*/

#include "pathDatatypes.hpp"
#include "waypointManager.hpp"

#ifndef CRUISING_STATE
#define CRUISING_STATE

enum _ModifyFlightPathErrorCode { MODIFY_CRUISING_SUCCESS = 0, MODIFY_CRUISING_ERROR, MODIFY_CRUISING_INCORRECT_TELEMETRY_COMMAND };
enum _GetNextDirectionsErrorCode { PATH_CRUISING_SUCCESS = 0, PATH_CRUISING_ERROR, PATH_CRUISING_INCORRECT_TELEMETRY_COMMAND, PATH_CRUISING_UNINITIALIZED_HOMEBASE };

struct _CruisingState_Telemetry_Return {
    char editingFlightPathErrorCode; // 0 = success, 1 = error, 2 = incorrect telemetry command
    char pathFollowingErrorCode; // 0 = success, 1 = error, 2 = home base not initialized, 3 = incorrect telemetry command
    int currentWaypointId; 
    int currentWaypointIndex;
    bool homeBaseInitialized;
};

// The following functions are used to interact with the Waypoint Manager module
_ModifyFlightPathErrorCode editFlightPath(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager, int * idArray);

_GetNextDirectionsErrorCode pathFollow(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager, _WaypointManager_Data_In input, _WaypointManager_Data_Out * output, bool& goingHome, bool& inHold);

void setReturnValues(_CruisingState_Telemetry_Return * _returnToGround, WaypointManager& cruisingStateManager, _ModifyFlightPathErrorCode editErrorCode, _GetNextDirectionsErrorCode pathErrorCode);

// The following functions are used to update the ID array that is a part of the CruisingState class
void appendNewElement(int * idArray, int newId);
int indexOfDesiredId(int * idArray, int id);
void insertNewElement(int * idArray, int prevId, int newId);
void updateElement(int * idArray, int oldid, int newId);
void removeElement(int * idArray, int id);
void clearArray(int * idArray);


#endif

