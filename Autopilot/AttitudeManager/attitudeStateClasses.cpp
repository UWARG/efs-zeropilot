#include "attitudeStateClasses.hpp"

#include "GetFromPathManager.hpp"
#include "SensorFusion.hpp"

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void fetchInstructionsMode::execute(attitudeManager* attitudeMgr)
{
    PMCommands Instructions;

    PMError_t ErrorStruct = PM_GetCommands(&Instructions);

    if (ErrorStruct.errorCode == 0)
    {
        attitudeMgr->setState(sensorFusionMode::getInstance());
    }
    else
    {
        attitudeMgr->setState(FatalFailureMode::getInstance());
    }
}

attitudeState& fetchInstructionsMode::getInstance()
{
    static fetchInstructionsMode singleton;
    return singleton;
}

void sensorFusionMode::execute(attitudeManager* attitudeMgr)
{
    SFOutput_t SFOutput;

    SFError_t ErrorStruct = SF_GetResult(&SFOutput);

    if (ErrorStruct.errorCode == 0)
    {
        attitudeMgr->setState(PIDloopMode::getInstance());
    }
    else
    {
        attitudeMgr->setState(FatalFailureMode::getInstance());
    }
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

void FatalFailureMode::execute(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(FatalFailureMode::getInstance());
}

attitudeState& FatalFailureMode::getInstance()
{
    static FatalFailureMode singleton;
    return singleton;
}
