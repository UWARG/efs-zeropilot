/**
 * Waypoint Manager Methods and Helpers Implementation
 * Author: Dhruv Rawat
 * Created: November 2020
 * Last Updated: November 2020 (Dhruv)
 */

#include "../Inc/waypointManager.hpp"

#define LINE_FOLLOWING 0
#define ORBIT_FOLLOWING 1


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
#define RELATIVE_LATITUDE 43.467998128
#define RELATIVE_LONGITUDE 80.537331184

static float k_gain[2] = {0.01, 1};


/*** INITIALIZATION ***/


WaypointManager::WaypointManager() {
    nextAssignedId = 0;
    currentIndex = 0;

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

    for(int i = numWaypoints; i < PATH_BUFFER_SIZE; i++) {
        waypointBuffer[i] = nullptr;
    }

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

    for(int i = numWaypoints; i < PATH_BUFFER_SIZE; i++) {
        waypointBuffer[i] = nullptr;
    }
    

    errorStatus = WAYPOINT_SUCCESS;
    return errorStatus;
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
    
    // Gets current position
    float position[3]; 

    // Gets current heading
    float currentHeading = (float) currentStatus.heading;
    
    // Holding is given higher priority to heading home
    if (inHold) {   // If plane is currently circling and waiting for commands
        if(turnRadius <= 0 || turnDirection < 1 || turnDirection > 2) {
            return UNDEFINED_FAILURE;
        }

        // Sets position array
        position[0] = deg2rad(currentStatus.longitude);
        position[1] = deg2rad(currentStatus.latitude);
        position[2] = (float) currentStatus.altitude;

        // Calculates desired heading 
        follow_hold_pattern(position, currentHeading);

        outputType = ORBIT_FOLLOW;

        dataIsNew = true;

        update_return_data(Data); // Updates the return structure

        return errorCode;
    }

    get_coordinates(currentStatus.longitude, currentStatus.latitude, position);
    position[2] = currentStatus.altitude;

    if (goingHome) { // If plane was instructed to go back to base (and is awaiting for waypointBuffer to be updated)
        // Do stuff
        dataIsNew = true;
        update_return_data(Data); // Updates the return structure
        return errorCode;
    }

    // Calls method to follow waypoints
    follow_waypoints(waypointBuffer[currentIndex], (float*) position, currentHeading);

    dataIsNew = true;
    update_return_data(Data); // Updates the return structure
    return errorCode;
}

void WaypointManager::update_return_data(_WaypointManager_Data_Out *Data) {
    Data->desiredHeading = desiredHeading;
    Data->desiredAltitude =  desiredAltitude;
    Data->distanceToNextWaypoint = distanceToNextWaypoint;
    Data->radius = turnRadius;
    Data->turnDirection = turnDirection;
    Data->errorCode = errorCode;
    Data->isDataNew = dataIsNew;
    dataIsNew = false;
    Data->timeOfData = 0;

    // Not setting time of data yet bc I think we need to come up with a way to get it???
}

void WaypointManager::start_circling(float radius, int direction, int altitude, bool cancelTurning) {
    if (!cancelTurning) {
        inHold = true;
        desiredAltitude = altitude;
        turnRadius = radius;
        turnDirection = direction;
    } else {
        inHold = false;
    }
}

void WaypointManager::head_home() {
    if (!goingHome) {
        clear_path_nodes(); // Clears path nodes so state machine can input new flight path
        goingHome = true;
    } else {
        goingHome = false;
    }
}

void WaypointManager::follow_hold_pattern(float* position, float heading) {

    float turnCenter[3]; // Coordinates of the circle center
    turnCenter[2] = desiredAltitude;
    float turnCenterBearing = 0.0f; // Bearing of line pointing to the center point of the turn

    if (turnDirection == 1) {
        turnCenterBearing = heading + 90;
    } else if (turnDirection == 2) {
        turnCenterBearing = heading - 90;
    }

    // Normalizes heading (keeps it between 0.0 and 259.9999)
    // while (turnCenterBearing >= 360.0) {
    //     turnCenterBearing -= 360.0; // IF THERE IS A WAY TO DO THIS WITHOUT A WHILE LOOP PLS LMK
    // }

    // while (turnCenterBearing < 0.0) {
    //     turnCenterBearing += 360.0; // IF THERE IS A WAY TO DO THIS WITHOUT A WHILE LOOP PLS LMK
    // }

    float angularDisplacement = turnRadius / (EARTH_RADIUS * 1000);

    double turnCenterBearing_Radians = deg2rad(turnCenterBearing);

    // std::cout << turnCenterBearing << " " << angularDisplacement << " " << turnCenterBearing_Radians << std::endl;
    // std::cout << position[0] << " " << position[1] << " " << position[2] << std::endl;
    // std::cout << 0 << std::endl;

    // Calculates latitude and longitude of end coordinates (Calculations taken from here: http://www.movable-type.co.uk/scripts/latlong.html#destPoint)
    turnCenter[1] = asin(sin(position[1]) * cos(angularDisplacement) + cos(position[1]) * sin(angularDisplacement) * cos(turnCenterBearing_Radians)); // latitude
    turnCenter[0] = position[0] + atan2(sin(turnCenterBearing_Radians) * sin(angularDisplacement) * cos(position[1]), cos(angularDisplacement) - sin(position[1]) * sin(turnCenter[1])); // Longitude
    
    #ifdef UNIT_TESTING
        orbitCentreLong = rad2deg(turnCenter[0]);
        orbitCentreLat = rad2deg(turnCenter[1]);
        orbitCentreAlt = turnCenter[2];

        // std::cout << "Check 1: Lat - " << orbitCentreLat << " " << orbitCentreLong << std::endl;
    #endif

    // Converts the position array and turnCenter array from radians to an xy coordinate system. The get_coordinates() method expects values to be in degrees
    get_coordinates(rad2deg(position[0]), rad2deg(position[1]), position);
    get_coordinates(rad2deg(turnCenter[0]), rad2deg(turnCenter[1]), turnCenter);

    // std::cout << "Check 2: Lat - " << turnCenter[1] << " " << turnCenter[0] << std::endl;
    // std::cout << "Lat - " << position[1] << " " << position[0] << " " << heading << std::endl;

    // Calls follow_orbit method 
    follow_orbit(turnCenter, position, heading);
}

