/**
 * @file   Spike.c
 * @author Chris Hajduk
 * @date July 2, 2015
 * @copyright Waterloo Aerial Robotics Group 2017 \n
 *   https://raw.githubusercontent.com/UWARG/PICpilot/master/LICENCE
 */

#include "Spike.h"
#include "Attitude.h"
#include "Common.h"
#include "PID.h"
#include "OrientationControl.h"
#include "Sensors.h"
#include "Interchip_A.h"
#include "ProgramStatus.h"

#if VEHICLE_TYPE == FIXED_WING

#define AIRSPEED 0

static int16_t outputSignal[PWM_NUM_CHANNELS];
static int16_t control_Roll, control_Pitch, control_Yaw, control_Throttle;

int16_t input_RC_Flap;
int16_t input_GS_Flap;

float adverse_yaw_mix = 0.5; // Roll rate -> yaw rate scaling (to counter adverse yaw)
float roll_turn_mix = 1.0; // Roll angle -> pitch rate scaling (for banked turns) 

/* Private Function Prototypes */
void setDefaultPWM();
void checkLimits(int16_t* channelOut);
void inputMixing(int16_t* channelIn, int16_t* rollRate, int16_t* pitchRate, int16_t* throttle, int16_t* yawRate);
void outputMixing(int16_t* channelOut, int16_t* control_Roll, int16_t* control_Pitch, int16_t* control_Throttle, int16_t* control_Yaw);

/* Public Functions */
void Vehicle_Initialization(){
    setDefaultPWM();
    setProgramStatus(UNARMED);
}

void Vehicle_Arm(){
    setProgramStatus(ARMING); //TODO: Needed?

    //resetHeartbeatTimer();

    setProgramStatus(MAIN_EXECUTION);
}

void Vehicle_Dearm(){
    setProgramStatus(UNARMED);
    setDefaultPWM();
}

void Vehicle_GetInputRC(int16_t* roll, int16_t* pitch, int16_t* throttle, int16_t* yaw){
  inputMixing(Interchip_GetPWM(), roll, pitch, throttle, yaw);
}

void Vehicle_HighLevelControl(){

    if (getControlValue(ALTITUDE_CONTROL) == CONTROL_ON) {
        setAltitudeSetpoint(getAltitudeInput(getControlValue(ALTITUDE_CONTROL_SOURCE)));
        setPitchAngleSetpoint(PIDcontrol(getPID(ALTITUDE), getAltitudeSetpoint() - Sensors_getAltitude(), 1));
#if !AIRSPEED
        setThrottleSetpoint(PIDcontrol(getPID(ALTITUDE), getAltitudeSetpoint() - Sensors_getAltitude(), HALF_PWM_RANGE / 2) + getThrottleSetpoint());
#endif
    } else {
        setPitchAngleSetpoint(getPitchAngleInput(getControlValue(PITCH_CONTROL_SOURCE)));
        setThrottleSetpoint(getThrottleInput(getControlValue(THROTTLE_CONTROL_SOURCE)));
    }

#if AIRSPEED // TODO: airspeed control. Need to verify sensor values.
    if (getControlValue(AIRSPEED_CONTROL) == CONTROL_ON) {
        setAirspeedSetpoint(gs_airspeed); // let the ground station always determine airspeed
        setThrottleSetpoint(PIDcontrol(getPID(AIRSPEED), getAirspeedSetpoint() - getAirspeed()), HALF_PWM_RANGE / 20);
        setFlaps(flaps_scale / getAirspeed()); // low speed -> more flaps
    }
#endif

    if (getControlValue(HEADING_CONTROL) == CONTROL_ON) {
        setHeadingSetpoint(getHeadingInput(getControlValue(HEADING_CONTROL_SOURCE)));
        setRollAngleSetpoint(PIDcontrol(getPID(HEADING), wrap_180(getHeadingSetpoint() - Sensors_getHeading()), 1));
    } else {
        setRollAngleSetpoint(getRollAngleInput(getControlValue(ROLL_CONTROL_SOURCE)));
    }
}

