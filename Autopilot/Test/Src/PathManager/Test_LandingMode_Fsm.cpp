/*
* Author: Dhruv Rawat
*/

#include "fff.h"
#include <gtest/gtest.h>

#include "pathManager.hpp"
#include "pathStateClasses.hpp"

#include "cruisingMode.hpp"


#include "landingModeStages.hpp"
#include "landingMode.hpp"
#include "pathModeSelector.hpp"

// No need to re-initialize fake functions

TEST(LandingModeFSM, InitialStageIsLandingTransition) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

    LandingMode landing_mode_instance = *(static_cast<LandingMode*>(&LandingMode::getInstance()));

	/********************STEPTHROUGH********************/

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(landing_mode_instance.getCurrentStage()), LandingTransitionStage::getInstance());
    EXPECT_EQ(landing_mode_instance.getCurrentStageEnum(), LANDING_TRANSITION);

}

TEST(LandingModeFSM, LandingModeCalledByModeExecutor) {

	/***********************SETUP***********************/
	pathManager pathMan; 

	/********************DEPENDENCIES*******************/

	PathModeSelector* path_mode_selector = PathModeSelector::getInstance();
	path_mode_selector->setCurrentMode(LandingMode::getInstance());

	/********************STEPTHROUGH********************/

	pathMan.setState(modeExecutor::getInstance());
    pathMan.execute();

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(path_mode_selector->getCurrentMode()), LandingMode::getInstance());
	EXPECT_EQ(path_mode_selector->getCurrentModeEnum(), MODE_LANDING);
    EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}

TEST(LandingModeFSM, LandingTransitionTransitionsToLandingTransition) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	LandingMode landing_mode_instance = *(static_cast<LandingMode*>(&LandingMode::getInstance()));

    landing_mode_instance.setCurrentStage(&LandingTransitionStage::getInstance());
    landing_mode_instance.setCurrentStageEnum(LANDING_TRANSITION);

    Telemetry_PIGO_t telem_data;
    telem_data.stoppingDirectionHeading = 100;

	SFOutput_t sf_data {};

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	landing_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(landing_mode_instance.getCurrentStage()), LandingTransitionStage::getInstance());
    EXPECT_EQ(landing_mode_instance.getCurrentStageEnum(), LANDING_TRANSITION);
}


TEST(LandingModeFSM, LandingTransitionTransitionsToLandingSlope) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	LandingMode landing_mode_instance = *(static_cast<LandingMode*>(&LandingMode::getInstance()));

    landing_mode_instance.setCurrentStage(&LandingTransitionStage::getInstance());
    landing_mode_instance.setCurrentStageEnum(LANDING_TRANSITION);

    Telemetry_PIGO_t telem_data;
    telem_data.stoppingDirectionHeading = 0;

	SFOutput_t sf_data {};

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	landing_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(landing_mode_instance.getCurrentStage()), LandingSlopeStage::getInstance());
    EXPECT_EQ(landing_mode_instance.getCurrentStageEnum(), LANDING_SLOPE);
}

TEST(LandingModeFSM, LandingSlopeTransitionToLandingSlope) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	LandingMode landing_mode_instance = *(static_cast<LandingMode*>(&LandingMode::getInstance()));

    landing_mode_instance.setCurrentStage(&LandingSlopeStage::getInstance());
    landing_mode_instance.setCurrentStageEnum(LANDING_SLOPE);

    Telemetry_PIGO_t telem_data;
    telem_data.stoppingAltitude = -100;

	SFOutput_t sf_data {};

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	landing_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(landing_mode_instance.getCurrentStage()), LandingSlopeStage::getInstance());
    EXPECT_EQ(landing_mode_instance.getCurrentStageEnum(), LANDING_SLOPE);
}

TEST(LandingModeFSM, LandingSlopeTransitionToLandingFlare) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	LandingMode landing_mode_instance = *(static_cast<LandingMode*>(&LandingMode::getInstance()));

    landing_mode_instance.setCurrentStage(&LandingSlopeStage::getInstance());
    landing_mode_instance.setCurrentStageEnum(LANDING_SLOPE);

    Telemetry_PIGO_t telem_data;
    telem_data.stoppingAltitude = 0;

	SFOutput_t sf_data {};

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	landing_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(landing_mode_instance.getCurrentStage()), LandingFlareStage::getInstance());
    EXPECT_EQ(landing_mode_instance.getCurrentStageEnum(), LANDING_FLARE);
}

TEST(LandingModeFSM, LandingFlareTransitionToLandingFlare) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	LandingMode landing_mode_instance = *(static_cast<LandingMode*>(&LandingMode::getInstance()));

    landing_mode_instance.setCurrentStage(&LandingFlareStage::getInstance());
    landing_mode_instance.setCurrentStageEnum(LANDING_FLARE);

    Telemetry_PIGO_t telem_data;
    telem_data.stoppingAltitude = -100;

	SFOutput_t sf_data {};

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	landing_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(landing_mode_instance.getCurrentStage()), LandingFlareStage::getInstance());
    EXPECT_EQ(landing_mode_instance.getCurrentStageEnum(), LANDING_FLARE);
}

TEST(LandingModeFSM, LandingFlareTransitionToLandingDecrab) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	LandingMode landing_mode_instance = *(static_cast<LandingMode*>(&LandingMode::getInstance()));

    landing_mode_instance.setCurrentStage(&LandingFlareStage::getInstance());
    landing_mode_instance.setCurrentStageEnum(LANDING_FLARE);

    Telemetry_PIGO_t telem_data;
    telem_data.stoppingAltitude = 0;

	SFOutput_t sf_data {};

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	landing_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(landing_mode_instance.getCurrentStage()), LandingDecrabStage::getInstance());
    EXPECT_EQ(landing_mode_instance.getCurrentStageEnum(), LANDING_DECRAB);
}

TEST(LandingModeFSM, LandingDecrabTransitionToLandingDecrab) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	LandingMode landing_mode_instance = *(static_cast<LandingMode*>(&LandingMode::getInstance()));

    landing_mode_instance.setCurrentStage(&LandingDecrabStage::getInstance());
    landing_mode_instance.setCurrentStageEnum(LANDING_DECRAB);

    Telemetry_PIGO_t telem_data;
    telem_data.stoppingAltitude = -100;

	SFOutput_t sf_data {};

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	landing_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(landing_mode_instance.getCurrentStage()), LandingDecrabStage::getInstance());
    EXPECT_EQ(landing_mode_instance.getCurrentStageEnum(), LANDING_DECRAB);
}

TEST(LandingModeFSM, LandingDecrabTransitionToLandingTouchdown) {
    /***********************SETUP***********************/

	/********************DEPENDENCIES*******************/

	LandingMode landing_mode_instance = *(static_cast<LandingMode*>(&LandingMode::getInstance()));

    landing_mode_instance.setCurrentStage(&LandingDecrabStage::getInstance());
    landing_mode_instance.setCurrentStageEnum(LANDING_DECRAB);

    Telemetry_PIGO_t telem_data;
    telem_data.stoppingAltitude = 0;

	SFOutput_t sf_data {};

	IMU_Data_t imu_data {};

	/********************STEPTHROUGH********************/

	landing_mode_instance.execute(telem_data, sf_data, imu_data);

	/**********************ASSERTS**********************/

	EXPECT_EQ(*(landing_mode_instance.getCurrentStage()), LandingTouchdownStage::getInstance());
    EXPECT_EQ(landing_mode_instance.getCurrentStageEnum(), LANDING_TOUCHDOWN);
}
