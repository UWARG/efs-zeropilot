/*
* This file contains all the tests that relate to ensuring the transitions between states of the attitude manager FSM are correct.
* Author: Anthony Berbari
*/

#include "fff.h"
#include <gtest/gtest.h>

#include "attitudeManager.hpp"
#include "attitudeStateClasses.hpp"

#include "GetFromPathManager.hpp"
#include "SensorFusion.hpp"

using namespace std;
using ::testing::Test;

/***********************************************************************************************************************
 * Fakes
 **********************************************************************************************************************/

FAKE_VALUE_FUNC(PMError_t, PM_GetCommands, PMCommands * );
FAKE_VALUE_FUNC(SFError_t, SF_GetResult, SFOutput_t * );

/***********************************************************************************************************************
 * Test Fixtures
 **********************************************************************************************************************/

class AttitudeManagerFSM : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(PM_GetCommands);
			RESET_FAKE(SF_GetResult);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}

};

/***********************************************************************************************************************
 * Tests
 **********************************************************************************************************************/

TEST(AttitudeManagerFSM, InitialStateIsFetchInstructions) {

   	/***********************SETUP***********************/

	attitudeManager attMng;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
	/**********************ASSERTS**********************/

	ASSERT_EQ(*(attMng.getCurrentState()), fetchInstructionsMode::getInstance());

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

	ASSERT_EQ(*(attMng.getCurrentState()), sensorFusionMode::getInstance());

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

	ASSERT_EQ(*(attMng.getCurrentState()), FatalFailureMode::getInstance());

}

TEST(AttitudeManagerFSM, IfSensorFusionSucceedsTransitionToPID) {

   	/***********************SETUP***********************/

	attitudeManager attMng;
	SFError_t SFNoError;
	SFNoError.errorCode = 0;

	/********************DEPENDENCIES*******************/

	SF_GetResult_fake.return_val = SFNoError;

	/********************STEPTHROUGH********************/

	attMng.setState(sensorFusionMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	ASSERT_EQ(*(attMng.getCurrentState()), PIDloopMode::getInstance());
}

TEST(AttitudeManagerFSM, IfSensorFusionFailsTransitionToFailed) {

   	/***********************SETUP***********************/

	attitudeManager attMng;
	SFError_t SFError;
	SFError.errorCode = 1;

	/********************DEPENDENCIES*******************/

	SF_GetResult_fake.return_val = SFError;

	/********************STEPTHROUGH********************/

	attMng.setState(sensorFusionMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	ASSERT_EQ(*(attMng.getCurrentState()), FatalFailureMode::getInstance());
}


TEST(AttitudeManagerFSM, TransitionFromPIDToOutputMixing) {

   	/***********************SETUP***********************/

	attitudeManager attMng;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	attMng.setState(PIDloopMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	ASSERT_EQ(*(attMng.getCurrentState()), OutputMixingMode::getInstance());
}

TEST(AttitudeManagerFSM, TransitionFromOutputMixingToSendToSafety) {

   	/***********************SETUP***********************/

	attitudeManager attMng;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	attMng.setState(OutputMixingMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	ASSERT_EQ(*(attMng.getCurrentState()), sendToSafetyMode::getInstance());
}

TEST(AttitudeManagerFSM, TransitionFromSendToSafetyToFetchInstructions) {

   	/***********************SETUP***********************/

	attitudeManager attMng;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	attMng.setState(sendToSafetyMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	ASSERT_EQ(*(attMng.getCurrentState()), fetchInstructionsMode::getInstance());
}
