
#include "cruisingState.hpp"

 /*
    Integer codes from the telemetryData object:

        waypointModifyFlightPathCommand:
            0 = nothing
            1 = initialize flight path
            2 = append waypoint
            3 = insert waypoint
            4 = update waypoint
            5 = delete waypoint
            6 = nuke flight path (remove all waypoints)

        waypointNextDirectionsCommand:
            0 = nothing (regular path following)
            1 = start/end holding
            2 = head home
        
        initializingHomeBase:
            1 = yes
            2 = no

    Integer codes for error codes:
        editingFlightPathErrorCode:
            0 = success
            1 = error
            2 = incorrect telemetry command

        pathFollowingErrorCode:
            0 = success
            1 = error
            2 = incorrect telemetry command
            3 = homebase not initialized
*/


int editFlightPath(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager) {
    _WaypointOutputType waypointType = PATH_FOLLOW;
    _WaypointStatus editingStatus = WAYPOINT_SUCCESS;

    /* Editing the flight path */

    if (telemetryData->numWaypoints == 1 && (telemetryData->waypointModifyFlightPathCommand >= 2 && telemetryData->waypointModifyFlightPathCommand <= 5)) { // Inserting, Appending, Updating, or Deleting a waypoint
        
        // Set the output type of the new waypoint. Need if statements because of the enum parameter of the initialize_waypoint() method
        if (telemetryData->waypoints[0]->waypointType == 0) {
            waypointType = PATH_FOLLOW;
        } else if (telemetryData->waypoints[0]->waypointType == 1) {
            waypointType = ORBIT_FOLLOW;
        } else {
            waypointType = HOLD_WAYPOINT;
        }

        // Depending on the waypointType of the waypoint, we will need to call a different initialize_waypoint() method
        _PathData * modifyWaypoint;
        if (waypointType == PATH_FOLLOW) {
            modifyWaypoint = cruisingStateManager.initialize_waypoint(telemetryData->waypoints[0]->longitude, telemetryData->waypoints[0]->latitude, telemetryData->waypoints[0]->altitude, waypointType); 
        } else {
            modifyWaypoint = cruisingStateManager.initialize_waypoint(telemetryData->waypoints[0]->longitude, telemetryData->waypoints[0]->latitude, telemetryData->waypoints[0]->altitude, waypointType, telemetryData->waypoints[0]->turnRadius); // Create a _PathData object
        }
        
        // Update flight path by passing in the appropriate parameters to update_path_nodes()
        if (telemetryData->waypointModifyFlightPathCommand == 2) { // Append
            editingStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, APPEND_WAYPOINT, 0, 0, 0);
        } else if (telemetryData->waypointModifyFlightPathCommand == 3) { // Insert
            editingStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, INSERT_WAYPOINT, 0, telemetryData->prevId, telemetryData->nextId);
        } else if (telemetryData->waypointModifyFlightPathCommand == 4) { // Update
            editingStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, UPDATE_WAYPOINT, telemetryData->modifyId, 0, 0);
        } else { // Delete
            editingStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, DELETE_WAYPOINT, telemetryData->modifyId, 0, 0);
        }
    } else if (telemetryData->numWaypoints > 1 && telemetryData->waypointModifyFlightPathCommand == 1) { // Initialize flight path array

        _PathData * newFlightPath[PATH_BUFFER_SIZE];

        for(int i = 0; i < telemetryData->numWaypoints && i < PATH_BUFFER_SIZE; i++) {
            // Set the output type of the new waypoint. Need if statements because of the enum parameter of the initialize_waypoint() method
            if (telemetryData->waypoints[i]->waypointType == 0) {
                waypointType = PATH_FOLLOW;
            } else if (telemetryData->waypoints[i]->waypointType == 1) {
                waypointType = ORBIT_FOLLOW;
            } else {
                waypointType = HOLD_WAYPOINT;
            }
            
            // Depending on the waypointType of the waypoint, we will need to call a different initialize_waypoint() method
            if (waypointType == PATH_FOLLOW) {
                newFlightPath[i] = cruisingStateManager.initialize_waypoint(telemetryData->waypoints[i]->longitude, telemetryData->waypoints[i]->latitude, telemetryData->waypoints[i]->altitude, waypointType); 
            } else {
                newFlightPath[i] = cruisingStateManager.initialize_waypoint(telemetryData->waypoints[i]->longitude, telemetryData->waypoints[i]->latitude, telemetryData->waypoints[i]->altitude, waypointType, telemetryData->waypoints[i]->turnRadius); 
            }
        }

        cruisingStateManager.clear_path_nodes(); // Nukes current flight path (ensures there are no memory leaks)

        if (telemetryData->initializingHomeBase) { // If we are initializing the home base object too
            cruisingStateManager.clear_home_base(); // Remove current home base

            waypointType = HOLD_WAYPOINT; // Set the output type of the new waypoint. Need if statements because of the enum parameter of the initialize_waypoint() method

            // The homebase will be a hold waypoint, so no selection required!
            _PathData * newHomeBase = cruisingStateManager.initialize_waypoint(telemetryData->homebase.longitude, telemetryData->homebase.latitude, telemetryData->homebase.altitude, waypointType, telemetryData->homebase.turnRadius); 
    
            editingStatus = cruisingStateManager.initialize_flight_path(newFlightPath, telemetryData->numWaypoints, newHomeBase);
        } else { // Only initializing the flight path
            editingStatus = cruisingStateManager.initialize_flight_path(newFlightPath, telemetryData->numWaypoints);
        }
    } else if (telemetryData->numWaypoints == 0 && telemetryData->waypointModifyFlightPathCommand == 6) {  // Nuke flight path
        cruisingStateManager.clear_path_nodes();
    } else if (telemetryData->numWaypoints != 0) { // Incorrect commands from telemetry
        // Set important values to their defaults. This will ensure that if the telemetry struct is not change, our plane will behave as expected
        telemetryData->waypointModifyFlightPathCommand = 0; 
        telemetryData->numWaypoints = 0; 
        telemetryData->initializingHomeBase = 0; 
        
        return 2;
    }

    // Set important values to their defaults. This will ensure that if the telemetry struct is not change, our plane will behave as expected
    telemetryData->waypointModifyFlightPathCommand = 0; 
    telemetryData->numWaypoints = 0; 
    telemetryData->initializingHomeBase = 0; 

    // Return appropriate error code
    if (editingStatus == WAYPOINT_SUCCESS) {
        return 0;
    } else {
        return 1;
    }
} 

