#include "telemetryManager.hpp"
#include "telemetryStateClasses.hpp"

//initial state definition
telemetryManager::telemetryManager()
{
    currentState=&initialMode::getInstance();
}

void telemetryManager::setState(telemetryState& newState)
{
    currentState->exit(this);
    currentState=&newState;
    currentState->enter(this);
}

void telemetryManager::execute()
{
    currentState->execute(this);
}

