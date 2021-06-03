#include "PathManagerInterface.h"
#include "pathManager.hpp"

static pathManager pathMng;
PathMan::_Path_Manager_Cycle_Status pathManStatus;


void PathManagerInterfaceExecute(void) {

     do
        {
            pathMng.execute();
            pathManStatus = pathMng.getStatus();

            if (pathManStatus == PathMan::FAILURE_MODE)
            {
                // Something is quite wrong, need to switch over to safety
            }

        } while(pathManStatus != PathMan::COMPLETED_CYCLE);
}