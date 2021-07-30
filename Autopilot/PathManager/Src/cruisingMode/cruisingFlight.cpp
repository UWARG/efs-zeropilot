/*
* Defines the functions that interact with the Waypoint Manager
* Author: Dhruv Rawat
*/

#include "cruisingFlight.hpp"

// The following functions are used to update the ID array that is a part of the CruisingState class
static void appendNewElement(int * idArray, int newId);             // Adds newId to the first free element in idArray
static int indexOfDesiredId(int * idArray, int id);                 // Returns the index of id in idArray
static void insertNewElement(int * idArray, int prevId, int newId); // Inserts newId after prevId in idArray
static void updateElement(int * idArray, int oldId, int newId);     // Replaces oldId with newId in idArray
static void removeElement(int * idArray, int id);                   // Removes id from idArray
static void clearArray(int * idArray);                              // Resets all elements in idArray to 0


_ModifyFlightPathErrorCode editFlightPath(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager, int * idArray) {
    
    // If no commands given, just skip over this function
    if (telemetryData->waypointModifyFlightPathCommand == NO_FLIGHT_PATH_EDIT) {        
        return MODIFY_CRUISING_SUCCESS;
    }
    
    _WaypointOutputType waypointType = PATH_FOLLOW;
    _WaypointStatus editingStatus = WAYPOINT_SUCCESS;

    /* Editing the flight path */

    if (telemetryData->numWaypoints == 1 && (telemetryData->waypointModifyFlightPathCommand >= 2 && telemetryData->waypointModifyFlightPathCommand <= 4)) { // Inserting, Appending, or Updating a waypoint

        // Set the output type of the new waypoint. Need if statements because of the enum parameter of the initialize_waypoint() method
        if (telemetryData->waypoints[0].waypointType == 0) {
            waypointType = PATH_FOLLOW;
        } else if (telemetryData->waypoints[0].waypointType == 1) {
            waypointType = ORBIT_FOLLOW;
        } else {
            waypointType = HOLD_WAYPOINT;
        }

        // Depending on the waypointType of the waypoint, we will need to call a different initialize_waypoint() method
        _PathData * modifyWaypoint;
        if (waypointType == PATH_FOLLOW) {
            modifyWaypoint = cruisingStateManager.initialize_waypoint(telemetryData->waypoints[0].longitude, telemetryData->waypoints[0].latitude, telemetryData->waypoints[0].altitude, waypointType); 
        } else {
            modifyWaypoint = cruisingStateManager.initialize_waypoint(telemetryData->waypoints[0].longitude, telemetryData->waypoints[0].latitude, telemetryData->waypoints[0].altitude, waypointType, telemetryData->waypoints[0].turnRadius); // Create a _PathData object
        }
        
        // Update flight path by passing in the appropriate parameters to update_path_nodes()
        if (telemetryData->waypointModifyFlightPathCommand == APPEND) { // Append

            editingStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, APPEND_WAYPOINT, 0, 0, 0);

            if (editingStatus == WAYPOINT_SUCCESS) { // Update ID array if edit was successful
                appendNewElement(idArray, modifyWaypoint->waypointId); // Update the idArray
            }

        } else if (telemetryData->waypointModifyFlightPathCommand == INSERT) { // Insert

            editingStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, INSERT_WAYPOINT, 0, telemetryData->prevId, telemetryData->nextId);

            if (editingStatus == WAYPOINT_SUCCESS) { // Update ID array if edit was successful
                insertNewElement(idArray, telemetryData->prevId, modifyWaypoint->waypointId); // Update the idArray
            }

        } else if (telemetryData->waypointModifyFlightPathCommand == UPDATE) { // Update

            editingStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, UPDATE_WAYPOINT, telemetryData->modifyId, 0, 0);
            
            if (editingStatus == WAYPOINT_SUCCESS) { // Update ID array if edit was successful
                updateElement(idArray, telemetryData->modifyId, modifyWaypoint->waypointId); // Update the idArray
            }

        } 
    } else if (telemetryData->numWaypoints == 0 && telemetryData->waypointModifyFlightPathCommand == DELETE) { // Deleting a waypoint

        editingStatus = cruisingStateManager.update_path_nodes(nullptr, DELETE_WAYPOINT, telemetryData->modifyId, 0, 0);
        
        if (editingStatus == WAYPOINT_SUCCESS) { // Update ID array if edit was successful
            removeElement(idArray, telemetryData->modifyId); // Update the idArray
        }

    } else if (telemetryData->numWaypoints > 1 && telemetryData->waypointModifyFlightPathCommand == INITIALIZE_FLIGHT_PATH) { // Initialize flight path array

        cruisingStateManager.clear_path_nodes(); // Nukes current flight path (ensures there are no memory leaks)
        clearArray(idArray);

        _PathData * newFlightPath[PATH_BUFFER_SIZE];

        for(int i = 0; i < telemetryData->numWaypoints && i < PATH_BUFFER_SIZE; i++) {
            // Set the output type of the new waypoint. Need if statements because of the enum parameter of the initialize_waypoint() method
            if (telemetryData->waypoints[i].waypointType == 0) {
                waypointType = PATH_FOLLOW;
            } else if (telemetryData->waypoints[i].waypointType == 1) {
                waypointType = ORBIT_FOLLOW;
            } else {
                waypointType = HOLD_WAYPOINT;
            }
            
            // Depending on the waypointType of the waypoint, we will need to call a different initialize_waypoint() method
            if (waypointType == PATH_FOLLOW) {
                newFlightPath[i] = cruisingStateManager.initialize_waypoint(telemetryData->waypoints[i].longitude, telemetryData->waypoints[i].latitude, telemetryData->waypoints[i].altitude, waypointType); 
            } else {
                newFlightPath[i] = cruisingStateManager.initialize_waypoint(telemetryData->waypoints[i].longitude, telemetryData->waypoints[i].latitude, telemetryData->waypoints[i].altitude, waypointType, telemetryData->waypoints[i].turnRadius); 
            }

            appendNewElement(idArray, newFlightPath[i]->waypointId); // Append elements to the idArray while we go :))
        }

        if (telemetryData->initializingHomeBase) { // If we are initializing the home base object too

            cruisingStateManager.clear_home_base(); // Remove current home base

            waypointType = HOLD_WAYPOINT; // Set the output type of the new waypoint. Need if statements because of the enum parameter of the initialize_waypoint() method

            // The homebase will be a hold waypoint, so no selection required!
            _PathData * newHomeBase = cruisingStateManager.initialize_waypoint(telemetryData->homebase.longitude, telemetryData->homebase.latitude, telemetryData->homebase.altitude, waypointType, telemetryData->homebase.turnRadius); 
    
            editingStatus = cruisingStateManager.initialize_flight_path(newFlightPath, telemetryData->numWaypoints, newHomeBase);
        } else { // Only initializing the flight path
            editingStatus = cruisingStateManager.initialize_flight_path(newFlightPath, telemetryData->numWaypoints);
        }

        // If initializing the flight path was not successful, then we will clear our idArray to prevent future confusion
        if (editingStatus != WAYPOINT_SUCCESS) {
            clearArray(idArray);
        }

    } else if (telemetryData->numWaypoints == 0 && telemetryData->waypointModifyFlightPathCommand == NUKE) {  // Nuke flight path
        cruisingStateManager.clear_path_nodes();
        clearArray(idArray);
    } else if (telemetryData->numWaypoints != 0) { // Incorrect commands from telemetry
        // Set important values to their defaults. This will ensure that if the telemetry struct is not change, our plane will behave as expected
        telemetryData->waypointModifyFlightPathCommand = NO_FLIGHT_PATH_EDIT; 
        telemetryData->numWaypoints = 0; 
        telemetryData->initializingHomeBase = 0; 
    
        return MODIFY_CRUISING_INCORRECT_TELEMETRY_COMMAND;
    }

    // Set important values to their defaults. This will ensure that if the telemetry struct is not change, our plane will behave as expected
    telemetryData->waypointModifyFlightPathCommand = NO_FLIGHT_PATH_EDIT; 
    telemetryData->numWaypoints = 0; 
    telemetryData->initializingHomeBase = 0; 

    // Return appropriate error code
    if (editingStatus == WAYPOINT_SUCCESS) {
        return MODIFY_CRUISING_SUCCESS;
    } else {
        return MODIFY_CRUISING_ERROR;
    }
} 

