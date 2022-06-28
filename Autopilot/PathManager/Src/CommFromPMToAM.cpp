/**
 * Defines functions for sending data from the path manager to the attitude manager.
 * Has function definitions used by both the attitude and path manager.
 * Author: Anthony Bertnyk
 */

#include "CommWithAttitudeManager.hpp"
#include "CommWithPathManager.hpp"

extern "C"
{
#include "cmsis_os.h"
}

const char PATH_ATTITUDE_MAIL_Q_SIZE = 1;

//Set up a mail queue for sending commands to the attitude manager
osMailQDef(commandsMailQ, PATH_ATTITUDE_MAIL_Q_SIZE, CommandsForAM);
osMailQId commandsMailQID;

void CommFromPMToAMInit()
{
    commandsMailQID = osMailCreate(osMailQ(commandsMailQ), NULL);
}

void SendFromPMToAM(CommandsForAM *commands)
{
    //Remove previous command from mail queue if it exists
    osEvent event = osMailGet(commandsMailQID, 0);
    if(event.status == osEventMail)
    {
        osMailFree(commandsMailQID, static_cast<CommandsForAM *>(event.value.p));
    }

    //Allocate mail slot
    CommandsForAM *commandsOut;
    commandsOut = static_cast<CommandsForAM *>(osMailAlloc(commandsMailQID, osWaitForever));
    
    //Fill mail slot with data
    *commandsOut = *commands;

    //Post mail slot to mail queue
    osStatus sendStatus = osMailPut(commandsMailQID, commandsOut);
}

#if IS_FIXED_WING
bool GetFromPMToAM(CommandsForAM *commands)
{
    //Try to get commands from mail queue
    osEvent event;
    CommandsForAM * commandsIn;
    event = osMailGet(commandsMailQID, 0);
    if(event.status == osEventMail)
    {
        commandsIn = static_cast<CommandsForAM *>(event.value.p);
        
        //Keep the command and remove it from the queue
        *commands = *commandsIn;
        osMailFree(commandsMailQID, commandsIn);
        return true;
    }
    else
    {
        //Indicate that no new commands are available.
        return false;
    }
}
#endif
