/**
 * Implements methods for communication with the path manager.
 * Author: Anthony Bertnyk, messed with by Gordon Fountain to co-opt for Telemetry
 */

#include "CommWithPathManager.hpp"

extern "C"
{
#include "cmsis_os.h"
}

void CommWithPMInit()
{
    PMcommandsMailQ = osMailCreate(osMailQ(commandsMailQ), NULL);
}

void SendCommandsForPM(TelemToPMData *commands)
{
    //Remove previous command from mail queue if it exists
    osEvent event = osMailGet(PMcommandsMailQ, 0);
    if(event.status == osEventMail)
    {
        osMailFree(PMcommandsMailQ, static_cast<TelemToPMData *>(event.value.p));
    }

    //Allocate mail slot
    CommandsForPM *commandsOut;
    commandsOut = static_cast<TelemToPMData *>(osMailAlloc(PMcommandsMailQ, osWaitForever));
    
    //Fill mail slot with data
    *commandsOut = *commands;

    //Post mail slot to mail queue
    osMailPut(PMcommandsMailQ, commandsOut);
}

bool GetTelemData(TeleToPMData *data)
{
    //Try to get data from mail queue
    osEvent event;
    TelemData * dataIn;
    event = osMailGet(telemDataMailQ, 0);
    if(event.status == osEventMail)
    {
        dataIn = static_cast<TelemToPMData *>(event.value.p);
        
        //Keep the data and remove it from the queue
        *data = *dataIn;
        osMailFree(telemDataMailQ, dataIn);
        return true;
    }
    else
    {
        //Indicate that no new data is available.
        return false;
    }
}
