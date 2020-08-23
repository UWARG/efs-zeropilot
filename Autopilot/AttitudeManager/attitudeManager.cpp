#include "attitudeManager.hpp"
#include "attitudeStateClasses.hpp"

attitudeManager::attitudeManager()
{
    currentState = &sensorInputMode::getInstance();
}

void attitudeManager::setState(attitudeState& newState)
{
    currentState->exit(this);
    currentState = &newState;
    currentState->enter(this);
}

void attitudeManager::toggle()
{
    currentState->toggle(this);
}
