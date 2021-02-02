/*
* Author: Nixon Chan
*/

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
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(obtainDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), decodeDataMode::getInstance());
}

TEST(TelemetryManagerFSM, decodeToPass){ //testing transition from decodeDataMode to passToPathMode
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

TEST(TelemetryManagerFSM, analyzeToReport){ //testing transition from analyzeDataMode to reportMode
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

TEST(TelemetryManagerFSM, encodeToSend){ //testing transition from encodeDataMode to sendDataMode
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

TEST(TelemetryManagerFSM, sendToObtain){ //testing transition from sendDataMode to obtainDataMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(sendDataMode::getInstance());
    telemetryMng.regularReport=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), obtainDataMode::getInstance());
}

TEST(TelemetryManagerFSM, sendToInitialThroughDataValid){//testing if statements and functionality
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.dataError=true;
    telemetryMng.dataValid=true;
    telemetryMng.setState(reportMode::getInstance());
    telemetryMng.execute(); //this execute allows the machine to set the regularReport variable using if statements in reportMode class
    telemetryMng.setState(sendDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), initialMode::getInstance());
}

TEST(TelemetryManagerFSM, sendToObtainThroughDataValid){//testing if statements and functionality
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.dataError=false;
    telemetryMng.dataValid=true;
    telemetryMng.setState(reportMode::getInstance());
    telemetryMng.execute(); //this execute allows the machine to set the regularReport variable using if statements in reportMode class
    telemetryMng.setState(sendDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), obtainDataMode::getInstance());
}

TEST(TelemetryManagerFSM, sendToInitialThroughDataInvalid){//testing if statements and functionality
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.cycleCounter=5;
    telemetryMng.dataValid=false;
    telemetryMng.setState(reportMode::getInstance());
    telemetryMng.execute(); //this execute allows the machine to set the regularReport variable using if statements in reportMode class
    telemetryMng.setState(sendDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), initialMode::getInstance());
}

TEST(TelemetryManagerFSM, sendToObtainThroughDataInvalid){//testing if statements and functionality
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.dataValid=false;
    telemetryMng.setState(reportMode::getInstance());
    telemetryMng.execute(); //this execute allows the machine to set the regularReport variable using if statements in reportMode class
    telemetryMng.setState(sendDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), obtainDataMode::getInstance());
}

/****************************
TESTING FAILURE AT EACH STATE
****************************/
TEST(TelemetryManagerFSM, obtainFail){ 
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(obtainDataMode::getInstance());
    telemetryMng.fatalFail=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}

TEST(TelemetryManagerFSM, decodeFail){ 
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
    telemetryMng.setState(passToPathMode::getInstance());
    telemetryMng.fatalFail=true;
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}

TEST(TelemetryManagerFSM, readFail){ 
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(readFromPathMode::getInstance());
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

TEST(TelemetryManagerFSM, failToFail){ 
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(failureMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), failureMode::getInstance());
}
