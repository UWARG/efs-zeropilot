#include "attitudeManager.hpp"
#include "attitudeStateClasses.hpp"

attitudeManager::attitudeManager()
{
    currentState = &fetchInstructionsMode::getInstance();
    status = AttMan::COMPLETED_CYCLE;
}

void attitudeManager::setState(attitudeState& newState)
{
    currentState->exit(this);
    currentState = &newState;

    // Changes status variable so we can easily check if attitude manager has completed a cycle or not
    if (*currentState == fetchInstructionsMode::getInstance()) {
        status = AttMan::COMPLETED_CYCLE;
    } else if (*currentState == FatalFailureMode::getInstance()) {
        status = AttMan::COMPLETED_CYCLE;
    } else {
        status = AttMan::COMPLETED_CYCLE;
    }

    currentState->enter(this);
}

void attitudeManager::execute()
{
    currentState->execute(this);
}
