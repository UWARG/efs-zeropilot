/**
 * Gets the commanded orientation and airspeed from the path manager.
 * Author: Anthony Berbari
 */

#ifndef GET_FROM_PATH_MANAGER_HPP
#define GET_FROM_PATH_MANAGER_HPP

#include "pathDatatypes.hpp"

struct PMCommands{
	float roll, pitch;	// commanded orientation (radians)
	float rudderPercent;
	float throttlePercent;
	_PassbyControl passbyData;
};

struct PMError_t{
    int errorCode;
};

/**
* Communicates with the higher level state machine to retrieve orientation and airspeed commands.
* @param[out]	Commands 	Pointer to the struct containing the commands.
* @return 					error struct
*/
PMError_t PM_GetCommands(PMCommands *Commands);

#endif // ATTITUDE_PATH_INTERFACE_HPP
