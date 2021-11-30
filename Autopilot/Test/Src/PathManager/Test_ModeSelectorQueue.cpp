/*
* Author: Steven Gong
*/

#include "fff.h"
#include <gtest/gtest.h>

#include "pathManager.hpp"
#include "pathStateClasses.hpp"

#include "waypointManager.hpp"
#include "landingTakeoffManager.hpp"
#include "CommWithAttitudeManager.hpp"
#include "CommsWithTelemetry.hpp"

#include "landingMode.hpp"
#include "takeoffMode.hpp"
#include "cruisingMode.hpp"

#include "SensorFusion.hpp"
#include "AttitudePathInterface.hpp"


using namespace std;
using ::testing::Test;

/***********************************************************************************************************************
 * Test Fixtures
 **********************************************************************************************************************/
static Telemetry_Waypoint_Data_t createTelemetryWaypoint(long double lon, long double lat, int alt, float turnRadius, int type) {
    Telemetry_Waypoint_Data_t waypoint {};

    waypoint.longitude = lon;
    waypoint.latitude = lat;
    waypoint.altitude = alt;
    waypoint.turnRadius = turnRadius;
    waypoint.waypointType = type;

    return waypoint;
}

TEST (ModeSelectorQueue, CommsWithAttitudeTransitionToCommsWithTelemetry) {
	/***********************SETUP***********************/

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	/**********************ASSERTS**********************/

}
TEST (ModeSelectorQueue, testtoname) {
	/***********************SETUP***********************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();

	TakeoffMode takeoff_mode_instance = *(static_cast<TakeoffMode*>(&TakeoffMode::getInstance()));
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

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
    path_mode_selector->setCurrentMode(cruising_mode_instance);
    path_mode_selector->execute(telem_data, sf_data, imu_data);
    
    Telemetry_PIGO_t instruction = path_mode_selector->dequeueflightPathEditInstructions();
	/**********************ASSERTS**********************/
    EXPECT_EQ(instruction.numWaypoints, 4);
    EXPECT_EQ(instruction.waypointModifyFlightPathCommand, INITIALIZE_FLIGHT_PATH);
    EXPECT_EQ(instruction.initializingHomeBase, 1);
    
    EXPECT_EQ(path_mode_selector->flightPathEditInstructionsIsEmpty(), true);
}
