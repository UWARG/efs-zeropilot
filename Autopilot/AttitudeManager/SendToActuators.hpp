/**
 * Functions for communicating data to servos, motors
 * and other actuators over PWM
 * Author: Anthony Berbari
 */

#ifndef SEND_TO_ACTUATORS_HPP
#define	SEND_TO_ACTUATORS_HPP

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

typedef struct
{
	int errorCode;

}SendToActuators_error_t;

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

/**
* Initialises internal parameters and sets all actuator channels to their default safe settings.
* Should be called exactly once before anything is attempted to be done with the module.
*/
void SendToActuators_Init(void);

/**
* Communicates the desired actuator commands encoded in the channelOut array over PWM to
* the appropriate actuator channels.
* @param[in]		channelOut 		the array (size of 4 floats) of percentages each channel should be set to.
* @return							the error struct, containing all info about any errors that may have occured.
*/
SendToActuators_error_t SendToActuators_Execute(float *channelOut);

#endif
