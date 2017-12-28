#include "Common.h"
#include "cmsis_os.h"
#include "Interchip_A.h"
#include "Attitude.h"
#include "OrientationControl.h"
#include "Spike.h"

// Control level bit masks (indexed by CtrlType enum)
static const uint16_t ctrl_mask[16] = {
            0b00000001, // Pitch Rate(0) or Pitch Angles(1)
            0b00000010, // Pitch from Controller(0) or Ground Station(1)
            0b00000100, // Roll Rates(0) or Roll Angles(1)
            0b00001000, // Roll from Controller(0) or Ground Station(1)
            0b00110000, // Throttle from Controller(0) or Ground Station(1) or Autopilot(2)(Controlled by the GroundSpeed/Altitude).
            0b00000000, // Empty. Throttle is 2 bits, so index 5 is unused.
            0b01000000, // Altitude from Ground Station(0) or Autopilot(1)
            0b10000000, // Altitude Off(0) or On(1)
    0b0000000100000000, // Heading from Ground Station(0) or Autopilot(1)
    0b0000001000000000, // Heading Off(0) or On(1)
    0b0000110000000000  // Flaps from Controller(0) or Ground Station(1) or Autopilot(2) // TODO:remove
};

// Setpoints
int sp_Throttle = PWM_MIN;

int sp_RollRate = 0; // Degrees per second
int sp_PitchRate = 0;
int sp_YawRate = 0;

int sp_RollAngle = 0; // degrees
int sp_PitchAngle = 0;

int sp_Heading = 0;
int sp_Altitude = 0;
float sp_GroundSpeed = 0;

//Ground Station Input Signals
int input_GS_Roll = 0;
int input_GS_Pitch = 0;
int input_GS_Throttle = 0;
int input_GS_Yaw = 0;
int input_GS_RollRate = 0;
int input_GS_PitchRate = 0;
int input_GS_YawRate = 0;
int input_GS_Altitude = 0;
int input_GS_Heading = 0;

int input_AP_Altitude = 0;
int input_AP_Heading = 0;


extern int input_RC_Flap; // Flaps need to finish being refactored.
extern int input_GS_Flap;

//RC Input Signals (Input Capture Values)
int input_RC_Throttle = PWM_MIN;
int input_RC_RollRate = 0;
int input_RC_PitchRate = 0;
int input_RC_YawRate = 0;
int input_RC_Aux1 = 0; //0=Roll, 1= Pitch, 2=Yaw
int input_RC_Aux2 = 0; //0 = Saved Value, 1 = Edit Mode
int input_RC_Switch1 = 0;


int controlLevel = 0;

void Attitude_Run(void const * argument){
  //Init
  int16_t PWM[PWM_NUM_CHANNELS] = {};

  while(1){
    highLevelControl();
    lowLevelControl();

    //Send PWM data
    Interchip_SetPWM(PWM);
    osDelay(1); //TODO set delay properly
  }
}

int getRollAngleSetpoint(){
    return sp_RollAngle;
}
int getPitchAngleSetpoint(){
    return sp_PitchAngle;
}
int getPitchRateSetpoint(){
    return sp_PitchRate;
}
int getRollRateSetpoint(){
    return sp_RollRate;
}
int getYawRateSetpoint(){
    return sp_YawRate;
}
int getThrottleSetpoint(){
    return sp_Throttle;
}
int getAltitudeSetpoint(){
    return sp_Altitude;
}
int getHeadingSetpoint(){
    return sp_Heading;
}

void setPitchAngleSetpoint(int16_t setpoint){
  constrain(&setpoint, -MAX_PITCH_ANGLE, MAX_PITCH_ANGLE);
  sp_PitchAngle = setpoint;
}
void setRollAngleSetpoint(int16_t setpoint){
  constrain(&setpoint, -MAX_ROLL_ANGLE, MAX_ROLL_ANGLE);
  sp_RollAngle = setpoint;
}
void setPitchRateSetpoint(int16_t setpoint){
  constrain(&setpoint, -MAX_PITCH_RATE, MAX_PITCH_RATE);
  sp_PitchRate = setpoint;
}
void setRollRateSetpoint(int16_t setpoint){
  constrain(&setpoint, -MAX_ROLL_RATE, MAX_ROLL_RATE);
  sp_RollRate = setpoint;
}
void setYawRateSetpoint(int16_t setpoint){
  constrain(&setpoint, -MAX_YAW_RATE, MAX_YAW_RATE);
  sp_YawRate = setpoint;
}
void setThrottleSetpoint(int16_t setpoint){
  sp_Throttle = setpoint;
}
void setAltitudeSetpoint(int16_t setpoint){
  sp_Altitude = setpoint;
}
void setHeadingSetpoint(int16_t setpoint){
  sp_Heading = setpoint;
}

int getPitchAngleInput(char source){
    if (source == RC_SOURCE){
        return (int)(input_RC_PitchRate / (HALF_PWM_RANGE / MAX_PITCH_ANGLE));
    }
    else if (source == GS_SOURCE){
        return input_GS_Pitch;
    }
    else
        return 0;
}
int getPitchRateInput(char source){
    if (source == RC_SOURCE){
        return (int)(input_RC_PitchRate / (HALF_PWM_RANGE / MAX_PITCH_RATE));
    }
    else if (source == GS_SOURCE){
        return input_GS_PitchRate;
    }
    else
        return 0;
}
int getRollAngleInput(char source){
    if (source == RC_SOURCE){
        return (int)(input_RC_RollRate / (HALF_PWM_RANGE / MAX_ROLL_ANGLE));
    }
    else if (source == GS_SOURCE){
        return input_GS_Roll;
    }
    else{
        return 0;
    }
}
int getRollRateInput(char source){
    if (source == RC_SOURCE){
        return (int)(input_RC_RollRate / (HALF_PWM_RANGE / MAX_ROLL_RATE));
    }
    else if (source == GS_SOURCE){
        return input_GS_RollRate;
    }
    else
        return 0;
}

int getYawRateInput(char source){
    if (source == RC_SOURCE){
        return (int)(input_RC_YawRate / (HALF_PWM_RANGE / MAX_YAW_RATE));
    }
    else if (source == GS_SOURCE){
        return input_GS_YawRate;
    }
    else
        return 0;
}
int getThrottleInput(char source){
    if (source == RC_SOURCE){
        return input_RC_Throttle;
    }
    else if (source == GS_SOURCE){
        return input_GS_Throttle;
    }
    else if (source == AP_SOURCE){
//        return input_AP_Throttle;
        return PWM_MIN;
    }
    else
        return 0;
}

int getFlapInput(char source){
    if (source == RC_SOURCE){
        return input_RC_Flap;
    }
    else if (source == GS_SOURCE){
        return input_GS_Flap;
    }
    else if (source == AP_SOURCE){
//        return input_AP_Flap;
        return 0;
    }
    else
        return 0;
}

int getAltitudeInput(char source){
    if (source == ALTITUDE_GS_SOURCE){
        return input_GS_Altitude;
    }
    else if (source == ALTITUDE_AP_SOURCE){
        return input_AP_Altitude;
    }
    else
        return 0;
}

int getHeadingInput(char source){
    if (source == HEADING_GS_SOURCE){
        return input_GS_Heading;
    }
    else if (source == HEADING_AP_SOURCE){
        return input_AP_Heading;
    }
    else
        return 0;
}

// Type is both bit shift value and index of bit mask array
uint8_t getControlValue(CtrlType type) {
    return (controlLevel & ctrl_mask[type]) >> type;
}