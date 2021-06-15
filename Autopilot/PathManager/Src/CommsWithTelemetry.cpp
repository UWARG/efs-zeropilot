/**
 * Implements methods for sending path manager data to telemetry.
 * Author: Anthony Bertnyk, Messed with for telemetry use by Gordon Fountain
 */

#include "CommsWithTelemetry.hpp"

extern "C"
{
#include "cmsis_os.h"
}

//Set up a mail queue for sending commands to the attitude manager
osMailQDef(commandsMailQ, PATH_TELEM_MAIL_Q_SIZE, dataOut);
osMailQId commandsMailQID;

void CommWithTelemInit()
{
    telemDataMailQ = osMailCreate(osMailQ(telemDataMailQ), NULL);
}

void SendPathData(Telemetry_POGI_t *data)
{
    //Remove previous data from mail queue if it exists
    osEvent event = osMailGet(telemDataMailQ, 0);
    if(event.status == osEventMail)
    {
        osMailFree(telemDataMailQ, static_cast<Telemetry_POGI_t *>(event.value.p));
    }

    //Allocate mail slot
    Telemetry_POGI_t *dataOut;
    dataOut = static_cast<Telemetry_POGI_t *>(osMailAlloc(telemDataMailQ, osWaitForever));
    //Fill mail slot with data
    *dataOut = *data;

    //Post mail slot to mail queue
    osMailPut(telemDataMailQ, dataOut);
}

bool GetTelemetryCommands(Telemetry_PIGO_t *commands)
{
    
    //Try to get commands from mail queue
    osEvent event;
    Telemetry_POGI_t * commandsIn;
    event = osMailGet(PMcommandsMailQ, 0);
    if(event.status == osEventMail)
    {
        commandsIn = static_cast<Telemetry_POGI_t *>(event.value.p);

        //Keep the command and remove it from the queue
        Telemetry_POGI_t * commands;
        *commands = *commandsIn;
        osMailFree(PMcommandsMailQ, commandsIn);
        return true;
    }
    else
    {
        //Indicate that no new commands are available.
        return false;
    }
    
    return true;
}
