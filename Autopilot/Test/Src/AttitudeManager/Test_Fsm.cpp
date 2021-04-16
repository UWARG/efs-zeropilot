/*
* This file contains all the tests that relate to ensuring the transitions between states of the attitude manager FSM are correct.
* Author: Anthony Berbari, Dhruv Rawat
*/

#include "fff.h"
#include <gtest/gtest.h>

#include "attitudeManager.hpp"
#include "attitudeStateClasses.hpp"
#include "AttitudeDatatypes.hpp"

#include "GetFromPathManager.hpp"
#include "SensorFusion.hpp"
#include "OutputMixing.hpp"
#include "SendInstructionsToSafety.hpp"

#include <string.h>

using namespace std;
using ::testing::Test;

/***********************************************************************************************************************
 * Test Fixtures
 **********************************************************************************************************************/

FAKE_VALUE_FUNC(PMError_t, PM_GetCommands, PMCommands * );
FAKE_VOID_FUNC(SendToSafety_Init);
FAKE_VOID_FUNC(SF_Init);
FAKE_VALUE_FUNC(SFOutput_t, SF_GetResult);
FAKE_VALUE_FUNC(OutputMixing_error_t, OutputMixing_Execute, PID_Output_t * , float * );
FAKE_VALUE_FUNC(SendToSafety_error_t, SendToSafety_Execute, int, int);

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#define ARBITRARY_FLOAT 46.5f

/***********************************************************************************************************************
 * Test Fixtures
 **********************************************************************************************************************/

class AttitudeManagerFSM : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(PM_GetCommands);
			RESET_FAKE(SendToSafety_Init);
			RESET_FAKE(OutputMixing_Execute);
			RESET_FAKE(SendToSafety_Execute);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}

};

class AttitudeManagerDataHandoff : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(PM_GetCommands);
			RESET_FAKE(SendToSafety_Init);
			RESET_FAKE(OutputMixing_Execute);
			RESET_FAKE(SendToSafety_Execute);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}

};

/***********************************************************************************************************************
 * Custom Fakes
 **********************************************************************************************************************/

static float channelOut_custom[4];
static OutputMixing_error_t OutputMixing_Execute_GivesChannelOutCustom(PID_Output_t *PidOutput, float *channelOut)
{
	OutputMixing_error_t dummyError = {0};

	(void) PidOutput;

	for(int i = 0; i < 4; i++)
	{
		channelOut[i] = channelOut_custom[i];
	}

	return dummyError;
}

/***********************************************************************************************************************
 * State Transition Tests (make sure the correct states are reached given some set of circumstances)
 **********************************************************************************************************************/

TEST(AttitudeManagerFSM, InitialStateIsFetchInstructions) {

   	/***********************SETUP***********************/

	attitudeManager attMng;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), fetchInstructionsMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), COMPLETED_CYCLE);

}

TEST(AttitudeManagerFSM, IfFetchInstructionsSucceedsTransitionToSensorFusion) {

   	/***********************SETUP***********************/

	attitudeManager attMng;

	PMError_t PmNoError;
	PmNoError.errorCode = 0;

	/********************DEPENDENCIES*******************/

	PM_GetCommands_fake.return_val = PmNoError;

	/********************STEPTHROUGH********************/

	attMng.setState(fetchInstructionsMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), sensorFusionMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), IN_CYCLE);

}

TEST(AttitudeManagerFSM, IfFetchInstructionsFailsTransitionToFatalFailure) {

   	/***********************SETUP***********************/

	attitudeManager attMng;

	PMError_t PmError;
	PmError.errorCode = 1;

	/********************DEPENDENCIES*******************/

	PM_GetCommands_fake.return_val = PmError;

	/********************STEPTHROUGH********************/

	attMng.setState(fetchInstructionsMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), FatalFailureMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), FAILURE_MODE);

}

TEST(AttitudeManagerFSM, IfSensorFusionSucceedsTransitionToPID) {

   	/***********************SETUP***********************/

	attitudeManager attMng;
	#if 0
	SFError_t SFNoError;
	SFNoError.errorCode = 0;
	#endif

	/********************DEPENDENCIES*******************/

	#if 0
	SF_GetAttitude_fake.return_val = SFNoError;
	#endif

	/********************STEPTHROUGH********************/

	attMng.setState(sensorFusionMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), PIDloopMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), IN_CYCLE);
}

