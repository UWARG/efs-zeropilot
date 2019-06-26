#include <gtest/gtest.h>
#include "fff.h"
#include "adc.h"
#include "airSpeed.h"


#define INT_VALUE_1 132

#define DUMMY_FLOAT_VALUE	12.7f


// the correct value passed by getValue is transmitted to the user.

/***********************************************************************************************************************
 * Mocks
 **********************************************************************************************************************/

DEFINE_FFF_GLOBALS;


FAKE_VOID_FUNC(MX_ADC3_Init);
FAKE_VOID_FUNC(HAL_ADC_MspInit, ADC_HandleTypeDef * );
FAKE_VALUE_FUNC(HAL_StatusTypeDef, HAL_ADC_Start, ADC_HandleTypeDef * );
FAKE_VALUE_FUNC(HAL_StatusTypeDef, HAL_ADC_PollForConversion, ADC_HandleTypeDef *,  uint32_t );
FAKE_VALUE_FUNC(uint32_t, HAL_ADC_GetValue, ADC_HandleTypeDef * );
FAKE_VALUE_FUNC(HAL_StatusTypeDef, HAL_ADC_Stop, ADC_HandleTypeDef * );

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

ADC_HandleTypeDef hadc3; // this var is global in adc.c and extern in adc.h. Since the mocks have no knoweledge of the .c file, it must be defined here for test purposes.

/***********************************************************************************************************************
 * Test Fixtures
 **********************************************************************************************************************/


class ADC_Init : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(MX_ADC3_Init);
			RESET_FAKE(HAL_ADC_MspInit);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}

};

class ADC_GetValue : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(HAL_ADC_Start);
			RESET_FAKE(HAL_ADC_PollForConversion);
			RESET_FAKE(HAL_ADC_GetValue);
			RESET_FAKE(HAL_ADC_Stop);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}

};

/***********************************************************************************************************************
 * Tests
 **********************************************************************************************************************/



TEST_F(ADC_Init, InitCallsAppropriateAdcInit) { 

   	/***********************SETUP***********************/
	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	airSpeed_Init();

	/**********************ASSERTS**********************/

    ASSERT_EQ(MX_ADC3_Init_fake.call_count, 1);

}

TEST_F(ADC_Init, InitCallsMspInitWithAppropriateArg) { 

   	/***********************SETUP***********************/

	memset(&hadc3, INT_VALUE_1, sizeof(ADC_HandleTypeDef));

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	airSpeed_Init();

	/**********************ASSERTS**********************/

	ASSERT_EQ(HAL_ADC_MspInit_fake.call_count, 1);
	ASSERT_TRUE(memcmp(HAL_ADC_MspInit_fake.arg0_val, &hadc3, sizeof(ADC_HandleTypeDef)) == 0);
}


TEST_F(ADC_GetValue, AllconversionFuncsCalledInCorrectOrder) { 

   	/***********************SETUP***********************/

	float dummyAirspeed = DUMMY_FLOAT_VALUE;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	airSpeed_GetAirSpeed(&dummyAirspeed);

	/**********************ASSERTS**********************/

    ASSERT_EQ(fff.call_history[0], (void *)HAL_ADC_Start);
    ASSERT_EQ(fff.call_history[1], (void *)HAL_ADC_PollForConversion);
    ASSERT_EQ(fff.call_history[2], (void *)HAL_ADC_GetValue);
    ASSERT_EQ(fff.call_history[3], (void *)HAL_ADC_Stop);
}

TEST_F(ADC_GetValue, AllconversionFuncsCalledWithCorrectParams) { 

   	/***********************SETUP***********************/

	float dummyAirspeed = DUMMY_FLOAT_VALUE;

	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/

	airSpeed_GetAirSpeed(&dummyAirspeed);

	/**********************ASSERTS**********************/

	ASSERT_TRUE(memcmp(HAL_ADC_Start_fake.arg0_val, &hadc3, sizeof(ADC_HandleTypeDef)) == 0);
	ASSERT_TRUE(memcmp(HAL_ADC_PollForConversion_fake.arg0_val, &hadc3, sizeof(ADC_HandleTypeDef)) == 0);
	/* TODO: POLL FOR CONVERSION HAS A SECOND ARGUMENT: TIMEOUT, I DO NOT KNOW WHAT ITS VALUE SHOULD BE SO IT IS NOT TESTED HERE.*/
	ASSERT_TRUE(memcmp(HAL_ADC_GetValue_fake.arg0_val, &hadc3, sizeof(ADC_HandleTypeDef)) == 0);
	ASSERT_TRUE(memcmp(HAL_ADC_Stop_fake.arg0_val, &hadc3, sizeof(ADC_HandleTypeDef)) == 0);
}

TEST_F(ADC_GetValue, IfAllConversionFuncsReturnHal_OKGetAirspeedShouldReturnSucceeded) { 

   	/***********************SETUP***********************/

	float dummyAirspeed = DUMMY_FLOAT_VALUE;

	/********************DEPENDENCIES*******************/

	HAL_ADC_Start_fake.return_val = HAL_OK;
	HAL_ADC_PollForConversion_fake.return_val = HAL_OK;
	HAL_ADC_Stop_fake.return_val = HAL_OK;

	/********************STEPTHROUGH********************/

	airSpeedState_t state = airSpeed_GetAirSpeed(&dummyAirspeed);

	/**********************ASSERTS**********************/

	ASSERT_EQ(state, AIR_SPEED_SUCCEEDED);

}

TEST_F(ADC_GetValue, IfAnyConversionFuncsDoNotReturnHal_OKGetAirspeedShouldReturnFailed) { 

   	/***********************SETUP***********************/

	float dummyAirspeed = DUMMY_FLOAT_VALUE;

	/********************DEPENDENCIES*******************/

	HAL_ADC_Start_fake.return_val = HAL_ERROR;
	HAL_ADC_PollForConversion_fake.return_val = HAL_OK;
	/* TODO: maybe timeouts should be handled differently?*/
	HAL_ADC_Stop_fake.return_val = HAL_TIMEOUT;

	/********************STEPTHROUGH********************/

	airSpeedState_t state = airSpeed_GetAirSpeed(&dummyAirspeed);

	/**********************ASSERTS**********************/

	ASSERT_EQ(state, AIR_SPEED_FAILED);

}
