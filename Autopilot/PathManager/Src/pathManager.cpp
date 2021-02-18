#include "pathManager.hpp"
#include "pathManagerStateClasses.hpp"

pathManager::pathManager()
{
    currentState = &fetchInstructionsMode::getInstance(); // CHANGE THIS ONCE U CHANGE THE INITIAL STATE!!!!!!!
    status = COMPLETED_CYCLE;
}

void pathManager::setState(pathManagerState& newState)
{
    currentState->exit(this);
    currentState = &newState;

    // Changes status variable so we can easily check if path manager has completed a cycle or not
    if (*currentState == fetchInstructionsMode::getInstance()) { // CHANGE THE INITIAL STATE!!!
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