int pathFollow(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager, _WaypointManager_Data_In input, _WaypointManager_Data_Out * output, bool& goingHome, bool& inHold) {
    _WaypointStatus pathFollowingStatus = WAYPOINT_SUCCESS;
    _HeadHomeStatus goingHomeStatus = HOME_TRUE;


    if (telemetryData->waypointNextDirectionsCommand == 0) { // Regular path following
        pathFollowingStatus = cruisingStateManager.get_next_directions(input, output);
    } else if (telemetryData->waypointNextDirectionsCommand == 1) { // Holding pattern
        // Sees if the plane is currently circling. If it is, then we will cancel circling. Else, start turning
        if (inHold) {
            inHold = false;
        } else {
            inHold = true;
        }

        pathFollowingStatus = cruisingStateManager.start_circling(input, telemetryData->holdingTurnRadius, telemetryData->holdingTurnDirection, telemetryData->holdingAltitude, inHold);
    } else if (telemetryData->waypointNextDirectionsCommand == 2) { // Heading home
        // Sees if the plane is currently going home. If it is, then we will cancel. Else, start going home
        if (goingHome) {
            goingHome = false;
        } else {
            goingHome = true;
        }

        goingHomeStatus = cruisingStateManager.head_home(goingHome);
    } else { // Incorrect commands from telemetry
        telemetryData->waypointNextDirectionsCommand = 0; // Set important values to their defaults. This will ensure that if the telemetry struct is not change, our plane will behave as expected

        return 2;
    }

    telemetryData->waypointNextDirectionsCommand = 0;  // Set important values to their defaults. This will ensure that if the telemetry struct is not change, our plane will behave as expected

    // Return appropriate error code
    if (pathFollowingStatus == WAYPOINT_SUCCESS) {
        return 0;
    } else if (goingHomeStatus == HOME_UNDEFINED_PARAMETER) {
        return 3;
    } else {
        return 1;
    }
}

void setReturnValues(_CruisingState_Telemetry_Return * _returnToGround, WaypointManager& cruisingStateManager, int editErrorCode, int pathErrorCode) {
    _returnToGround->currentWaypointId = cruisingStateManager.get_id_of_current_index();
    _returnToGround->currentWaypointIndex = cruisingStateManager.get_current_index();

    _returnToGround->editingFlightPathErrorCode = editErrorCode;
    _returnToGround->pathFollowingErrorCode = pathErrorCode;
}




