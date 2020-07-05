/*
*
* A skeleton for testing the sensor fusion algorithm
*
*/

#include <gtest/gtest.h>
#include "fff.h"

#include "airspeed_Mock.hpp"
#include "IMU_Mock.hpp"
#include "SensorFusion.hpp"


using namespace std;
using ::testing::Test;

/***********************************************************************************************************************
 * Mocks
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Tests
 **********************************************************************************************************************/

TEST(SensorFusion, FailedBusyIMUDataReturnsNegative1 ) {

   	/***********************SETUP***********************/

	IMUData_t IMUBad;
	IMUBad.sensorStatus = -1;

	airspeedData_t AirspeedGood;
	AirspeedGood.sensorStatus = 0;

	SFError_t error;
	SFOutput_t output;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	SF_GetResult(&output);	

	/**********************ASSERTS**********************/

	ASSERT_EQ(error.errorCode, -1);
}

TEST(SensorFusion, FailedBusyAirspeedDataReturnsNegative1 ) {

   	/***********************SETUP***********************/

	IMUData_t IMUGood;
	IMUGood.sensorStatus = 0;

	airspeedData_t AirspeedBad;
	AirspeedBad.sensorStatus = -1;

	SFError_t error;
	SFOutput_t output;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	SF_GetResult(&output);	

	/**********************ASSERTS**********************/

	ASSERT_EQ(error.errorCode, -1);
}

TEST(SensorFusion, OldDataReturns1) {

   	/***********************SETUP***********************/
	IMUData_t IMUOld;
	IMUOld.isDataNew = 0;

	//Dummy values 
	IMUOld.magx = 0;
	IMUOld.magy = 0;
	IMUOld.magz = 0;
	IMUOld.accx = 0;
	IMUOld.accy = 0;
	IMUOld.accz = 0;
	IMUOld.gyrx = 0;
	IMUOld.gyry = 0;
	IMUOld.gyrz = 0;	


	airspeedData_t AirspeedOld;
	AirspeedOld.sensorStatus = -1;

	//Dummy values
	AirspeedOld.airspeed = 0;

	SFError_t error;
	SFOutput_t output;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	SF_GetResult(&output);	

	/**********************ASSERTS**********************/

	ASSERT_EQ(error.errorCode, 1);
}

