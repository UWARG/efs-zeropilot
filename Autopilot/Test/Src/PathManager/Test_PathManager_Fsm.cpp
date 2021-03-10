/*
* Author: Dhruv Rawat
*/

#include <gtest/gtest.h>

#include "pathManager.hpp"
#include "pathStateClasses.hpp"
#include "waypointManager.hpp"
#include "cruisingState.hpp"


using namespace std; 
using ::testing::Test;

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

