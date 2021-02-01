#include "telemetryManager.hpp"
#include "telemetryStateClasses.hpp"

//initial state definition
telemetryManager::telemetryManager()
{
<<<<<<< Updated upstream:Autopilot/TelemetryManager/telemetryManager.cpp
    currentState=&initialMode::getInstance();
=======
    currentState = &initialMode::getInstance(); //initialize
>>>>>>> Stashed changes:TelemetryManager/telemetryManager.cpp
}

void telemetryManager::setState(telemetryState& newState)
{
    currentState -> exit(this);
    currentState = &newState;
    //detects and changes the status of the manager
    if(*currentState==obtainDataMode::getInstance())
    {
        status=COMPLETED_CYCLE;
    }
    else if(*currentState==failureMode::getInstance())
    {
        status=FAILURE_MODE;
    }
    else
    {
        status=IN_CYCLE;
    }
    currentState -> enter(this);
}

void telemetryManager::execute()
{
    currentState -> execute(this);
}

