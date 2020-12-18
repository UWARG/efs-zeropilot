/**
 * Waypoint Manager Methods and Helpers Implementation
 * Author: Dhruv Rawat
 * Created: November 2020
 * Last Updated: December 2020 (Dhruv)
 */

#include "waypointManager.hpp"

// Values for orbitPathStatus parameter of WaypointManager
#define LINE_FOLLOWING 0
#define ORBIT_FOLLOWING 1

//Constants
#define EARTH_RADIUS 6378.137
#define PI 3.14159265358979323846 // Was giving me problems with M_PI, so I resorted to defining it myself
#define MAX_PATH_APPROACH_ANGLE PI/2

//Basic Mathematical Conversions
#define deg2rad(angle_in_degrees) ((angle_in_degrees) * PI/180.0)
#define rad2deg(angle_in_radians) ((angle_in_radians) * 180.0/PI)


/*** INITIALIZATION ***/


WaypointManager::WaypointManager(float relLat, float relLong) {
    // Initializes important array and id navigation constants
    currentIndex = 0;
    nextAssignedId = 0;
    numWaypoints = 0;
    nextFilledIndex = 0;

    // Sets relative long and lat
    relativeLongitude = relLong;
    relativeLatitude = relLat;

    homeBase = nullptr; // Sets the pointer to null

    // Sets boolean variables
    inHold = false;
    goingHome = false;
    dataIsNew = false;
    orbitPathStatus = PATH_FOLLOW;
    errorStatus = WAYPOINT_SUCCESS;

    // Initialize all other parameters (defaults)
    desiredHeading = 0;
    desiredAltitude = 0;
    distanceToNextWaypoint = 0.0;
    errorCode = WAYPOINT_SUCCESS;
    dataIsNew = false;
    outputType = PATH_FOLLOW;
    turnDesiredAltitude = 0;
    turnDirection = 0; // 1 for CW, 2 for CCW
    turnRadius = 0.0;

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        waypointBufferStatus[i] = FREE;
    }
}

_WaypointStatus WaypointManager::initialize_flight_path(_PathData ** initialWaypoints, int numberOfWaypoints, _PathData * currentLocation) {
    errorStatus = WAYPOINT_SUCCESS; 

    // The waypointBuffer array must be empty before we initialize the flight path
    if (numWaypoints != 0) {
        errorStatus = UNDEFINED_FAILURE;
        return errorStatus;
    }

    // If user passes in too many waypoints, the enum will notify them, but the flight path will be set with the maximum amount of waypoints allowed 
    if (numberOfWaypoints > PATH_BUFFER_SIZE) {
        errorStatus = TOO_MANY_WAYPOINTS;
        numberOfWaypoints = PATH_BUFFER_SIZE;
    }
    
    // If currentLocation was passed, then initializes homeBase
    if (currentLocation != nullptr) {
        homeBase = currentLocation;
    }

    numWaypoints = numberOfWaypoints;
    nextFilledIndex = 0;
    
    #ifdef UNIT_TESTING
        currentIndex = 2;
    #else
        currentIndex = 0;
    #endif

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

    // Sets empty elements to null to prevent segmentation faults
    for(int i = numWaypoints; i < PATH_BUFFER_SIZE; i++) {
        waypointBuffer[i] = nullptr;
    }

    return errorStatus;
}

_PathData* WaypointManager::initialize_waypoint() {
    _PathData* waypoint = new _PathData; // Create new waypoint in the heap

    if (!waypoint) {
        return NULL;
    }

    waypoint->waypointId = nextAssignedId;
    nextAssignedId++; // Increment ID so next waypoint has a different one
    waypoint->latitude = -1;
    waypoint->longitude = -1;
    waypoint->altitude = 10; // Sets this to 10 as a default so plane does not crash. This can be changed by state machine.
    waypoint->waypointType = PATH_FOLLOW;
    waypoint->turnRadius = -1;
    // Set next and previous waypoints to empty for now
    waypoint->next = nullptr;
    waypoint->previous = nullptr;

    return waypoint;
}

_PathData* WaypointManager::initialize_waypoint(long double longitude, long double latitude, int altitude, _WaypointOutputType waypointType) {
    _PathData* waypoint = new _PathData; // Create new waypoint in the heap

    if (!waypoint) {
        return NULL;
    }

    waypoint->waypointId = nextAssignedId; 
    nextAssignedId++; // Increment ID so next waypoint has a different one 
    waypoint->latitude = latitude;
    waypoint->longitude = longitude;

    // Does error catching before assigning value
    if (altitude >= 0) {
        waypoint->altitude = altitude;
    } else {
        waypoint->altitude = 10; // Default value
    }    
    
    waypoint->waypointType = waypointType;
    waypoint->turnRadius = -1; 
    // Set next and previous waypoints to empty for now
    waypoint->next = nullptr;
    waypoint->previous = nullptr;

    return waypoint;
}

