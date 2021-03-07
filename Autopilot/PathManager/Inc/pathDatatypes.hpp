#ifndef PATH_DATATYPES_HPP
#define PATH_DATATYPES_HPP

#include <cstdint>

/********************************************************************
* Enums
********************************************************************/

enum _ModifyFlightPathCommand { NO_FLIGHT_PATH_EDIT = 0, INITIALIZE_FLIGHT_PATH, APPEND, INSERT, UPDATE, DELETE, NUKE }; // Used by cruisingState
enum _GetNextDirectionsCommand { REGULAR_PATH_FOLLOWING = 0, TOGGLE_HOLDING, TOGGLE_HEAD_HOME }; // Used by cruisingState

/********************************************************************
* Important Structs
********************************************************************/

struct Telemetry_Waypoint_Data_t {
    long double latitude;
    long double longitude;
    int altitude;
    float turnRadius;
    uint8_t waypointType; // 0 = Path follow, 1 = Orbit, 2 = Hold
};

struct Telemetry_PIGO_t {
    /* Parameters for the waypoint manager (crusingState) */
    int numWaypoints;
    
    _ModifyFlightPathCommand waypointModifyFlightPathCommand; 
    bool initializingHomeBase; // 0 = no, 1 = yes
    _GetNextDirectionsCommand waypointNextDirectionsCommand; 
    int holdingAltitude;
    int holdingTurnRadius;
    uint8_t holdingTurnDirection; // 0 = CW, 1 = CCW

    // When modifying the flight path.
    int nextId;
    int prevId;
    int modifyId;

    Telemetry_Waypoint_Data_t waypoints[100]; // Somehow need to get PATH_BUFFER_SIZE here...
    Telemetry_Waypoint_Data_t homebase;

    // landing and takeoff 
    double latitude;
    double longitude;
    double altitude;
    float directionLanding;
    bool beginLanding;
    bool beginTakeoff;
    // Struct for stopping point gps coordinates

};

#endif
