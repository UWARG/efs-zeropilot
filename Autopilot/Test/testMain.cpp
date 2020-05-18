#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fff.h"

DEFINE_FFF_GLOBALS;

/***********************************************************************************************************************
 * Google test main, unless you know exactly what your'e doing, you probably should not be modifying this file.
 **********************************************************************************************************************/

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
