#include "pathManager.hpp"
#include "pathStateClasses.hpp"

pathManager::pathManager()
{
    currentState = &commsWithAttitude::getInstance(); 
    status = COMPLETED_CYCLE;
}

void pathManager::setState(pathManagerState& newState)
{
    currentState->exit(this);
    currentState = &newState;

    if (*currentState == commsWithAttitude::getInstance()) { 
        status = COMPLETED_CYCLE;
    } else if (*currentState == FatalFailureMode::getInstance()) {
        status = FAILURE_MODE;
    } else {
        status = IN_CYCLE;
    }

    currentState->enter(this);
}

void pathManager::execute()
{
    currentState->execute(this);
}


