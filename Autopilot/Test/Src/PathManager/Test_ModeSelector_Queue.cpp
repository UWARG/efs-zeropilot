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
#include "cruisingModeStages.hpp"

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

/*
Test cases To implement

Adding to an empty queue
adding to a full queue
dequeuing a full queue
dequeuing an empty queue
*/
TEST (ModeSelectorQueue, CommsWithAttitudeTransitionToCommsWithTelemetry) {
	/***********************SETUP***********************/

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	/**********************ASSERTS**********************/

}
TEST (ModeSelectorQueue, CheckQueueafterTransitionFromTakeOffToCruisingMode) {
	/***********************SETUP***********************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();

	TakeoffMode takeoff_mode_instance = *(static_cast<TakeoffMode*>(&TakeoffMode::getInstance()));
	CruisingMode cruising_mode_instance = *(static_cast<CruisingMode*>(&CruisingMode::getInstance()));

	Telemetry_PIGO_t telem_data_0;

	telem_data_0.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
	telem_data_0.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
	telem_data_0.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
	telem_data_0.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	telem_data_0.numWaypoints = 4;
	telem_data_0.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
	telem_data_0.initializingHomeBase = 1;
	telem_data_0.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING;
	telem_data_0.holdingAltitude = 0;
	telem_data_0.holdingTurnRadius = 0;
	telem_data_0.holdingTurnDirection = 0;
	telem_data_0.nextId = 0;
	telem_data_0.prevId = 0;
	telem_data_0.modifyId = 0;
	telem_data_0.homebase = createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0);

	Telemetry_PIGO_t telem_data_1;

	telem_data_1.waypoints[0] = createTelemetryWaypoint(10.0, 0.0, 16, 0.0, 0);
	telem_data_1.waypoints[1] = createTelemetryWaypoint(0.0, 20.0, 17, 0.0, 0);

	telem_data_1.numWaypoints = 2;
	telem_data_1.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
	telem_data_1.initializingHomeBase = 1;
	telem_data_1.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING;
	telem_data_1.holdingAltitude = 0;
	telem_data_1.holdingTurnRadius = 0;
	telem_data_1.holdingTurnDirection = 0;
	telem_data_1.nextId = 0;
	telem_data_1.prevId = 0;
	telem_data_1.modifyId = 0;
	telem_data_1.homebase = createTelemetryWaypoint(0.0, 0.0, 200, 0.0, 0);

	SFOutput_t sf_data;
	sf_data.track = -1;
	sf_data.altitude = -2;
	sf_data.airspeed = -3;

	IMU_Data_t imu_data {};

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
    path_mode_selector->setCurrentMode(takeoff_mode_instance);
    path_mode_selector->execute(telem_data_0, sf_data, imu_data);

    // path_mode_selector->setCurrentMode(cruising_mode_instance);
    // path_mode_selector->execute(telem_data_1, sf_data, imu_data);

    
	// /**********************ASSERTS**********************/
    // EXPECT_EQ(path_mode_selector->flightPathEditInstructionsIsEmpty(), true);
    // EXPECT_EQ(instruction.numWaypoints, 4);
    // EXPECT_EQ(instruction.waypointModifyFlightPathCommand, INITIALIZE_FLIGHT_PATH);
    // EXPECT_EQ(instruction.initializingHomeBase, 1);
    // EXPECT_EQ(path_mode_selector->flightPathEditInstructionsIsEmpty(), true);
}

TEST (ModeSelectorQueue, CheckDequeueInstruction) {
	/***********************SETUP***********************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();
	TakeoffMode takeoff_mode_instance = *(static_cast<TakeoffMode*>(&TakeoffMode::getInstance()));

	Telemetry_PIGO_t telem_data_0;

	telem_data_0.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
	telem_data_0.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
	telem_data_0.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
	telem_data_0.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	telem_data_0.numWaypoints = 4;
	telem_data_0.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
	telem_data_0.initializingHomeBase = 1;
	telem_data_0.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING;
	telem_data_0.holdingAltitude = 0;
	telem_data_0.holdingTurnRadius = 0;
	telem_data_0.holdingTurnDirection = 0;
	telem_data_0.nextId = 0;
	telem_data_0.prevId = 0;
	telem_data_0.modifyId = 0;
	telem_data_0.homebase = createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0);

	Telemetry_PIGO_t telem_data_1;

	telem_data_1.waypoints[0] = createTelemetryWaypoint(10.0, 0.0, 16, 0.0, 0);
	telem_data_1.waypoints[1] = createTelemetryWaypoint(0.0, 20.0, 17, 0.0, 0);
	telem_data_1.numWaypoints = 2;
	telem_data_1.waypointModifyFlightPathCommand = APPEND;
	telem_data_1.initializingHomeBase = 1;
	telem_data_1.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING;
	telem_data_1.holdingAltitude = 0;
	telem_data_1.holdingTurnRadius = 0;
	telem_data_1.holdingTurnDirection = 0;
	telem_data_1.nextId = 0;
	telem_data_1.prevId = 0;
	telem_data_1.modifyId = 0;
	telem_data_1.homebase = createTelemetryWaypoint(0.0, 0.0, 200, 0.0, 0);

	SFOutput_t sf_data;
	sf_data.track = -1;
	sf_data.altitude = -2;
	sf_data.airspeed = -3;

	IMU_Data_t imu_data {};

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
	path_mode_selector->setCurrentMode(takeoff_mode_instance);
    path_mode_selector->execute(telem_data_0, sf_data, imu_data);
	path_mode_selector->setCurrentMode(takeoff_mode_instance);
    path_mode_selector->execute(telem_data_1, sf_data, imu_data);
    
	//Manually dequeue
	int length_0 = path_mode_selector->checkflightPathEditInstructionsLength();
    Telemetry_PIGO_t instruction_0 = path_mode_selector->dequeueflightPathEditInstructions();
	int length_1 = path_mode_selector->checkflightPathEditInstructionsLength();
	// Telemetry_PIGO_t instruction_1 = path_mode_selector->dequeueflightPathEditInstructions();
	// /**********************ASSERTS**********************/
	EXPECT_EQ(length_0, 2); //False, Returns 4
	EXPECT_EQ(length_1, 1); //False, returns 0
    // EXPECT_EQ(instruction_0.numWaypoints, 4);
    // EXPECT_EQ(instruction_0.waypointModifyFlightPathCommand, INITIALIZE_FLIGHT_PATH);
    // EXPECT_EQ(instruction_0.initializingHomeBase, 1);
    // // EXPECT_EQ(instruction_1.numWaypoints, 2);
    // EXPECT_EQ(instruction_1.waypointModifyFlightPathCommand, APPEND);
    // // EXPECT_EQ(instruction_1.initializingHomeBase, 1);
    // EXPECT_EQ(path_mode_selector->flightPathEditInstructionsIsEmpty(), true);

}
