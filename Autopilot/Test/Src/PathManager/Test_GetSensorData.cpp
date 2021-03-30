#include <gtest/gtest.h>
#include "fff.h"

#include "pathDataTypes.hpp"

#include "altimeter_mock.hpp"
#include "gpsMock.hpp"
#include "getSensorData.hpp"

#include <math.h>


using namespace std;
using ::testing::Test;
using ::testing::_;
using ::testing::SetArgReferee;

AltimeterData_t AltimeterTestData;
GpsData_t GpsTestData;
Altimeter_Data_t AltimeterPathTestData; 
Gps_Data_t GpsPathTestData; 

/***********************************************************************************************************************
 * Tests
 **********************************************************************************************************************/

TEST(getSensorData, FailedBusyAltimeterDataReturnsNegative1) {


   	/***********************SETUP***********************/

	MockAltimeter altimetermock;
	MockGps gpsmock;

	AltimeterTestData.status = -1;
	GpsTestData.sensorStatus = 0;
	AltimeterTestData.isDataNew = 1;
	GpsTestData.dataIsNew = 1;

    AltimeterPathTestData.status = -1;
	GpsPathTestData.sensorStatus = 0;
	AltimeterPathTestData.isDataNew = 1;
	GpsPathTestData.dataIsNew = 1;

	SensorError_t getSensorDataError;
	
	/********************DEPENDENCIES*******************/

	EXPECT_CALL(altimetermock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(AltimeterTestData)));

	EXPECT_CALL(gpsmock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(GpsTestData)));

	/********************STEPTHROUGH********************/
	getSensorDataError = SensorMeasurements_GetResult(&altimetermock, &gpsmock, &AltimeterPathTestData, &GpsPathTestData);

	/**********************ASSERTS**********************/
	EXPECT_EQ(getSensorDataError.errorCode, -1);
}

TEST(getSensorData, FailedBusyGpsDataReturnsNegative1) {


   	/***********************SETUP***********************/

	MockAltimeter altimetermock;
	MockGps gpsmock;

	AltimeterTestData.status = 0;
	GpsTestData.sensorStatus = -1;
	AltimeterTestData.isDataNew = 1;
	GpsTestData.dataIsNew = 1;

    AltimeterPathTestData.status = 0;
	GpsPathTestData.sensorStatus = -1;
	AltimeterPathTestData.isDataNew = 1;
	GpsPathTestData.dataIsNew = 1;

	SensorError_t getSensorDataError;

	/********************DEPENDENCIES*******************/

	EXPECT_CALL(altimetermock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(AltimeterTestData)));

	EXPECT_CALL(gpsmock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(GpsTestData)));

	/********************STEPTHROUGH********************/
	getSensorDataError = SensorMeasurements_GetResult(&altimetermock, &gpsmock, &AltimeterPathTestData, &GpsPathTestData);

	/**********************ASSERTS**********************/
	EXPECT_EQ(getSensorDataError.errorCode, -1);
}

TEST(getSensorData, AltimeterOldDataReturns1) {

/***********************SETUP***********************/
	MockAltimeter altimetermock;
	MockGps gpsmock;

	AltimeterTestData.status = 0;
	GpsTestData.sensorStatus = 0;
	AltimeterTestData.isDataNew = 0;
	GpsTestData.dataIsNew = 1;

    AltimeterPathTestData.status = 0;
	GpsPathTestData.sensorStatus = 0;
	AltimeterPathTestData.isDataNew = 0;
	GpsPathTestData.dataIsNew = 1;

	SensorError_t getSensorDataError;

    //Dummy values
    AltimeterTestData.pressure = 0; 
    AltimeterTestData.altitude = NAN; 
    AltimeterTestData.temp = 0; 

    //Dummy values
    GpsTestData.latitude = 0; 
    GpsTestData.longitude = 0; 
    GpsTestData.utcTime = 0; 
    GpsTestData.groundSpeed = 0; 
    GpsTestData.altitude = 0; 
    GpsTestData.heading = 0; 
    GpsTestData.numSatellites = 0; 

	/********************DEPENDENCIES*******************/

	EXPECT_CALL(altimetermock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(AltimeterTestData)));

	EXPECT_CALL(gpsmock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(GpsTestData)));

	/********************STEPTHROUGH********************/
	getSensorDataError = SensorMeasurements_GetResult(&altimetermock, &gpsmock, &AltimeterPathTestData, &GpsPathTestData);

	/**********************ASSERTS**********************/
	EXPECT_EQ(getSensorDataError.errorCode, 1);

}

TEST(getSensorData, GpsOldDataReturns1) {

/***********************SETUP***********************/
	MockAltimeter altimetermock;
	MockGps gpsmock;

	AltimeterTestData.status = 0;
	GpsTestData.sensorStatus = 0;
	AltimeterTestData.isDataNew = 1;
	GpsTestData.dataIsNew = 0;

    AltimeterPathTestData.status = 0;
	GpsPathTestData.sensorStatus = 0;
	AltimeterPathTestData.isDataNew = 1;
	GpsPathTestData.dataIsNew = 0;

	SensorError_t getSensorDataError;

    //Dummy values
    AltimeterTestData.pressure = 0; 
    AltimeterTestData.altitude = NAN; 
    AltimeterTestData.temp = 0; 

    //Dummy values
    GpsTestData.latitude = 0; 
    GpsTestData.longitude = 0; 
    GpsTestData.utcTime = 0; 
    GpsTestData.groundSpeed = 0; 
    GpsTestData.altitude = 0; 
    GpsTestData.heading = 0; 
    GpsTestData.numSatellites = 0; 

	/********************DEPENDENCIES*******************/

	EXPECT_CALL(altimetermock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(AltimeterTestData)));

	EXPECT_CALL(gpsmock, GetResult(_))
		.WillOnce(DoAll(SetArgReferee<0>(GpsTestData)));

	/********************STEPTHROUGH********************/
	getSensorDataError = SensorMeasurements_GetResult(&altimetermock, &gpsmock, &AltimeterPathTestData, &GpsPathTestData);

	/**********************ASSERTS**********************/
	EXPECT_EQ(getSensorDataError.errorCode, 1);

}

