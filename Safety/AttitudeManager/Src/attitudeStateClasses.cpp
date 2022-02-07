#include "attitudeStateClasses.hpp"
#include "Controls.hpp"
#include "PWM.hpp"
#include "safetyConfig.hpp"
#include "RSSI.hpp"
#include "PID.hpp"
#include "PPM.hpp"
// #include "CommFromPMToAM.hpp"
#include "../../boardfiles/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal.h"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

float OutputMixingMode::_channelOut[4];
SFOutput_t sensorFusionMode::_SFOutput;
PID_Output_t *PIDloopMode::_PidOutput;
Instructions_t *_ControlsInstructions;
PWMChannel pwm; 
PPMChannel ppm;
CommandsForAM fetchInstructionsMode::_PMInstructions;
PPM_Instructions_t fetchInstructionsMode::_TeleopInstructions;
bool fetchInstructionsMode::_isAutonomous = false;
uint8_t fetchInstructionsMode::teleopTimeoutCount;
uint8_t fetchInstructionsMode::PMTimeoutCount;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void pwmSetup::execute(attitudeManager* attitudeMgr) 
{
    pwm.setup(); // setup PWM channel, only done once
    ppm.setNumChannels(MAX_PPM_CHANNELS); // setup PPM channel, only done once

    // set state to fetchInstructionsMode, this state will not be set again unless the system is restarted
    attitudeMgr -> setState(fetchInstructionsMode::getInstance());
}

attitudeState& pwmSetup::getInstance()
{
    static pwmSetup singleton;
    return singleton;
}


//Populate instruction data and decide between manual and auto flight modes

void fetchInstructionsMode::execute(attitudeManager* attitudeMgr)
{    
    const uint8_t TIMEOUT_THRESHOLD = 2; //Max cycles without data until connection is considered broken
    fetchInstructionsMode::_isAutonomous = false; // assumed to be in teleop

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
    
    // PM will timeout if it fails to report data while autonomous mode is triggered
    if(!isTeleopCommandingAuto /* || GetFromPMToAM(&_PMInstructions)*/) 
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
        fetchInstructionsMode::_isAutonomous = (isTeleopCommandingAuto && PMTimeoutCount < TIMEOUT_THRESHOLD);
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
    if(ppm.is_disconnected(HAL_GetTick()))
    {
        return false;
    }
    
    for(int i = 0; i < MAX_PPM_CHANNELS; i++)
    {
        _TeleopInstructions.PPMValues[i] = ppm.get(i);
        return true;
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
        _ControlsInstructions -> input1 = teleopInstructions->PPMValues[0];
        _ControlsInstructions -> input2 = teleopInstructions->PPMValues[1];
        _ControlsInstructions -> input3 = teleopInstructions->PPMValues[2];
        _ControlsInstructions -> input4 = teleopInstructions->PPMValues[3];
        _PidOutput = runControlsAndGetPWM(_ControlsInstructions, SFOutput);
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
    #endif // endif FIXED_WING

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
        pwm.set(FRONT_LEFT_MOTOR_CHANNEL, PidOutput -> frontLeftMotorPercent);
        pwm.set(FRONT_RIGHT_MOTOR_CHANNEL, PidOutput -> frontRightMotorPercent);
        pwm.set(BACK_LEFT_MOTOR_CHANNEL, PidOutput -> backLeftMotorPercent);
        pwm.set(BACK_RIGHT_MOTOR_CHANNEL, PidOutput -> backRightMotorPercent);
        attitudeMgr->setState(fetchInstructionsMode::getInstance()); // returning to beginning of state machine
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

void FatalFailureMode::execute(attitudeManager* attitudeMgr)
{
    //setting PWM channel values to 0 as per CONOPS guidelines for multicopters
    pwm.set(FRONT_LEFT_MOTOR_CHANNEL, 0);
    pwm.set(FRONT_RIGHT_MOTOR_CHANNEL, 0);
    pwm.set(BACK_LEFT_MOTOR_CHANNEL, 0);
    pwm.set(BACK_RIGHT_MOTOR_CHANNEL, 0);
}

attitudeState& FatalFailureMode::getInstance()
{
    static FatalFailureMode singleton;
    return singleton;
}

