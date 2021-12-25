#include "attitudeStateClasses.hpp"
#include "RSSI.hpp"
#include "PPM.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

float OutputMixingMode::_channelOut[4];
SFOutput_t sensorFusionMode::_SFOutput;
PID_Output_t PIDloopMode::_PidOutput;
PWMChannel pwm; 

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

//Populate instruction data and decide between manual and auto flight modes
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
    const uint8_t TIMEOUT_THRESHOLD = 2; //Max cycles without data until connection is considered broken
    _isAutonomous = false;

    //Note: GetFromTeleop and GetFromPM should leave their corresponding instructions unchanged and return false when they fail
    
    if(ReceiveTeleopInstructions())
    {
        teleopTimeoutCount = 0;
    }
    else
    {
        if(teleopTimeoutCount < TIMEOUT_THRESHOLD)
            teleopTimeoutCount++;
    }

    //TODO: Determine if RC is commanding to go autonomous
    bool isTeleopCommandingAuto = false;
    
    if(!isTeleopCommandingAuto || GetFromPM(&_PMInstructions))
    {
        PMTimeoutCount = 0;
    }
    else
    {
        if(PMTimeoutCount < TIMEOUT_THRESHOLD)
            PMTimeoutCount++;
    }
    
    if(teleopTimeoutCount < TIMEOUT_THRESHOLD && !CommsFailed())
    {
        _isAutonomous = (isTeleopCommandingAuto && PMTimeoutCount < TIMEOUT_THRESHOLD);
    }
    else
    {   
        //Abort due to teleop failure
        attitudeMgr->setState(FatalFailureMode::getInstance());
        return;
    }

    // The support is also here for sending stuff to Path manager, but there's nothing I need to send atm.
    attitudeMgr->setState(sensorFusionMode::getInstance());
}

attitudeState& fetchInstructionsMode::getInstance()
{
    static fetchInstructionsMode singleton;
    return singleton;
}

bool fetchInstructionsMode::ReceiveTeleopInstructions(void)
{
    if(PPMChannel::is_disconnected(HAL_GetTick()))
    {
        return false;
    }
    
    for(int i = 0; i < MAX_PPM_CHANNELS; i++)
    {
        _TeleopInstructions.PPMValues[i] = PPMChannel::get(i);
    }
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
    CommandsForAM *PMInstructions = nullptr;
    SFOutput_t *SFOutput = sensorFusionMode::GetSFOutput();

    PID_Output_t *pidOut = nullptr;
    if(fetchInstructionsMode::isAutonomous())
    {
        PMInstructions = fetchInstructionsMode::GetPMInstructions();
        //TODO: Run controls module with PMInstructions
    }
    else
    {
        PPM_Instructions_t *teleopInstructions = fetchInstructionsMode::GetTeleopInstructions();
        _PidOutput = getPIDFromControls(teleopInstructions, SFOutput);
    }

    #ifdef FIXED_WING
    PMInstructions = fetchInstructionsMode::GetPMInstructions();
    
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
    #endif

    attitudeMgr->setState(OutputMixingMode::getInstance());

}

attitudeState& PIDloopMode::getInstance()
{
    static PIDloopMode singleton;
    return singleton;
}

void OutputMixingMode::execute(attitudeManager* attitudeMgr)
{
    // *PIDOutput an array of four values corresponding to motor percentages for the 
    PID_Output_t *PidOutput = PIDloopMode::GetPidOutput(); 

    OutputMixing_error_t ErrorStruct = OutputMixing_Execute(PidOutput, _channelOut);

    if (ErrorStruct.errorCode == 0)
    {
        // setting PWM channel values
        pwm.set(0, PID_Output_t -> frontLeftPercent);
        pwm.set(1, PID_Output_t -> frontRightPercent);
        pwm.set(2, PID_Output_t -> backLeftPercent);
        pwm.set(3, PID_Output_t -> backRightPercent);
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

