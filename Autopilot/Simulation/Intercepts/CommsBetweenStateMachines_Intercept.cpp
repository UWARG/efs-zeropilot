#include "CommWithPathManager.hpp"
#include "CommWithAttitudeManager.hpp"

#include <fstream>

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

static CommandsForAM commandsForAM = {};
static AttitudeData attitudeData = {};

static std::fstream PathToAttComLog;

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

static void writeAttitudeCommandsToFile(CommandsForAM *commands);

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

//PM To AM intercepts
void CommFromPMToAMInit()
{
    PathToAttComLog.open("DebugLogs/PathToAttComLog.txt", std::fstream::out);
    PathToAttComLog << "";
    PathToAttComLog.close();

}

void SendFromPMToAM(CommandsForAM *commands)
{
    commandsForAM = *commands;
    writeAttitudeCommandsToFile(commands);
}

bool GetFromPMToAM(CommandsForAM *commands)
{
    *commands = commandsForAM;
}

//AM To PM intercepts

void CommFromAMToPMInit()
{
}

void SendFromAMToPM(AttitudeData *data)
{
    attitudeData = *data;
}

bool GetFromAMToPM(AttitudeData *data)
{
    *data = attitudeData;
}

static void writeAttitudeCommandsToFile(CommandsForAM *commands)
{

    PathToAttComLog.open("DebugLogs/PathToAttComLog.txt", std::fstream::app);

    PathToAttComLog << std::boolalpha; // bools are printed as "true" and "false" rather than "1" and "0"

    PathToAttComLog << "{roll: " << commands->roll << ", pitch: " << commands->pitch << ", rudderPercent: " << commands->rudderPercent << ", throttlePercent:" << commands->throttlePercent << ", passbyData.rollPercent: " << commands->passbyData.rollPercent << ", passbyData.rollPassby: " << commands->passbyData.rollPassby << ", passbyData.pitchPercent: " << commands->passbyData.pitchPercent << ", passbyData.pitchPassby: " << commands->passbyData.pitchPassby << ", passbyData.rudderPercent : " << commands->passbyData.rudderPercent << ", passbyData.rudderPassby: " << commands->passbyData.rudderPassby << ", passbyData.throttlePercent: " << commands->passbyData.throttlePercent << ", passbyData.throttlePassby: " << commands->passbyData.throttlePassby << "}" << std::endl << std::endl;

    PathToAttComLog.close();
}
