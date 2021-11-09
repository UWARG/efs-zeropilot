/**
 * Functions for communicating actuator commands to the safety chip,
 * which in turn, sends the data out to the actuators.
 * Author: Anthony Berbari
 */

#ifndef SEND_INSTRUCTIONS_TO_SAFETY_HPP
#define	SEND_INSTRUCTIONS_TO_SAFETY_HPP

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/


typedef struct
{
	int errorCode; // 0 if all is successfull. Other values correspond to various errors which are tbd.

}SendToSafety_error_t;

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

/**
* Initialises internal parameters and instructs the safety to set all actuator channels to their default safe settings.
* Should be called exactly once before anything is attempted to be done with the module.
*/
void SendToSafety_Init(void);

/**
* Communicates the desired actuator commands encoded in the channelOut array over I2C to
* the safety chip. Internally, this function performs a conversion from percentages to pwm values.
* This function is non blocking and returns right away.
* @param[in]		channel 		the output channel for PWM
* @param[in]		percent 		the percentage between 0-100 that the PWM channel should be set to
* @return							the error struct, containing all info about any errors that may have occured.
*/
SendToSafety_error_t SendToSafety_Execute(int channel, int percent);

#endif
