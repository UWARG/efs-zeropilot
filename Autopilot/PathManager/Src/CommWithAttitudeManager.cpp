/**
 * Implements methods for communication with the attitude manager.
 * Author: Anthony Bertnyk
 */

#include "CommWithAttitudeManager.hpp"
#include "CommWithPathManager.hpp"

extern "C"
{
#include "cmsis_os.h"
}

//Set up a mail queue for sending commands to the attitude manager
osMailQDef(commandsMailQ, PATH_ATTITUDE_MAIL_Q_SIZE, CommandsForAM);
osMailQId commandsMailQ;

void CommFromPMToAMInit()
{
    commandsMailQ = osMailCreate(osMailQ(commandsMailQ), NULL);
}

void SendFromPMToAM(CommandsForAM *commands)
{
    //Remove previous command from mail queue if it exists
    osEvent event = osMailGet(commandsMailQ, 0);
    if(event.status == osEventMail)
    {
        osMailFree(commandsMailQ, static_cast<CommandsForAM *>(event.value.p));
    }

    //Allocate mail slot
    CommandsForAM *commandsOut;
    commandsOut = static_cast<CommandsForAM *>(osMailAlloc(commandsMailQ, osWaitForever));
    
    //Fill mail slot with data
    *commandsOut = *commands;

    //Post mail slot to mail queue
    osStatus sendStatus = osMailPut(commandsMailQ, commandsOut);
}


bool GetFromPMToAM(CommandsForAM *commands)
{
    //Try to get commands from mail queue
    osEvent event;
    CommandsForAM * commandsIn;
    event = osMailGet(commandsMailQ, 0);
    if(event.status == osEventMail)
    {
        commandsIn = static_cast<CommandsForAM *>(event.value.p);
        
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
}
