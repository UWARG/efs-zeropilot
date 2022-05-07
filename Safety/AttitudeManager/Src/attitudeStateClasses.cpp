#include "attitudeStateClasses.hpp"
#include "AttitudeDatatypes.hpp"
#include "Controls.hpp"
#include "PPM.hpp"
#include "main.h"
#include "safetyConfig.hpp"
#include "stm32f0xx_hal_gpio.h"
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
int fetchInstructionsMode::gimbalGrabberState;
Instructions_t *_ControlsInstructions = new Instructions_t();
CommandsForAM fetchInstructionsMode::_PMInstructions;
PPM_Instructions_t fetchInstructionsMode::_TeleopInstructions;
bool fetchInstructionsMode::_isAutonomous = false;
uint8_t fetchInstructionsMode::teleopTimeoutCount;
uint8_t fetchInstructionsMode::PMTimeoutCount;
uint8_t DisarmMode:: _armDisarmPPMValue;
uint8_t DisarmMode:: armDisarmTimeoutCount;
const uint8_t MIN_ARM_VALUE = 50; //Minimum PPM value for ARM instruction
const uint8_t GIMBAL_THRESHOLD = 33; // Maximum PPM value for drone to be in gimbal operations state
const uint8_t NO_GIMBAL_GRABBER_THRESHOLD = 66; // Maximum PPM value for drone to be in neither gimbal nor grabber (between 33 and 66)
const uint8_t GRABBER_THRESHOLD = 100; // Maximum PPM value for drone to be in grabber mode (higher than 66)

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
        // attitudeMgr->setState(DisarmMode::getInstance());
        // return;
    }

    // determine what actuator operation state the drone is (gimbal, grabber, or off)
    setGimbalGrabberState(updateGimbalGrabberState());

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

int fetchInstructionsMode::updateGimbalGrabberState()
{
    int retVal;
    if (_TeleopInstructions.PPMValues[GIMBAL_GRABBER_TOGGLE_INDEX] <= GIMBAL_THRESHOLD) {
        retVal = 0; // the drone will be able to operate gimbal only in this mode
    }
    else if (_TeleopInstructions.PPMValues[GIMBAL_GRABBER_TOGGLE_INDEX] > NO_GIMBAL_GRABBER_THRESHOLD) {
        retVal = 2; // the drone will be able to operate grabber only in this mode 
    }
    else {
        retVal = 1; // the drone won't be able to operate gimbal or grabber in this mode
    }
    return retVal;
}

