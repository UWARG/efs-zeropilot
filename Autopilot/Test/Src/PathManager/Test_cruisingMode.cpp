/*
* Author: Dhruv Rawat
*/

#include "fff.h"
#include <gtest/gtest.h>

#include "pathManager.hpp"
#include "pathStateClasses.hpp"

#include "pathModeSelector.hpp"

FAKE_VOID_FUNC(CommFromPMToAMInit);
FAKE_VOID_FUNC(SendFromPMToAM, CommandsForAM*);
FAKE_VALUE_FUNC(bool, GetFromAMToPM, AttitudeData*);
FAKE_VALUE_FUNC(SFError_t, SF_GetResult, SFOutput_t *);
FAKE_VALUE_FUNC(IMU_Data_t, SF_GetRawIMU);
FAKE_VOID_FUNC(AutoSteer_Init);
FAKE_VOID_FUNC(CommWithAMInit);
FAKE_VOID_FUNC(SendCommandsForAM, CommandsForAM*);
FAKE_VOID_FUNC(CommWithTelemInit);
FAKE_VALUE_FUNC(bool, GetTelemetryCommands, Telemetry_PIGO_t*);
FAKE_VALUE_FUNC(bool, GetAttitudeData, AttitudeData*);
FAKE_VOID_FUNC(AutoSteer_ComputeCoordinatedTurn, CoordinatedTurnInput_t*, CoordinatedTurnAttitudeManagerCommands_t*);
FAKE_VOID_FUNC(AutoSteer_ComputeAltitudeAndAirspeed, AltitudeAirspeedInput_t*, AltitudeAirspeedCommands_t*);

class PathManagerFSM : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(CommFromPMToAMInit);
			RESET_FAKE(SendFromPMToAM);
			RESET_FAKE(GetFromAMToPM);
			RESET_FAKE(SF_GetResult);
			RESET_FAKE(SF_GetRawIMU);
			RESET_FAKE(AutoSteer_Init);
			RESET_FAKE(CommWithAMInit);
			RESET_FAKE(SendCommandsForAM);
			RESET_FAKE(GetAttitudeData);
			RESET_FAKE(AutoSteer_ComputeCoordinatedTurn);
			RESET_FAKE(AutoSteer_ComputeAltitudeAndAirspeed);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}

};

class PathManagerDataHandoff : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(SF_GetResult);
			RESET_FAKE(SF_GetRawIMU);
			RESET_FAKE(AutoSteer_Init);
			RESET_FAKE(CommWithAMInit);
			RESET_FAKE(SendCommandsForAM);
			RESET_FAKE(GetAttitudeData);
			RESET_FAKE(AutoSteer_ComputeCoordinatedTurn);
			RESET_FAKE(AutoSteer_ComputeAltitudeAndAirspeed);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}
};



TEST(CruisingModeTest, WillPrint) {

    pathManager pathMan;
    pathMan.setState(modeExecutor::getInstance());
    pathMan.execute();

    EXPECT_EQ(*(pathMan.getCurrentState()), coordinateTurnElevation::getInstance());
}


