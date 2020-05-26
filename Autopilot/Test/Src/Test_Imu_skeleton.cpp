#include <gtest/gtest.h>
#include "fff.h"
#include "i2c.h"
#include "mpu9255.h"


/* Info on google test and google mock */
/* https://github.com/google/googletest/blob/master/googletest/docs/primer.md */
/* https://github.com/google/googletest/blob/master/googlemock/README.md */


/*
* This file exists because there is a partial implementation of the IMU driver. That implementation attempts to speak
* with hardware, which obviously can't happen on a PC. So the hardware functions are faked to allow the code to run off target.
*/


/***********************************************************************************************************************
 * Mocks
 **********************************************************************************************************************/

FAKE_VOID_FUNC(HAL_Delay, uint32_t);
