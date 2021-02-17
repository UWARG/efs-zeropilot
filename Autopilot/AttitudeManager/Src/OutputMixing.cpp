#include "OutputMixing.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

// tails supported by this module
#define CONVENTIONAL_TAIL 0
#define INV_V_TAIL 1

#ifdef SPIKE
	#define TAIL_TYPE INV_V_TAIL
	static const float RUDDER_PROPORTION = 0.75f; // has to do with the angle of the inverse V tail, 0.75 means spike's tail is at 45 degrees.
	static const float ELEVATOR_PROPORTION 0.75f;
#else
	#define TAIL_TYPE CONVENTIONAL_TAIL
#endif

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

static int checkInputValidity(PID_Output_t *PidOutput);
static void constrainOutput(float *channelOut);

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

// TODO: What about flaps ?

OutputMixing_error_t OutputMixing_Execute(PID_Output_t *PidOutput, float *channelOut)
{

	OutputMixing_error_t error;
	error.errorCode = checkInputValidity(PidOutput);

#if TAIL_TYPE == INV_V_TAIL
    channelOut[L_TAIL_OUT_CHANNEL] =  (PidOutput->yawPercent * RUDDER_PROPORTION) - (PidOutput->pitchPercent * ELEVATOR_PROPORTION); //Tail Output Left
    channelOut[R_TAIL_OUT_CHANNEL] =  (PidOutput->yawPercent * RUDDER_PROPORTION) + (PidOutput->pitchPercent * ELEVATOR_PROPORTION); //Tail Output Right
#else
    channelOut[ELEVATOR_OUT_CHANNEL] =  PidOutput->pitchPercent;
    channelOut[RUDDER_OUT_CHANNEL] =  PidOutput->yawPercent;
#endif

    channelOut[AILERON_OUT_CHANNEL] = PidOutput->rollPercent;
    channelOut[THROTTLE_OUT_CHANNEL] = PidOutput->throttlePercent;

    constrainOutput(channelOut);

	return error;
}

static int checkInputValidity(PID_Output_t *PidOutput)
{
	int errorCode;

	if ( (PidOutput->rollPercent < -100.0f) || (PidOutput->pitchPercent < -100.0f) || (PidOutput->yawPercent < -100.0f) || (PidOutput->throttlePercent < 0.0f) )
	{
		errorCode = 1;
	}
	else if ( (PidOutput->rollPercent > 100.0f) || (PidOutput->pitchPercent > 100.0f) || (PidOutput->yawPercent > 100.0f) || (PidOutput->throttlePercent > 100.0f) )
	{
		errorCode = 2;
	}
	else
	{
		errorCode = 0;
	}

	return errorCode;
}

static void constrainOutput(float *channelOut)
{
	for (int i = 0; i < 4; i++)
	{
		if (channelOut[i] < -100.0f)
		{
			channelOut[i] = -100.0f;
		}
		else if (channelOut[i] > 100.0f)
		{
			channelOut[i] = 100.0f;
		}
	}
}
