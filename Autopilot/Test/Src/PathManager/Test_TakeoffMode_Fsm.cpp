/*
* Author: Oumeima Arij Ben Fredj (based on work by Dhruv Rawat)
*/

#include "fff.h"
#include <gtest/gtest.h>

#include "pathManager.hpp"
#include "pathStateClasses.hpp"

#include "cruisingMode.hpp"


#include "takeoffModeStages.hpp"
#include "takeoffMode.hpp"
#include "pathModeSelector.hpp"
#include "pathMode.hpp"

// No need to re-initialize fake functions

TEST(TakeoffModeFSM, InitialStageIsTakeoffRoll) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

    TakeoffMode takeoff_mode_instance = *(static_cast<TakeoffMode*>(&TakeoffMode::getInstance()));

	/********************STEPTHROUGH********************/

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(takeoff_mode_instance.getCurrentStage()), TakeoffRollStage::getInstance());
    EXPECT_EQ(takeoff_mode_instance.getCurrentStageEnum(), TAKEOFF_ROLL);

}

TEST(TakeoffModeFSM, TakeoffModeCalledByModeExecutor) {

	/***********************SETUP***********************/
	pathManager pathMan; 

	/********************DEPENDENCIES*******************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();
	path_mode_selector->setCurrentMode(TakeoffMode::getInstance());

	/********************STEPTHROUGH********************/

	pathMan.setState(modeExecutor::getInstance());
    pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(path_mode_selector->getCurrentMode()), TakeoffMode::getInstance());
	EXPECT_EQ(path_mode_selector->getCurrentModeEnum(), MODE_TAKEOFF);
    EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

TEST(TakeoffModeFSM, TakeoffRollTransitionsToTakeoffClimb) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	TakeoffMode takeoff_mode_instance = *(static_cast<TakeoffMode*>(&TakeoffMode::getInstance()));

    takeoff_mode_instance.setCurrentStage(&TakeoffRollStage::getInstance());
    takeoff_mode_instance.setCurrentStageEnum(TAKEOFF_ROLL);

    Telemetry_PIGO_t telem_data;

	SFOutput_t sf_data {};

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	takeoff_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(takeoff_mode_instance.getCurrentStage()), TakeoffClimbStage::getInstance());
    EXPECT_EQ(takeoff_mode_instance.getCurrentStageEnum(), TAKEOFF_CLIMB);
}


TEST(TakeoffModeFSM, TakeoffClimbTransitionsToCruising) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();

	TakeoffMode takeoff_mode_instance = *(static_cast<TakeoffMode*>(&TakeoffMode::getInstance()));

    takeoff_mode_instance.setCurrentStage(&TakeoffClimbStage::getInstance());
    takeoff_mode_instance.setCurrentStageEnum(TAKEOFF_CLIMB);

    Telemetry_PIGO_t telem_data;
	telem_data.holdingAltitude = 20;

	SFOutput_t sf_data {};

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	takeoff_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(path_mode_selector->getCurrentMode()), CruisingMode::getInstance());
	EXPECT_EQ(path_mode_selector->getCurrentModeEnum(), MODE_CRUISING);
}