void Vehicle_LowLevelControl(){
    if (getControlValue(ROLL_CONTROL_TYPE) == ANGLE_CONTROL || getControlValue(HEADING_CONTROL) == CONTROL_ON) {
        setRollRateSetpoint(PIDcontrol(getPID(ROLL_ANGLE), getRollAngleSetpoint() - Sensors_getRoll(), MAX_ROLL_RATE / MAX_ROLL_ANGLE));
    } else {
        setRollRateSetpoint(getRollRateInput(getControlValue(ROLL_CONTROL_SOURCE)));
        setYawRateSetpoint(getYawRateInput(getControlValue(ROLL_CONTROL_SOURCE))); // No bit for yaw. If they have roll, they probably need yaw too.
    }

    if (getControlValue(PITCH_CONTROL_TYPE) == ANGLE_CONTROL || getControlValue(ALTITUDE_CONTROL) == CONTROL_ON){
        setPitchRateSetpoint(PIDcontrol(getPID(PITCH_ANGLE), getPitchAngleSetpoint() - Sensors_getPitch(), MAX_PITCH_RATE / MAX_PITCH_ANGLE));
    } else {
        setPitchRateSetpoint(getPitchRateInput(getControlValue(PITCH_CONTROL_SOURCE)));
    }
    setPitchRateSetpoint(getPitchRateSetpoint() + (fabsf(Sensors_getRoll()) * roll_turn_mix)); //Apply Coordinated Turn //Linear Function

    control_Roll = PIDcontrol(getPID(ROLL_RATE), getRollRateSetpoint() - Sensors_getRollRate(), HALF_PWM_RANGE / MAX_ROLL_RATE);
    control_Pitch = PIDcontrol(getPID(PITCH_RATE), getPitchRateSetpoint() - Sensors_getPitchRate(), HALF_PWM_RANGE / MAX_PITCH_RATE);
    control_Yaw = PIDcontrol(getPID(YAW_RATE), getYawRateSetpoint() - Sensors_getYawRate(), HALF_PWM_RANGE / MAX_YAW_RATE);
    control_Throttle = getThrottleSetpoint();

    outputSignal[FLAP_OUT_CHANNEL - 1] = getFlapInput(getControlValue(FLAP_CONTROL_SOURCE)); // don't need to mix the flaps

    //Mixing!
    outputMixing(outputSignal, &control_Roll, &control_Pitch, &control_Throttle, &control_Yaw);

    //Error Checking
    checkLimits(outputSignal);
    //Then Output
    if (getProgramStatus() == KILL_MODE) { //if in kill mode, full deflection of all control surfaces
         outputSignal[THROTTLE_OUT_CHANNEL -1] = PWM_MIN;  //Throttle
         outputSignal[ROLL_OUT_CHANNEL -1] = PWM_MIN;      //Roll
         outputSignal[L_TAIL_OUT_CHANNEL -1] = PWM_MIN;    //Pitch
         outputSignal[R_TAIL_OUT_CHANNEL -1] = PWM_MIN;    //Yaw
    }

    Interchip_SetPWM(outputSignal);

    //Check for kill mode
#if COMP_MODE
    if (getUHFStatus() == CONNECTION_WARN && getProgramStatus() != KILL_MODE_WARNING && getProgramStatus() != KILL_MODE){
        info("Setting kill mode warning due to UHF disconnect");
        setProgramStatus(KILL_MODE_WARNING);
    } else if (getUHFStatus() == CONNECTION_EXPIRED && getProgramStatus() != KILL_MODE){
        info("Setting  kill mode due to UHF disconnect");
        setProgramStatus(KILL_MODE);
    }

    if (getHeartbeatStatus() == CONNECTION_WARN && getProgramStatus() != KILL_MODE_WARNING && getProgramStatus() != KILL_MODE){
        interchip_send_buffer.am_data.command = PM_RETURN_HOME;
        sendInterchipData();
        info("Setting kill mode warning due to HEARTBEAT");
        setProgramStatus(KILL_MODE_WARNING);
    } else if (getHeartbeatStatus() == CONNECTION_EXPIRED && getProgramStatus() != KILL_MODE){
        info("Setting  kill mode due to HEARTBEAT");
        setProgramStatus(KILL_MODE);
    }
#endif

}

/* Private Functions */