_PathData* WaypointManager::initialize_waypoint(long double longitude, long double latitude, int altitude, _WaypointOutputType waypointType, float turnRadius) {
    _PathData* waypoint = new _PathData; // Create new waypoint in the heap

    if (!waypoint) {
        return NULL;
    }

    waypoint->waypointId = nextAssignedId; 
    nextAssignedId++; // Increment ID so next waypoint has a different one 
    waypoint->latitude = latitude;
    waypoint->longitude = longitude;

    // Does error catching before assigning value
    if (altitude >= 0) {
        waypoint->altitude = altitude;
    } else {
        waypoint->altitude = 10; // Default value
    }
    
    waypoint->waypointType = waypointType;

    // Does error catching before assigning value
    if (turnRadius > 0) {
        waypoint->turnRadius = turnRadius;
    } else {
        waypoint->turnRadius = 30; // Default value
    }
    
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

void WaypointManager::get_coordinates(long double longitude, long double latitude, float* xyCoordinates) { // Parameters expected to be in degrees
    xyCoordinates[0] = get_distance(relativeLatitude, relativeLongitude, relativeLatitude, longitude); //Calculates longitude (x coordinate) relative to defined origin (RELATIVE_LONGITUDE, RELATIVE_LATITUDE)
    xyCoordinates[1] = get_distance(relativeLatitude, relativeLongitude, latitude, relativeLongitude); //Calculates latitude (y coordinate) relative to defined origin (RELATIVE_LONGITUDE, RELATIVE_LATITUDE)
}

float WaypointManager::get_distance(long double lat1, long double lon1, long double lat2, long double lon2) { // Parameters expected to be in degrees
    // Longitude and latitude stored in degrees
    // This calculation uses the Haversine formula
    long double change_in_Lat = deg2rad(lat2 - lat1); //Converts change in latitude to radians
    long double change_in_lon = deg2rad(lon2 - lon1); //Converts change in longitude to radians

    double haversine_ans = sin(change_in_Lat / 2) * sin(change_in_Lat / 2) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * sin(change_in_lon / 2) * sin(change_in_lon / 2); // In kilometers

    if ((change_in_Lat >= 0 && change_in_lon >=0)||(change_in_Lat < 0 && change_in_lon < 0)){
        return EARTH_RADIUS * (2 * atan2(sqrt(haversine_ans),sqrt(1 - haversine_ans))) * 1000; //Multiply by 1000 to convert to metres
    } else {
        return EARTH_RADIUS * (2 * atan2(sqrt(haversine_ans),sqrt(1 - haversine_ans))) * -1000;
    }
}

_WaypointStatus WaypointManager::change_current_index(int id) {
    int waypointIndex = get_waypoint_index_from_id(id); // Gets index of waypoint in waypointBuffer array

    if (waypointIndex == -1 || waypointBuffer[waypointIndex]->next == nullptr || waypointBuffer[waypointIndex]->next->next == nullptr) { // If waypoint with set id does not exist. Or if the next waypoint or next to next waypoints are not defined. 
        return INVALID_PARAMETERS;
    }

    currentIndex = waypointIndex; // If checks pass, then the current index value is updated
    
    return WAYPOINT_SUCCESS;
}


/*** NAVIGATION ***/


_WaypointStatus WaypointManager::get_next_directions(_WaypointManager_Data_In currentStatus, _WaypointManager_Data_Out *Data) {

    errorCode = WAYPOINT_SUCCESS;

    float position[3]; 
    // Gets current heading
    float currentHeading = (float) currentStatus.heading;

    // Holding is given higher priority to heading home
    if (inHold) { // If plane is currently circling and waiting for commands
        // Checks if parameters are valid. If state machine gets this error it should immediately cancel the hold because output data will not be updated while the parameters are incorrect
        if(turnRadius <= 0 || (turnDirection != -1 && turnDirection != 1)) { 
            errorCode = INVALID_PARAMETERS;
            return errorCode;
        }

        // Sets position array
        position[0] = currentStatus.longitude;
        position[1] = currentStatus.latitude;
        position[2] = (float) currentStatus.altitude;

        // Calculates desired heading, altitude, and all output values
        follow_hold_pattern(position, currentHeading);

        // Updates the return structure
        outputType = ORBIT_FOLLOW;
        dataIsNew = true;
        update_return_data(Data); 

        return errorCode;
    }

    // Sets position array
    get_coordinates(currentStatus.longitude, currentStatus.latitude, position);
    position[2] = (float) currentStatus.altitude;

    if (goingHome) { // If plane was instructed to go back to base (and is awaiting for waypointBuffer to be updated)
        if (homeBase == nullptr) {
            errorCode = UNDEFINED_PARAMETER;
            return errorCode;
        }

        // Creates a path data object to represent current position
        _PathData * currentPosition = new _PathData;
        currentPosition->latitude = currentStatus.latitude;
        currentPosition->longitude = currentStatus.longitude;
        currentPosition->altitude = currentStatus.altitude;
        currentPosition->turnRadius = -1;
        currentPosition->waypointType = PATH_FOLLOW;
        currentPosition->previous = nullptr;
        currentPosition->next = homeBase;

        // Updates home base object accordingly
        homeBase->previous = currentPosition;
        homeBase->next = nullptr;
        homeBase->waypointType = HOLD_WAYPOINT;
        
        // Calculates desired heading, altitude, and all output values
        follow_waypoints(currentPosition, position, currentHeading);
        
        // Updates the return structure
        dataIsNew = true;
        outputType = PATH_FOLLOW;
        update_return_data(Data); 

        // Removes currentPosition path data object from memory
        homeBase->previous = nullptr;
        currentPosition->next = nullptr;
        delete currentPosition; 

        return errorCode;
    }

    // Ensures that the currentIndex parameter will not cause a segmentation fault
    if (numWaypoints - currentIndex < 1 && numWaypoints >= 0) { 
        errorCode = CURRENT_INDEX_INVALID;
        return errorCode;
    }

    // Calculates desired heading, altitude, and all output values
    follow_waypoints(waypointBuffer[currentIndex], position, currentHeading);

    // Updates the return structure
    dataIsNew = true;
    outputType = PATH_FOLLOW;
    update_return_data(Data); 

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
    Data->timeOfData = 0; // Not setting time of data yet bc I think we need to come up with a way to get it???
    Data->out_type = outputType;
}

_WaypointStatus WaypointManager::start_circling(_WaypointManager_Data_In currentStatus, float radius, int direction, int altitude, bool cancelTurning) {
    if (!cancelTurning) {
        // If parameters are not valid. Minimum altitude of 10 metres
        if (radius <= 0 || (direction != -1 && direction != 1) || altitude < 10) { // SHOULD I JUST SET THIS TO DEFAULT VALUES INSTEAD??????
            return INVALID_PARAMETERS; 
        }

        inHold = true; // Sets holding indicator to true

        // Sets class parameters
        turnDesiredAltitude = altitude;
        turnRadius = radius;
        turnDirection = direction;

        // Gets current heading
        float currentHeading = (float) currentStatus.heading;

        // Gets current position
        float position[3]; 
        position[0] = deg2rad(currentStatus.longitude);
        position[1] = deg2rad(currentStatus.latitude);
        position[2] = (float) currentStatus.altitude;

        turnCenter[2] = turnDesiredAltitude;
        float turnCenterBearing = 0.0f; // Bearing of line pointing to the center point of the turn

        if (turnDirection == -1) { // CW
            turnCenterBearing = currentHeading + 90;
        } else if (turnDirection == 1) { // CCW
            turnCenterBearing = currentHeading - 90;
        }

        // Normalizes heading (keeps it between 0.0 and 259.9999)
        if (turnCenterBearing >= 360.0) {
            turnCenterBearing = fmod(turnCenterBearing, 360.0);
        } else if (turnCenterBearing < 0.0) {
            turnCenterBearing = fmod(turnCenterBearing, 360.0) + 360.0;
        }

        float angularDisplacement = turnRadius / (EARTH_RADIUS * 1000);

        double turnCenterBearing_Radians = deg2rad(turnCenterBearing);

        // Calculates latitude and longitude of end coordinates (Calculations taken from here: http://www.movable-type.co.uk/scripts/latlong.html#destPoint)
        turnCenter[1] = asin(sin(position[1]) * cos(angularDisplacement) + cos(position[1]) * sin(angularDisplacement) * cos(turnCenterBearing_Radians)); // latitude
        turnCenter[0] = position[0] + atan2(sin(turnCenterBearing_Radians) * sin(angularDisplacement) * cos(position[1]), cos(angularDisplacement) - sin(position[1]) * sin(turnCenter[1])); // Longitude

        #ifdef UNIT_TESTING
            orbitCentreLong = rad2deg(turnCenter[0]);
            orbitCentreLat = rad2deg(turnCenter[1]);
            orbitCentreAlt = turnCenter[2];

            // std::cout << "Check 1: Lat - " << orbitCentreLat << " " << orbitCentreLong << std::endl;
        #endif

        get_coordinates(rad2deg(turnCenter[0]), rad2deg(turnCenter[1]), turnCenter);
    } else {
        inHold = false;
    }

    return WAYPOINT_SUCCESS;
}

_HeadHomeStatus WaypointManager::head_home(bool startHeadingHome) {
    if (homeBase == nullptr) { // Checks if home waypoint is actually initialized.
        return HOME_UNDEFINED_PARAMETER;
    }

    if (startHeadingHome) {
        clear_path_nodes(); // Clears path nodes so state machine can input new flight path
        goingHome = true;
        return HOME_TRUE;
    } else {
        goingHome = false;
        return HOME_FALSE;
    }
}

void WaypointManager::follow_hold_pattern(float* position, float heading) {
    // Converts the position array and turnCenter array from radians to an xy coordinate system.
    get_coordinates(position[0], position[1], position);

    // Calls follow_orbit method 
    follow_orbit(position, heading);
}

void WaypointManager::follow_waypoints(_PathData * currentWaypoint, float* position, float heading) {
    float waypointPosition[3]; 
    get_coordinates(currentWaypoint->longitude, currentWaypoint->latitude, waypointPosition);
    waypointPosition[2] = currentWaypoint->altitude;

    if (currentWaypoint->next == nullptr) { // If target waypoint is not defined
        // std::cout << "Next not defined" << std::endl;
        follow_last_line_segment(currentWaypoint, position, heading);
        return;
    }
    if (currentWaypoint->next->next == nullptr) { // If waypoint after target waypoint is not defined
        // std::cout << "Next to next not defined" << std::endl;
        follow_line_segment(currentWaypoint, position, heading);
        return;
    }

    // Defines target waypoint
    _PathData * targetWaypoint = currentWaypoint->next;
    float targetCoordinates[3];
    get_coordinates(targetWaypoint->longitude, targetWaypoint->latitude, targetCoordinates);
    targetCoordinates[2] = targetWaypoint->altitude;

    // Defines waypoint after target waypoint
    _PathData* waypointAfterTarget = targetWaypoint->next;
    float waypointAfterTargetCoordinates[3];
    get_coordinates(waypointAfterTarget->longitude, waypointAfterTarget->latitude, waypointAfterTargetCoordinates);
    waypointAfterTargetCoordinates[2] = waypointAfterTarget->altitude;

    // Gets the unit vectors representing the direction towards the target waypoint
    float waypointDirection[3];
    float norm = sqrt(pow(targetCoordinates[0] - waypointPosition[0],2) + pow(targetCoordinates[1] - waypointPosition[1],2) + pow(targetCoordinates[2] - waypointPosition[2],2));
    waypointDirection[0] = (targetCoordinates[0] - waypointPosition[0])/norm;
    waypointDirection[1] = (targetCoordinates[1] - waypointPosition[1])/norm;
    waypointDirection[2] = (targetCoordinates[2] - waypointPosition[2])/norm;

    // std::cout << "Here1.1 --> " << position[0] << " " << position[1] << " " << position[2] << std::endl;
    // std::cout << "Here1.2 --> " << targetCoordinates[0] << " " << targetCoordinates[1] << " " << targetCoordinates[2] << std::endl;
    // std::cout << "Here1.3 --> " << waypointPosition[0] << " " << waypointPosition[1] << " " << waypointPosition[2] << std::endl;

    // Gets the unit vectors representing the direction vector from the target waypoint to the waypoint after the target waypoint 
    float nextWaypointDirection[3];
    float norm2 = sqrt(pow(waypointAfterTargetCoordinates[0] - targetCoordinates[0],2) + pow(waypointAfterTargetCoordinates[1] - targetCoordinates[1],2) + pow(waypointAfterTargetCoordinates[2] - targetCoordinates[2],2));
    nextWaypointDirection[0] = (waypointAfterTargetCoordinates[0] - targetCoordinates[0])/norm2;
    nextWaypointDirection[1] = (waypointAfterTargetCoordinates[1] - targetCoordinates[1])/norm2;
    nextWaypointDirection[2] = (waypointAfterTargetCoordinates[2] - targetCoordinates[2])/norm2;

    // Required turning angle
    float turningAngle = acos(-deg2rad(waypointDirection[0] * nextWaypointDirection[0] + waypointDirection[1] * nextWaypointDirection[1] + waypointDirection[2] * nextWaypointDirection[2]));
    // Calculates tangent factor that helps determine centre of turn 
    float tangentFactor = targetWaypoint->turnRadius/tan(turningAngle/2);

    float halfPlane[3];
    halfPlane[0] = targetCoordinates[0] - tangentFactor * waypointDirection[0];
    halfPlane[1] = targetCoordinates[1] - tangentFactor * waypointDirection[1];
    halfPlane[2] = targetCoordinates[2] - tangentFactor * waypointDirection[2];

    // Calculates distance to next waypoint
    float distanceToWaypoint = sqrt(pow(targetCoordinates[0] - position[0],2) + pow(targetCoordinates[1] - position[1],2) + pow(targetCoordinates[2] - position[2],2));
    distanceToNextWaypoint = distanceToWaypoint; 

    // Checks if plane is orbiting or flying in a straight line
    if (orbitPathStatus == PATH_FOLLOW) {
        float dotProduct = waypointDirection[0] * (position[0] - halfPlane[0]) + waypointDirection[1] * (position[1] - halfPlane[1]) + waypointDirection[2] * (position[2] - halfPlane[2]);
        
        if (dotProduct > 0){
            orbitPathStatus = ORBIT_FOLLOW;
            if (targetWaypoint->waypointType == HOLD_WAYPOINT) {
                inHold = true;
                turnDirection = 1; // Automatically turn CCW
                turnRadius = targetWaypoint->turnRadius;
                turnDesiredAltitude = targetWaypoint->altitude;
                turnCenter[0] = targetWaypoint->longitude;
                turnCenter[1] = targetWaypoint->latitude;
                turnCenter[2] = turnDesiredAltitude;
                /*
                    Increment the currentIndex so the plane is not perpetually stuck in a holding pattern
                */
                currentIndex++; 
            }
        }

        // std::cout << "Here2 --> " << position[0] << " " << position[1] << " " << position[2] << std::endl;
        // std::cout << "Here3 --> " << waypointDirection[0] << " " << waypointDirection[1] << " " << waypointDirection[2] << std::endl;
        // std::cout << "Here4 --> " << targetCoordinates[0] << " " << targetCoordinates[1] << " " << targetCoordinates[2] << std::endl;

        follow_straight_path(waypointDirection, targetCoordinates, position, heading);
    } else {
        // Determines turn direction (CCW returns 2; CW returns 1)
        turnDirection = waypointDirection[0] * nextWaypointDirection[1] - waypointDirection[1] * nextWaypointDirection[0]>0?1:-1;
        
        // Since the Earth is not flat *waits for the uproar to die down* we need to do some fancy geometry. Introducing!!!!!!!!!! EUCLIDIAN GEOMETRY! (translation: I have no idea what this line does but it should work)
        float euclideanWaypointDirection = sqrt(pow(nextWaypointDirection[0] - waypointDirection[0],2) + pow(nextWaypointDirection[1] - waypointDirection[1],2) + pow(nextWaypointDirection[2] - waypointDirection[2],2)) * ((nextWaypointDirection[0] - waypointDirection[0]) < 0?-1:1) * ((nextWaypointDirection[1] - waypointDirection[1]) < 0?-1:1) * ((nextWaypointDirection[2] - waypointDirection[2]) < 0?-1:1);

        // Determines coordinates of the turn center
        turnCenter[0] = targetCoordinates[0] + (tangentFactor * (nextWaypointDirection[0] - waypointDirection[0])/euclideanWaypointDirection);
        turnCenter[1] = targetCoordinates[1] + (tangentFactor * (nextWaypointDirection[1] - waypointDirection[1])/euclideanWaypointDirection);
        turnCenter[2] = targetCoordinates[2] + (tangentFactor * (nextWaypointDirection[2] - waypointDirection[2])/euclideanWaypointDirection);

        float dotProduct = nextWaypointDirection[0] * (position[0] - halfPlane[0]) + nextWaypointDirection[1] * (position[1] - halfPlane[1]) + nextWaypointDirection[2] * (position[2] - halfPlane[2]);
        
        if (dotProduct > 0){
            /*
                It makes the most sense to increment the current index here. 

                To explain this, I will define three waypoints: A, B, and C. The waypoints are located in adjacent indeces in 
                the waypointBuffer array in alphabetic order. 

                Let's say that the currentIndex initially pointed at waypoint A, meaning the plane was travelling from A to B. 
                Since the lines AB and BC are not parallel, the plane needs to execute a (fabulous) turn before reaching waypoint 
                B so it can travel along line BC. Now, this if statement is triggered when the plane has finished its turn and is 
                now travelling along line BC. 

                As a result, it makes sense that we increment the currentIndex parameter here since now we are targeting waypoint C 
                (meaning we need to set waypoint B as the curent waypoint).

                (NB: We will need to test to see if it is possible to accidentally trigger this)
            */   
            currentIndex++; 

            orbitPathStatus = PATH_FOLLOW;
        }

        //If two waypoints are parallel to each other (no turns)
        if (euclideanWaypointDirection == 0){
            // For same reasons above, since the waypoints are parallel, we can switch the current waypoint and target the next one
            currentIndex++;

            orbitPathStatus = PATH_FOLLOW;
        }

        outputType = ORBIT_FOLLOW;

        follow_orbit(position, heading);
    }
}

void WaypointManager::follow_line_segment(_PathData * currentWaypoint, float* position, float heading) {
    float waypointPosition[3];
    get_coordinates(currentWaypoint->longitude, currentWaypoint->latitude, waypointPosition);
    waypointPosition[2] = currentWaypoint->altitude;

    // Defines target waypoint
    _PathData * targetWaypoint = currentWaypoint->next;
    float targetCoordinates[3];
    get_coordinates(targetWaypoint->longitude, targetWaypoint->latitude, targetCoordinates);
    targetCoordinates[2] = targetWaypoint->altitude;

    // Direction to next waypoint
    float waypointDirection[3];
    float norm = sqrt(pow(targetCoordinates[0] - waypointPosition[0],2) + pow(targetCoordinates[1] - waypointPosition[1],2) + pow(targetCoordinates[2] - waypointPosition[2],2));
    waypointDirection[0] = (targetCoordinates[0] - waypointPosition[0])/norm;
    waypointDirection[1] = (targetCoordinates[1] - waypointPosition[1])/norm;
    waypointDirection[2] = (targetCoordinates[2] - waypointPosition[2])/norm;

    // Calculates distance to next waypoint
    float distanceToWaypoint = sqrt(pow(targetCoordinates[0] - position[0],2) + pow(targetCoordinates[1] - position[1],2) + pow(targetCoordinates[2] - position[2],2));
    distanceToNextWaypoint = distanceToWaypoint; // Stores distance to next waypoint :))

    // std::cout << "Here1.1 --> " << waypointDirection[0] << " " << waypointDirection[1] << " " << waypointDirection[2] << std::endl;
    // std::cout << "Here1.2 --> " << targetCoordinates[0] << " " << targetCoordinates[1] << " " << targetCoordinates[2] << std::endl;
    // std::cout << "Here1.3 --> " << waypointPosition[0] << " " << waypointPosition[1] << " " << waypointPosition[2] << std::endl;
    // std::cout << heading << std::endl;

    follow_straight_path(waypointDirection, targetCoordinates, position, heading);
}

void WaypointManager::follow_last_line_segment(_PathData * currentWaypoint, float* position, float heading) {
    // Current position is set to waypointPosition
    float waypointPosition[3];
    waypointPosition[0] = position[0];
    waypointPosition[1] = position[1];
    waypointPosition[2] = position[2];

    // Target waypoint is the current waypoint
    _PathData * targetWaypoint = currentWaypoint;
    float targetCoordinates[3];
    get_coordinates(targetWaypoint->longitude, targetWaypoint->latitude, targetCoordinates);
    targetCoordinates[2] = targetWaypoint->altitude;

    // Direction between waypoints
    float waypointDirection[3];
    float norm = sqrt(pow(targetCoordinates[0] - waypointPosition[0],2) + pow(targetCoordinates[1] - waypointPosition[1],2) + pow(targetCoordinates[2] - waypointPosition[2],2));
    waypointDirection[0] = (targetCoordinates[0] - waypointPosition[0])/norm;
    waypointDirection[1] = (targetCoordinates[1] - waypointPosition[1])/norm;
    waypointDirection[2] = (targetCoordinates[2] - waypointPosition[2])/norm;

    // Calculates distance to next waypoint
    float distanceToWaypoint = sqrt(pow(targetCoordinates[0] - position[0],2) + pow(targetCoordinates[1] - position[1],2) + pow(targetCoordinates[2] - position[2],2));
    distanceToNextWaypoint = distanceToWaypoint; // Stores distance to next waypoint :))

    // If dot product positive, then wait for commands
    float dotProduct = waypointDirection[0] * (position[0] - targetCoordinates[0]) + waypointDirection[1] * (position[1] - targetCoordinates[1]) + waypointDirection[2] * (position[2] - targetCoordinates[2]);
    if (dotProduct > 0){
        inHold = true;
        turnDirection = 1; // Automatically turn CCW
        turnRadius = 50;
        turnDesiredAltitude = targetWaypoint->altitude;
        turnCenter[0] = targetWaypoint->longitude;
        turnCenter[1] = targetWaypoint->latitude;
        turnCenter[2] = turnDesiredAltitude; 
    }

    follow_straight_path(waypointDirection, targetCoordinates, position, heading);
}

void WaypointManager::follow_orbit(float* position, float heading) {
    heading = deg2rad(90 - heading);

    // Distance from centre of circle
    float orbitDistance = sqrt(pow(position[0] - turnCenter[0],2) + pow(position[1] - turnCenter[1],2));
    float courseAngle = atan2(position[1] - turnCenter[1], position[0] - turnCenter[0]); // (y,x) format

    // Normalizes angles
    // First gets the angle between 0 and 2 pi
    if (courseAngle - heading >= 2 * PI) {
        courseAngle = fmod(courseAngle, 2 * PI);
    } else if (courseAngle - heading < 0.0) {
        courseAngle = fmod(courseAngle, 2 * PI) + 2 * PI;
    }
    // Now ensures that courseAngle is between -pi and pi
    if (courseAngle > PI && courseAngle <= 2 * PI) {
        courseAngle -= 2 * PI;
    }

    // Desired heading
    int calcHeading = round(90 - rad2deg(courseAngle + turnDirection * (PI/2 + atan(k_gain[ORBIT_FOLLOW] * (orbitDistance - turnRadius)/turnRadius)))); //Heading in degrees (magnetic)
    
    // Normalizes heading (keeps it between 0.0 and 259.9999)
    if (calcHeading >= 360.0) {
        calcHeading = fmod(calcHeading, 360.0);
    } else if (calcHeading < 0.0) {
        calcHeading = fmod(calcHeading, 360.0) + 360.0;
    }
    
    // Sets the return values
    desiredHeading = calcHeading;
    distanceToNextWaypoint = 0.0;
    outputType = ORBIT_FOLLOW;
    desiredAltitude = turnDesiredAltitude;
}

void WaypointManager::follow_straight_path(float* waypointDirection, float* targetWaypoint, float* position, float heading) {
    heading = deg2rad(90 - heading);//90 - heading = magnetic heading to cartesian heading
    float courseAngle = atan2(waypointDirection[1], waypointDirection[0]); // (y,x) format
    
    // Normalizes angles
    // First gets the angle between 0 and 2 pi
    if (courseAngle - heading >= 2 * PI) {
        courseAngle = fmod(courseAngle, 2 * PI);
    } else if (courseAngle - heading < 0.0) {
        courseAngle = fmod(courseAngle, 2 * PI) + 2 * PI;
    }
    // Now ensures that courseAngle is between -pi and pi
    if (courseAngle > PI && courseAngle <= 2 * PI) {
        courseAngle -= 2 * PI;
    }

    // Calculates desired heading
    float pathError = -sin(courseAngle) * (position[0] - targetWaypoint[0]) + cos(courseAngle) * (position[1] - targetWaypoint[1]);
    int calcHeading = 90 - rad2deg(courseAngle - MAX_PATH_APPROACH_ANGLE * 2/PI * atan(k_gain[PATH_FOLLOW] * pathError)); //Heading in degrees (magnetic) 
    
    // Normalizes heading (keeps it between 0.0 and 259.9999)
    if (calcHeading >= 360.0) {
        calcHeading = fmod(calcHeading, 360.0);
    } else if (calcHeading < 0.0) {
        calcHeading = fmod(calcHeading, 360.0) + 360.0;
    }
    
    // Sets the return values 
    desiredHeading = calcHeading;
    outputType = PATH_FOLLOW;
    desiredAltitude = targetWaypoint[2];

    if (!inHold) {
        turnRadius = 0;
        turnDirection = 0;
    }
}


/*** FLIGHT PATH MANAGEMENT ***/


_WaypointStatus WaypointManager::update_path_nodes(_PathData * waypoint, _WaypointBufferUpdateType updateType, int waypointId, int previousId, int nextId) {
    // If array is already full, if we insert it will cause a segmentation fault
    if (numWaypoints == PATH_BUFFER_SIZE && (updateType == APPEND_WAYPOINT || updateType == INSERT_WAYPOINT)) { 
        destroy_waypoint(waypoint); // To pevent memory leaks from occuring, if there is an error the waypoint is removed from memory.
        errorCode = INVALID_PARAMETERS;
        return errorCode;
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
            destroy_waypoint(waypointBuffer[i]); // Remove waypoint from the heap
        }
        waypointBufferStatus[i] = FREE; // Set array element free
        waypointBuffer[i] = nullptr; //Set buffer element to empty struct
    }

    // Resets buffer status variables
    numWaypoints = 0;
    nextFilledIndex = 0;
    nextAssignedId = 0;
    currentIndex = 0;
}

