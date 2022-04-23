/**
 * Implements methods for sending path manager data to telemetry.
 * Author: Anthony Bertnyk, Messed with for telemetry use by Gordon Fountain
 */

#include "CommsWithTelemetry.hpp"

extern "C"
{
#include "cmsis_os.h"
}

void CommWithTelemInit()
{
    /*
    telemDataMailQ = osMailCreate(osMailQ(telemDataMailQ), NULL);
    */
}

void SendPathData(POGI *data)
{
    /*
    //Remove previous data from mail queue if it exists
    osEvent event = osMailGet(telemDataMailQ, 0);
    if(event.status == osEventMail)
    {
        osMailFree(telemDataMailQ, static_cast<POGI *>(event.value.p));
    }

    //Allocate mail slot
    POGI *dataOut;
    dataOut = static_cast<POGI *>(osMailAlloc(telemDataMailQ, osWaitForever));

    //Fill mail slot with data
    *dataOut = *data;

    //Post mail slot to mail queue
    osMailPut(telemDataMailQ, dataOut);
    */
}

bool GetTelemetryCommands(Telemetry_PIGO_t *commands)
{
    /*
    //Try to get commands from mail queue
    osEvent event;
    POGI * commandsIn;
    event = osMailGet(PMcommandsMailQ, 0);
    if(event.status == osEventMail)
    {
        commandsIn = static_cast<POGI *>(event.value.p);

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
    return true;
    */
}
