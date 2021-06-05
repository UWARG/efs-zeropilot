
#include "attitudeManagerInterface.h"
#include "attitudeManager.hpp"

static attitudeManager attMng;
AttMan::_Attitude_Manager_Cycle_Status attManStatus;


bool AttitudeManagerInterfaceExecute(void)
{

    do {
            attMng.execute();
            attManStatus = attMng.getStatus();

            if (attManStatus == AttMan::FAILURE_MODE)
            {
                // Something is quite wrong, need to switch over to safety
                return false;
            }

    } while(attManStatus != AttMan::COMPLETED_CYCLE);
    return true;
}