void checkLimits(int16_t* channelOut){
    constrain(&(channelOut[THROTTLE_OUT_CHANNEL - 1]), PWM_MIN, PWM_MAX);

    constrain(&(channelOut[ROLL_OUT_CHANNEL - 1]), MIN_ROLL_PWM, MAX_ROLL_PWM);

    constrain(&(channelOut[L_TAIL_OUT_CHANNEL - 1]), MIN_L_TAIL_PWM, MAX_L_TAIL_PWM);

    constrain(&(channelOut[R_TAIL_OUT_CHANNEL - 1]), MIN_R_TAIL_PWM, MAX_R_TAIL_PWM);

    constrain(&(channelOut[FLAP_OUT_CHANNEL - 1]), PWM_MIN, PWM_MAX);
}

void setDefaultPWM(){
  for (uint8_t i = 0; i < PWM_NUM_CHANNELS; i++){
    outputSignal[i] = PWM_MIN;
  }

  //Init special channels
  outputSignal[ROLL_OUT_CHANNEL-1] = 0;
  outputSignal[L_TAIL_OUT_CHANNEL-1] = 0;
  outputSignal[R_TAIL_OUT_CHANNEL-1] = 0;

  Interchip_SetPWM(outputSignal);
}

void outputMixing(int16_t* channelOut, int16_t* control_Roll, int16_t* control_Pitch, int16_t* control_Throttle, int16_t* control_Yaw){
    *control_Yaw += *control_Roll * adverse_yaw_mix; // mix roll rate into rudder to counter adverse yaw

    //code for different tail configurations
    #if TAIL_TYPE == STANDARD_TAIL  //is a normal t-tail
    channelOut[PITCH_OUT_CHANNEL - 1] = (*control_Pitch);
    channelOut[YAW_OUT_CHANNEL - 1] = (*control_Yaw);

    #elif TAIL_TYPE == V_TAIL    //V-tail
    // TODO

    #elif TAIL_TYPE == INV_V_TAIL   //Inverse V-Tail
    channelOut[L_TAIL_OUT_CHANNEL - 1] =  (*control_Yaw) * RUDDER_PROPORTION - (*control_Pitch) * ELEVATOR_PROPORTION ; //Tail Output Left
    channelOut[R_TAIL_OUT_CHANNEL - 1] =  (*control_Yaw) * RUDDER_PROPORTION + (*control_Pitch) * ELEVATOR_PROPORTION ; //Tail Output Right

    #endif
    channelOut[ROLL_OUT_CHANNEL - 1] = (*control_Roll);
    channelOut[THROTTLE_OUT_CHANNEL - 1] = (*control_Throttle);
}

void inputMixing(int16_t* channelIn, int16_t* rollRate, int16_t* pitchRate, int16_t* throttle, int16_t* yawRate){
    if (getControlValue(THROTTLE_CONTROL_SOURCE) == RC_SOURCE) {
        *throttle = channelIn[THROTTLE_IN_CHANNEL - 1];
    }

#if TAIL_TYPE == STANDARD_TAIL
    if (getControlValue(ROLL_CONTROL_SOURCE) == RC_SOURCE){
        *rollRate = -channelIn[ROLL_IN_CHANNEL - 1];
    }
    if (getControlValue(PITCH_CONTROL_SOURCE) == RC_SOURCE){
        *pitchRate = -channelIn[PITCH_IN_CHANNEL - 1];
    }
    *yawRate = -channelIn[YAW_IN_CHANNEL - 1];

#elif TAIL_TYPE == V_TAIL
    
#elif TAIL_TYPE == INV_V_TAIL
    if (getControlValue(ROLL_CONTROL_SOURCE) == RC_SOURCE) {
        *rollRate = channelIn[ROLL_IN_CHANNEL - 1];
    }
    if (getControlValue(PITCH_CONTROL_SOURCE) == RC_SOURCE){
        *pitchRate = (channelIn[R_TAIL_IN_CHANNEL - 1] - channelIn[L_TAIL_IN_CHANNEL - 1]) / (2 * ELEVATOR_PROPORTION);
    }
    *yawRate = (channelIn[L_TAIL_IN_CHANNEL - 1] + channelIn[R_TAIL_IN_CHANNEL - 1] ) / (2 * RUDDER_PROPORTION);
#endif

    if (getControlValue(FLAP_CONTROL_SOURCE) == RC_SOURCE) {
        input_RC_Flap = channelIn[FLAP_IN_CHANNEL - 1];
    }
}

#endif
