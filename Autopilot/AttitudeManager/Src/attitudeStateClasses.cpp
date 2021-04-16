#include "attitudeStateClasses.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

float OutputMixingMode::_channelOut[4];
PMCommands fetchInstructionsMode::_PMInstructions;
SFAttitudeOutput_t sensorFusionMode::_SFOutput;
PID_Output_t PIDloopMode::_PidOutput;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void fetchInstructionsMode::execute(attitudeManager* attitudeMgr)
{

    PMError_t ErrorStruct = PM_GetCommands(&_PMInstructions);

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
    SFOutput_t _SFOutput;
    SF_GetResult(&_SFOutput);

    attitudeMgr->setState(PIDloopMode::getInstance());
}

attitudeState& sensorFusionMode::getInstance()
{
    static sensorFusionMode singleton;
    return singleton;
}

void PIDloopMode::execute(attitudeManager* attitudeMgr)
{

    PMCommands *PMInstructions = fetchInstructionsMode::GetPMInstructions();
    SFAttitudeOutput_t *SFOutput = sensorFusionMode::GetSFOutput();

    // Gets roll, pitch, rudder, and throttle commands from the path manager module
    PMCommands pathManagerOutput;
    PMError_t pmError = PM_GetCommands(&pathManagerOutput);

    _PidOutput.rollPercent = _rollPid.execute(PMInstructions->roll, SFOutput->IMUroll, SFOutput->IMUrollrate);
    _PidOutput.pitchPercent = _pitchPid.execute(PMInstructions->pitch, SFOutput->IMUpitch, SFOutput->IMUpitchrate);
    _PidOutput.rudderPercent = pathManagerOutput.rudderPercent;
    _PidOutput.throttlePercent = pathManagerOutput.throttlePercent;

    if (pmError.errorCode == 0)
    {
        attitudeMgr->setState(OutputMixingMode::getInstance());
    }
    else
    {
        attitudeMgr->setState(FatalFailureMode::getInstance());
    }
}

attitudeState& PIDloopMode::getInstance()
{
    static PIDloopMode singleton;
    return singleton;
}

void OutputMixingMode::execute(attitudeManager* attitudeMgr)
{
    PID_Output_t *PidOutput = PIDloopMode::GetPidOutput();

    OutputMixing_error_t ErrorStruct = OutputMixing_Execute(PidOutput, _channelOut);

    if (ErrorStruct.errorCode == 0)
    {
        attitudeMgr->setState(sendToSafetyMode::getInstance());
    }
    else
    {
        attitudeMgr->setState(FatalFailureMode::getInstance());
    }

}

attitudeState& OutputMixingMode::getInstance()
{
    static OutputMixingMode singleton;
    return singleton;
}

void sendToSafetyMode::execute(attitudeManager* attitudeMgr)
{
    SendToSafety_error_t ErrorStruct;
    float *channelOut = OutputMixingMode::GetChannelOut();
    for(int channel = 0; channel < 4; channel++)
    {
        ErrorStruct = SendToSafety_Execute(channel, channelOut[channel]);
        if(ErrorStruct.errorCode == 1)
        {
            attitudeMgr->setState(FatalFailureMode::getInstance());
            break;
        }
    }

    if (ErrorStruct.errorCode == 0)
    {
        attitudeMgr->setState(fetchInstructionsMode::getInstance());
    }

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
