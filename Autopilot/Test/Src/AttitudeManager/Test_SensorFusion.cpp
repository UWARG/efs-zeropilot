/*
* A skeleton for testing the sensor fusion algorithm
* Author: Lucy Gong, Dhruv Rawat
*/

#include <gtest/gtest.h>
#include "fff.h"

#include "AttitudeDatatypes.hpp"

#include "airspeed_Mock.hpp"
#include "IMU_Mock.hpp"
#include "SensorFusion.hpp"
#include "fetchSensorMeasurementsMode.hpp"

#include <math.h>


using namespace std;
using ::testing::Test;
using ::testing::_;
using ::testing::SetArgReferee;

/***********************************************************************************************************************
 * Mocks
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/
IMUData_t IMUTestData;
airspeedData_t airspeedTestData;
IMU_Data_t IMUAttitudeTestData;
Airspeed_Data_t AirspeedAttitudeTestData;

/***********************************************************************************************************************
 * Tests
 **********************************************************************************************************************/

TEST(SensorFusion, FailedBusyIMUDataReturnsNegative1) {


   	/***********************SETUP***********************/

	MockIMU imumock;
	MockAirspeed airspeedmock;

	IMUTestData.sensorStatus = -1;
	airspeedTestData.sensorStatus = 0;
	IMUTestData.isDataNew = 1;
	airspeedTestData.isDataNew = 1;

	IMUAttitudeTestData.sensorStatus = -1;
	AirspeedAttitudeTestData.sensorStatus = 0;
	IMUAttitudeTestData.isDataNew = 1;
	AirspeedAttitudeTestData.isDataNew = 1;

	SFError_t error;
	SensorError_t fetchMeasurementsError;
	SFOutput_t output;

	/********************DEPENDENCIES*******************/

	EXPECT_CALL(imumock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(IMUTestData)));

	EXPECT_CALL(airspeedmock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(airspeedTestData)));

	/********************STEPTHROUGH********************/
	
	fetchMeasurementsError = SensorMeasurements_GetResult(&imumock, &airspeedmock, &IMUAttitudeTestData, &AirspeedAttitudeTestData);

	error = SF_GetResult(&output, &IMUAttitudeTestData, &AirspeedAttitudeTestData);

	/**********************ASSERTS**********************/

	EXPECT_EQ(fetchMeasurementsError.errorCode, -1);
	EXPECT_EQ(error.errorCode, -1);
}

TEST(SensorFusion, FailedBusyAirspeedDataReturnsNegative1 ) {

   	/***********************SETUP***********************/

	MockIMU imumock;
	MockAirspeed airspeedmock;

	IMUTestData.sensorStatus = 0;
	airspeedTestData.sensorStatus = -1;
	IMUTestData.isDataNew = 1;
	airspeedTestData.isDataNew = 1;

	IMUAttitudeTestData.sensorStatus = 0;
	AirspeedAttitudeTestData.sensorStatus = -1;
	IMUAttitudeTestData.isDataNew = 1;
	AirspeedAttitudeTestData.isDataNew = 1;

	SFError_t error;
	SFOutput_t output;
	SensorError_t fetchMeasurementsError;

	/********************DEPENDENCIES*******************/

	EXPECT_CALL(imumock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(IMUTestData)));

	EXPECT_CALL(airspeedmock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(airspeedTestData)));

	/********************STEPTHROUGH********************/

	fetchMeasurementsError = SensorMeasurements_GetResult(&imumock, &airspeedmock, &IMUAttitudeTestData, &AirspeedAttitudeTestData);

	error = SF_GetResult(&output, &IMUAttitudeTestData, &AirspeedAttitudeTestData);

	/**********************ASSERTS**********************/

	EXPECT_EQ(fetchMeasurementsError.errorCode, -1);
	EXPECT_EQ(error.errorCode, -1);
}

TEST(SensorFusion, OldDataReturns1) {

   	/***********************SETUP***********************/
	MockIMU imumock;
	MockAirspeed airspeedmock;

	IMUTestData.sensorStatus = 0;
	airspeedTestData.sensorStatus = 0;
	IMUTestData.isDataNew = 0;
	airspeedTestData.isDataNew = 1;

	IMUAttitudeTestData.sensorStatus = 0;
	AirspeedAttitudeTestData.sensorStatus = 0;
	IMUAttitudeTestData.isDataNew = 0;
	AirspeedAttitudeTestData.isDataNew = 1;

	//Dummy values
	IMUTestData.magx = NAN;
	IMUTestData.magy = 0;
	IMUTestData.magz = NAN; // Make some of these NAN to see if code can handle this case
	IMUTestData.accx = 0;
	IMUTestData.accy = 0;
	IMUTestData.accz = 0;
	IMUTestData.gyrx = 0;
	IMUTestData.gyry = 0;
	IMUTestData.gyrz = 0;

	//Dummy values
	airspeedTestData.airspeed = 0;

	SensorError_t fetchMeasurementsError;
	SFError_t error;
	SFOutput_t output;

	/********************DEPENDENCIES*******************/

	EXPECT_CALL(imumock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(IMUTestData)));

	EXPECT_CALL(airspeedmock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(airspeedTestData)));

	/********************STEPTHROUGH********************/

	fetchMeasurementsError = SensorMeasurements_GetResult(&imumock, &airspeedmock, &IMUAttitudeTestData, &AirspeedAttitudeTestData);

	error = SF_GetResult(&output, &IMUAttitudeTestData, &AirspeedAttitudeTestData);

	/**********************ASSERTS**********************/

	EXPECT_EQ(fetchMeasurementsError.errorCode, 1);
	EXPECT_EQ(error.errorCode, 1);
}

