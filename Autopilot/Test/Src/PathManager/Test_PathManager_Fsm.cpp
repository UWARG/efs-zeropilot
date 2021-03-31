/*
* Author: Dhruv Rawat
*/

#include "fff.h"
#include <gtest/gtest.h>

#include "pathManager.hpp"
#include "pathStateClasses.hpp"

#include "waypointManager.hpp"
#include "cruisingState.hpp"
#include "getSensorData.hpp"
#include "gps.hpp"
#include "altimeter.hpp"

using namespace std; 
using ::testing::Test;

/***********************************************************************************************************************
 * Test Fixtures
 **********************************************************************************************************************/

FAKE_VALUE_FUNC(SensorError_t, Path_SensorMeasurements_GetResult, Altimeter*, Gps*, Altimeter_Data_t*, Gps_Data_t*);

class PathManagerFSM : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(Path_SensorMeasurements_GetResult);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}

};

class PathManagerDataHandoff : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(Path_SensorMeasurements_GetResult);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}
};

/***********************************************************************************************************************
 * State Transition Tests (make sure the correct states are reached given some set of circumstances)
 **********************************************************************************************************************/


TEST (PathManagerFSM, InitialStateIsCommsWithAttitude) {
    /***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
	/**********************ASSERTS**********************/

    EXPECT_EQ(*(pathMan.getCurrentState()), commsWithAttitude::getInstance());
}

/***************************************
TESTING TRANSITION BETWEEN NORMAL STATES
****************************************/

TEST (PathManagerFSM, CommsWithAttitudeTransitionToCommsWithTelemetry) {
    /***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

    pathMan.setState(commsWithAttitude::getInstance());
    pathMan.execute();

	/**********************ASSERTS**********************/

    EXPECT_EQ(*(pathMan.getCurrentState()), commsWithTelemetry::getInstance());
}

TEST (PathManagerFSM, CommsWithTelemetryTransitionToGetSensorData) {
    /***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

    pathMan.setState(commsWithTelemetry::getInstance());
    pathMan.execute();

	/**********************ASSERTS**********************/

    EXPECT_EQ(*(pathMan.getCurrentState()), getSensorData::getInstance());
}

TEST (PathManagerFSM, GetSensorDataTransitionToSensorFusion) {
    /***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

    pathMan.setState(getSensorData::getInstance());
    pathMan.execute();

	/**********************ASSERTS**********************/

    EXPECT_EQ(*(pathMan.getCurrentState()), sensorFusion::getInstance());
}

TEST (PathManagerFSM, SensorFusionTransitionToCruisingState) {
    /***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

    pathMan.setState(sensorFusion::getInstance());
    pathMan.execute();

	/**********************ASSERTS**********************/

    EXPECT_EQ(*(pathMan.getCurrentState()), cruisingState::getInstance());
}

TEST (PathManagerFSM, CruisingStateTransitionToCoordinatedTurnElevation) {
    /***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

    pathMan.setState(cruisingState::getInstance());
    pathMan.execute();

	/**********************ASSERTS**********************/

    EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

TEST (PathManagerFSM, CoordinatedTurnElevationTransitionToCommsWithAttitude) {
    /***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

    pathMan.setState(coordinateTurnElevation::getInstance());
    pathMan.execute();

	/**********************ASSERTS**********************/

    EXPECT_EQ(*(pathMan.getCurrentState()), commsWithAttitude::getInstance());
}

