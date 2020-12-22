/**
 * Implements methods for communication with the attitude manager.
 * Author: Anthony Bertnyk
 */

#include "CommWithAttitudeManager.hpp"

extern "C"
{
#include "cmsis_os.h"
}

void CommWithAMInit()
{
    commandsMailQ = osMailCreate(osMailQ(commandsMailQ), NULL);
}

void SendCommandsForAM(CommandsForAM *commands)
{
    //Remove previous command from mail queue if it exists
    osEvent event = osMailGet(commandsMailQ, 0);
    if(event.status == osEventMail)
    {
        osMailFree(commandsMailQ, static_cast<CommandsForAM *>(event.value.p));
    }

    //Allocate mail slot
    CommandsForAM *commandsOut;
    commandsOut = static_cast<CommandsForAM *>(osMailAlloc(commandsMailQ, osWaitForever));
    
    //Fill mail slot with data
    *commandsOut = *commands;

    //Post mail slot to mail queue
    osMailPut(commandsMailQ, commandsOut);
}

bool GetAttitudeData(AttitudeData *data)
{
    //Try to get data from mail queue
    osEvent event;
    AttitudeData * dataIn;
    event = osMailGet(attitudeDataMailQ, 0);
    if(event.status == osEventMail)
    {
        dataIn = static_cast<AttitudeData *>(event.value.p);
        
        //Keep the data and remove it from the queue
        *data = *dataIn;
        osMailFree(attitudeDataMailQ, dataIn);
        return true;
    }
    else
    {
        //Indicate that no new data is available.
        return false;
    }
}
