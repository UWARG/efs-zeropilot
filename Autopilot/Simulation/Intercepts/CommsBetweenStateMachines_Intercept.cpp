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

void CommWithAMInit()
{
    PathToAttComLog.open("DebugLogs/PathToAttComLog.txt", std::fstream::out);
    PathToAttComLog << "";
    PathToAttComLog.close();

}

void SendCommandsForAM(CommandsForAM *commands)
{
    commands->passbyData.pitchPassby=false;
    commands->passbyData.rollPassby=false;
    commands->passbyData.throttlePassby=false;
    commands->passbyData.rudderPassby=false;
    commandsForAM = *commands;
    writeAttitudeCommandsToFile(commands);
}

bool GetAttitudeData(AttitudeData *data)
{
    *data = attitudeData;
}

void CommWithPMInit()
{
}

void SendAttitudeData(AttitudeData *data)
{
    attitudeData = *data;
}

bool GetCommands(CommandsForAM *commands)
{
    *commands = commandsForAM;
}

static void writeAttitudeCommandsToFile(CommandsForAM *commands)
{

    PathToAttComLog.open("DebugLogs/PathToAttComLog.txt", std::fstream::app);

    PathToAttComLog << std::boolalpha; // bools are printed as "true" and "false" rather than "1" and "0"

    PathToAttComLog << "{roll: " << commands->roll << ", pitch: " << commands->pitch << ", rudderPercent: " << commands->rudderPercent << ", throttlePercent:" << commands->throttlePercent << ", passbyData.rollPercent: " << commands->passbyData.rollPercent << ", passbyData.rollPassby: " << commands->passbyData.rollPassby << ", passbyData.pitchPercent: " << commands->passbyData.pitchPercent << ", passbyData.pitchPassby: " << commands->passbyData.pitchPassby << ", passbyData.rudderPercent : " << commands->passbyData.rudderPercent << ", passbyData.rudderPassby: " << commands->passbyData.rudderPassby << ", passbyData.throttlePercent: " << commands->passbyData.throttlePercent << ", passbyData.throttlePassby: " << commands->passbyData.throttlePassby << "}" << std::endl << std::endl;

    PathToAttComLog.close();
}
