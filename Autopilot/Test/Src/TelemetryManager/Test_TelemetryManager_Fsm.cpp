/*
* Author: Nixon Chan
*/

#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include <fff.h>

#include "telemetryManager.hpp"
#include "telemetryStateClasses.hpp"
#include "CommsWithPathManager.hpp"
#include "CommsWithTelemetry.hpp"
#include "xbee.hpp"


using namespace std; 
using ::testing::Test;


FAKE_VOID_FUNC(CommFromTMToPMInit);
FAKE_VOID_FUNC(CommFromPMToTMInit);
FAKE_VOID_FUNC(SendFromTMToPM, Telemetry_PIGO_t*);
FAKE_VALUE_FUNC(bool, GetFromPMToTM, Telemetry_POGI_t*);

class TelemetryManagerFSM : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(CommFromTMToPMInit);
			RESET_FAKE(CommFromPMToTMInit);
			RESET_FAKE(SendFromTMToPM);
            RESET_FAKE(GetFromPMToTM);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}

};


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

TEST(TelemetryManagerFSM, analyzeToEncode){ //testing transition from analyzeDataMode to reportMode
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.setState(analyzeDataMode::getInstance());
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

/* DATA IS NEVER VALID. COMMENTING OUT FOR NOW */

// TEST(TelemetryManagerFSM, sendToInitialThroughDataValid){//testing if statements and functionality
//     /***SETUP***/
//     telemetryManager telemetryMng;
//     telemetryMng.dataError=true;
//     telemetryMng.dataValid=true;
//     telemetryMng.setState(analyzeDataMode::getInstance());
//     telemetryMng.execute(); //this execute allows the machine to set the regularReport variable using if statements in reportMode class
//     telemetryMng.setState(sendDataMode::getInstance());
//     /***STEPTHROUGH***/
//     telemetryMng.execute();
//     /***ASSERTS***/
//     EXPECT_EQ(*(telemetryMng.getCurrentState()), initialMode::getInstance());
// }

// TEST(TelemetryManagerFSM, sendToObtainThroughDataValid){//testing if statements and functionality
//     /***SETUP***/
//     telemetryManager telemetryMng;
//     telemetryMng.dataError=false;
//     telemetryMng.dataValid=true;
//     telemetryMng.setState(analyzeDataMode::getInstance());
//     telemetryMng.execute(); //this execute allows the machine to set the regularReport variable using if statements in reportMode class
//     telemetryMng.setState(sendDataMode::getInstance());
//     /***STEPTHROUGH***/
//     telemetryMng.execute();
//     /***ASSERTS***/
//     EXPECT_EQ(*(telemetryMng.getCurrentState()), obtainDataMode::getInstance());
// }

TEST(TelemetryManagerFSM, sendToInitialThroughDataInvalid){//testing if statements and functionality
    /***SETUP***/
    telemetryManager telemetryMng;
    telemetryMng.failCycleCounter=5;
    telemetryMng.dataValid=false;
    telemetryMng.setState(analyzeDataMode::getInstance());
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
    telemetryMng.setState(analyzeDataMode::getInstance());
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
