/**
 * Implements methods for communication with the attitude manager.
 * Author: Anthony Bertnyk
 */

#include "CommWithAttitudeManager.hpp"

extern "C"
{
#include "cmsis_os.h"
}

//Set up a mail queue for sending commands to the attitude manager
const uint8_t COMMANDS_MAIL_Q_SIZE = 10;
osMailQDef(commandsMailQ, COMMANDS_MAIL_Q_SIZE, CommandsForAM);
osMailQId commandsMailQ;

void CommWithAMInit()
{
    commandsMailQ = osMailCreate(osMailQ(commandsMailQ), NULL);
}

void SendCommandsForAM(CommandsForAM *Commands)
{
    //Allocate mail slot
    CommandsForAM *commandsOut;
    commandsOut = static_cast<CommandsForAM *>(osMailAlloc(commandsMailQ, osWaitForever));
    
    //Fill mail slot with data
    *commandsOut = *Commands;

    //Post mail slot to mail queue
    osMailPut(commandsMailQ, commandsOut);
}

/*
Set up a mail queue for recieving data from to the attitude manager.
This may have to be moved to another file accessible by both the
attitude manager and path manager interfaces.
*/
const uint8_t ATTITUDE_DATA_MAIL_Q_SIZE = 10;
osMailQDef(attitudeDataMailQ, ATTITUDE_DATA_MAIL_Q_SIZE, AttitudeData);
osMailQId attitudeDataMailQ;

AttitudeData GetAttitudeData()
{
    osEvent event;
    AttitudeData * dataIn;
    event = osMailGet(attitudeDataMailQ, osWaitForever);
    if(event.status == osEventMail)
    {
        dataIn = static_cast<AttitudeData *>(event.value.p);
    }

    return *dataIn;
}

void FreeAttitudeData(AttitudeData *data)
{
    osMailFree(attitudeDataMailQ, data);
}
