#include <gtest/gtest.h>
#include "fff.h"

DEFINE_FFF_GLOBALS;


/***********************************************************************************************************************
 * GtestMain
 **********************************************************************************************************************/

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}