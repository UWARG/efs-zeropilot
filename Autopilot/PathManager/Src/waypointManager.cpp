/**
 * Waypoint Manager Methods and Helpers Implementation
 * Author: Dhruv Rawat
 * Created: November 2020
 * Last Updated: November 2020 (Dhruv)
 */

#include "../Inc/waypointManager.hpp"

#define ORBIT_FOLLOWING 0
#define LINE_FOLLOWING 1

//Constants
#define EARTH_RADIUS 6378.137
#define MAX_PATH_APPROACH_ANGLE M_PI/2

//Basic Mathematical Conversions
#define deg2rad(angle_in_degrees) ((angle_in_degrees) * M_PI/180.0)
#define rad2deg(angle_in_radians) ((angle_in_radians) * 180.0/M_PI)

//Waypoint Types
#define DEFAULT_WAYPOINT 0 // Used to navigate flight path
#define WAYPOINT_USED 1 // Used by the waypint manager to signal that a waypoint has been used
#define HOLD_WAYPOINT 2 // Circle

// Latitude and longitude of starting area... will need to change accordingly
#define RELATIVE_LATITUDE 43.473004
#define RELATIVE_LONGITUDE -80.539678


/*** INITIALIZATION ***/


WaypointManager::WaypointManager() {
    nextAssignedId = 0;

    // Sets boolean variables
    inHold = false;
    orbiting = false; 
    goingHome = false;
    dataIsNew = false;

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        waypointBufferStatus[i] = FREE;
    }
}

_WaypointStatus WaypointManager::initialize_flight_path(_PathData ** initialWaypoints, int numberOfWaypoints, _PathData *currentLocation) {
    
    // The waypointBuffer array must be empty before we initialize the flight path
    if (numWaypoints != 0) {
        errorStatus = UNDEFINED_FAILURE;
        return errorStatus;
    }
    
    homeBase = currentLocation;
    
    numWaypoints = numberOfWaypoints;

    nextFilledIndex = 0;

    // Initializes the waypointBuffer array
    for (int i = 0; i < numWaypoints; i++) {
        waypointBuffer[i] = initialWaypoints[i]; // Sets the element in the waypointBuffer
        waypointBufferStatus[i] = FULL;
        nextFilledIndex = i + 1;
    }

    // Links waypoints together
    for (int i = 0; i < numWaypoints; i++) {
        if (i == 0) { // If first waypoint, link to next one only
            waypointBuffer[i]->next = waypointBuffer[i+1];
            waypointBuffer[i]->previous = nullptr;
        } else if (i == numWaypoints - 1) { // If last waypoint, link to previous one only
            waypointBuffer[i]->next = nullptr;
            waypointBuffer[i]->previous = waypointBuffer[i-1];
        } else {
            waypointBuffer[i]->next = waypointBuffer[i+1];
            waypointBuffer[i]->previous = waypointBuffer[i-1];
        }
    }

    #ifdef UNIT_TESTING
        for(int i = numWaypoints; i < PATH_BUFFER_SIZE; i++) {
            waypointBuffer[i] = initialize_waypoint();
        }
    #else
        for(int i = numWaypoints; i < PATH_BUFFER_SIZE; i++) {
            waypointBuffer[i] = nullptr;
        }
    #endif

    errorStatus = WAYPOINT_SUCCESS;
    return errorStatus;
}

_WaypointStatus WaypointManager::initialize_flight_path(_PathData ** initialWaypoints, int numberOfWaypoints) {

    // The waypointBuffer array must be empty before we initialize the flight path
    if (numWaypoints != 0) {
        errorStatus = UNDEFINED_FAILURE;
        return errorStatus;
    }

    numWaypoints = numberOfWaypoints;
    nextFilledIndex = 0;

    // Initializes the waypointBuffer array
    for (int i = 0; i < numWaypoints; i++) {
        waypointBuffer[i] = initialWaypoints[i]; // Sets the element in the waypointBuffer
        waypointBufferStatus[i] = FULL;
        nextFilledIndex = i + 1;
    }

    // Links waypoints together
    for (int i = 0; i < numWaypoints; i++) {
        if (i == 0) { // If first waypoint, link to next one only
            waypointBuffer[i]->next = waypointBuffer[i+1];
            waypointBuffer[i]->previous = nullptr;
        } else if (i == numWaypoints - 1) { // If last waypoint, link to previous one only
            waypointBuffer[i]->next = nullptr;
            waypointBuffer[i]->previous = waypointBuffer[i-1];
        } else {
            waypointBuffer[i]->next = waypointBuffer[i+1];
            waypointBuffer[i]->previous = waypointBuffer[i-1];
        }
    }

    #ifdef UNIT_TESTING
        for(int i = numWaypoints; i < PATH_BUFFER_SIZE; i++) {
            waypointBuffer[i] = initialize_waypoint();
        }
    #else
        for(int i = numWaypoints; i < PATH_BUFFER_SIZE; i++) {
            waypointBuffer[i] = nullptr;
        }
    #endif

    errorStatus = WAYPOINT_SUCCESS;
    return errorStatus;
}

