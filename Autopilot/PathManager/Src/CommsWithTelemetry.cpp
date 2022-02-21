/**
 * Implements methods for sending path manager data to telemetry.
 * Author: Anthony Bertnyk, Messed with for telemetry use by Gordon Fountain
 */

#include "CommsWithTelemetry.hpp"

extern "C"
{
#include "cmsis_os.h"
}

osMailQDef(POGIMailQ, PATH_TELEM_MAIL_Q_SIZE, POGI);
osMailQId POGIMailQ;

void CommWithTelemInit()
{
    POGIMailQ = osMailCreate(osMailQ(POGIMailQ), NULL);
}

void SendPathData(POGI *data)
{

    //Remove previous data from mail queue if it exists
    osEvent event = osMailGet(POGIMailQ, 0);
    if(event.status == osEventMail)
    {
        osMailFree(POGIMailQ, static_cast<POGI *>(event.value.p));
    }

    //Allocate mail slot
    POGI *dataOut;
    dataOut = static_cast<POGI *>(osMailAlloc(POGIMailQ, osWaitForever));

    //Fill mail slot with data
    *dataOut = *data;

    //Post mail slot to mail queue
    osMailPut(POGIMailQ, dataOut);

}

bool GetTelemData(POGI *data)
{
    
    //Try to get data from mail queue
    osEvent event;
    POGI * dataIn;
    event = osMailGet(POGIMailQ, 0);
    if(event.status == osEventMail)
    {
        dataIn = static_cast<POGI *>(event.value.p);

        //Keep the data and remove it from the queue
        *data = *dataIn;
        osMailFree(POGIMailQ, dataIn);
        return true;
    }
    else
    {
        //Indicate that no new data is available.
        return false;
    }
    
    return true;
}

// bool GetTelemetryCommands(Telemetry_PIGO_t *commands)
// {

//     //Try to get commands from mail queue
//     osEvent event;
//     Telemetry_PIGO_t * commandsIn;
//     event = osMailGet(PIGOMailQ, 0);
//     if(event.status == osEventMail)
//     {
//         commandsIn = static_cast<Telemetry_PIGO_t *>(event.value.p);

//         //Keep the command and remove it from the queue
//         *commands = *commandsIn;
//         osMailFree(PIGOMailQ, commandsIn);
//         return true;
//     }
//     else
//     {
//         //Indicate that no new commands are available.
//         return false;
//     }
//     return true;

// }
