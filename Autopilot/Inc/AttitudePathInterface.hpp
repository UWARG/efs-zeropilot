/**
 * Provides declarations common to the attitude and path manager interfaces.
 * Author: Anthony Bertnyk, Anthony Berbari
 */

#ifndef ATTITUDE_PATH_INTERFACE_HPP
#define ATTITUDE_PATH_INTERFACE_HPP

#include "pathDatatypes.hpp"
#include "AttitudeDatatypes.hpp"


//Commands for path manager to send to attitude manager.

#if IS_FIXED_WING

typedef struct CommandsForAM{
	float roll,pitch;	// commanded orientation (radians)
	float rudderPercent;
	float throttlePercent;
    _PassbyControl passbyData;
} CommandsForAM;


//Data for attitude manager to send to path manager.
typedef struct AttitudeData{
	float roll,pitch,yaw;	// current orientation (radians)
	float airspeed;			// current airspeed (m/s)
	float accX, accY, accZ; // accelerometer data
	float gyrX, gyrY, gyrZ; // gyroscope data
} AttitudeData;

#else 
struct CommandsForAM{
	float rotation; //orientation? in radians? 
	float desiredX, desiredY, desiredZ;
	float airspeed; 
}; 	 		



#endif





//RTOS stuff that the cpp files need
#ifdef TARGET_BUILD

extern "C"
{
#include "cmsis_os.h"
}

const char PATH_ATTITUDE_MAIL_Q_SIZE = 1;

#endif

#endif
