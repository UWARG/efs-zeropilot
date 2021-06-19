/**
 * Implements methods for sending path manager data to telemetry.
 * Author: Anthony Bertnyk, Messed with for telemetry use by Gordon Fountain
 */

#include "CommsWithTelemetry.hpp"

extern "C"
{
#include "cmsis_os.h"
}

//Set up a mail queue for sending data to telemetry
osMailQDef(telemDataMailQ, PATH_TELEM_MAIL_Q_SIZE, Telemetry_POGI_t);
osMailQId telemDataMailQID;

void CommFromPMToTMInit()
{
    telemDataMailQID = osMailCreate(osMailQ(telemDataMailQ), NULL);
}

// Send data from path manager to telemetry manager
void SendFromPMToTM(Telemetry_POGI_t *commands)
{
    //Remove previous data from mail queue if it exists
    osEvent event = osMailGet(telemDataMailQID, 0);
    if(event.status == osEventMail)
    {
        osMailFree(telemDataMailQID, static_cast<Telemetry_POGI_t *>(event.value.p));
    }

    //Allocate mail slot
    Telemetry_POGI_t *commandsOut;
    commandsOut = static_cast<Telemetry_POGI_t *>(osMailAlloc(telemDataMailQID, osWaitForever));
    //Fill mail slot with data
    *commandsOut = *commands;

    //Post mail slot to mail queue
    osMailPut(telemDataMailQID, commandsOut);
}

// Get data from telemetry manager
bool GetFromPMToTM(Telemetry_POGI_t *commands)
{
    
    //Try to get commands from mail queue
    osEvent event;
    Telemetry_POGI_t * commandsIn;
    event = osMailGet(telemDataMailQID, 0);
    if(event.status == osEventMail)
    {
        commandsIn = static_cast<Telemetry_POGI_t *>(event.value.p);

        //Keep the command and remove it from the queue
        *commands = *commandsIn;
        osMailFree(telemDataMailQID, commandsIn);
        return true;
    }
    else
    {
        //Indicate that no new commands are available.
        return false;
    }
    
    return true;
}

