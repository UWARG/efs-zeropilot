/**
 * Implements methods for communication with the path manager.
 * Author: Anthony Bertnyk
 */

#include "CommWithPathManager.hpp"

extern "C"
{
#include "cmsis_os.h"
}

void CommWithPMInit()
{
    attitudeDataMailQ = osMailCreate(osMailQ(attitudeDataMailQ), NULL);
}

void SendAttitudeData(AttitudeData *data)
{
    //Remove previous data from mail queue if it exists
    osEvent event = osMailGet(attitudeDataMailQ, 0);
    if(event.status == osEventMail)
    {
        osMailFree(attitudeDataMailQ, static_cast<AttitudeData *>(event.value.p));
    }

    //Allocate mail slot
    AttitudeData *dataOut;
    dataOut = static_cast<AttitudeData *>(osMailAlloc(attitudeDataMailQ, osWaitForever));
    
    //Fill mail slot with data
    *dataOut = *data;

    //Post mail slot to mail queue
    osMailPut(attitudeDataMailQ, dataOut);
}

bool GetCommands(CommandsForAM *commands)
{
    //Try to get commands from mail queue
    osEvent event;
    CommandsForAM * commandsIn;
    event = osMailGet(commandsMailQ, 0);
    if(event.status == osEventMail)
    {
        commandsIn = static_cast<CommandsForAM *>(event.value.p);
        
        //Keep the command and remove it from the queue
        *commands = *commandsIn;
        osMailFree(commandsMailQ, commandsIn);
        return true;
    }
    else
    {
        //Indicate that no new commands are available.
        return false;
    }
}
