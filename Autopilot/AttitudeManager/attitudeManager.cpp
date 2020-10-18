#include "attitudeManager.hpp"
#include "attitudeStateClasses.hpp"

attitudeManager::attitudeManager()
{
    currentState = &fetchInstructionsMode::getInstance();
}

void attitudeManager::setState(attitudeState& newState)
{
    currentState->exit(this);
    currentState = &newState;
    currentState->enter(this);
}

void attitudeManager::execute()
{
    currentState->execute(this);
}
