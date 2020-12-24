#include <gtest/gtest.h>
#include "fff.h"

#include "PID.hpp"

using namespace std;
using ::testing::Test;

/***********************************************************************************************************************
 * Very basic PID tests
 **********************************************************************************************************************/

TEST(PID, OutputCannotBeLargerThanMaxOutput) {

   	/***********************SETUP***********************/

   	float maxOutput = 100;

	PIDController Pid{1, 0, 0, 0, 0, maxOutput};

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	float output = Pid.execute(1000000, 0);

	/**********************ASSERTS**********************/

	EXPECT_EQ(output, maxOutput);
}

TEST(PID, OutputCannotBeSmallerThanMinOutput) {

   	/***********************SETUP***********************/

   	float minOutput = -100;

	PIDController Pid{1, 0, 0, 0, minOutput, 0};

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	float output = Pid.execute(-1000000, 0);

	/**********************ASSERTS**********************/

	EXPECT_EQ(output, minOutput);
}
