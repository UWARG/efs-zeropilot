/**
 * Implements methods for sending commands to the path manager from telemetry.
 * Author: Anthony Bertnyk, messed with by Gordon Fountain to co-opt for Telemetry
 */

#include "CommsWithPathManager.hpp"

extern "C"
{
#include "cmsis_os.h"
}

osMailQDef(PIGOMailQ, PATH_TELEM_MAIL_Q_SIZE, Telemetry_PIGO_t);
osMailQId PIGOMailQ;

void TelemCommWithPMInit()
{
    PIGOMailQ = osMailCreate(osMailQ(PIGOMailQ), NULL);
}

void SendCommandsForPM(Telemetry_PIGO_t *commands)
{
    
    //Remove previous command from mail queue if it exists
    osEvent event = osMailGet(PIGOMailQ, 0);
    if(event.status == osEventMail)
    {
        osMailFree(PIGOMailQ, static_cast<Telemetry_PIGO_t *>(event.value.p));
    }

    //Allocate mail slot
    Telemetry_PIGO_t *commandsOut;
    commandsOut = static_cast<Telemetry_PIGO_t *>(osMailAlloc(PIGOMailQ, osWaitForever));

    //Fill mail slot with data
    *commandsOut = *commands;

    //Post mail slot to mail queue
    osMailPut(PIGOMailQ, commandsOut);
    
}

// bool GetTelemData(POGI *data)
// {
    
//     //Try to get data from mail queue
//     osEvent event;
//     POGI * dataIn;
//     event = osMailGet(POGIMailQ, 0);
//     if(event.status == osEventMail)
//     {
//         dataIn = static_cast<POGI *>(event.value.p);

//         //Keep the data and remove it from the queue
//         *data = *dataIn;
//         osMailFree(POGIMailQ, dataIn);
//         return true;
//     }
//     else
//     {
//         //Indicate that no new data is available.
//         return false;
//     }
    
//     return true;
// }

bool GetTelemetryCommands(Telemetry_PIGO_t *commands)
{

    //Try to get commands from mail queue
    osEvent event;
    Telemetry_PIGO_t * commandsIn;
    event = osMailGet(PIGOMailQ, 0);
    if(event.status == osEventMail)
    {
        commandsIn = static_cast<Telemetry_PIGO_t *>(event.value.p);

        //Keep the command and remove it from the queue
        *commands = *commandsIn;
        osMailFree(PIGOMailQ, commandsIn);
        return true;
    }
    else
    {
        //Indicate that no new commands are available.
        return false;
    }
    return true;

}
