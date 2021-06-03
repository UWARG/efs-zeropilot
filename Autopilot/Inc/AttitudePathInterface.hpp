/**
 * Provides declarations common to the attitude and path manager interfaces.
 * Author: Anthony Bertnyk, Anthony Berbari
 */

#ifndef ATTITUDE_PATH_INTERFACE_HPP
#define ATTITUDE_PATH_INTERFACE_HPP

#include "pathDatatypes.hpp"
#include "AttitudeDatatypes.hpp"


//Commands for path manager to send to attitude manager.
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

//RTOS stuff that the cpp files need
#ifdef TARGET_BUILD

extern "C"
{
#include "cmsis_os.h"
}

const char PATH_ATTITUDE_MAIL_Q_SIZE = 1;

//Set up a mail queue for sending commands to the attitude manager
extern osMailQDef(commandsMailQ, PATH_ATTITUDE_MAIL_Q_SIZE, CommandsForAM);
extern osMailQId commandsMailQ;


//Set up a mail queue for sending data to the path manager
extern osMailQDef(attitudeDataMailQ, PATH_ATTITUDE_MAIL_Q_SIZE, AttitudeData);
extern osMailQId attitudeDataMailQ;

#endif

#endif
