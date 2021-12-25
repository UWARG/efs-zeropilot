#include "attitudeStateClasses.hpp"
#include "PWM.hpp"
#include "PWMChannel.hpp"
#include "safetyConfig.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

float OutputMixingMode::_channelOut[4];
bool decisionModuleMode::_autonomous;
CommandsForAM fetchInstructionsMode::_MovementInstructions;
SFOutput_t sensorFusionMode::_SFOutput;
PID_Output_t PIDloopMode::_PidOutput;
PWMChannel pwm; 

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void pwmSetup::execute(attitudeManager* attitudeMgr) 
{
    pwm.setup(); // setup PWM channel, only done once

    // set state to fetchInstructionsMode, this state will not be set again unless the system is restarted
    attitudeMgr -> setState(fetchInstructionsMode::getInstance());
}

attitudeState& pwmSetup::getInstance()
{
    static pwmSetup singleton;
    return singleton;
}

void fetchInstructionsMode::execute(attitudeManager* attitudeMgr)
{

    bool autonomous = decisionModuleMode::getAutonomousMode();

    if (autonomous) {
        GetFromPMToAM(&_MovementInstructions); // data sent from path manager
    }

    else {
        GetFromTeleop(&_MovementInstructions); // data from RC transmitter
    }

    // The support is also here for sending stuff to Path manager, but there's nothing I need to send atm.
    attitudeMgr->setState(sensorFusionMode::getInstance());
}

attitudeState& fetchInstructionsMode::getInstance()
{
    static fetchInstructionsMode singleton;
    return singleton;
}

void sensorFusionMode::execute(attitudeManager* attitudeMgr)
{
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

    CommandsForAM *PMInstructions = fetchInstructionsMode::GetPMInstructions();
    SFOutput_t *SFOutput = sensorFusionMode::GetSFOutput();

    //executes PID's to acheive desired roll, pitch angle
    //if manual control is needed, use loaded in percents instead!
    if(PMInstructions->passbyData.pitchPassby)
    {
        _PidOutput.pitchPercent = PMInstructions->passbyData.pitchPercent;
    }
    else
    {
        _PidOutput.pitchPercent = _pitchPid.execute(PMInstructions->pitch, SFOutput->pitch, SFOutput->pitchRate);
    }

    if(PMInstructions->passbyData.rollPassby)
    {
        _PidOutput.rollPercent = PMInstructions->passbyData.rollPercent;
    }
    else
    {
        _PidOutput.rollPercent = _rollPid.execute(PMInstructions->roll, SFOutput->roll, SFOutput->rollRate);
    }

    if(PMInstructions->passbyData.rudderPassby)
    {
        _PidOutput.rudderPercent = PMInstructions->passbyData.rudderPercent;
    }
    else
    {
        _PidOutput.rudderPercent = PMInstructions->rudderPercent;
    }

    if(PMInstructions->passbyData.throttlePassby)
    {
        _PidOutput.throttlePercent = PMInstructions->passbyData.throttlePercent;
    }
    else
    {
        _PidOutput.throttlePercent = PMInstructions->throttlePercent;
    }

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
        // setting PWM channel values
        pwm.set(FRONT_LEFT_MOTOR_CHANNEL, PID_Output_t -> frontLeftPercent);
        pwm.set(FRONT_RIGHT_MOTOR_CHANNEL, PID_Output_t -> frontRightPercent);
        pwm.set(BACK_LEFT_MOTOR_CHANNEL, PID_Output_t -> backLeftPercent);
        pwm.set(BACK_RIGHT_MOTOR_CHANNEL, PID_Output_t -> backRightPercent);
        attitudeMgr->setState(fetchInstructionsMode::getInstance()); // returning to beginning of state machine
    }o
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

void FatalFailureMode::execute(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(FatalFailureMode::getInstance());
}

attitudeState& FatalFailureMode::getInstance()
{
    static FatalFailureMode singleton;
    return singleton;
}

