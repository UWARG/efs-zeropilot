#include "CommWithPathManaer.hpp"
#include "CommWithAttitudeManager.hpp"

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

static CommandsForAM commandsForAM = {};
static AttitudeData attitudeData = {};

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void CommWithAMInit()
{
}

void SendCommandsForAM(CommandsForAM *commands)
{
    commandsForAM = *commands;
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
