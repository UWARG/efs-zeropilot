#include <gtest/gtest.h>

#include "telemetryManager.hpp"
#include "telemetryStateClasses.hpp"


using namespace std; 
using ::testing::Test;

TEST(TelemetryManagerFSM, InitialStateIsInitialMode){ //testing if initialized correctly
    /***SETUP***/
    telemetryManager telemetryMng;
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), initialMode::getInstance());
    EXPECT_EQ(telemetryMng.getStatus(), COMPLETED_CYCLE);
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
    EXPECT_EQ(telemetryMng.getStatus(), IN_CYCLE);
}

TEST(TelemetryManagerFSM, ObtainToDecode){ //testing transition from obtainDataMode to decodeDataMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(obtainDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), decodeDataMode::getInstance());
    EXPECT_EQ(telemetryMng.getStatus(), IN_CYCLE);
}

TEST(TelemetryManagerFSM, decodeToPass){ //testing transition from decodeDataMode to passToPathMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(decodeDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), passToPathMode::getInstance());
    EXPECT_EQ(telemetryMng.getStatus(), IN_CYCLE);
}

TEST(TelemetryManagerFSM, passToRead){ //testing transition from passToPathMode to readFromPathMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(passToPathMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), readFromPathMode::getInstance());
    EXPECT_EQ(telemetryMng.getStatus(), IN_CYCLE);
}

TEST(TelemetryManagerFSM, readToAnalyze){ //testing transition from readFromPathMode to analyzeDataMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(readFromPathMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), analyzeDataMode::getInstance());
    EXPECT_EQ(telemetryMng.getStatus(), IN_CYCLE);
}

TEST(TelemetryManagerFSM, analyzeToReport){ //testing transition from analyzeDataMode to reportMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(analyzeDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), reportMode::getInstance());
    EXPECT_EQ(telemetryMng.getStatus(), IN_CYCLE);
}

TEST(TelemetryManagerFSM, reportToEncode){ //testing transition from reportMode to encodeDataMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(reportMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), encodeDataMode::getInstance());
    EXPECT_EQ(telemetryMng.getStatus(), IN_CYCLE);
}

TEST(TelemetryManagerFSM, encodeToSend){ //testing transition from encodeDataMode to sendDataMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(encodeDataMode::getInstance());
    /***STEPTHROUGH***/
    telemetryMng.execute();
    /***ASSERTS***/
    EXPECT_EQ(*(telemetryMng.getCurrentState()), sendDataMode::getInstance());
    EXPECT_EQ(telemetryMng.getStatus(), IN_CYCLE);
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
    EXPECT_EQ(telemetryMng.getStatus(), COMPLETED_CYCLE);
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
    EXPECT_EQ(telemetryMng.getStatus(), IN_CYCLE);
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
    EXPECT_EQ(telemetryMng.getStatus(), FAILURE_MODE);
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
    EXPECT_EQ(telemetryMng.getStatus(), FAILURE_MODE);
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
    EXPECT_EQ(telemetryMng.getStatus(), FAILURE_MODE);
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
    EXPECT_EQ(telemetryMng.getStatus(), FAILURE_MODE);
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
    EXPECT_EQ(telemetryMng.getStatus(), FAILURE_MODE);
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
    EXPECT_EQ(telemetryMng.getStatus(), FAILURE_MODE);
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
    EXPECT_EQ(telemetryMng.getStatus(), FAILURE_MODE);
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
    EXPECT_EQ(telemetryMng.getStatus(), FAILURE_MODE);
}