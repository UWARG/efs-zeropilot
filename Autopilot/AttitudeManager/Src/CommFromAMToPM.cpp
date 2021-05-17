/**
 * Defines functions for sending data from the attitude manager to the path manager.
 * Has function definitions used by both the attitude and path manager.
 * Author: Anthony Bertnyk
 */

#include "CommWithAttitudeManager.hpp"
#include "CommWithPathManager.hpp"

extern "C"
{
#include "cmsis_os.h"
}

//Set up a mail queue for sending data to the path manager
osMailQDef(attitudeDataMailQ, PATH_ATTITUDE_MAIL_Q_SIZE, AttitudeData);
osMailQId attitudeDataMailQID;

void CommFromAMToPMInit()
{
    attitudeDataMailQID = osMailCreate(osMailQ(attitudeDataMailQ), NULL);
}

void SendFromAMToPM(AttitudeData *data)
{
    //Remove previous data from mail queue if it exists
    osEvent event = osMailGet(attitudeDataMailQID, 0);
    if(event.status == osEventMail)
    {
        osMailFree(attitudeDataMailQID, static_cast<AttitudeData *>(event.value.p));
    }

    //Allocate mail slot
    AttitudeData *dataOut;
    dataOut = static_cast<AttitudeData *>(osMailAlloc(attitudeDataMailQID, osWaitForever));
    
    //Fill mail slot with data
    *dataOut = *data;

    //Post mail slot to mail queue
    osMailPut(attitudeDataMailQID, dataOut);
}


bool GetFromAMToPM(AttitudeData *data)
{
    //Try to get data from mail queue
    osEvent event;
    AttitudeData * dataIn;
    event = osMailGet(attitudeDataMailQID, 0);
    if(event.status == osEventMail)
    {
        dataIn = static_cast<AttitudeData *>(event.value.p);
        
        //Keep the data and remove it from the queue
        *data = *dataIn;
        osMailFree(attitudeDataMailQID, dataIn);
        return true;
    }
    else
    {
        //Indicate that no new data is available.
        return false;
    }
}
