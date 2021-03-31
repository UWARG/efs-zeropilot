#include "attitudeStateClasses.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

float OutputMixingMode::_channelOut[4];
PMCommands fetchInstructionsMode::_PMInstructions;
SFOutput_t sensorFusionMode::_SFOutput;
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
    SFError_t _SFError = SF_GetResult(&_SFOutput);

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
    SFOutput_t *SFOutput = sensorFusionMode::GetSFOutput();

    // Gets roll, pitch, rudder, and throttle commands from the path manager module
    PMCommands pathManagerOutput;
    PMError_t pmError = PM_GetCommands(&pathManagerOutput);

    //executes PID's to acheive desired roll, pitch angle
    //if manual control is needed, use loaded in percents instead!
    if(pathManagerOutput.passbyData.pitchPassby)
    {
        _PidOutput.pitchPercent = pathManagerOutput.passbyData.pitchPercent;
    }
    else
    {
        _PidOutput.pitchPercent = _pitchPid.execute(PMInstructions->pitch, SFOutput->pitch, SFOutput->pitchRate);
    }

    if(pathManagerOutput.passbyData.rollPassby)
    {
        _PidOutput.rollPercent = pathManagerOutput.passbyData.rollPercent;
    }
    else
    {
        _PidOutput.rollPercent = _rollPid.execute(PMInstructions->roll, SFOutput->roll, SFOutput->rollRate);
    }

    if(pathManagerOutput.passbyData.rudderPassby)
    {
        _PidOutput.rudderPercent = pathManagerOutput.passbyData.rudderPercent;
    }
    else
    {
        _PidOutput.rudderPercent = pathManagerOutput.rudderPercent;
    }

    if(pathManagerOutput.passbyData.throttlePassby)
    {
        _PidOutput.throttlePercent = pathManagerOutput.passbyData.throttlePercent;
    } 
    else
    {
        _PidOutput.throttlePercent = pathManagerOutput.throttlePercent;
    }

    //if manual control is needed, use loaded in percents instead!
    if(pathManagerOutput.passbyData.pitchPassby)
    {
        _PidOutput.pitchPercent = pathManagerOutput.passbyData.pitchPercent;
    } 
    if(pathManagerOutput.passbyData.rollPassby)
    {
        _PidOutput.rollPercent = pathManagerOutput.passbyData.rollPercent;
    } 
    if(pathManagerOutput.passbyData.rudderPassby)
    {
        _PidOutput.rudderPercent = pathManagerOutput.passbyData.rudderPercent;
    } 
    if(pathManagerOutput.passbyData.throttlePassby)
    {
        _PidOutput.throttlePercent = pathManagerOutput.passbyData.throttlePercent;
    } 

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
