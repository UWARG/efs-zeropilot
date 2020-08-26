/*
*
* A skeleton for testing the sensor fusion algorithm
*
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

	attMng.execute();

	/**********************ASSERTS**********************/

	ASSERT_EQ(*(attMng.getCurrentState()), sensorFusionMode::getInstance());

}

TEST(AttitudeManagerFSM, TransitionFromSensorFusionToPID) {

   	/***********************SETUP***********************/

	attitudeManager attMng;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	attMng.execute();
	attMng.execute();

	/**********************ASSERTS**********************/

	ASSERT_EQ(*(attMng.getCurrentState()), PIDloopMode::getInstance());
}
