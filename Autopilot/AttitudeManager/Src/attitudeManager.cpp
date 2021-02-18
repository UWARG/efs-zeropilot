#include "attitudeManager.hpp"
#include "attitudeStateClasses.hpp"

attitudeManager::attitudeManager()
{
    currentState = &fetchInstructionsMode::getInstance();
    status = COMPLETED_CYCLE;
}

void attitudeManager::setState(attitudeState& newState)
{
    currentState->exit(this);
    currentState = &newState;

    // Changes status variable so we can easily check if attitude manager has completed a cycle or not
    if (*currentState == fetchInstructionsMode::getInstance()) {
        status = COMPLETED_CYCLE;
    } else if (*currentState == FatalFailureMode::getInstance()) {
        status = FAILURE_MODE;
    } else {
        status = IN_CYCLE;
    }

    currentState->enter(this);
}

void attitudeManager::execute()
{
    currentState->execute(this);
}
