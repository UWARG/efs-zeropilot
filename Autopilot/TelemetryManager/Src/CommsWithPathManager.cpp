/**
 * Implements methods for sending commands to the path manager from telemetry.
 * Author: Anthony Bertnyk, messed with by Gordon Fountain to co-opt for Telemetry
 */

#include "CommsWithPathManager.hpp"

extern "C"
{
#include "cmsis_os.h"
}

osMailQDef(commandsMailQ, PATH_TELEM_MAIL_Q_SIZE, commandsOut);
osMailQId commandsMailQID;

void TelemCommWithPMInit()
{
    commandsMailQ = osMailCreate(osMailQ(commandsMailQ), NULL);
}

void SendCommandsForPM(Telemetry_PIGO_t *commands)
{
    //Remove previous command from mail queue if it exists
    osEvent event = osMailGet(commandsMailQ, 0);
    if(event.status == osEventMail)
    {
        osMailFree(commandsMailQ, static_cast<Telemetry_PIGO_t *>(event.value.p));
    }

    //Allocate mail slot
    Telemetry_PIGO_t *commandsOut;
    commandsOut = static_cast<Telemetry_PIGO_t *>(osMailAlloc(commandsMailQ, osWaitForever));

    //Fill mail slot with data
    *commandsOut = *commands;

    //Post mail slot to mail queue
    osMailPut(commandsMailQ, commandsOut);
}

bool GetTelemData(Telemetry_POGI_t *data)
{
    
    //Try to get data from mail queue
    osEvent event;
    Telemetry_POGI_t * dataIn;
    event = osMailGet(telemDataMailQ, 0);
    if(event.status == osEventMail)
    {
        dataIn = static_cast<Telemetry_PIGO_t *>(event.value.p);

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
    
    return true;
}
