/**
 * Provides declarations common to the telemetry and path manager interfaces.
 * Author: Anthony Bertnyk, Messed with by Gordon Fountain to turn from Attitude Manager to Telemetry
 */

#ifndef TELEM_PATH_INTERFACE_HPP
#define TELEM_PATH_INTERFACE_HPP

//Data for path manager to send to telemetry.
typedef struct POGI{
	int errorCode; //Code for specified errors
	double gpsLattitude,gpsLongitude, curAltitude; //Current gps position and altitude
	float curAirspeed; //Airspeed data
	float roll,pitch,yaw;	//Current orientation (radians)
	float camRoll,camPitch,camYaw;	//Current camera orientation (radians)
	bool isLanded; //Switch to check if landed (maybe limit switch?)
	uint8_t editingFlightPathErrorCode, flightPathFollowingErrorCode; //Flight path error codes
	uint8_t currentWaypointId, currentWaypointIndex; //current waypoint data
	bool homeBaseInit; //is home base initialized
} POGI;

//Data for telemetry to send to path manager.
typedef struct PIGO{
	double gpsLattitude,gpsLongitude, altitude; //Commanded gps position and altitude
	float landDirection,heading,distance //Commanded heading/distance/landing direction
	float pitch,yaw;	//Commanded orientation (radians)
	bool beginLand; //Command to start landing process
	bool beginTakeoff; //Starting command
	uint8_t numWaypoints; //number of waypoints
	uint8_t waypointModifyFlightPathCommand, waypointNextDirectionsCommand; //extra waypoint commands
	uint8_t holdingAltitude, holdingTurnRadius, flightPathModifyNextId, flightPathModifyPrevId, flightPathModifyId; //flight path data
	uint8_t holdingTurnDirection; //turn direction
	//waypoint* waypoints; //list of waypoints (need to see if this is defined in waypoint code)
	//homebase data (possibly constants?)
	bool disconnectAutopilot; //Command to switch off autopilot
} PIGO;

//RTOS stuff that the cpp files need
#ifdef TARGET_BUILD

extern "C"
{
#include "cmsis_os.h"
}

const char MAIL_Q_SIZE = 1;

//Set up a mail queue for sending commands to the path manager
extern osMailQDef(PMcommandsMailQ, MAIL_Q_SIZE, PIGO);
extern osMailQId PMcommandsMailQ;


//Set up a mail queue for sending data to telemetry
extern osMailQDef(telemDataMailQ, MAIL_Q_SIZE, POGI);
extern osMailQId telemDataMailQ;

#endif

#endif
