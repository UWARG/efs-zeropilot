#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fff.h"
#include "i2c.h"
#include "mpu9255.h"

#include "IMU_Mock.hpp"
#include "IMU.hpp"

using namespace std;
using ::testing::Test;

/* Info on google test and google mock */
/* https://github.com/google/googletest/blob/master/googletest/docs/primer.md */
/* https://github.com/google/googletest/blob/master/googlemock/README.md */

/***********************************************************************************************************************
 * Mocks (For C fakes/mocks, we are using the fake function framework (fff). For C++ fakes/mocks, we use google mock, though no examples of google mock are show here)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Test Fixtures
 **********************************************************************************************************************/
