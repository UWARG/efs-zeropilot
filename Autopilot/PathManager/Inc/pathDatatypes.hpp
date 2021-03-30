#include <stdint.h>
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
    float windDirectionHeading;
    float stoppingDirectionHeading;
    // Struct for stopping point gps coordinates

};

struct Altimeter_Data_t {

    float pressure, altitude, temp;

    bool isDataNew; 
    int status; //TBD but probably 0 = SUCCESS, -1 = FAIL, 1 = BUSY 
    float utcTime; //Last time GetResult was called
};

struct Gps_Data_t{
    long double latitude;  // 8 Bytes
    long double longitude; // 8 Bytes
    float utcTime;     // 4 Bytes. Time in seconds since 00:00 (midnight)
    float groundSpeed; // in m/s
    int altitude; // in m
    int16_t heading; // in degrees. Should be between 0-360 at all times, but using integer just in case
    uint8_t numSatellites;    // 1 Byte

    uint8_t sensorStatus; // 0 = no fix, 1 = gps fix, 2 = differential gps fix (DGPS) (other codes are possible)
    bool dataIsNew; // true if data has been refreshed since the previous time GetResult was called, false otherwise.

};



#endif