void fetchInstructionsMode::setGimbalGrabberState(int toSet) {
    fetchInstructionsMode::gimbalGrabberState = toSet;
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

    PPM_Instructions_t *teleopInstructions = fetchInstructionsMode::GetTeleopInstructions();

    if (ErrorStruct.errorCode == 0)
    {
        // setting PWM channel values
        // attitudeMgr->pwm->set(FRONT_LEFT_MOTOR_CHANNEL, PidOutput -> frontLeftMotorPercent);
        // attitudeMgr->pwm->set(FRONT_RIGHT_MOTOR_CHANNEL, PidOutput -> frontRightMotorPercent);
        // attitudeMgr->pwm->set(BACK_LEFT_MOTOR_CHANNEL, PidOutput -> backLeftMotorPercent);
        // attitudeMgr->pwm->set(BACK_RIGHT_MOTOR_CHANNEL, PidOutput -> backRightMotorPercent);

        // if (fetchInstructionsMode::getGimbalGrabberState() == 0) 
        // {
        // // set gimbal position according to PPM values of sliders on left and right side (channel 6 and 7)
        //     // attitudeMgr->pwm->set(LEFT_GIMBAL, teleopInstructions->PPMValues[LEFT_GIMBAL_GRABBER_CRANE]);
        //     // attitudeMgr->pwm->set(RIGHT_GIMBAL,teleopInstructions->PPMValues[RIGHT_GIMBAL_GRABBER_MOUTH]);   
        // }

        // else if (fetchInstructionsMode::getGimbalGrabberState() == 2)
        // {
            // set grabber position according to PPM values of switches on transmitter and speed
            int craneState = 0;
            int grabberState = 0;
            if (teleopInstructions->PPMValues[LEFT_GIMBAL_GRABBER_CRANE] < 33) {
                craneState = -1; // crane lowering
            }
            else if (teleopInstructions->PPMValues[LEFT_GIMBAL_GRABBER_CRANE] <= 66) {
                craneState = 0; // crane braked
            }

            else if (teleopInstructions->PPMValues[LEFT_GIMBAL_GRABBER_CRANE] > 66) {
                craneState = 1; // crane raising
            }
            

            if (teleopInstructions->PPMValues[RIGHT_GIMBAL_GRABBER_MOUTH] < 33) {
                grabberState = -1; // grabber closing
            }
            else if (teleopInstructions->PPMValues[RIGHT_GIMBAL_GRABBER_MOUTH] <= 66) {
                grabberState = 0; // grabber braked
            }

            else if (teleopInstructions->PPMValues[RIGHT_GIMBAL_GRABBER_MOUTH] > 66) {
                grabberState = 1; // grabber opening
            }

            if (craneState == -1) {
                HAL_GPIO_WritePin(GRABBER_M2A_Port, GRABBER_Pin_9_M2A, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GRABBER_M2B_Port, GRABBER_Pin_5_M2B, GPIO_PIN_RESET);

                HAL_GPIO_WritePin(GRABBER_M1A_Port, GRABBER_Pin_11_M1A, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOC, GRABBER_Pin_12_M1B, GPIO_PIN_RESET);
            }
            
            else if (craneState == 0) {
                HAL_GPIO_WritePin(GRABBER_M2A_Port, GRABBER_Pin_9_M2A, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOC, GRABBER_Pin_5_M2B, GPIO_PIN_RESET);

                HAL_GPIO_WritePin(GRABBER_M1A_Port, GRABBER_Pin_11_M1A, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOC, GRABBER_Pin_12_M1B, GPIO_PIN_RESET);
            }

            else {
                HAL_GPIO_WritePin(GRABBER_M2A_Port, GRABBER_Pin_9_M2A, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GRABBER_M2B_Port, GRABBER_Pin_5_M2B, GPIO_PIN_SET);

                HAL_GPIO_WritePin(GRABBER_M1A_Port, GRABBER_Pin_11_M1A, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOC, GRABBER_Pin_12_M1B, GPIO_PIN_SET);
            }

            if (grabberState == -1) {

                HAL_GPIO_WritePin(CRANE_M1A_Port, CRANE_Pin_7_M1A, GPIO_PIN_SET);
                HAL_GPIO_WritePin(CRANE_M1B_Port, CRANE_Pin_8_M1B, GPIO_PIN_RESET);
            }

            else if (grabberState == 0) {

                HAL_GPIO_WritePin(CRANE_M1A_Port, CRANE_Pin_7_M1A, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(CRANE_M1B_Port, CRANE_Pin_8_M1B, GPIO_PIN_RESET);
            }

            else {
                HAL_GPIO_WritePin(CRANE_M1A_Port, CRANE_Pin_7_M1A, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(CRANE_M1B_Port, CRANE_Pin_8_M1B, GPIO_PIN_SET);
            }

        // }

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
    // attitudeMgr->pwm->set(FRONT_LEFT_MOTOR_CHANNEL, 0);
    // attitudeMgr->pwm->set(FRONT_RIGHT_MOTOR_CHANNEL, 0);
    // attitudeMgr->pwm->set(BACK_LEFT_MOTOR_CHANNEL, 0);
    // attitudeMgr->pwm->set(BACK_RIGHT_MOTOR_CHANNEL, 0);
}

attitudeState& FatalFailureMode::getInstance()
{
    static FatalFailureMode singleton;
    return singleton;
}

void DisarmMode:: execute(attitudeManager* attitudeMgr)
{
    attitudeMgr->setState(fetchInstructionsMode::getInstance());
    return;

    //setting PWM channel values to lowest "disarm" value
    // attitudeMgr->pwm->set(FRONT_LEFT_MOTOR_CHANNEL, 0);
    // attitudeMgr->pwm->set(FRONT_RIGHT_MOTOR_CHANNEL, 0);
    // attitudeMgr->pwm->set(BACK_LEFT_MOTOR_CHANNEL, 0);
    // attitudeMgr->pwm->set(BACK_RIGHT_MOTOR_CHANNEL, 0);

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
    else// if (isArmed())
    {   
        attitudeMgr->setState(fetchInstructionsMode::getInstance());
    }
    // else
    // {
        //Do nothing, stay in this state
        //attitudeMgr->setState(DisarmMode::getInstance());
    // }

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