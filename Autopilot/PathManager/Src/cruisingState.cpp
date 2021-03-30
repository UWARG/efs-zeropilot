/*
* Defines the functions that interact with the Waypoint Manager
* Author: Dhruv Rawat
*/

#include "cruisingState.hpp"

// #include <iostream>
using namespace std;

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


int editFlightPath(Telemetry_PIGO_t * telemetryData, WaypointManager& cruisingStateManager, int * idArray) {
    
    // If no commands given, just skip over this function
    if (telemetryData->waypointModifyFlightPathCommand == 0) {        
        return 0;
    }
    
    _WaypointOutputType waypointType = PATH_FOLLOW;
    _WaypointStatus editingStatus = WAYPOINT_SUCCESS;

    /* Editing the flight path */

    if (telemetryData->numWaypoints == 1 && (telemetryData->waypointModifyFlightPathCommand >= 2 && telemetryData->waypointModifyFlightPathCommand <= 4)) { // Inserting, Appending, or Updating a waypoint
        
        // cout << "Editing flight path (Not deleting)" << endl;

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

            // cout << "Editing flight path: Append" << endl;

            editingStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, APPEND_WAYPOINT, 0, 0, 0);
            // cout << "Editing Status: " << editingStatus << endl;
            if (editingStatus == WAYPOINT_SUCCESS) { // Update ID array if edit was successful
                appendNewElement(idArray, modifyWaypoint->waypointId); // Update the idArray
            }

        } else if (telemetryData->waypointModifyFlightPathCommand == 3) { // Insert

            // cout << "Editing flight path: Insert" << endl;

            editingStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, INSERT_WAYPOINT, 0, telemetryData->prevId, telemetryData->nextId);

            if (editingStatus == WAYPOINT_SUCCESS) { // Update ID array if edit was successful
                insertNewElement(idArray, telemetryData->prevId, modifyWaypoint->waypointId); // Update the idArray
            }

        } else if (telemetryData->waypointModifyFlightPathCommand == 4) { // Update

            // cout << "Editing flight path: Update" << endl;

            editingStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, UPDATE_WAYPOINT, telemetryData->modifyId, 0, 0);
            
            if (editingStatus == WAYPOINT_SUCCESS) { // Update ID array if edit was successful
                updateElement(idArray, telemetryData->modifyId, modifyWaypoint->waypointId); // Update the idArray
            }

        } 
    } else if (telemetryData->numWaypoints == 0 && telemetryData->waypointModifyFlightPathCommand == 5) { // Deleting a waypoint

        // cout << "Editing flight path: Delete" << endl;

        editingStatus = cruisingStateManager.update_path_nodes(nullptr, DELETE_WAYPOINT, telemetryData->modifyId, 0, 0);
        
        if (editingStatus == WAYPOINT_SUCCESS) { // Update ID array if edit was successful
            removeElement(idArray, telemetryData->modifyId); // Update the idArray
        }

    } else if (telemetryData->numWaypoints > 1 && telemetryData->waypointModifyFlightPathCommand == 1) { // Initialize flight path array

        // cout << "Initialize flight path" << endl;

        cruisingStateManager.clear_path_nodes(); // Nukes current flight path (ensures there are no memory leaks)
        clearArray(idArray);

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

            // cout << newFlightPath[i]->waypointId << endl;
            appendNewElement(idArray, newFlightPath[i]->waypointId); // Append elements to the idArray while we go :))
            // cout << idArray[i] << endl;
        }

        if (telemetryData->initializingHomeBase) { // If we are initializing the home base object too
            // cout << "Homebase stuff" << endl;
            cruisingStateManager.clear_home_base(); // Remove current home base
            // cout << "Cleared home base" << endl;

            waypointType = HOLD_WAYPOINT; // Set the output type of the new waypoint. Need if statements because of the enum parameter of the initialize_waypoint() method

            // The homebase will be a hold waypoint, so no selection required!
            _PathData * newHomeBase = cruisingStateManager.initialize_waypoint(telemetryData->homebase.longitude, telemetryData->homebase.latitude, telemetryData->homebase.altitude, waypointType, telemetryData->homebase.turnRadius); 
    
            editingStatus = cruisingStateManager.initialize_flight_path(newFlightPath, telemetryData->numWaypoints, newHomeBase);
        } else { // Only initializing the flight path

            // cout << "Regular initialization (no home base)" << endl;

            editingStatus = cruisingStateManager.initialize_flight_path(newFlightPath, telemetryData->numWaypoints);
        }

        // If initializing the flight path was not successful, then we will clear our idArray to prevent future confusion
        if (editingStatus != WAYPOINT_SUCCESS) {
            clearArray(idArray);
        }

    } else if (telemetryData->numWaypoints == 0 && telemetryData->waypointModifyFlightPathCommand == 6) {  // Nuke flight path

        // cout << "Nuke flight path" << endl;

        cruisingStateManager.clear_path_nodes();
        clearArray(idArray);
    } else if (telemetryData->numWaypoints != 0) { // Incorrect commands from telemetry

        // cout << "Bad command " << endl;

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

    _WaypointStatus pathFollowingStatus = UNDEFINED_PARAMETER;
    _HeadHomeStatus goingHomeStatus = HOME_TRUE;

    if (telemetryData->waypointNextDirectionsCommand == 0) { // Regular path following

        // cout << "Flight Path: regular" << endl;

        pathFollowingStatus = cruisingStateManager.get_next_directions(input, output);
    } else if (telemetryData->waypointNextDirectionsCommand == 1) { // Holding pattern

        // cout << "Flight Path: circling/holding" << endl;

        pathFollowingStatus = cruisingStateManager.start_circling(input, telemetryData->holdingTurnRadius, telemetryData->holdingTurnDirection, telemetryData->holdingAltitude, inHold);
        
        // Updates the in hold flag if execution was successful
        if (inHold && pathFollowingStatus == WAYPOINT_SUCCESS) {
            inHold = false;
        } else if (!inHold && pathFollowingStatus == WAYPOINT_SUCCESS) {
            inHold = true;
        }
    } else if (telemetryData->waypointNextDirectionsCommand == 2) { // Heading home

        // cout << "Flight Path: going home" << endl;

        // Sees if the plane is currently going home. If it is, then we will cancel. Else, start going home
        if (goingHome) {
            goingHome = false;
        } else {
            goingHome = true;
        }

        goingHomeStatus = cruisingStateManager.head_home(goingHome);

        if (goingHomeStatus == HOME_UNDEFINED_PARAMETER && goingHome) { // If setting home mode fails, reverse change
            goingHome = false;
            pathFollowingStatus = UNDEFINED_PARAMETER;
        } else if (goingHomeStatus == HOME_TRUE || goingHomeStatus == HOME_FALSE) {
            pathFollowingStatus = WAYPOINT_SUCCESS;
        }
    } else { // Incorrect commands from telemetry

        // cout << "Flight Path: incorrect command" << endl;

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
    _returnToGround->homeBaseInitialized = cruisingStateManager.is_home_base_initialized();

    _returnToGround->editingFlightPathErrorCode = editErrorCode;
    _returnToGround->pathFollowingErrorCode = pathErrorCode;
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


