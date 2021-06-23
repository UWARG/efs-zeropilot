/**
 * Provides declarations common to the telemetry and path manager interfaces.
 * Author: Anthony Bertnyk, Messed with by Gordon Fountain to turn from Attitude Manager to Telemetry
 */

#ifndef TELEM_PATH_INTERFACE_HPP
#define TELEM_PATH_INTERFACE_HPP

 #include <cstdint>
 #include <vector>

/********************************************************************
* Definitions
********************************************************************/

enum _ModifyFlightPathCommand { NO_FLIGHT_PATH_EDIT = 0, INITIALIZE_FLIGHT_PATH, APPEND, INSERT, UPDATE, DELETE, NUKE }; // Used by cruisingState
enum _GetNextDirectionsCommand { REGULAR_PATH_FOLLOWING = 0, TOGGLE_HOLDING, TOGGLE_HEAD_HOME }; // Used by cruisingState


struct Telemetry_Waypoint_Data_t {
    float latitude;
    float longitude;
    int altitude;
    float turnRadius;
    uint8_t waypointType; // 0 = Path follow, 1 = Orbit, 2 = Hold
};

//Data for path manager to send to telemetry.
struct Telemetry_POGI_t{
	int errorCode; //Code for specified errors
    uint32_t timeStamp;
	float gpsLatitude,gpsLongitude,curAltitude; //Current gps position and altitude
	float curAirspeed; //Airspeed data
	float roll,pitch,yaw;	//Current orientation (radians)
	float camRoll,camPitch,camYaw;	//Current camera orientation (radians)
	bool isLanded; //Switch to check if landed (maybe limit switch?)
	uint8_t editingFlightPathErrorCode,flightPathFollowingErrorCode; //Flight path error codes
	int currentWaypointId, currentWaypointIndex; //current waypoint data
	bool homeBaseInit; //is home base initialized
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

    std::vector<Telemetry_Waypoint_Data_t> waypoints; // Somehow need to get PATH_BUFFER_SIZE here...
    Telemetry_Waypoint_Data_t homebase;

    // landing and takeoff 
    bool beginLanding;
    bool beginTakeoff;
    float stoppingDirectionHeading;
    float stoppingLongitude;
    float stoppingLatitude;
    float stoppingAltitude;
    float takeoffDirectionHeading;

    // Gimbal Commands
    float gimbalPitch;
    float gimbalYaw;

    // Ground Commands
    float groundCommandsHeading;
    float latestDistance;
};

//RTOS stuff that the cpp files need
#ifdef TARGET_BUILD

extern "C"
{
#include "cmsis_os.h"
}

const char PATH_TELEM_MAIL_Q_SIZE = 1;

#endif

#endif