_GetNextDirectionsErrorCode pathFollow(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager, _WaypointManager_Data_In input, _WaypointManager_Data_Out * output, bool& goingHome, bool& inHold) {

    _WaypointStatus pathFollowingStatus = UNDEFINED_PARAMETER;
    _HeadHomeStatus goingHomeStatus = HOME_TRUE;

    if (telemetryData->waypointNextDirectionsCommand == REGULAR_PATH_FOLLOWING) { // Regular path following

        pathFollowingStatus = cruisingStateManager.get_next_directions(input, output);

        output->desiredAirspeed = CRUISING_AIRSPEED;

    } else if (telemetryData->waypointNextDirectionsCommand == TOGGLE_HOLDING) { // Holding pattern

        pathFollowingStatus = cruisingStateManager.start_circling(input, telemetryData->holdingTurnRadius, telemetryData->holdingTurnDirection, telemetryData->holdingAltitude, inHold);

        output->desiredAirspeed = CRUISING_AIRSPEED;
        
        // Updates the in hold flag if execution was successful
        if (inHold && pathFollowingStatus == WAYPOINT_SUCCESS) {
            inHold = false;
        } else if (!inHold && pathFollowingStatus == WAYPOINT_SUCCESS) {
            inHold = true;
        }
    } else if (telemetryData->waypointNextDirectionsCommand == TOGGLE_HEAD_HOME) { // Heading home

        // Sees if the plane is currently going home. If it is, then we will cancel. Else, start going home
        if (goingHome) {
            goingHome = false;
        } else {
            goingHome = true;
        }

        goingHomeStatus = cruisingStateManager.head_home(goingHome);

        output->desiredAirspeed = CRUISING_AIRSPEED;

        if (goingHomeStatus == HOME_UNDEFINED_PARAMETER && goingHome) { // If setting home mode fails, reverse change
            goingHome = false;
            pathFollowingStatus = UNDEFINED_PARAMETER;
        } else if (goingHomeStatus == HOME_TRUE || goingHomeStatus == HOME_FALSE) {
            pathFollowingStatus = WAYPOINT_SUCCESS;
        }
    } else { // Incorrect commands from telemetry
        telemetryData->waypointModifyFlightPathCommand = NO_FLIGHT_PATH_EDIT; // Set important values to their defaults. This will ensure that if the telemetry struct is not change, our plane will behave as expected
        
        return PATH_CRUISING_INCORRECT_TELEMETRY_COMMAND;
    }

    telemetryData->waypointModifyFlightPathCommand = NO_FLIGHT_PATH_EDIT;  // Set important values to their defaults. This will ensure that if the telemetry struct is not change, our plane will behave as expected

    // Return appropriate error code
    if (pathFollowingStatus == WAYPOINT_SUCCESS) {
        return PATH_CRUISING_SUCCESS;
    } else if (goingHomeStatus == HOME_UNDEFINED_PARAMETER) {
        return PATH_CRUISING_UNINITIALIZED_HOMEBASE;
    } else {
        return PATH_CRUISING_ERROR;
    }
}

