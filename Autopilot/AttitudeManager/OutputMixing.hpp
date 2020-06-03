/**
 * Algorithms for converting PID outputs into actuator commands
 * for various types of aircraft.
 * Author: Anthony Berbari
 */

#ifndef OUTPUT_MIXING_HPP
#define	OUTPUT_MIXING_HPP

#include "AttitudeDatatypes.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

typedef struct
{
	int errorCode;

}OutputMixing_error_t;

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

/**
* Converts the desired roll, pitch, yaw, and thrust percentages into an array of percentages
* corresponding directly to what percentage of full the actuator attached to each channel should be set to.
* @param[in]		PidOutput 		pointer to the struct containing the desired attitude percentages.
* @param[out]		channelOut 		the array (size of 4 floats) of percentages each channel should be set to.
* @return							the error struct, containing all info about any errors that may have occured.
*/
OutputMixing_error_t OutputMixing_Execute(PID_Output_t *PidOutput, float *channelOut);

#endif