#if 0 // disactivating until the sensor fusion function returns an error code.
TEST(AttitudeManagerFSM, IfSensorFusionFailsTransitionToFailed) {

   	/***********************SETUP***********************/

	attitudeManager attMng;
	SFError_t SFError;
	SFError.errorCode = 1;

	/********************DEPENDENCIES*******************/

	SF_GetAttitude_fake.return_val = SFError;

	/********************STEPTHROUGH********************/

	attMng.setState(sensorFusionMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), FatalFailureMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), FAILURE_MODE);
}
#endif

TEST(AttitudeManagerFSM, IfPIDLoopModeSuccessTransitionToOutputMixing) {

   	/***********************SETUP***********************/

	attitudeManager attMng;
	PMError_t error;
	error.errorCode = 0;

	/********************DEPENDENCIES*******************/

	PM_GetCommands_fake.return_val = error;

	/********************STEPTHROUGH********************/

	attMng.setState(PIDloopMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), OutputMixingMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), IN_CYCLE);

}

TEST(AttitudeManagerFSM, IfPIDLoopModeFailsTransitionToFailed) {

   	/***********************SETUP***********************/

	attitudeManager attMng;
	PMError_t error;
	error.errorCode = -1;

	/********************DEPENDENCIES*******************/

	PM_GetCommands_fake.return_val = error;

	/********************STEPTHROUGH********************/

	attMng.setState(PIDloopMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), FatalFailureMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), FAILURE_MODE);

}

TEST(AttitudeManagerFSM, IfOutputMixingSucceedsTransitionToSendToSafety) {

   	/***********************SETUP***********************/

	attitudeManager attMng;
	OutputMixing_error_t OutputMixingNoError;
	OutputMixingNoError.errorCode = 0;

	/********************DEPENDENCIES*******************/

	OutputMixing_Execute_fake.return_val = OutputMixingNoError;

	/********************STEPTHROUGH********************/

	attMng.setState(OutputMixingMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), sendToSafetyMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), IN_CYCLE);

}

TEST(AttitudeManagerFSM, IfOutputMixingFailsTransitionToFailed) {

   	/***********************SETUP***********************/

	attitudeManager attMng;
	OutputMixing_error_t OutputMixingError;
	OutputMixingError.errorCode = 1;

	/********************DEPENDENCIES*******************/

	OutputMixing_Execute_fake.return_val = OutputMixingError;

	/********************STEPTHROUGH********************/

	attMng.setState(OutputMixingMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), FatalFailureMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), FAILURE_MODE);

}

TEST(AttitudeManagerFSM, IfSendToSafetySucceedsTransitionToFetchInstructions) {

   	/***********************SETUP***********************/

	attitudeManager attMng;
	SendToSafety_error_t SendToSafetyNoError;
	SendToSafetyNoError.errorCode = 0;

	/********************DEPENDENCIES*******************/

	SendToSafety_Execute_fake.return_val = SendToSafetyNoError;

	/********************STEPTHROUGH********************/

	attMng.setState(sendToSafetyMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), fetchInstructionsMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), COMPLETED_CYCLE);

}

TEST(AttitudeManagerFSM, IfSendToSafetyFailsTransitionToFailed) {

   	/***********************SETUP***********************/

	attitudeManager attMng;
	SendToSafety_error_t SendToSafetyError;
	SendToSafetyError.errorCode = 1;

	/********************DEPENDENCIES*******************/

	SendToSafety_Execute_fake.return_val = SendToSafetyError;

	/********************STEPTHROUGH********************/

	attMng.setState(sendToSafetyMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(attMng.getCurrentState()), FatalFailureMode::getInstance());
	EXPECT_EQ(attMng.getStatus(), FAILURE_MODE);

}

/***********************************************************************************************************************
 * Data Handoff Tests (make sure the correct data structures are passed around between states)
 **********************************************************************************************************************/

TEST(AttitudeManagerDataHandoff, CorrectDataIsFedFromOutputMixingToSendToSafety) {

   	/***********************SETUP***********************/

   	channelOut_custom[0] = ARBITRARY_FLOAT;
   	channelOut_custom[1] = ARBITRARY_FLOAT;
   	channelOut_custom[2] = ARBITRARY_FLOAT;
   	channelOut_custom[3] = ARBITRARY_FLOAT;

	attitudeManager attMng;

	/********************DEPENDENCIES*******************/

	OutputMixing_Execute_fake.custom_fake = OutputMixing_Execute_GivesChannelOutCustom;

	/********************STEPTHROUGH********************/

	attMng.setState(OutputMixingMode::getInstance());
	attMng.execute();
	attMng.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(0, memcmp(OutputMixing_Execute_fake.arg1_val, channelOut_custom, 4*sizeof(float)));
	EXPECT_EQ(attMng.getStatus(), FAILURE_MODE);

}