void setReturnValues(_CruisingState_Telemetry_Return * _returnToGround, WaypointManager& cruisingStateManager, _ModifyFlightPathErrorCode editErrorCode, _GetNextDirectionsErrorCode pathErrorCode) {
    _returnToGround->currentWaypointId = cruisingStateManager.get_id_of_current_index();
    _returnToGround->currentWaypointIndex = cruisingStateManager.get_current_index();
    _returnToGround->homeBaseInitialized = cruisingStateManager.is_home_base_initialized();

    _returnToGround->editingFlightPathErrorCode = (uint8_t) editErrorCode;
    _returnToGround->pathFollowingErrorCode = (uint8_t) pathErrorCode;
}

void appendNewElement(int * idArray, int newId) {
    int counter = 0;
    bool appended = false;

    // Goes through array until empty index is found or we reach end of array
    while (!appended && counter < PATH_BUFFER_SIZE) {
        if (idArray[counter] == 0) { // If an element has a value of 0, it is empty
            idArray[counter] = newId;
            appended = true;
        }

        counter++;
    }
}

int indexOfDesiredId(int * idArray, int id) {
    int counter = 0;

    // Goes through array until desired index is found or we reach end of array
    while (counter < PATH_BUFFER_SIZE) {
        if (idArray[counter] == id) { 
            return counter;
        }
        counter++;
    }

    return -1;
}

void insertNewElement(int * idArray, int prevId, int newId) {
    int index = indexOfDesiredId(idArray, prevId);

    if (index != -1) {
        for (int i = PATH_BUFFER_SIZE - 1; i > index + 1; i--) {
            idArray[i] = idArray[i-1];
        }

        idArray[index + 1] = newId;
    }
}

void removeElement(int * idArray, int id) {
    int index = indexOfDesiredId(idArray, id);

    if (index != -1) {
           for (int i = index; i < PATH_BUFFER_SIZE - 1; i++) {
               idArray[i] = idArray[i+1];
           }
           idArray[PATH_BUFFER_SIZE - 1] = 0;
    }
}

void updateElement(int * idArray, int oldId, int newId) {
    int counter = 0;
    bool updated = false;

    // Goes through array until desired index is found or we reach end of array
    while (!updated && counter < PATH_BUFFER_SIZE) {
        if (idArray[counter] == oldId) { 
            idArray[counter] = newId;
            updated = true;
        }
        counter++;
    }
}

void clearArray(int * idArray) {
    for (int i = 0; i < PATH_BUFFER_SIZE; i++) {
        idArray[i] = 0;
    }
}


