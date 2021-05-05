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

    PathToAttComLog << "{roll: " << commands->roll << ", pitch: " << commands->pitch << ", rudderPercent: " << commands->rudderPercent << ", throttlePercent:" << commands->throttlePercent << ", passbyData.rollPercent: " << commands->passbyData.rollPercent << ", passbyData.rollPassby: " << commands->passbyData.rollPassby << ", passbyData.pitchPercent: " << commands->passbyData.pitchPercent << ", passbyData.pitchPassby: " << commands->passbyData.pitchPassby << "}" << std::endl;

    PathToAttComLog.close();
}
