#include "attitudeStateClasses.hpp"

void fetchInstructionsMode::execute(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(sensorFusionMode::getInstance());
}

attitudeState& fetchInstructionsMode::getInstance()
{
    static fetchInstructionsMode singleton;
    return singleton;
}

void sensorFusionMode::execute(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(PIDloopMode::getInstance());
}

attitudeState& sensorFusionMode::getInstance()
{
    static sensorFusionMode singleton;
    return singleton;
}

void PIDloopMode::execute(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(OutputMixingMode::getInstance());
}

attitudeState& PIDloopMode::getInstance()
{
    static PIDloopMode singleton;
    return singleton;
}

void OutputMixingMode::execute(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(sendToSafetyMode::getInstance());
}

attitudeState& OutputMixingMode::getInstance()
{
    static OutputMixingMode singleton;
    return singleton;
}

void sendToSafetyMode::execute(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(fetchInstructionsMode::getInstance());
}

attitudeState& sendToSafetyMode::getInstance()
{
    static sendToSafetyMode singleton;
    return singleton;
}
