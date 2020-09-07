#include "GetFromPathManager.hpp"

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/


#ifdef SIMULATION

PMError_t PM_GetCommands(PMCommands *Commands)
{
	// Commands are hard coded atm. If people want to get really fancy, they can make some python script to generate an input file read by this function or something


	Commands->roll = 0;
	Commands->pitch = 0;
	Commands->yaw = 0;
	Commands->airspeed = 50;

	PMError_t errorStruct;
	errorStruct.errorCode = 0;

	return errorStruct;
}

#else

// dummy implementation until real one is required.
PMError_t PM_GetCommands(PMCommands *Commands)
{
	(void) Commands;

	PMError_t errorStruct;
	errorStruct.errorCode = 0;

	return errorStruct;
}

#endif
