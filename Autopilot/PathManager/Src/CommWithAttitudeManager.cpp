/**
 * Implements methods for communication with the attitude manager.
 * Author: Anthony Bertnyk
 */

#include "../Inc/CommWithAttitudeManager.hpp"

void CommWithAMInit()
{
    commandsMailQ = osMailCreate(osMailQ(commandsMailQ), NULL);
}

void SendCommandsForAM(CommandsForAM *Commands)
{
    //Allocate mail slot
    CommandsForAM *commandsOut;
    commandsOut = static_cast<CommandsForAM *>(osMailAlloc(commandsMailQ, osWaitForever));
    
    //Fill mail slot with data
    *commandsOut = *Commands;

    //Post mail slot to mail queue
    osMailPut(commandsMailQ, commandsOut);
}

AttitudeData GetAttitudeData()
{
    
}
