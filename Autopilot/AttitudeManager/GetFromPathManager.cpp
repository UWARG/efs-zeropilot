#include "GetFromPathManager.hpp"

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

// dummy implementation until real one is required.
PMError_t PM_GetCommands(PMCommands *Commands)
{
	// (void) Commands;

	/*** Just for tests to pass ***/
	Commands->yaw = 0.0f;
	Commands->roll = 0;
	Commands->pitch = 0;
	Commands->airspeed = 0;

	PMError_t errorStruct;
	errorStruct.errorCode = 0;

	return errorStruct;
}
