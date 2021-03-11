
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

    // Nixon's stuff
    bool packageOnBoard;
    float windSpeed;
    float stoppingDirectionHeading;
    float longitude;
    float latitude;
    float altitude;
    // Struct for stopping point gps coordinates

};

#endif