#include "attitudeStateClasses.hpp"

void sensorInputMode::toggle(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(PIDloopMode::getInstance());
}

attitudeState& sensorInputMode::getInstance()
{
    static sensorInputMode singleton;
    return singleton;
}

void PIDloopMode::toggle(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(sendToSafetyMode::getInstance());
}

attitudeState& PIDloopMode::getInstance()
{
    static PIDloopMode singleton;
    return singleton;
}

void sendToSafetyMode::toggle(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(sensorInputMode::getInstance());
}

attitudeState& sendToSafetyMode::getInstance()
{
    static sendToSafetyMode singleton;
    return singleton;
}