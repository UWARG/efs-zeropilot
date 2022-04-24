#include "attitudeStateClasses.hpp"
#include "Controls.hpp"
#include "safetyConfig.hpp"
#include "RSSI.hpp"
#include "PID.hpp"
// #include "CommFromPMToAM.hpp"
#include "../../boardfiles/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal.h"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

float OutputMixingMode::_channelOut[4];
SFOutput_t sensorFusionMode::_SFOutput;
PID_Output_t *PIDloopMode::_PidOutput;
Instructions_t *_ControlsInstructions = new Instructions_t();
CommandsForAM fetchInstructionsMode::_PMInstructions;
PPM_Instructions_t fetchInstructionsMode::_TeleopInstructions;
bool fetchInstructionsMode::_isAutonomous = false;
uint8_t fetchInstructionsMode::teleopTimeoutCount;
uint8_t fetchInstructionsMode::PMTimeoutCount;
uint8_t DisarmMode:: _armDisarmPPMValue;
uint8_t DisarmMode:: armDisarmTimeoutCount;
const uint8_t MIN_ARM_VALUE = 50; //Minimum PPM value for ARM instruction

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

//Populate instruction data and decide between manual and auto flight modes

void fetchInstructionsMode::execute(attitudeManager* attitudeMgr)
{    
    const uint8_t TIMEOUT_THRESHOLD = 2; //Max cycles without data until connection is considered broken
    fetchInstructionsMode::_isAutonomous = false; // assumed to be in teleop

    //Note: GetFromTeleop and GetFromPM should leave their corresponding instructions unchanged and return false when they fail
    
    if(ReceiveTeleopInstructions(attitudeMgr))
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

    //Check if Disarm instruction was sent
    if (!isArmed())
    {
        attitudeMgr->setState(DisarmMode::getInstance());
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

bool fetchInstructionsMode::ReceiveTeleopInstructions(attitudeManager* attitudeMgr)
{
    if(attitudeMgr->ppm->is_disconnected(HAL_GetTick()))
    {
        return false;
    }
    
    for(int i = 0; i < MAX_PPM_CHANNELS; i++)
    {
        _TeleopInstructions.PPMValues[i] = attitudeMgr->ppm->get(i);
    }
    return true;
}

bool fetchInstructionsMode:: isArmed()
{
    bool retVal = false;

    if (_TeleopInstructions.PPMValues[ARM_DISARM_CHANNEL_INDEX] >= MIN_ARM_VALUE)
    {
        retVal = true;
    }
    
    return retVal;
}

void sensorFusionMode::execute(attitudeManager* attitudeMgr)
{
    SFError_t _SFError = SF_GenerateNewResult();
    _SFError = SF_GetResult(&_SFOutput);

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
        attitudeMgr->pwm->set(FRONT_LEFT_MOTOR_CHANNEL, PidOutput -> frontLeftMotorPercent);
        attitudeMgr->pwm->set(FRONT_RIGHT_MOTOR_CHANNEL, PidOutput -> frontRightMotorPercent);
        attitudeMgr->pwm->set(BACK_LEFT_MOTOR_CHANNEL, PidOutput -> backLeftMotorPercent);
        attitudeMgr->pwm->set(BACK_RIGHT_MOTOR_CHANNEL, PidOutput -> backRightMotorPercent);
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
    attitudeMgr->pwm->set(FRONT_LEFT_MOTOR_CHANNEL, 0);
    attitudeMgr->pwm->set(FRONT_RIGHT_MOTOR_CHANNEL, 0);
    attitudeMgr->pwm->set(BACK_LEFT_MOTOR_CHANNEL, 0);
    attitudeMgr->pwm->set(BACK_RIGHT_MOTOR_CHANNEL, 0);
}

attitudeState& FatalFailureMode::getInstance()
{
    static FatalFailureMode singleton;
    return singleton;
}

void DisarmMode:: execute(attitudeManager* attitudeMgr)
{
    //setting PWM channel values to lowest "disarm" value
    attitudeMgr->pwm->set(FRONT_LEFT_MOTOR_CHANNEL, 0);
    attitudeMgr->pwm->set(FRONT_RIGHT_MOTOR_CHANNEL, 0);
    attitudeMgr->pwm->set(BACK_LEFT_MOTOR_CHANNEL, 0);
    attitudeMgr->pwm->set(BACK_RIGHT_MOTOR_CHANNEL, 0);

    const uint8_t TIMEOUT_THRESHOLD = 2; //Max cycles without data until connection is considered broken

    //Get Arm Disarm instruction
    if(ReceiveArmDisarmInstruction(attitudeMgr))
    {
        armDisarmTimeoutCount = 0;
    }
    else
    {
        if(armDisarmTimeoutCount < TIMEOUT_THRESHOLD)
            armDisarmTimeoutCount++;
    }

    /*
        3 possibilities:
             1. Go into FatalFailureMode bec of timeout
             2. Go into fetchInstructionsMode bec "Arm" instruction was sent
             3. Do nothing, stay in the disarm state
    */
    if(armDisarmTimeoutCount > TIMEOUT_THRESHOLD && CommsFailed())
    {
        //Abort due to timeout failures
        attitudeMgr->setState(FatalFailureMode::getInstance());
        return;
    }
    else if (isArmed())
    {   
        attitudeMgr->setState(fetchInstructionsMode::getInstance());
    }
    else
    {
        //Do nothing, stay in this state
        //attitudeMgr->setState(DisarmMode::getInstance());
    }

}

attitudeState& DisarmMode:: getInstance()
{
    static DisarmMode singleton;
    return singleton;
}

bool DisarmMode:: ReceiveArmDisarmInstruction(attitudeManager* attitudeMgr)
{
    bool retVal = true;
    if(attitudeMgr->ppm->is_disconnected(HAL_GetTick()))
    {
        retVal = false;
    }
    
    _armDisarmPPMValue = attitudeMgr->ppm->get(ARM_DISARM_CHANNEL_INDEX);

    return retVal;
}

bool DisarmMode:: isArmed()
{
    bool retVal = false;
    
    if (_armDisarmPPMValue >= MIN_ARM_VALUE)
    {
        retVal = true;
    }

    return retVal;
}