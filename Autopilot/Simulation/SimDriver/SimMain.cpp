#include "attitudeManager.hpp"
#include "pathManager.hpp"

#include <iostream>

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

#define NUM_SIMULATION_STEPS 1000

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/


int main(void)
{
    pathManager pathmMng;
	attitudeManager attMng;

    PathMan::_Path_Manager_Cycle_Status pathManStatus;
    AttMan::_Attitude_Manager_Cycle_Status attManStatus;


	for (int i = 0; i < NUM_SIMULATION_STEPS; i++)
	{
        do
        {
            pathmMng.execute();
            pathManStatus = pathmMng.getStatus();

            if (pathManStatus == PathMan::FAILURE_MODE)
            {
                std::cerr << "Path Manager has entered failed state, killing simulation." << std::endl;

                return -1;
            }

        } while(pathManStatus != PathMan::COMPLETED_CYCLE);


        do
        {
            attMng.execute();
            attManStatus = attMng.getStatus();

            if (attManStatus == AttMan::FAILURE_MODE)
            {
                std::cerr << "Attitude Manager has entered failed state, killing simulation." << std::endl;

                return -2;
            }

        } while(attManStatus != AttMan::COMPLETED_CYCLE);

	}

	return 0;
}
