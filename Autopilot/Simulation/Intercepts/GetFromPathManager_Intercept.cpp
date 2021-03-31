#include "GetFromPathManager.hpp"

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

// dummy implementation until real one is required.
PMError_t PM_GetCommands(PMCommands *Commands)
{
// Commands are hard coded until the path manager state machine is written and can communicate with attitude manager.


    Commands->roll = 0;
    Commands->pitch = 0;
    Commands->rudderPercent = 0;
    Commands->throttlePercent = 100;
    PMError_t errorStruct;
    errorStruct.errorCode = 0;

    return errorStruct;

}