_PathData* WaypointManager::initialize_waypoint_without_id() {
    _PathData* waypoint = new _PathData; // Create new waypoint in the heap
    waypoint->waypointId = 0; // Set ID and increment
    waypoint->latitude = -1;
    waypoint->longitude = -1;
    waypoint->altitude = -1;
    waypoint->waypointType = -1;
    waypoint->turnRadius = -1;
    // Set next and previous waypoints to empty for now
    waypoint->next = nullptr;
    waypoint->previous = nullptr;

    return waypoint;
}

_PathData* WaypointManager::initialize_waypoint() {
    _PathData* waypoint = new _PathData; // Create new waypoint in the heap
    waypoint->waypointId = nextAssignedId++; // Set ID and increment
    waypoint->latitude = -1;
    waypoint->longitude = -1;
    waypoint->altitude = -1;
    waypoint->waypointType = -1;
    waypoint->turnRadius = -1;
    // Set next and previous waypoints to empty for now
    waypoint->next = nullptr;
    waypoint->previous = nullptr;

    return waypoint;
}

_PathData* WaypointManager::initialize_waypoint(long double longitude, long double latitude, int altitude, int waypointType) {
    _PathData* waypoint = new _PathData; // Create new waypoint in the heap
    waypoint->waypointId = nextAssignedId; // Set ID and increment
    nextAssignedId++;
    waypoint->latitude = latitude;
    waypoint->longitude = longitude;
    waypoint->altitude = altitude;
    waypoint->waypointType = waypointType;
    waypoint->turnRadius = -1;
    // Set next and previous waypoints to empty for now
    waypoint->next = nullptr;
    waypoint->previous = nullptr;

    return waypoint;
}

_PathData* WaypointManager::initialize_waypoint(long double longitude, long double latitude, int altitude, int waypointType, float turnRadius) {
    _PathData* waypoint = new _PathData; // Create new waypoint in the heap
    waypoint->waypointId = nextAssignedId; // Set ID and increment
    nextAssignedId++;
    waypoint->latitude = latitude;
    waypoint->longitude = longitude;
    waypoint->altitude = altitude;
    waypoint->waypointType = waypointType;
    waypoint->turnRadius = turnRadius;
    // Set next and previous waypoints to empty for now
    waypoint->next = nullptr;
    waypoint->previous = nullptr;

    return waypoint;
}


/*** UNIVERSAL HELPERS (universal to this file, ofc) ***/


