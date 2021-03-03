
#ifndef PATH_DATATYPES_HPP
#define PATH_DATATYPES_HPP

struct Telemetry_Waypoint_Data_t {
    long double latitude;
    long double longitude;
    int altitude;
    float turnRadius;
    int waypointType; // 0 = Path follow, 1 = Orbit, 2 = Hold
};

struct Telemetry_PIGO_t {

    /* Parameters for the waypoint manager (crusingState) */
    char numWaypoints;
    
    char waypointModifyFlightPathCommand; // 0 = nothing, 1 = initialize flight path, 2 = append, 3 = insert, 4 = update, 5 = delete, 6 = nuke
    bool initializingHomeBase; // 0 = no, 1 = yes
    char waypointNextDirectionsCommand; // 0 = nothing, 1 = start/end holding, 2 = head home
    int holdingAltitude;
    int holdingTurnRadius;
    char holdingTurnDirection; // 0 = CW, 1 = CCW

    // When modifying the flight path.
    int nextId;
    int prevId;
    int modifyId;

    Telemetry_Waypoint_Data_t * waypoints[100]; // Somehow need to get PATH_BUFFER_SIZE here...
    Telemetry_Waypoint_Data_t homebase;
};

struct _CruisingState_Telemetry_Return {
    char editingFlightPathErrorCode; // 0 = success, 1 = error, 2 = incorrect telemetry command
    char pathFollowingErrorCode; // 0 = success, 1 = error, 2 = home base not initialized, 3 = incorrect telemetry command
    int currentWaypointId; 
    int currentWaypointIndex;
};

#endif
