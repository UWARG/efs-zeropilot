#include <gtest/gtest.h>

#include "telemetryManager.hpp"
#include "telemetryStateClasses.hpp"


using namespace std; 
using ::testing::Test;

TEST(TelemetryManagerFSM, InitialStateIsInitialMode){
    /***SETUP***/
    telemetryManager telemetryMng;
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), initialMode::getInstance());
}

<<<<<<< Updated upstream:Autopilot/Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
TEST(TelemetryManagerFSM, InitialTransitionToGround){
=======
/***************************************
TESTING TRANSITION BETWEEN NORMAL STATES
****************************************/

TEST(TelemetryManagerFSM, InitialTransitionToObtain){ //testing if first transition worked properly
    /***SETUP***/
    telemetryManager telemetryMng;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), obtainDataMode::getInstance());
}

TEST(TelemetryManagerFSM, ObtainToDecode){ //testing transition from obtainDataMode to decodeDataMode
>>>>>>> Stashed changes:Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(obtainDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), decodeDataMode::getInstance());
}

<<<<<<< Updated upstream:Autopilot/Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
TEST(TelemetryManagerFSM, GroundToRegularThroughDataValid){
=======
TEST(TelemetryManagerFSM, decodeToPass){ //testing transition from decodeDataMode to passToPathMode
>>>>>>> Stashed changes:Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(decodeDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), passToPathMode::getInstance());
}

TEST(TelemetryManagerFSM, passToRead){ //testing transition from passToPathMode to readFromPathMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(passToPathMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), readFromPathMode::getInstance());
}

TEST(TelemetryManagerFSM, readToAnalyze){ //testing transition from readFromPathMode to analyzeDataMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(readFromPathMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), analyzeDataMode::getInstance());
}

<<<<<<< Updated upstream:Autopilot/Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
TEST(TelemetryManagerFSM, GroundToRegularThroughDataInvalid){
=======
TEST(TelemetryManagerFSM, analyzeToReport){ //testing transition from analyzeDataMode to reportMode
>>>>>>> Stashed changes:Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(analyzeDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), reportMode::getInstance());
}

TEST(TelemetryManagerFSM, reportToEncode){ //testing transition from reportMode to encodeDataMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(reportMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), encodeDataMode::getInstance());
}

<<<<<<< Updated upstream:Autopilot/Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
TEST(TelemetryManagerFSM, GroundToErrorThroughDataValid){
=======
TEST(TelemetryManagerFSM, encodeToSend){ //testing transition from encodeDataMode to sendDataMode
>>>>>>> Stashed changes:Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(encodeDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), sendDataMode::getInstance());
}

TEST(TelemetryManagerFSM, sendToInitial){ //testing transition from sendDataMode to initialMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(sendDataMode::getInstance());
    telemetryMng.regularReport=false;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), initialMode::getInstance());
}

<<<<<<< Updated upstream:Autopilot/Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
TEST(TelemetryManagerFSM, GroundToErrorThroughDataInvalid){
=======
TEST(TelemetryManagerFSM, sendToObtain){ //testing transition from sendDataMode to obtainDataMode
>>>>>>> Stashed changes:Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(sendDataMode::getInstance());
    telemetryMng.regularReport=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), obtainDataMode::getInstance());
}
<<<<<<< Updated upstream:Autopilot/Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
TEST(TelemetryManagerFSM, RegularToGround){
=======

/****************************
TESTING FAILURE AT EACH STATE
****************************/
TEST(TelemetryManagerFSM, obtainFail){ 
>>>>>>> Stashed changes:Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(obtainDataMode::getInstance());
    telemetryMng.fatalFail=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}
<<<<<<< Updated upstream:Autopilot/Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
TEST(TelemetryManagerFSM, ErrorToInitial){
=======

TEST(TelemetryManagerFSM, decodeFail){ 
>>>>>>> Stashed changes:Test/Src/TelemetryManager/Test_TelemetryFSM.cpp
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(decodeDataMode::getInstance());
    telemetryMng.fatalFail=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}

TEST(TelemetryManagerFSM, passFail){ 
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(passDataMode::getInstance());
    telemetryMng.fatalFail=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}

TEST(TelemetryManagerFSM, readFail){ 
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(readDataMode::getInstance());
    telemetryMng.fatalFail=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}

TEST(TelemetryManagerFSM, analyzeFail){ 
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(analyzeDataMode::getInstance());
    telemetryMng.fatalFail=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}

TEST(TelemetryManagerFSM, reportFail){ 
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(reportMode::getInstance());
    telemetryMng.fatalFail=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}

TEST(TelemetryManagerFSM, encodeFail){ 
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(encodeDataMode::getInstance());
    telemetryMng.fatalFail=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}

TEST(TelemetryManagerFSM, sendFail){ 
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(sendDataMode::getInstance());
    telemetryMng.fatalFail=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}