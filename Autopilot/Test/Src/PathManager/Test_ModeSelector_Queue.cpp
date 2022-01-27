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

static void emptyQueue(PathModeSelector *selector) { 
	// Helper function because the queue seems to be non-empty at the beginning of some tests (because we are using singletons).
	while (!selector->flightPathEditInstructionsIsEmpty()) {
		selector->dequeueflightPathEditInstructions();
	}
}

static Telemetry_PIGO_t updateTelemetryData(Telemetry_PIGO_t telem_data, int numWaypoints, _ModifyFlightPathCommand waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH, 
	int initializingHomeBase = 1, _GetNextDirectionsCommand waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING, int holdingAltitude = 0, int holdingTurnRadius = 0, int holdingTurnDirection = 0,
	int nextId = 0, int prevId = 0, int modifyId = 0) {

	telem_data.numWaypoints = numWaypoints;
	telem_data.waypointModifyFlightPathCommand = waypointModifyFlightPathCommand;
	telem_data.initializingHomeBase = initializingHomeBase;
	telem_data.waypointNextDirectionsCommand = waypointNextDirectionsCommand;
	telem_data.holdingAltitude = holdingAltitude;
	telem_data.holdingTurnRadius = holdingTurnRadius;
	telem_data.holdingTurnDirection = holdingTurnDirection;
	telem_data.nextId = nextId;
	telem_data.prevId = prevId;
	telem_data.modifyId = modifyId;
	telem_data.homebase = createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0);
	
	return telem_data;
	}


TEST (ModeSelectorQueue, CheckEmptyQueueBehavior) {
	/***********************SETUP***********************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();
	emptyQueue(path_mode_selector);

	TakeoffMode takeoff_mode_instance = *(static_cast<TakeoffMode*>(&TakeoffMode::getInstance()));
	CruisingMode cruising_mode_instance = *(static_cast<CruisingMode*>(&CruisingMode::getInstance()));

	Telemetry_PIGO_t telem_data; //Empty data
	SFOutput_t sf_data;
	sf_data.track = -1;
	sf_data.altitude = -2;
	sf_data.airspeed = -3;

	IMU_Data_t imu_data {};

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
    path_mode_selector->setCurrentMode(takeoff_mode_instance);
    path_mode_selector->execute(telem_data, sf_data, imu_data);
	int length = path_mode_selector->checkflightPathEditInstructionsLength(); //There is one instruction in the queue
    path_mode_selector->setCurrentMode(cruising_mode_instance); //When we transition into cruising mode, we empty the instruction Queue
    path_mode_selector->execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/
	EXPECT_EQ(length, 0);
	EXPECT_EQ(path_mode_selector->checkflightPathEditInstructionsLength(), 0);
}

TEST (ModeSelectorQueue, CheckQueueafterTransitionFromTakeOffToCruisingMode) {
	/* We expect the following behavior:
	1. In takeoff mode, we will add any new instructions to the InstructionQueue
	2. As soon as we transition into cruisingMode, we will empty the queue and automatically execute all instructions
	3. At this point, the queue should be constantly empty, as any new instructions added to the queue will be instantly
	executed
	*/

	/***********************SETUP***********************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();
	emptyQueue(path_mode_selector);

	TakeoffMode takeoff_mode_instance = *(static_cast<TakeoffMode*>(&TakeoffMode::getInstance()));
	CruisingMode cruising_mode_instance = *(static_cast<CruisingMode*>(&CruisingMode::getInstance()));

	Telemetry_PIGO_t telem_data_0;
	telem_data_0.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
	telem_data_0.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
	telem_data_0.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
	telem_data_0.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);
	telem_data_0 = updateTelemetryData(telem_data_0, 4);

	Telemetry_PIGO_t telem_data_1;
	telem_data_1.waypoints[0] = createTelemetryWaypoint(10.0, 0.0, 16, 0.0, 0);
	telem_data_1.waypoints[1] = createTelemetryWaypoint(0.0, 20.0, 17, 0.0, 0);
	telem_data_1 = updateTelemetryData(telem_data_1, 2, APPEND);

	SFOutput_t sf_data;
	sf_data.track = -1;
	sf_data.altitude = -2;
	sf_data.airspeed = -3;

	IMU_Data_t imu_data {};

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
    path_mode_selector->setCurrentMode(takeoff_mode_instance);
    path_mode_selector->execute(telem_data_0, sf_data, imu_data);
	int length = path_mode_selector->checkflightPathEditInstructionsLength(); //There is one instruction in the queue
    path_mode_selector->setCurrentMode(cruising_mode_instance); //When we transition into cruising mode, we empty the instruction Queue
    path_mode_selector->execute(telem_data_1, sf_data, imu_data);

	/**********************ASSERTS**********************/
	EXPECT_EQ(length, 1);
	EXPECT_EQ(path_mode_selector->checkflightPathEditInstructionsLength(), 0);
}

TEST (ModeSelectorQueue, CheckDequeueInstructionInTakeoffMode) {
	/***********************SETUP***********************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();
	emptyQueue(path_mode_selector); // ensure queue is actually empty before running test

	TakeoffMode takeoff_mode_instance = *(static_cast<TakeoffMode*>(&TakeoffMode::getInstance()));

	Telemetry_PIGO_t telem_data_0;
	telem_data_0.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
	telem_data_0.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
	telem_data_0.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
	telem_data_0.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);
	telem_data_0 = updateTelemetryData(telem_data_0, 4);

	Telemetry_PIGO_t telem_data_1;
	telem_data_1.waypoints[0] = createTelemetryWaypoint(10.0, 0.0, 16, 0.0, 0);
	telem_data_1.waypoints[1] = createTelemetryWaypoint(0.0, 20.0, 17, 0.0, 0);
	telem_data_1 = updateTelemetryData(telem_data_1, 2, APPEND);

	SFOutput_t sf_data;
	sf_data.track = -1;
	sf_data.altitude = -2;
	sf_data.airspeed = -3;

	IMU_Data_t imu_data {};

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
	//In takeoff mode, the queue should stack up as we add more instructions.
	path_mode_selector->setCurrentMode(takeoff_mode_instance); 
	int length_0 = path_mode_selector->checkflightPathEditInstructionsLength();
    path_mode_selector->enqueueFlightPathEditInstructions(telem_data_0);
	int length_1 = path_mode_selector->checkflightPathEditInstructionsLength();
    path_mode_selector->enqueueFlightPathEditInstructions(telem_data_1);
	int length_2 = path_mode_selector->checkflightPathEditInstructionsLength();

	//Manually dequeue
    Telemetry_PIGO_t instruction_0 = path_mode_selector->dequeueflightPathEditInstructions();
	Telemetry_PIGO_t instruction_1 = path_mode_selector->dequeueflightPathEditInstructions();
	
	// /**********************ASSERTS**********************/
	EXPECT_EQ(length_0, 0);
	EXPECT_EQ(length_1, 1);
	EXPECT_EQ(length_2, 2);
    EXPECT_EQ(instruction_0.numWaypoints, 4);
    EXPECT_EQ(instruction_0.waypointModifyFlightPathCommand, INITIALIZE_FLIGHT_PATH);
    EXPECT_EQ(instruction_0.initializingHomeBase, 1);
    EXPECT_EQ(instruction_1.numWaypoints, 2);
    EXPECT_EQ(instruction_1.waypointModifyFlightPathCommand, APPEND);
    EXPECT_EQ(instruction_1.initializingHomeBase, 1);
    EXPECT_EQ(path_mode_selector->flightPathEditInstructionsIsEmpty(), true);

}
