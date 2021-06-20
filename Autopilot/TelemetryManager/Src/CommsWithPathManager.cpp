/**
 * Implements methods for sending commands to the path manager from telemetry.
 * Author: Anthony Bertnyk, messed with by Gordon Fountain to co-opt for Telemetry
 */

#include "TelemPathInterface.hpp"
#include "CommsWithPathManager.hpp"

extern "C"
{
#include "cmsis_os.h"
}

//Set up a mail queue for sending commands to the path manager
osMailQDef(PMcommandsMailQ, PATH_TELEM_MAIL_Q_SIZE, Telemetry_PIGO_t);
osMailQId PMcommandsMailQID;

void CommFromTMToPMInit()
{
    PMcommandsMailQID = osMailCreate(osMailQ(PMcommandsMailQ), NULL);
}

void SendFromTMToPM(Telemetry_PIGO_t *commands)
{
    //Remove previous command from mail queue if it exists
    osEvent event = osMailGet(PMcommandsMailQID, 0);
    if(event.status == osEventMail)
    {
        osMailFree(PMcommandsMailQID, static_cast<Telemetry_PIGO_t *>(event.value.p));
    }

    //Allocate mail slot
    Telemetry_PIGO_t *commandsOut;
    commandsOut = static_cast<Telemetry_PIGO_t *>(osMailAlloc(PMcommandsMailQID, osWaitForever));

    //Fill mail slot with data
    *commandsOut = *commands;

    //Post mail slot to mail queue
    osMailPut(PMcommandsMailQID, commandsOut);
}

bool GetFromTMToPM(Telemetry_PIGO_t *commands)
{
    //Try to get data from mail queue
    osEvent event;
    Telemetry_PIGO_t * commandsIn;
    event = osMailGet(PMcommandsMailQID, 0);
    if(event.status == osEventMail)
    {
        commandsIn = static_cast<Telemetry_PIGO_t *>(event.value.p);

        //Keep the data and remove it from the queue
        *commands = *commandsIn;
        osMailFree(PMcommandsMailQID, commandsIn);
        return true;
    }
    else
    {
        //Indicate that no new data is available.
        return false;
    }
    
    return true;
}