void WaypointManager::follow_waypoints(_PathData * currentWaypoint, float* position, float heading) {

}

void WaypointManager::follow_line_segment(_PathData * currentWaypoint, float* position, float heading) {

}

void WaypointManager::follow_last_line_segment(_PathData * currentWaypoint, float* position, float heading) {

}

void WaypointManager::follow_orbit(float* orbitCenter, float* position, float currentHeading) {
    currentHeading = deg2rad(90 - currentHeading);

    float orbitDistance = sqrt(pow(position[0] - orbitCenter[0],2) + pow(position[1] - orbitCenter[1],2));
    float courseAngle = atan2(position[1] - orbitCenter[1], position[0] - orbitCenter[0]); // (y,x) format

    while (courseAngle - currentHeading < - M_PI){
        courseAngle += 2 * M_PI;
    }
    while (courseAngle - currentHeading > M_PI){
        courseAngle -= 2 * M_PI;
    }

    // std::cout << courseAngle << std::endl;

    int turnDirectionConstant = 0;
    if (turnDirection == 1) {
        turnDirectionConstant = 1;
    } else if (turnDirection == 2){
        turnDirectionConstant = -1;
    }

    // std::cout <<  courseAngle << " " << turnDirectionConstant * (M_PI/2 + atan(k_gain[ORBIT_FOLLOWING] * (orbitDistance - turnRadius)/turnRadius)) << " " <<  rad2deg(courseAngle + turnDirectionConstant * (M_PI/2 + atan(k_gain[ORBIT_FOLLOWING] * (orbitDistance - turnRadius)/turnRadius))) << std::endl;

    int calcHeading = round(90 - rad2deg(courseAngle + turnDirectionConstant * (M_PI/2 + atan(k_gain[ORBIT_FOLLOWING] * (orbitDistance - turnRadius)/turnRadius)))); //Heading in degrees (magnetic)
    
    // Normalizes heading (keeps it between 0.0 and 259.9999)
    while (calcHeading >= 360.0) {
        calcHeading -= 360.0; // IF THERE IS A WAY TO DO THIS WITHOUT A WHILE LOOP PLS LMK
    }

    while (calcHeading < 0.0) {
        calcHeading += 360.0; // IF THERE IS A WAY TO DO THIS WITHOUT A WHILE LOOP PLS LMK
    }
    
    desiredHeading = calcHeading;
    
    distanceToNextWaypoint = 0.0;
}

void WaypointManager::follow_straight_path(float* waypointDirection, float* targetWaypoint, float* position, float heading) {

}

float WaypointManager::maintain_altitude(_PathData* currentPath) {

}


/*** FLIGHT PATH MANAGEMENT ***/


_WaypointStatus WaypointManager::update_path_nodes(_PathData * waypoint, _WaypointBufferUpdateType updateType, int waypointId, int previousId, int nextId) {
    if (numWaypoints == PATH_BUFFER_SIZE && (updateType == APPEND_WAYPOINT || updateType == INSERT_WAYPOINT)) { // If array is already full, if we insert it will overflow
        return UNDEFINED_FAILURE;
    }

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
    currentIndex = 0;
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

    // If any of the waypoints could not be found. Or, if the two IDs do not correspond to adjacent elements in waypointBuffer[]
    if (nextIndex == -1 || previousIndex == -1 || nextIndex - 1 != previousIndex || nextIndex == 0){
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

    if (waypointIndex == -1) {
        return UNDEFINED_FAILURE;
    }

    _PathData* waypointToDelete = waypointBuffer[waypointIndex];

    // Links previous and next buffers together
    if (waypointIndex == 0) { //First element
        waypointBuffer[waypointIndex + 1]->previous = nullptr;
    } else if (waypointIndex == PATH_BUFFER_SIZE - 1 || waypointBufferStatus[waypointIndex+1] == FREE) { // Last element
        waypointBuffer[waypointIndex - 1]->next = nullptr;
    } else if (waypointBufferStatus[waypointIndex + 1] == FULL){ // Ensures that the next index is
        waypointBuffer[waypointIndex-1]->next = waypointBuffer[waypointIndex+1];
        waypointBuffer[waypointIndex+1]->previous = waypointBuffer[waypointIndex-1];
    }

    destroy_waypoint(waypointToDelete); // Frees heap memory

    // Adjusts indeces so there are no empty elements
    if(waypointIndex == numWaypoints - 1) { // Case where element is the last one in the current list
        waypointBuffer[waypointIndex] = nullptr;
        waypointBufferStatus[waypointIndex] = FREE;
    } else {
        for(int i = waypointIndex; i < numWaypoints-1; i++) {
            if (waypointBufferStatus[i+1] == FREE) {
                waypointBufferStatus[i] = FREE;
                waypointBuffer[i] = nullptr;
            } else if (waypointBufferStatus[i+1] == FULL) {
                waypointBufferStatus[i] = FULL;
                waypointBuffer[i] = waypointBuffer[i+1];
                waypointBufferStatus[i+1] = FREE;
            }
        }
    }

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