void WaypointManager::clear_home_base() {
    destroy_waypoint(homeBase);
}

void WaypointManager::destroy_waypoint(_PathData *waypoint) {
    // Ensures waypoint is not linked before deleting
    waypoint->next = nullptr;
    waypoint->previous = nullptr;
    delete waypoint; 
}

_WaypointStatus WaypointManager::append_waypoint(_PathData * newWaypoint) {
    // Gets index of waypoint that comes before appended waypoint
    int previousIndex = 0;
    previousIndex = nextFilledIndex - 1;

    // Before initializing elements, checks if new waypoint is not a duplicate
    if (previousIndex != -1 && waypointBuffer[previousIndex]->latitude == newWaypoint->latitude && waypointBuffer[previousIndex]->longitude == newWaypoint->longitude) {
        destroy_waypoint(newWaypoint); // To pevent memory leaks from occuring, if there is an error the waypoint is removed from memory.
        return INVALID_PARAMETERS;
    }

    waypointBuffer[nextFilledIndex] = newWaypoint;
    waypointBufferStatus[nextFilledIndex] = FULL;

    //If we are initializing the first element
    if (previousIndex == -1) { 
        nextFilledIndex++;
        numWaypoints++;

        return WAYPOINT_SUCCESS;
    }

    // Links previous waypoint with current one
    waypointBuffer[nextFilledIndex]->previous = waypointBuffer[previousIndex];
    waypointBuffer[previousIndex]->next = waypointBuffer[nextFilledIndex];

    // Increments array navigation parameters since a new waypoint was added
    nextFilledIndex++;
    numWaypoints++;

    return WAYPOINT_SUCCESS;
}

