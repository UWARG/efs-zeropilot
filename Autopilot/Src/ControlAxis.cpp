class ControlAxis{
  public:
  void set_RateSetpoint();
  void get_RateSetpoint();
  void set_Setpoint();
  void get_Setpoint();

  void set_RateInput();
  void get_RateInput();
  void set_Input();
  void get_ProportionalInput();
}
int ControlAxis::get_RateInput(uint8_t source){
    if (source == RC_SOURCE){
        return (int)(input_RC_Rate / (HALF_PWM_RANGE / MAX_PITCH_ANGLE));
    }
    else if (source == GS_SOURCE){
        return input_GS_Rate;
    }
    else
        return 0;
}