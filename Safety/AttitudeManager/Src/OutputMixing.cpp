#include "OutputMixing.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/


// All fixed wing directives being commented out for now, will be added back eventually when proper
// preprocessor directives are used

// tails supported by this module
// #define CONVENTIONAL_TAIL 0
// #define INV_V_TAIL 1

// #ifdef SPIKE
// 	#define TAIL_TYPE INV_V_TAIL
// 	static const float RUDDER_PROPORTION = 0.75f; // has to do with the angle of the inverse V tail, 0.75 means spike's tail is at 45 degrees.
// 	static const float ELEVATOR_PROPORTION = 0.75f;
// #else
// 	#define TAIL_TYPE CONVENTIONAL_TAIL
// #endif

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

static OutputMixingErrorCodes checkInputValidity(PID_Output_t *PidOutput);
static void constrainOutput(float *channelOut);

static float map(float num, float minInput, float maxInput, float minOutput, float maxOutput);

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

// TODO: What about flaps ?

OutputMixing_error_t OutputMixing_Execute(PID_Output_t *PidOutput, float *channelOut)
{

	OutputMixing_error_t error;
	error.errorCode = OUTPUT_MIXING_SUCCESS; //TODO: FIX ME THIS IS BAD
	// error.errorCode = checkInputValidity(PidOutput);

	// Populating the channel array with the outputted motor percentages and constraining them from 0 to 100%
	channelOut[FRONT_LEFT_MOTOR_CHANNEL] = PidOutput -> frontLeftMotorPercent;
	channelOut[FRONT_RIGHT_MOTOR_CHANNEL] = PidOutput -> frontRightMotorPercent;
	channelOut[BACK_LEFT_MOTOR_CHANNEL] = PidOutput -> backLeftMotorPercent;
	channelOut[BACK_RIGHT_MOTOR_CHANNEL] = PidOutput -> backRightMotorPercent;


	constrainOutput(channelOut);

// #if TAIL_TYPE == INV_V_TAIL
//     channelOut[L_TAIL_OUT_CHANNEL] =  (PidOutput->yawPercent * RUDDER_PROPORTION) - (PidOutput->pitchPercent * ELEVATOR_PROPORTION); //Tail Output Left
//     channelOut[R_TAIL_OUT_CHANNEL] =  (PidOutput->yawPercent * RUDDER_PROPORTION) + (PidOutput->pitchPercent * ELEVATOR_PROPORTION); //Tail Output Right
// #else
//     channelOut[ELEVATOR_OUT_CHANNEL] =  PidOutput->pitchPercent;
//     channelOut[RUDDER_OUT_CHANNEL] =  PidOutput->rudderPercent;
// #endif

//     channelOut[AILERON_OUT_CHANNEL] = PidOutput->rollPercent;
//     channelOut[THROTTLE_OUT_CHANNEL] = PidOutput->throttlePercent;

    

// #ifdef TARGET_BUILD

//     float adjustedAileron = map(channelOut[AILERON_OUT_CHANNEL], -100, 100, 0, 100);
//     float adjustedElevator = map(channelOut[ELEVATOR_OUT_CHANNEL], -100, 100, 0, 100);

//     channelOut[ELEVATOR_OUT_CHANNEL] = adjustedElevator;
//     channelOut[AILERON_OUT_CHANNEL] = adjustedAileron;

// #endif




	return error;
}

static OutputMixingErrorCodes checkInputValidity(PID_Output_t *PidOutput)
{
	OutputMixingErrorCodes errorCode;

	if ( (PidOutput->frontLeftMotorPercent < 0.0f) 
	|| (PidOutput->frontRightMotorPercent < 0.0f) 
	|| (PidOutput->backLeftMotorPercent < 0.0f) 
	|| (PidOutput->backRightMotorPercent < 0.0f) )
	{
		errorCode = OUTPUT_MIXING_VALUE_TOO_LOW;
	}
	else if ( (PidOutput->frontLeftMotorPercent > 100.0f) 
	|| (PidOutput->frontRightMotorPercent > 100.0f) 
	|| (PidOutput->backLeftMotorPercent > 100.0f) 
	|| (PidOutput->backRightMotorPercent > 100.0f) )
	{
		errorCode = OUTPUT_MIXING_VALUE_TOO_HIGH;
	}
	else
	{
		errorCode = OUTPUT_MIXING_SUCCESS;
	}

	return errorCode;
}

static void constrainOutput(float *channelOut)
{
	for (int i = 0; i < 4; i++)
	{
		if (channelOut[i] < 0.0f)
		{
			channelOut[i] = 0.0f;
		}
		else if (channelOut[i] > 100.0f)
		{
			channelOut[i] = 100.0f;
		}
	}
}

static float map(float num, float minInput, float maxInput, float minOutput, float maxOutput)
{
    return minOutput + ((num - minInput) * ((maxOutput - minOutput) / (maxInput - minInput)));
}

