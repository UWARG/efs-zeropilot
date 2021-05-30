/*
* Author: Nixon Chan
* Implementing Telem Comms: Gordon Fountain
*/

#include "telemetryStateClasses.hpp"
#include "xbee.hpp"
#include "TelemToPathManager.hpp"
#include "PathManagerToTelem.hpp"

void initialMode::execute(telemetryManager* telemetryMgr)
{
    //initial mode
    //Initialize mail queue to send data to Path Manager:
    CommWithPMInit();
    //Initializa mail queue to receive Path Manager data:
    //CommWithTelemInit(); is the one for Path manager to send data to telem. May need the implementation to check the mail queue.

    //State change:
    telemetryMgr -> setState(obtainDataMode::getInstance());
}

telemetryState& initialMode::getInstance()
{
    static initialMode singleton;
    return singleton;
}

void obtainDataMode::execute(telemetryManager* telemetryMgr)
{
    //obtain data from ground
    Receive_Data(); //Receives data in MavLink form from the XBEE

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(decodeDataMode::getInstance());
    }
}

telemetryState& obtainDataMode::getInstance()
{
    static obtainDataMode singleton;
    return singleton;
}

void decodeDataMode::execute(telemetryManager* telemetryMgr)
{
    //decode data with Mavlink
    //Call Jingting's function here

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(passToPathMode::getInstance());
    }
}

telemetryState& decodeDataMode::getInstance()
{
    static decodeDataMode singleton;
    return singleton;
}

void passToPathMode::execute(telemetryManager* telemetryMgr)
{
    //pass data to path manager
    GetTelemData(PIGO *data); //Receive data from Telemetry namager outgoing mail queue
    SendCommandsForPM(PIGO *data); //Send it off to the inbox for PathManager

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(readFromPathMode::getInstance());
    }
}

telemetryState& passToPathMode::getInstance()
{
    static passToPathMode singleton;
    return singleton;
}

void readFromPathMode::execute(telemetryManager* telemetryMgr)
{
    //read data out of path manager
    GetCommands(POGI *data)

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(analyzeDataMode::getInstance());
    }
}

telemetryState& readFromPathMode::getInstance()
{
    static readFromPathMode singleton;
    return singleton;
}

void analyzeDataMode::execute(telemetryManager* telemetryMgr)
{
    //set the dataValid here
    //not sure how to validate yet, guessing void function with dataValid flag to throw

    if(telemetryMgr ->dataValid)
    {
        //check for dataError, set it
    }

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(reportMode::getInstance());
    }
}

telemetryState& analyzeDataMode::getInstance()
{
    static analyzeDataMode singleton;
    return singleton;
}

void reportMode::execute(telemetryManager* telemetryMgr)
{
    //form report based on the the variables dataValid, dataError, and cycleCounter
    if(telemetryMgr -> dataValid)
    {
        telemetryMgr -> cycleCounter = 0;
        //detect if data contains error, load into bool dataError (error=true, no error=false)
        if(telemetryMgr -> dataError)
        {
            //form an error report
            telemetryMgr -> regularReport = false;
        }
        else
        {
            //form a regular report
            telemetryMgr -> regularReport = true;
        }
    }
    else
    {
        telemetryMgr -> cycleCounter++;
        if(telemetryMgr -> cycleCounter >= 5) //if data invalid more than 5 times, send error report
        {
            //form an error report
            telemetryMgr -> regularReport = false;
        }
        else
        {
            //form a regular report
            telemetryMgr -> regularReport = true;
        }
    }

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(encodeDataMode::getInstance());
    }
}

telemetryState& reportMode::getInstance()
{
    static reportMode singleton;
    return singleton;
}

void encodeDataMode::execute(telemetryManager* telemetryMgr)
{
    //encode data with mavlink (Jingting's function)

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(sendDataMode::getInstance());
    }
}

telemetryState& encodeDataMode::getInstance()
{
    static encodeDataMode singleton;
    return singleton;
}

void sendDataMode::execute(telemetryManager* telemetryMgr)
{
    //send data to ground
    //Not sure if this state is needed. From what I understand FREERTOS is calling the function to send down data.
    Send_Data();

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        if(telemetryMgr -> regularReport) //determines if the FSM should reinitialize
        {
            telemetryMgr -> setState(obtainDataMode::getInstance());
        }
        else
        {
            telemetryMgr -> setState(initialMode::getInstance());
        }
    }
}

telemetryState& sendDataMode::getInstance()
{
    static sendDataMode singleton;
    return singleton;
}

void failureMode::execute(telemetryManager* telemetryMgr)
{
    //State change:
    telemetryMgr -> setState(failureMode::getInstance());
}

telemetryState& failureMode::getInstance()
{
    static failureMode singleton;
    return singleton;
}
