#include <gtest/gtest.h>
#include "fff.h"

#include "OutputMixing.hpp"

using namespace std;
using ::testing::Test;

/***********************************************************************************************************************
 * Output Mixing Module Tests
 **********************************************************************************************************************/

TEST(AttitudeManager_OutputMixing, UnderNegative100RollReturnsError1) {

   	/***********************SETUP***********************/

	PID_Output_t NegativePid;
	NegativePid.rollPercent = -101.0f;
	NegativePid.pitchPercent = 10.0f;
	NegativePid.yawPercent = 10.0f;
	NegativePid.throttlePercent = 10.0f;

	OutputMixing_error_t error;

	float dummyMixedOutput[4]; // the prefix "dummy" indicates this variable needs to be here for things to work but has nothing to do with this test.


	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	error = OutputMixing_Execute(&NegativePid, dummyMixedOutput);

	/**********************ASSERTS**********************/

	EXPECT_EQ(error.errorCode, 1);
}

TEST(AttitudeManager_OutputMixing, UnderNegative100PitchReturnsError1) {

   	/***********************SETUP***********************/

	PID_Output_t NegativePid;
	NegativePid.rollPercent = 10.0f;
	NegativePid.pitchPercent = -101.0f;
	NegativePid.yawPercent = 10.0f;
	NegativePid.throttlePercent = 10.0f;

	OutputMixing_error_t error;

	float dummyMixedOutput[4];


	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	error = OutputMixing_Execute(&NegativePid, dummyMixedOutput);

	/**********************ASSERTS**********************/

	EXPECT_EQ(error.errorCode, 1);
}

TEST(AttitudeManager_OutputMixing, UnderNegative100YawReturnsError1) {

   	/***********************SETUP***********************/

	PID_Output_t NegativePid;
	NegativePid.rollPercent = 10.0f;
	NegativePid.pitchPercent = 10.0f;
	NegativePid.yawPercent = -101.0f;
	NegativePid.throttlePercent = 10.0f;

	OutputMixing_error_t error;

	float dummyMixedOutput[4];


	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	error = OutputMixing_Execute(&NegativePid, dummyMixedOutput);

	/**********************ASSERTS**********************/

	EXPECT_EQ(error.errorCode, 1);
}

TEST(AttitudeManager_OutputMixing, NegativeThrottleReturnsError1) {

   	/***********************SETUP***********************/

	PID_Output_t NegativePid;
	NegativePid.rollPercent = 10.0f;
	NegativePid.pitchPercent = 10.0f;
	NegativePid.yawPercent = 10.0f;
	NegativePid.throttlePercent = -5.0f;

	OutputMixing_error_t error;

	float dummyMixedOutput[4];


	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	error = OutputMixing_Execute(&NegativePid, dummyMixedOutput);

	/**********************ASSERTS**********************/

	EXPECT_EQ(error.errorCode, 1);
}

TEST(AttitudeManager_OutputMixing, Over100RollReturnsError2) {

   	/***********************SETUP***********************/

	PID_Output_t Over100Pid;
	Over100Pid.rollPercent = 101.0f;
	Over100Pid.pitchPercent = 10.0f;
	Over100Pid.yawPercent = 10.0f;
	Over100Pid.throttlePercent = 10.0f;

	OutputMixing_error_t error;

	float dummyMixedOutput[4];


	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	error = OutputMixing_Execute(&Over100Pid, dummyMixedOutput);

	/**********************ASSERTS**********************/

	EXPECT_EQ(error.errorCode, 2);
}

TEST(AttitudeManager_OutputMixing, Over100PitchReturnsError2) {

   	/***********************SETUP***********************/

	PID_Output_t Over100Pid;
	Over100Pid.rollPercent = 10.0f;
	Over100Pid.pitchPercent = 101.0f;
	Over100Pid.yawPercent = 10.0f;
	Over100Pid.throttlePercent = 10.0f;

	OutputMixing_error_t error;

	float dummyMixedOutput[4];


	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	error = OutputMixing_Execute(&Over100Pid, dummyMixedOutput);

	/**********************ASSERTS**********************/

	EXPECT_EQ(error.errorCode, 2);
}

TEST(AttitudeManager_OutputMixing, Over100YawReturnsError2) {

   	/***********************SETUP***********************/

	PID_Output_t Over100Pid;
	Over100Pid.rollPercent = 10.0f;
	Over100Pid.pitchPercent = 10.0f;
	Over100Pid.yawPercent = 101.0f;
	Over100Pid.throttlePercent = 10.0f;

	OutputMixing_error_t error;

	float dummyMixedOutput[4];


	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	error = OutputMixing_Execute(&Over100Pid, dummyMixedOutput);

	/**********************ASSERTS**********************/

	EXPECT_EQ(error.errorCode, 2);
}

TEST(AttitudeManager_OutputMixing, Over100ThrottleReturnsError2) {

   	/***********************SETUP***********************/

	PID_Output_t Over100Pid;
	Over100Pid.rollPercent = 10.0f;
	Over100Pid.pitchPercent = 10.0f;
	Over100Pid.yawPercent = 10.0f;
	Over100Pid.throttlePercent = 101.0f;

	OutputMixing_error_t error;

	float dummyMixedOutput[4];


	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	error = OutputMixing_Execute(&Over100Pid, dummyMixedOutput);

	/**********************ASSERTS**********************/

	EXPECT_EQ(error.errorCode, 2);
}

TEST(AttitudeManager_OutputMixing, AllInputValidReturnsError0) {

   	/***********************SETUP***********************/

	PID_Output_t Over100Pid;
	Over100Pid.rollPercent = -10.0f;
	Over100Pid.pitchPercent = 10.0f;
	Over100Pid.yawPercent = 0.0f;
	Over100Pid.throttlePercent = 10.0f;

	OutputMixing_error_t error;

	float dummyMixedOutput[4];


	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	error = OutputMixing_Execute(&Over100Pid, dummyMixedOutput);

	/**********************ASSERTS**********************/

	EXPECT_EQ(error.errorCode, 0);
}

TEST(AttitudeManager_OutputMixing, CannotReturnOutOfRangeValuesWhenGivenValidInput) {

   	/***********************SETUP***********************/

	PID_Output_t MaxVals;
	MaxVals.rollPercent = 100.0f;
	MaxVals.pitchPercent = 100.0f;
	MaxVals.yawPercent = 100.0f;
	MaxVals.throttlePercent = 100.0f;

	OutputMixing_error_t error;

	float mixedOutput[4];


	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	error = OutputMixing_Execute(&MaxVals, mixedOutput);

	/**********************ASSERTS**********************/

	EXPECT_EQ(error.errorCode, 0);


	EXPECT_TRUE( (mixedOutput[ELEVATOR_OUT_CHANNEL] <= 100.0f ) && (mixedOutput[ELEVATOR_OUT_CHANNEL] >= -100.0f ) );
	EXPECT_TRUE( (mixedOutput[RUDDER_OUT_CHANNEL] <= 100.0f ) && (mixedOutput[RUDDER_OUT_CHANNEL] >= -100.0f ) );
	EXPECT_TRUE( (mixedOutput[AILERON_OUT_CHANNEL] <= 100.0f ) && (mixedOutput[AILERON_OUT_CHANNEL] >= -100.0f ) );
	EXPECT_TRUE( (mixedOutput[THROTTLE_OUT_CHANNEL] <= 100.0f ) && (mixedOutput[THROTTLE_OUT_CHANNEL] >= 0.0f ) );
}
