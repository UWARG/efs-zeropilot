/*
* Declares the functions that interact with the Waypoint Manager (on behalf of the cruisingState state)
* Author: Dhruv Rawat
*/

#include "pathDatatypes.hpp"
#include "TelemPathInterface.hpp"
#include "waypointManager.hpp"
#include <cstdint>

#ifndef CRUISING_STATE
#define CRUISING_STATE

enum _ModifyFlightPathErrorCode { MODIFY_CRUISING_SUCCESS = 0, MODIFY_CRUISING_ERROR, MODIFY_CRUISING_INCORRECT_TELEMETRY_COMMAND };
enum _GetNextDirectionsErrorCode { PATH_CRUISING_SUCCESS = 0, PATH_CRUISING_ERROR, PATH_CRUISING_INCORRECT_TELEMETRY_COMMAND, PATH_CRUISING_UNINITIALIZED_HOMEBASE };

// Struct contains the data that will be returned via telemetry to ground station
struct _CruisingState_Telemetry_Return {
    uint8_t editingFlightPathErrorCode; // 0 = success, 1 = error, 2 = incorrect telemetry command
    uint8_t pathFollowingErrorCode; // 0 = success, 1 = error, 2 = home base not initialized, 3 = incorrect telemetry command
    int currentWaypointId;
    int currentWaypointIndex;
    bool homeBaseInitialized;
};

/**
 * Function performs any requested modifications on the flight path including appending, inserting, deleting, updating, initializing, and nuking
 *
 * @param telemetryData        --> contains the most recent commands from telemetry
 * @param cruisingStateManager --> this is the waypointManager class object for the cruisingState
 * @param idArray              --> an array of integers that is used to keep track of the ids of the waypoints in the flight path
 *
 * @return error code indicating success of operation
 */
_ModifyFlightPathErrorCode editFlightPath(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager, int * idArray);

/**
 * Function retrieves the next desired path for the aircraft
 *
 * @param telemetryData        --> contains the most recent commands from telemetry
 * @param cruisingStateManager --> this is the waypointManager class object for the cruisingState
 * @param input                --> struct contains inputs required to calculate the desired path for the aircraft
 * @param output               --> struct collects the output data from the Waypoint Manager for use in other states
 * @param goingHome            --> boolean flag that is used to indicate if we want to start/stop holding
 * @param inHold               --> boolean flag that is used to indicate if we want to start/stop heading back to our home base
 *
 * @return error code indicating success of operation
 */
_GetNextDirectionsErrorCode pathFollow(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager, _WaypointManager_Data_In input, _WaypointManager_Data_Out * output, bool& goingHome, bool& inHold);

/**
 * Function sets the _CruisingState_Telemetry_Return struct which will be sent to base via telemetry
 *
 * @param _returnToGround      --> contains the info we are sending back to telemetry
 * @param cruisingStateManager --> this is the waypointManager class object for the cruisingState
 * @param editErrorCode        --> error code that was returned by editFlightPath() on previous call
 * @param pathErrorCode        --> error code that was returned by pathFollow() on previous call
 */
void setReturnValues(_CruisingState_Telemetry_Return * _returnToGround, WaypointManager& cruisingStateManager, _ModifyFlightPathErrorCode editErrorCode, _GetNextDirectionsErrorCode pathErrorCode);

#endif

