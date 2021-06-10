#include <gtest/gtest.h>
#include "fff.h"

#include <SensorFusion.hpp>

using namespace std;
using ::testing::Test;

TEST(SensorFusion, GetResult) {

   	/***********************SETUP***********************/
	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	SFOutput_t o;
	SF_Init();
	SF_GenerateNewResult();
	SFError_t err = SF_GetResult(&o);
	float output = err.errorCode;

	/**********************ASSERTS**********************/

	EXPECT_EQ(output, 0);
}
