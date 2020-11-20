/**
 * Waypoint Manager Methods and Helpers Implementation
 * Author: Dhruv Rawat
 * Created: November 2020
 * Last Updated: November 2020 (Dhruv)
 */

#include "waypointManager.hpp"


/*** INITIALIZATION ***/


WaypointManager::WaypointManager(_PathData *initialWaypoints, int numberOfWaypoints, _PathData *currentLocation) {
    
    homeBase = _PathData(); //Initializes homebase first so there are no undefined behaviour issues
    homeBase = *currentLocation;

    numWaypoints = numberOfWaypoints;
    
    // Initializes the waypointBuffer array
    for (int i = 0; i < numWaypoints; i++) {
        // Links waypoints together
        if (i == 0) { // If first waypoint, link to next one only
            initialWaypoints[i].next = &initialWaypoints[i+1]; // Passes address so any updates in initialWaypoints[i+1] is reflected in initialWaypoints[i].next
        } else if (i == numWaypoints - 1) { // If last waypoint, link to previous one only
            initialWaypoints[i].previous = &initialWaypoints[i-1];
        } else {
            initialWaypoints[i].next = &initialWaypoints[i+1];
            initialWaypoints[i].previous = &initialWaypoints[i-1];
        }
        
        waypointBuffer[i] = initialWaypoints[i]; // Sets the element in the waypointBuffer
        waypointBufferStatus[i] = FULL;
        lastFilledIndex = i;
    }

    errorStatus = WAYPOINT_SUCCESS;
}


/*** UNIVERSAL HELPERS ***/


int WaypointManager::get_waypoint_index_from_id(int waypointId) {
    for (int i = 0; i < PATH_BUFFER_SIZE; i++) {
        if(waypointBufferStatus[i] = FREE) { // If array is empty at the specified index, waypoint is not in buffer
            return -1; 
        }
        if(waypointBuffer[i].waypointId == waypointId) { // If waypoint is found, return index
            return i;
        }
    }

    return -1; // If waypoint is not found
}

void WaypointManager::get_coordinates(long double longitude, long double latitude, float* xyCoordinates) {
    xyCoordinates[0] = get_distance(RELATIVE_LATITUDE, RELATIVE_LONGITUDE, RELATIVE_LATITUDE, longitude); //Calculates longitude (x coordinate) relative to defined origin (RELATIVE_LONGITUDE, RELATIVE_LATITUDE)
    xyCoordinates[1] = get_distance(RELATIVE_LATITUDE, RELATIVE_LONGITUDE, latitude, RELATIVE_LONGITUDE); //Calculates latitude (y coordinate) relative to defined origin (RELATIVE_LONGITUDE, RELATIVE_LATITUDE)
}

float WaypointManager::get_distance(long double lat1, long double lon1, long double lat2, long double lon2) {
    long double dLat = deg2rad(lat2 - lat1);
    long double dLon = deg2rad(lon2 - lon1);

    float a = sin(dLat / 2) * sin(dLat / 2) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * sin(dLon / 2) * sin(dLon / 2);

    if ((dLat >= 0 && dLon >=0)||(dLat < 0 && dLon < 0)){
        return EARTH_RADIUS * (2 * atan2(sqrt(a),sqrt(1 - a))) * 1000;
    }
    else {
         return EARTH_RADIUS * (2 * atan2(sqrt(a),sqrt(1 - a))) * -1000;
    }
}


/*** NAVIGATION ***/


_WaypointStatus WaypointManager::get_next_directions(_WaypointManager_Data_In currentStatus, _WaypointManager_Data_Out *Data) {
    if (goingHome) { // If plane was instructed to go back to base (and is awaiting for waypointBuffer to be updated)
        // Do stuff
        dataIsNew = true;
        update_return_data(Data); // Updates the return structure
        return errorCode;
    } 

    if (inHold) {   // If plane is currently circling and waiting for commands
        // Do stuff
        dataIsNew = true;
        update_return_data(Data); // Updates the return structure
        return errorCode;
    }

    dataIsNew = true;
    update_return_data(Data); // Updates the return structure
    return errorCode;
}

void WaypointManager::update_return_data(_WaypointManager_Data_Out *Data) {
    Data->desiredHeading = desiredHeading;
    Data->desiredAltitude = desiredAltitude;
    Data->distanceToNextWaypoint = distanceToNextWaypoint;
    Data->radius = turnRadius;
    Data->turnDirection = turnDirection;
    Data->errorCode = errorCode;
    Data->isDataNew = dataIsNew;
    dataIsNew = false;
    
    // Not setting time of data yet bc I think we need to come up with a way to get it???
}


void WaypointManager::start_circling(float radius, int direction, bool cancelTurning) {
    if (!cancelTurning) {
        inHold = true;
    } else {
        inHold = false;
    }

    turnRadius = radius;
    turnDirection = direction;
}

void WaypointManager::head_home() {
    if (!goingHome) {
        clear_path_nodes(); // Clears path nodes so state machine can input new flight path
        goingHome = true;
    } else {
        goingHome = false;
    }
    
}


/*** FLIGHT PATH MANAGEMENT ***/


_WaypointStatus WaypointManager::update_path_nodes(_PathData* waypoint, _WaypointBufferUpdateType updateType, int numWaypoints, int waypointId = 0, int previousId = 0, int nextId = 0) {
    // Conducts a different operation based on the update type
    if (updateType = APPEND_WAYPONT) {


        errorCode = WAYPOINT_SUCCESS;
    } else if (updateType = INSERT_WAYPOINT) {


        errorCode = WAYPOINT_SUCCESS;
    } else if (updateType = UPDATE_WAYPOINT) {


        errorCode = WAYPOINT_SUCCESS;
    } else if (updateType = DELETE_WAYPOINT) {


        errorCode = WAYPOINT_SUCCESS;
    }

    return errorCode;
}

void WaypointManager::clear_path_nodes() {
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        if (waypointBufferStatus[i]) { // If array element has a waypoint in it
            int destroyedId = destroy_waypoint(&waypointBuffer[i]); // Remove waypoint from the heap
        }
        waypointBufferStatus[i] = FREE; // Set array element free
        waypointBuffer[i] = _PathData(); //Set buffer element to empty struct
    }
    // Resets buffer status variables
    numWaypoints = 0; 
    lastFilledIndex = 0; 
    currentWaypointId = 0;
    currentIndex = 0;
}

int WaypointManager::destroy_waypoint(_PathData *waypoint) {
    int destroyedId = waypoint->waypointId;
    delete waypoint;
    return destroyedId;
}
