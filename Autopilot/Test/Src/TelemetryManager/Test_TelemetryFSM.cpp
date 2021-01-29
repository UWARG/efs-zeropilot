#include <gtest/gtest.h>

#include "telemetryManager.hpp"
#include "telemetryStateClasses.hpp"


using namespace std; 
using ::testing::Test;

TEST(TelemetryManagerFSM, InitialStateIsInitialMode){
    /***SETUP***/
    telemetryManager telemetryMng;
    /***ASSERTS***/
    ASSERT_EQ(*(telemetryMng.getCurrentState()), initialMode::getInstance());
}

TEST(TelemetryManagerFSM, InitialTransitionToGround){
    /***SETUP***/
    telemetryManager telemetryMng;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    ASSERT_EQ(*(telemetryMng.getCurrentState()), groundDataPathMode::getInstance());
}

TEST(TelemetryManagerFSM, GroundToRegularThroughDataValid){
    /***SETUP***/
    telemetryManager telemetryMng;
    /***DEPENDECIES***/
    telemetryMng.dataValid=true;
    telemetryMng.dataError=false;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    telemetryMng.execute();
    /***ASSERTS***/
    ASSERT_EQ(*(telemetryMng.getCurrentState()), regularReportMode::getInstance());
}

TEST(TelemetryManagerFSM, GroundToRegularThroughDataInvalid){
    /***SETUP***/
    telemetryManager telemetryMng;
    /***DEPENDENCIES***/
    telemetryMng.dataValid=false;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    telemetryMng.execute();
    /***ASSERTS***/
    ASSERT_EQ(*(telemetryMng.getCurrentState()), regularReportMode::getInstance());
}

TEST(TelemetryManagerFSM, GroundToErrorThroughDataValid){
    /***SETUP***/
    telemetryManager telemetryMng;
    /***DEPENDENCIES***/
    telemetryMng.dataValid=true;
    telemetryMng.dataError=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    telemetryMng.execute();
    /***ASSERTS***/
    ASSERT_EQ(*(telemetryMng.getCurrentState()), errorReportMode::getInstance());
}

TEST(TelemetryManagerFSM, GroundToErrorThroughDataInvalid){
    /***SETUP***/
    telemetryManager telemetryMng;
    /***DEPENDENCIES***/
    telemetryMng.dataValid=false;
    /***STEPTHROUGH***/
    telemetryMng.execute(); //to ground
    for(int index=0; index<4; index++)
    {
        telemetryMng.execute();//to regular report
        telemetryMng.execute();//to ground
    }
    //last increase in counter
    telemetryMng.execute();//to error report
    /***ASSERT***/
    ASSERT_EQ(*(telemetryMng.getCurrentState()), errorReportMode::getInstance());
}
TEST(TelemetryManagerFSM, RegularToGround){
    /***SETUP***/
    telemetryManager telemetryMng;
    /***DEPENDENCIES***/
    telemetryMng.dataValid=true;
    telemetryMng.dataError=false;
    /***STEPTHROUGH***/
    telemetryMng.execute();//to ground 
    telemetryMng.execute();//to regular report
    telemetryMng.execute();//back to ground
    ASSERT_EQ(*(telemetryMng.getCurrentState()), groundDataPathMode::getInstance());
}
TEST(TelemetryManagerFSM, ErrorToInitial){
    /***SETUP***/
    telemetryManager telemetryMng;
    /***DEPENDENCIES***/
    telemetryMng.dataValid=true;
    telemetryMng.dataError=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();//to ground 
    telemetryMng.execute();//to error report
    telemetryMng.execute();//back to initial
    ASSERT_EQ(*(telemetryMng.getCurrentState()), initialMode::getInstance());
}






