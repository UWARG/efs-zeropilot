/**
 * Provides declarations common to the telemetry and path manager interfaces.
 * Author: Anthony Bertnyk, Messed with by Gordon Fountain to turn from Attitude Manager to Telemetry
 */
 #include <cstdint>
 #include"FW_CV_Structs.hpp"
 #include"comms.hpp"

#ifndef TELEM_PATH_INTERFACE_HPP
#define TELEM_PATH_INTERFACE_HPP

/********************************************************************
* Definitions
********************************************************************/

enum _ModifyFlightPathCommand { NO_FLIGHT_PATH_EDIT = 0, INITIALIZE_FLIGHT_PATH, APPEND, INSERT, UPDATE, DELETE, NUKE }; // Used by cruisingState
enum _GetNextDirectionsCommand { REGULAR_PATH_FOLLOWING = 0, TOGGLE_HOLDING, TOGGLE_HEAD_HOME }; // Used by cruisingState


struct Telemetry_Waypoint_Data_t {
    long double latitude;
    long double longitude;
    int altitude;
    float turnRadius;
    uint8_t waypointType; // 0 = Path follow, 1 = Orbit, 2 = Hold
};


//Data for path manager to send to telemetry.
typedef struct POGI{
	int errorCode; //Code for specified errors
	double gpsLattitude,gpsLongitude,curAltitude; //Current gps position and altitude
	float curAirspeed; //Airspeed data
	float roll,pitch,yaw;	//Current orientation (radians)
	float camRoll,camPitch,camYaw;	//Current camera orientation (radians)
	bool isLanded; //Switch to check if landed (maybe limit switch?)
	uint8_t editingFlightPathErrorCode,flightPathFollowingErrorCode; //Flight path error codes
	uint8_t currentWaypointId, currentWaypointIndex; //current waypoint data
	bool homeBaseInit; //is home base initialized
    struct foji FOJI;
    pogiData POXI; 
} POGI;

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
    bool beginLanding;
    bool beginTakeoff;
    float stoppingDirectionHeading;
    double stoppingLongitude;
    double stoppingLatitude;
    float stoppingAltitude;
    float takeoffDirectionHeading;
    // Struct for stopping point gps coordinates


    // gimbal (pitch and yaw)
    float gimbalPitch;
    float gimbalYaw;

    struct fijo FIJO;

};

//RTOS stuff that the cpp files need
#ifdef TARGET_BUILD

extern "C"
{
#include "cmsis_os.h"
}

const char PATH_TELEM_MAIL_Q_SIZE = 1;

//Set up a mail queue for sending commands to the path manager
// extern osMailQDef(PIGOMailQ, PATH_TELEM_MAIL_Q_SIZE, Telemetry_PIGO_t);
// extern osMailQId PIGOMailQ;


//Set up a mail queue for sending data to telemetry
// extern osMailQDef(POGIMailQ, PATH_TELEM_MAIL_Q_SIZE, POGI);
// extern osMailQId POGIMailQ;

#endif

#endif
