
#include "attitudeManager.hpp"

#include "SendInstructionsToSafety.hpp"

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

#define NUM_SIMULATION_STEPS 1000

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/


int main(void)
{
	attitudeManager attMng;

	SendToSafety_Init();	// TODO Only until we have a proper way to initialize modules

	for (int i = 0; i < NUM_SIMULATION_STEPS; i++)
	{
		attMng.execute();
	}

	return 0;
}
