/*
* Author: Dhruv Rawat
*/

#include "fff.h"
#include <gtest/gtest.h>

#include "pathManager.hpp"
#include "pathStateClasses.hpp"

#include "landingMode.hpp"

#include "pathModeSelector.hpp"
#include "cruisingMode.hpp"
#include "cruisingModeStages.hpp"
#include "cruisingFlight.hpp"

FAKE_VOID_FUNC(CommFromPMToAMInit);
FAKE_VOID_FUNC(SendFromPMToAM, CommandsForAM*);
FAKE_VALUE_FUNC(bool, GetFromAMToPM, AttitudeData*);
FAKE_VALUE_FUNC(SFError_t, SF_GetResult, SFOutput_t *);
FAKE_VALUE_FUNC(IMU_Data_t, SF_GetRawIMU);
FAKE_VOID_FUNC(AutoSteer_Init);
FAKE_VOID_FUNC(CommWithAMInit);
FAKE_VOID_FUNC(SendCommandsForAM, CommandsForAM*);
FAKE_VOID_FUNC(CommWithTelemInit);
FAKE_VALUE_FUNC(bool, GetTelemetryCommands, Telemetry_PIGO_t*);
FAKE_VALUE_FUNC(bool, GetAttitudeData, AttitudeData*);
FAKE_VOID_FUNC(AutoSteer_ComputeCoordinatedTurn, CoordinatedTurnInput_t*, CoordinatedTurnAttitudeManagerCommands_t*);
FAKE_VOID_FUNC(AutoSteer_ComputeAltitudeAndAirspeed, AltitudeAirspeedInput_t*, AltitudeAirspeedCommands_t*);

class CruisingModeFSM : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(CommFromPMToAMInit);
			RESET_FAKE(SendFromPMToAM);
			RESET_FAKE(GetFromAMToPM);
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

class CruisingModeDataHandoff : public ::testing::Test
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

static Telemetry_Waypoint_Data_t createTelemetryWaypoint(long double lon, long double lat, int alt, float turnRadius, int type) {
    Telemetry_Waypoint_Data_t waypoint {};

    waypoint.longitude = lon;
    waypoint.latitude = lat;
    waypoint.altitude = alt;
    waypoint.turnRadius = turnRadius;
    waypoint.waypointType = type;

    return waypoint;
}

TEST(CruisingModeFSM, CruisingModeCalledByModeExecutor) {

	/***********************SETUP***********************/
	pathManager pathMan; 

	/********************DEPENDENCIES*******************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();
	path_mode_selector->setCurrentMode(CruisingMode::getInstance());

	/********************STEPTHROUGH********************/

	pathMan.setState(modeExecutor::getInstance());
    pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(path_mode_selector->getCurrentMode()), CruisingMode::getInstance());
	EXPECT_EQ(path_mode_selector->getCurrentModeEnum(), MODE_CRUISING);
	EXPECT_EQ(path_mode_selector->getEditingFlightPathErrorCode(), MODIFY_CRUISING_SUCCESS);
	EXPECT_EQ(path_mode_selector->getPathFollowingErrorCode(), PATH_CRUISING_SUCCESS);
	EXPECT_EQ(path_mode_selector->getCurrentWaypointId(), 3); // 3 because of stupid boogaloo in waypointManager.cpp constructor
	EXPECT_EQ(path_mode_selector->getCurrentWaypointIndex(), 2); // 2 because of stupid boogaloo in waypointManager.cpp constructor
	EXPECT_EQ(path_mode_selector->getHomeBaseInitialized(), true);
    EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

TEST(CruisingModeFSM, CruisingFlightTransitionToCruisingFlight) {

	/***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	CruisingMode cruising_mode_instance = *(static_cast<CruisingMode*>(&CruisingMode::getInstance()));

	Telemetry_PIGO_t telem_data;

	telem_data.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
	telem_data.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
	telem_data.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
	telem_data.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	telem_data.numWaypoints = 4;
	telem_data.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
	telem_data.initializingHomeBase = 1;
	telem_data.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING;
	telem_data.holdingAltitude = 0;
	telem_data.holdingTurnRadius = 0;
	telem_data.holdingTurnDirection = 0;
	telem_data.nextId = 0;
	telem_data.prevId = 0;
	telem_data.modifyId = 0;
	telem_data.homebase = createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0);

	SFOutput_t sf_data;
	sf_data.track = -1;
	sf_data.altitude = -2;
	sf_data.airspeed = -3;

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	cruising_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(cruising_mode_instance.getCurrentStage()), CruisingFlight::getInstance());
}

TEST(CruisingModeFSM, CruisingFlightTransitionToLandingTransitionModeWhenDone) {
	/***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();

	CruisingMode cruising_mode_instance = *(static_cast<CruisingMode*>(&CruisingMode::getInstance()));

	Telemetry_PIGO_t telem_data;

	telem_data.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
	telem_data.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
	telem_data.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
	telem_data.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	telem_data.beginLanding = true;
	telem_data.numWaypoints = 4;
	telem_data.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
	telem_data.initializingHomeBase = 1;
	telem_data.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING;
	telem_data.holdingAltitude = 0;
	telem_data.holdingTurnRadius = 0;
	telem_data.holdingTurnDirection = 0;
	telem_data.nextId = 0;
	telem_data.prevId = 0;
	telem_data.modifyId = 0;
	telem_data.homebase = createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0);

	SFOutput_t sf_data;
	sf_data.track = -1;
	sf_data.altitude = -2;
	sf_data.airspeed = -3;

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	cruising_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(path_mode_selector->getCurrentMode()), LandingMode::getInstance());
	EXPECT_EQ(path_mode_selector->getCurrentModeEnum(), MODE_LANDING);
}
