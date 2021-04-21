/*
* Author: Dhruv Rawat
*/

#include "fff.h"
#include <gtest/gtest.h>

#include "pathManager.hpp"
#include "pathStateClasses.hpp"

#include "waypointManager.hpp"
#include "cruisingState.hpp"
#include "landingTakeoffManager.hpp"
#include "CommWithAttitudeManager.hpp"

#include "SensorFusion.hpp"
#include "AttitudePathInterface.hpp"


using namespace std; 
using ::testing::Test;

/***********************************************************************************************************************
 * Test Fixtures
 **********************************************************************************************************************/

FAKE_VALUE_FUNC(SFError_t, SF_GetResult, SFOutput_t *);
FAKE_VALUE_FUNC(IMU_Data_t, SF_GetRawIMU);
FAKE_VOID_FUNC(AutoSteer_Init);
FAKE_VOID_FUNC(CommWithAMInit);
FAKE_VOID_FUNC(SendCommandsForAM, CommandsForAM*);
FAKE_VALUE_FUNC(bool, GetAttitudeData, AttitudeData*);
FAKE_VOID_FUNC(AutoSteer_ComputeCoordinatedTurn, CoordinatedTurnInput_t*, CoordinatedTurnAttitudeManagerCommands_t*);
FAKE_VOID_FUNC(AutoSteer_ComputeAltitudeAndAirspeed, AltitudeAirspeedInput_t*, AltitudeAirspeedCommands_t*);

class PathManagerFSM : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(SF_GetResult);
			RESET_FAKE(SF_GetRawIMU);
			RESET_FAKE(AutoSteer_Init);
			RESET_FAKE(CommWithAMInit);
			RESET_FAKE(SendCommandsForAM);
			RESET_FAKE(GetAttitudeData);
			RESET_FAKE(AutoSteer_ComputeCoordinatedTurn);
			RESET_FAKE(AutoSteer_ComputeAltitudeAndAirspeed);
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
			RESET_FAKE(SF_GetResult);
			RESET_FAKE(SF_GetRawIMU);
			RESET_FAKE(AutoSteer_Init);
			RESET_FAKE(CommWithAMInit);
			RESET_FAKE(SendCommandsForAM);
			RESET_FAKE(GetAttitudeData);
			RESET_FAKE(AutoSteer_ComputeCoordinatedTurn);
			RESET_FAKE(AutoSteer_ComputeAltitudeAndAirspeed);
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

TEST (PathManagerFSM, CommsWithTelemetryTransitionToSensorFusion) {
	/***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(commsWithTelemetry::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), sensorFusion::getInstance());
}

TEST (PathManagerFSM, SensorFusionTransitionToResetVariablesState) {
	/***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(sensorFusion::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), resetVariables::getInstance());
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

/************************************************************************************************************
 _FlightStage Enum Testing
 ***********************************************************************************************************/

TEST (PathManagerFSM, ResetVariablesToLandingTransition) {
	/***********************SETUP***********************/

	pathManager pathMan;
	pathMan.stage = TRANSITION;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(resetVariables::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), landingTransitionStage::getInstance());
}

TEST (PathManagerFSM, ResetVariablesToLandingSlope) {
	/***********************SETUP***********************/

	pathManager pathMan;
	pathMan.stage = SLOPE;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(resetVariables::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), landingSlopeStage::getInstance());
}

TEST (PathManagerFSM, ResetVariablesToLandingFlare) {
	/***********************SETUP***********************/

	pathManager pathMan;
	pathMan.stage = FLARE;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(resetVariables::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), landingFlareStage::getInstance());
}

TEST (PathManagerFSM, ResetVariablesToLandingDecrab) {
	/***********************SETUP***********************/

	pathManager pathMan;
	pathMan.stage = DECRAB;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(resetVariables::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), landingDecrabStage::getInstance());
}

TEST (PathManagerFSM, ResetVariablesToLandingTouchdown) {
	/***********************SETUP***********************/

	pathManager pathMan;
	pathMan.stage = TOUCHDOWN;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(resetVariables::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), landingTouchdownStage::getInstance());
}

TEST (PathManagerFSM, ResetVariablesToCruising) {
	/***********************SETUP***********************/

	pathManager pathMan;
	pathMan.stage = CRUISING;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(resetVariables::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), cruisingState::getInstance());
}

TEST (PathManagerFSM, ResetVariablesToTakeoffRoll) {
	/***********************SETUP***********************/

	pathManager pathMan;
	pathMan.stage = ROLL;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(resetVariables::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), takeoffRollStage::getInstance());
}

TEST (PathManagerFSM, ResetVariablesToTakeoffClimb) {
	/***********************SETUP***********************/

	pathManager pathMan;
	pathMan.stage = CLIMB;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(resetVariables::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), takeoffClimbStage::getInstance());
}

TEST (PathManagerFSM, TransitionToCT) {
	/***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(landingTransitionStage::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

TEST (PathManagerFSM, SlopeToCT) {
	/***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(landingSlopeStage::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

TEST (PathManagerFSM, FlareToCT) {
	/***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(landingFlareStage::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

TEST (PathManagerFSM, DecrabToCT) {
	/***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(landingDecrabStage::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

TEST (PathManagerFSM, TouchdownToCT) {
	/***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(landingTouchdownStage::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

TEST (PathManagerFSM, RollToCT) {
	/***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(takeoffRollStage::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

TEST (PathManagerFSM, ClimbToCT) {
	/***********************SETUP***********************/

	pathManager pathMan;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	pathMan.setState(takeoffClimbStage::getInstance());
	pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