_WaypointStatus WaypointManager::insert_new_waypoint(_PathData* newWaypoint, int previousId, int nextId) {
    int nextIndex = get_waypoint_index_from_id(nextId);
    int previousIndex = get_waypoint_index_from_id(previousId);

    // If any of the waypoints could not be found. Or, if the two IDs do not correspond to adjacent elements in waypointBuffer[]
    // Also ensures we are not inserting before the currentIndex
    if (nextIndex == -1 || previousIndex == -1 || nextIndex - 1 != previousIndex || nextIndex == 0 || previousIndex < currentIndex){
        destroy_waypoint(newWaypoint); // To pevent memory leaks from occuring, if there is an error the waypoint is removed from memory.
        return INVALID_PARAMETERS;
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

    // Increments array navigation parameters since a new waypoint was added
    nextFilledIndex++;
    numWaypoints++;

    return WAYPOINT_SUCCESS;
}

_WaypointStatus WaypointManager::delete_waypoint(int waypointId) {
    int waypointIndex = get_waypoint_index_from_id(waypointId);

    if (waypointIndex == -1) {
        return INVALID_PARAMETERS;
    }

    _PathData* waypointToDelete = waypointBuffer[waypointIndex];

    // Links previous and next buffers together
    if (waypointIndex == 0) { //First element
        waypointBuffer[waypointIndex + 1]->previous = nullptr;
    } else if (waypointIndex == PATH_BUFFER_SIZE - 1 || waypointBufferStatus[waypointIndex+1] == FREE) { // Last element
        waypointBuffer[waypointIndex - 1]->next = nullptr;
    } else if (waypointBufferStatus[waypointIndex + 1] == FULL){ // Middle element
        waypointBuffer[waypointIndex-1]->next = waypointBuffer[waypointIndex+1];
        waypointBuffer[waypointIndex+1]->previous = waypointBuffer[waypointIndex-1];
    }

    destroy_waypoint(waypointToDelete); // Frees heap memory

    // Adjusts indeces so there are no empty elements
    if(waypointIndex == numWaypoints - 1) { // Case where element is the last one in the current list
        waypointBuffer[waypointIndex] = nullptr;
        waypointBufferStatus[waypointIndex] = FREE;
    } else {
        for(int i = waypointIndex; i < numWaypoints-1; i++) { // Shifts all elements back one
            if (waypointBufferStatus[i+1] == FREE) { // If next element is free, then we are done and set current index to empty
                waypointBufferStatus[i] = FREE;
                waypointBuffer[i] = nullptr;
            } else if (waypointBufferStatus[i+1] == FULL) { // If next index has an element, then set the current index's values equal to it.
                waypointBufferStatus[i] = FULL;
                waypointBuffer[i] = waypointBuffer[i+1];
                waypointBufferStatus[i+1] = FREE;
            }
        }
    }

    // Decrements array trackers since a waypoint was removed
    numWaypoints--;
    nextFilledIndex--;

    return WAYPOINT_SUCCESS;
}

_WaypointStatus WaypointManager::update_waypoint(_PathData* updatedWaypoint, int waypointId) {
    int waypointIndex = get_waypoint_index_from_id(waypointId);

    if (waypointIndex == -1) {
        destroy_waypoint(updatedWaypoint); // To pevent memory leaks from occuring, if there is an error the waypoint is removed from memory.
        return INVALID_PARAMETERS;
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

_PathData * WaypointManager::get_waypoint(int index) {
    if (index < 0 || index > PATH_BUFFER_SIZE || !waypointBuffer[index]) {
        return nullptr;
    }

    return waypointBuffer[index];
}

_WaypointBufferStatus WaypointManager::get_status_of_index(int index) {
    if (index < 0 || index > PATH_BUFFER_SIZE) {
        return FULL;
    }

    return waypointBufferStatus[index];
}

_PathData * WaypointManager::get_home_base() {
    return homeBase;
}

int WaypointManager::get_current_index() {
    return currentIndex;
}

int WaypointManager::get_id_of_current_index() {
    return waypointBuffer[currentIndex]->waypointId;
}

// For valgrind tests
WaypointManager::~WaypointManager() {
    if (homeBase != nullptr) { // Only call if homeBase is initialized
        clear_home_base();
    }

    if (numWaypoints != 0) { // Only call if the waypointBuffer has waypoints in it
        clear_path_nodes();
    }
}



