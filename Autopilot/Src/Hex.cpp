#include "Stabilizer.h"

//Bit Mask Bit Shifts
typedef enum {
    STABILIZE,
    ALT_HOLD, //Altitude hold
    LOITER,//Hold at current point
    RTL, //Return to Launch
    AUTO //Full autopilot path following
} CtrlType;

class Hex: public Stabilizer{
  public:
    Hex();

  private:
  CtrlType flight_mode;
  ControlAxis roll, pitch, yaw, altitude;
}


Hex::Hex(){
  //create PID
  //create ControlAxies
  //Get Sensors
}

int16_t *Hex::Stabilize(){
  switch(flight_mode){
    case STABILIZE:
      Ctrl_Stabilize();
    break;
    /*case ALT_HOLD:
      Ctrl_Alt_hold();
    break;
    case LOITER:
      Ctrl_Loiter();
    case AUTO:
      auto();*/
  }

  roll.set_RateSetpoint(PIDcontrol(roll.get_Setpoint() - getRoll()));
  pitch.set_RateSetpoint(PIDcontrol(pitch.get_Setpoint() - getPitch()));

  control_Roll = PIDcontrol( roll.get_RateSetpoint() - getRollRate());
  control_Pitch = PIDcontrol(pitch.get_RateSetpoint() - getPitchRate());
  control_Yaw = PIDcontrol(yaw.get_RateSetpoint() - getYawRate());
  control_Throttle = altitude.get_RateSetpoint();

  return outputMixing(control_Roll, control_Pitch, control_Pitch, control_Throttle);
}

void Hex::setFlightMode(CtrlType _flight_mode){
  flight_mode = _flight_mode;
}

void Hex::Ctrl_Stabilize(){
  altitude.set_RateSetpoint(altitude.get_RateInput(RC_SOURCE));
  pitch.set_Setpoint(pitch.get_Input(RC_SOURCE));
  roll.set_Setpoint(roll.get_Input(RC_SOURCE));
  yaw.set_Setpoint(yaw.get_Input(RC_SOURCE));
}
/*
void Hex::Ctrl_Alt_hold(){
  setAltitudeSetpoint(getAltitudeInput(RC_SOURCE));
  setPitchAngleSetpoint(getPitchAngleInput(RC_SOURCE));
  setRollAngleSetpoint(getRollAngleInput(RC_SOURCE));
  setYawSetpoint(getYawInput(RC_SOURCE));
}

void Hex::Ctrl_Loiter(){
  setThrottleSetpoint(getThrottleInput(RC_SOURCE));
  setPitchAngleSetpoint(getPitchAngleInput(RC_SOURCE));
  setRollAngleSetpoint(getRollAngleInput(RC_SOURCE));
  setYawSetpoint(getYawInput(RC_SOURCE));
}

void Hex::AltitudeControl(){
  setAltitudeSetpoint(getAltitudeInput(GS_SOURCE));
  setPitchAngleSetpoint(getPitchAngleInput(RC_SOURCE));
  setRollAngleSetpoint(getRollAngleInput(RC_SOURCE));
  setYawSetpoint(getYawInput(RC_SOURCE));

  setRollRateSetpoint(PIDcontrol(getRollAngleSetpoint() - getRoll()));
  setPitchRateSetpoint(PIDcontrol(getPitchAngleSetpoint() - getPitch()));

  control_Roll = PIDcontrol(getRollRateSetpoint() - getRollRate());
  control_Pitch = PIDcontrol(getPitchRateSetpoint() - getPitchRate());
  control_Yaw = PIDcontrol(getYawRateSetpoint() - getYawRate());
  control_Throttle = getThrottleSetpoint();

  return outputMixing(control_Roll, control_Pitch, control_Pitch, control_Throttle);
}*/