int WaypointManager::get_waypoint_index_from_id(int waypointId) {
    for (int i = 0; i < PATH_BUFFER_SIZE; i++) {
        if(waypointBufferStatus[i] == FREE) { // If array is empty at the specified index, waypoint is not in buffer
            return -1;
        }
        if(waypointBuffer[i]->waypointId == waypointId) { // If waypoint is found, return index
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
    // Longitude and latitude stored in degrees
    // This calculation uses the Haversine formula
    long double change_in_Lat = deg2rad(lat2 - lat1); //Converts change in latitude to radians
    long double change_in_lon = deg2rad(lon2 - lon1); //Converts change in longitude to radians

    double haversine_ans = sin(change_in_Lat / 2) * sin(change_in_Lat / 2) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * sin(change_in_lon / 2) * sin(change_in_lon / 2); // In kilometers

    if ((change_in_Lat >= 0 && change_in_lon >=0)||(change_in_Lat < 0 && change_in_lon < 0)){
        return EARTH_RADIUS * (2 * atan2(sqrt(haversine_ans),sqrt(1 - haversine_ans))) * 1000; //Multiply by 1000 to convert to metres
    } else { // If result is negative.
        return EARTH_RADIUS * (2 * atan2(sqrt(haversine_ans),sqrt(1 - haversine_ans))) * -1000;
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

void WaypointManager::follow_waypoints(_PathData * currentWaypoint, float* position, float heading) {

}

void WaypointManager::follow_line_segment(_PathData * currentWaypoint, float* position, float heading) {

}

void WaypointManager::follow_last_line_segment(_PathData * currentWaypoint, float* position, float heading) {

}

void WaypointManager::follow_orbit(float* orbitCenter, float radius, char direction, float* position, float heading) {

}

void WaypointManager::follow_straight_path(float* waypointDirection, float* targetWaypoint, float* position, float heading) {

}

float WaypointManager::maintain_altitude(_PathData* currentPath) {

}


/*** FLIGHT PATH MANAGEMENT ***/


_WaypointStatus WaypointManager::update_path_nodes(_PathData * waypoint, _WaypointBufferUpdateType updateType, int waypointId, int previousId, int nextId) {
    if (numWaypoints == PATH_BUFFER_SIZE && (updateType == APPEND_WAYPOINT || updateType == INSERT_WAYPOINT)) { // If array is already full, if we insert it will overflow
        std::cout << "Gefefe" << std::endl;
        return UNDEFINED_FAILURE;
    }

    // std::cout << "Gefeferewrwefwee" << std::endl;

    // Conducts a different operation based on the update type
    if (updateType == APPEND_WAYPOINT) {
        errorCode = append_waypoint(waypoint);
    } else if (updateType == INSERT_WAYPOINT) {
        errorCode = insert_new_waypoint(waypoint, previousId, nextId);
    } else if (updateType == UPDATE_WAYPOINT) {
        errorCode = update_waypoint(waypoint, waypointId);
    } else if (updateType == DELETE_WAYPOINT) {
        errorCode = delete_waypoint(waypointId);
    }
    
    // std::cout << "Error Code: " << errorCode << std::endl;
    return errorCode;
}

void WaypointManager::clear_path_nodes() {
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        if (waypointBufferStatus[i] == FULL) { // If array element has a waypoint in it
            int destroyedId = destroy_waypoint(waypointBuffer[i]); // Remove waypoint from the heap
        }
        waypointBufferStatus[i] = FREE; // Set array element free
        waypointBuffer[i] = 0; //Set buffer element to empty struct
    }
    // Resets buffer status variables
    numWaypoints = 0;
    nextFilledIndex = 0;
    nextAssignedId = 0;
}

int WaypointManager::destroy_waypoint(_PathData *waypoint) {
    int destroyedId = waypoint->waypointId;
    delete waypoint; // Free heap memory
    return destroyedId;
}

_WaypointStatus WaypointManager::append_waypoint(_PathData * newWaypoint) {
    int previousIndex = 0;

    previousIndex = nextFilledIndex - 1;

    // Before initializing elements, checks if new waypoint is not a duplicate
    if (previousIndex != -1 && waypointBuffer[previousIndex]->latitude == newWaypoint->latitude && waypointBuffer[previousIndex]->longitude == newWaypoint->longitude) {
        // std::cout << "Here " << waypointBuffer[previousIndex]->latitude << " " <<  newWaypoint->latitude << " | " << waypointBuffer[previousIndex]->longitude  << " " <<  newWaypoint->longitude << std::endl;
        return UNDEFINED_FAILURE;
    }

    waypointBuffer[nextFilledIndex] = newWaypoint;
    waypointBufferStatus[nextFilledIndex] = FULL;

    if (previousIndex == -1) { //If we are initializing the first element
        nextFilledIndex++;
        numWaypoints++;

        return WAYPOINT_SUCCESS;
    }

    // Links previous waypoint with current one
    waypointBuffer[nextFilledIndex]->previous = waypointBuffer[previousIndex];
    waypointBuffer[previousIndex]->next = waypointBuffer[nextFilledIndex];

    nextFilledIndex++;
    numWaypoints++;

    return WAYPOINT_SUCCESS;
}

_WaypointStatus WaypointManager::insert_new_waypoint(_PathData* newWaypoint, int previousId, int nextId) {
    int nextIndex = get_waypoint_index_from_id(nextId);
    int previousIndex = get_waypoint_index_from_id(previousId);

    // std::cout << nextIndex << " " << previousIndex << std::endl;

    // If any of the waypoints could not be found. Or, if the two IDs do not correspond to adjacent elements in waypointBuffer[]
    if (nextIndex == -1 || previousIndex == -1 || nextIndex - 1 != previousIndex || nextIndex == 0){
        // std::cout << "Here1" << std::endl;
        return UNDEFINED_FAILURE;
    }

    // Adjusts array. Starts at second last element
    for (int i = PATH_BUFFER_SIZE - 2; i >= nextIndex; i--) {
        if (waypointBufferStatus[i] == FULL) { // If current element is initialized
            waypointBuffer[i+1] = waypointBuffer[i]; // Sets next element to current element
            waypointBufferStatus[i+1] = FULL; // Updates state array
        }
    }

    // Put new waypoint in buffer
    waypointBuffer[nextIndex] = newWaypoint;
    waypointBufferStatus[nextIndex] = FULL;

    // Links waypoints together
    waypointBuffer[nextIndex]->next =  waypointBuffer[nextIndex+1];
    waypointBuffer[nextIndex]->previous =  waypointBuffer[previousIndex];
    waypointBuffer[previousIndex]->next = newWaypoint;
    waypointBuffer[nextIndex+1]->previous = newWaypoint;

    return WAYPOINT_SUCCESS;
}

_WaypointStatus WaypointManager::delete_waypoint(int waypointId) {
    int waypointIndex = get_waypoint_index_from_id(waypointId);

    // std::cout << waypointIndex << std::endl;

    if (waypointIndex == -1) {
        return UNDEFINED_FAILURE;
    }

    _PathData* waypointToDelete = waypointBuffer[waypointIndex];

    // Links previous and next buffers together
    if (waypointIndex == 0) { //First element
        waypointBuffer[waypointIndex + 1]->previous = nullptr;
    } else if (waypointIndex == PATH_BUFFER_SIZE - 1 || waypointBufferStatus[waypointIndex+1] == FREE) { // Last element
        std::cout << "Last eleemntn" << std::endl;
        waypointBuffer[waypointIndex - 1]->next = nullptr;
    } else if (waypointBufferStatus[waypointIndex + 1] == FULL){ // Ensures that the next index is
        std::cout << " Note Last eleemntn" << std::endl;
        waypointBuffer[waypointIndex-1]->next = waypointBuffer[waypointIndex+1];
        waypointBuffer[waypointIndex+1]->previous = waypointBuffer[waypointIndex-1];
    }

    destroy_waypoint(waypointToDelete); // Frees heap memory

    // std::cout << "here" << std::endl;

    // Adjusts indeces so there are no empty elements
    // std::cout << numWaypoints - 1 << " " << waypointIndex << " " << waypointBufferStatus[numWaypoints] << " " <<  waypointBufferStatus[numWaypoints - 1] << std::endl;
    if(waypointIndex == numWaypoints - 1) { // Case where element is the last one in the current list
        waypointBuffer[waypointIndex] = nullptr;
        waypointBufferStatus[waypointIndex] = FREE;
    } else {
        for(int i = waypointIndex; i < numWaypoints-1; i++) {
            if (waypointBufferStatus[i+1] == FREE) {
                // std::cout << i << " "  << "Here" << std::endl;
                waypointBufferStatus[i] = FREE;
                waypointBuffer[i] = nullptr;
            } else if (waypointBufferStatus[i+1] == FULL) {
                // std::cout << i << " "  << "Here2" << std::endl;
                waypointBufferStatus[i] = FULL;
                waypointBuffer[i] = waypointBuffer[i+1];
                waypointBufferStatus[i+1] = FREE;
            }
        }
    }
    
    // std::cout << "here2" << std::endl;

    // for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
    //     std::cout << i << " " << waypointBufferStatus[i] << std::endl;
    // }

    // Updates array trackers
    numWaypoints--;
    nextFilledIndex--;

    return WAYPOINT_SUCCESS;
}

_WaypointStatus WaypointManager::update_waypoint(_PathData* updatedWaypoint, int waypointId) {
    int waypointIndex = get_waypoint_index_from_id(waypointId);

    if (waypointIndex == -1) {
        return UNDEFINED_FAILURE;
    }

    _PathData * oldWaypoint = waypointBuffer[waypointIndex];
    waypointBuffer[waypointIndex] = updatedWaypoint; // Updates waypoint

    //Links waypoints together
    if (waypointIndex == 0) { // First element
        waypointBuffer[waypointIndex]->next = waypointBuffer[waypointIndex+1];
        waypointBuffer[waypointIndex + 1]->previous = waypointBuffer[waypointIndex];
    } else if (waypointIndex == PATH_BUFFER_SIZE - 1 || waypointBufferStatus[waypointIndex+1] == FREE) { // Last element
        waypointBuffer[waypointIndex]->previous = waypointBuffer[waypointIndex-1];
        waypointBuffer[waypointIndex - 1]->next = waypointBuffer[waypointIndex];
    } else {
        waypointBuffer[waypointIndex]->next = waypointBuffer[waypointIndex+1];
        waypointBuffer[waypointIndex]->previous = waypointBuffer[waypointIndex-1];
        waypointBuffer[waypointIndex - 1]->next = waypointBuffer[waypointIndex];
        waypointBuffer[waypointIndex + 1]->previous = waypointBuffer[waypointIndex];
    }

    destroy_waypoint(oldWaypoint); // Frees old waypoint from heap memory

    return WAYPOINT_SUCCESS;
}


/*** MISCELLANEOUS ***/


_PathData ** WaypointManager::get_waypoint_buffer() {
    return waypointBuffer;
}

_WaypointBufferStatus WaypointManager::get_status_of_index(int index) {
    return waypointBufferStatus[index];
}

_PathData * WaypointManager::get_home_base() {
    return homeBase;
}

