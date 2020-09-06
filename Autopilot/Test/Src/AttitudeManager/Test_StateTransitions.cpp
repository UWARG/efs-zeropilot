/*
* This file contains all the tests that relate to ensuring the transitions between states of the attitude manager FSM are correct.
* Author: Anthony Berbari
*/

#include <gtest/gtest.h>
#include "fff.h"

#include "attitudeManager.hpp"
#include "attitudeStateClasses.hpp"

using namespace std;
using ::testing::Test;

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

TEST(AttitudeManagerFSM, TransitionFromFetchInstructionsToSensorFusion) {

   	/***********************SETUP***********************/

	attitudeManager attMng;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	attMng.setState(fetchInstructionsMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	ASSERT_EQ(*(attMng.getCurrentState()), sensorFusionMode::getInstance());

}

TEST(AttitudeManagerFSM, TransitionFromSensorFusionToPID) {

   	/***********************SETUP***********************/

	attitudeManager attMng;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	attMng.setState(sensorFusionMode::getInstance());
	attMng.execute();

	/**********************ASSERTS**********************/

	ASSERT_EQ(*(attMng.getCurrentState()), PIDloopMode::getInstance());
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
