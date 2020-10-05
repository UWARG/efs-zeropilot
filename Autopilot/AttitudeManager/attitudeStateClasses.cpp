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
    SFError_t ErrorStruct = SF_GetResult(&_SFOutput, &ImuSens, &AirspeedSens);

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

    PMCommands *PMInstructions = fetchInstructionsMode::GetPMInstructions();
    SFOutput_t *SFOutput = sensorFusionMode::GetSFOutput();

    // TODO the IMU also gives us the measured derivative of roll, pitch, and yaw. That should be used in the PID algorithm
    _PidOutput.rollPercent = _rollPid.PIDControl(PMInstructions->roll - SFOutput->IMUroll);
    _PidOutput.pitchPercent = _pitchPid.PIDControl(PMInstructions->pitch - SFOutput->IMUpitch);
    _PidOutput.yawPercent = _yawPid.PIDControl(PMInstructions->yaw - SFOutput->IMUyaw);
    _PidOutput.throttlePercent = _airspeedPid.PIDControl(PMInstructions->airspeed - SFOutput->Airspeed);

    _PidOutput.rollPercent = 0;  // TODO Pid needs fixing
    _PidOutput.pitchPercent = 0;
    _PidOutput.yawPercent = 0;
    _PidOutput.throttlePercent = 50;

    attitudeMgr->setState(OutputMixingMode::getInstance());
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
    float *channelOut = OutputMixingMode::GetChannelOut();

    SendToSafety_error_t ErrorStruct = SendToSafety_Execute(channelOut);

    if (ErrorStruct.errorCode == 0)
    {
        attitudeMgr->setState(fetchInstructionsMode::getInstance());
    }
    else
    {
        attitudeMgr->setState(FatalFailureMode::getInstance());
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
