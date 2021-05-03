/**
 * Provides declarations common to the telemetry and path manager interfaces.
 * Author: Anthony Bertnyk, Messed with by Gordon Fountain to turn from Attitude Manager to Telemetry
 */

#ifndef TELEM_PATH_INTERFACE_HPP
#define TELEM_PATH_INTERFACE_HPP

//Data for path manager to send to telemetry.
typedef struct POGI{
	int errorCode; //Code for specified errors
	float gpsLattitude,gpsLongitude; //Current gps position
	int curAltitude,curAirspeed; //Airspeed and altitude data
	float roll,pitch,yaw;	//Current orientation (radians)
	bool isLanded; //Switch to check if landed (maybe limit switch?)
} POGI;

//Data for telemetry to send to path manager.
typedef struct PIGO{
	float gpsLattitude,gpsLongitude; //Commanded gps position
	float heading,distance //Commanded heading/distance
	float roll,pitch,yaw;	//Commanded orientation (radians)
	bool beginLand; //Command to start landing process
	bool beginTakeoff; //Starting command
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
