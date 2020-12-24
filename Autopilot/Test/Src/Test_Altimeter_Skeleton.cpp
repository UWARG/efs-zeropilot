#include <gtest/gtest.h>
#include "fff.h"
#include "i2c.h"
#include "altimeter.h"

#include "altimeter.hpp"
#include "altimeter_mock.hpp"

/* Info on google test and google mock */
/* https://github.com/google/googletest/blob/master/googletest/docs/primer.md */
/* https://github.com/google/googletest/blob/master/googlemock/README.md */


/*
* This file exists because there is a partial implementation of the altimeter driver. That implementation attempts to speak
* with hardware, which obviously can't happen on a PC. So the hardware functions are faked to allow the code to run off target.
*/

/***********************************************************************************************************************
 * Mocks (For C fakes/mocks, we are using the fake function framework (fff). For C++ fakes/mocks, we use google mock, though no examples of google mock are show here)
 **********************************************************************************************************************/

FAKE_VOID_FUNC(MX_I2C1_Init);
FAKE_VOID_FUNC(MX_I2C2_Init);
FAKE_VOID_FUNC(MX_I2C4_Init);
FAKE_VALUE_FUNC(I2C_HandleTypeDef*, I2C_GetHandle, I2C_TypeDef*);
FAKE_VOID_FUNC(I2C_WriteByte, I2C_HandleTypeDef *, uint16_t, uint8_t, uint8_t);
FAKE_VOID_FUNC(I2C_WriteBytes, I2C_HandleTypeDef *, uint16_t, uint8_t, uint8_t *, uint16_t);
FAKE_VALUE_FUNC(uint8_t, I2C_ReadByte, I2C_HandleTypeDef *, uint16_t, uint8_t);
FAKE_VOID_FUNC(I2C_ReadBytes, I2C_HandleTypeDef *, uint16_t, uint8_t, uint8_t *, uint16_t);
FAKE_VALUE_FUNC(HAL_StatusTypeDef, HAL_I2C_IsDeviceReady, I2C_HandleTypeDef *, uint16_t, uint32_t, uint32_t);

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

I2C_HandleTypeDef hi2c2; // this var is global in i2c.c and extern in i2c.h. Since the mocks have no knoweledge of the .c file, it must be defined here for test purposes.extern I2C_HandleTypeDef hi2c2;

/***********************************************************************************************************************
 * Test Fixtures
 **********************************************************************************************************************/

class ExampleClass : public ::testing::Test
{
	public:

		virtual void SetUp() // this function is run right before every test associated to the fixture "ADC_INIT" is run.
		{
			RESET_FAKE(MX_I2C1_Init);
			RESET_FAKE(MX_I2C2_Init);
		}

		virtual void TearDown() // this function is run right after every test associated to the fixture "ADC_INIT" is run.
		{
			FFF_RESET_HISTORY();
		}
};

class ExampleClass2 : public ::testing::Test
{
	public:

		virtual void SetUp() // this function is run right before every test associated to the fixture "ADC_INIT" is run.
		{
			RESET_FAKE(I2C_WriteByte);
			RESET_FAKE(I2C_ReadBytes);
		}

		virtual void TearDown() // this function is run right after every test associated to the fixture "ADC_INIT" is run.
		{
			FFF_RESET_HISTORY();
		}
};


/***********************************************************************************************************************
 * Tests
 **********************************************************************************************************************/

TEST_F(ExampleClass, ExampleTest) {

   	/***********************SETUP***********************/
	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
	/**********************ASSERTS**********************/

	EXPECT_EQ(true, true);
}

TEST_F(ExampleClass2, ExampleTest) {

   	/***********************SETUP***********************/
	/********************DEPENDENCIES*******************/
	/********************STEPTHROUGH********************/
	/**********************ASSERTS**********************/

	EXPECT_LT(5.1, 7.0);
}

