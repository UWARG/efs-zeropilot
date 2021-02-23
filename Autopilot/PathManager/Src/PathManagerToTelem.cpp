/**
 * Implements methods for sending path manager data to telemetry.
 * Author: Anthony Bertnyk, Messed with for telemetry use by Gordon Fountain
 */

#include "PathManagerToTelem.hpp"

extern "C"
{
#include "cmsis_os.h"
}

void CommWithTelemInit()
{
    attitudeDataMailQ = osMailCreate(osMailQ(telemDataMailQ), NULL);
}

void SendPathData(PathData *data)
{
    //Remove previous data from mail queue if it exists
    osEvent event = osMailGet(telemDataMailQ, 0);
    if(event.status == osEventMail)
    {
        osMailFree(telemDataMailQ, static_cast<PathData *>(event.value.p));
    }

    //Allocate mail slot
    PathData *dataOut;
    dataOut = static_cast<PathData *>(osMailAlloc(telemDataMailQ, osWaitForever));
    
    //Fill mail slot with data
    *dataOut = *data;

    //Post mail slot to mail queue
    osMailPut(telemDataMailQ, dataOut);
}

bool GetCommands(CommandsForPM *commands)
{
    //Try to get commands from mail queue
    osEvent event;
    CommandsForPM * commandsIn;
    event = osMailGet(commandsMailQ, 0);
    if(event.status == osEventMail)
    {
        commandsIn = static_cast<CommandsForPM *>(event.value.p);
        
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
