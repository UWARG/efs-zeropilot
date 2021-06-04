/*
* Author: Nixon Chan
*/

#include "telemetryManager.hpp"
#include "telemetryStateClasses.hpp"

//initial state definition
telemetryManager::telemetryManager()
{
    currentState = &initialMode::getInstance(); //initialize
}

void telemetryManager::setState(telemetryState& newState) //set state function
{
    currentState -> exit(this);
    currentState = &newState;
    //detects and changes the status of the manager
    if(*currentState==obtainDataMode::getInstance())
    {
        status=telemMan::COMPLETED_CYCLE;
    }
    else if(*currentState==failureMode::getInstance())
    {
        status=telemMan::FAILURE_MODE;
    }
    else
    {
        status=telemMan::IN_CYCLE;
    }
    currentState -> enter(this);
}

void telemetryManager::execute() //execute actions within the state class, defined in stateclasses.cpp
{
    currentState -> execute(this);
}

