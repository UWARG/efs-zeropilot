
int Setpoints_getRollAngle(){
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

void setPitchAngleSetpoint(int setpoint){
    if (limitSetpoint) {
        constrain(&setpoint, -MAX_PITCH_ANGLE, MAX_PITCH_ANGLE);
    }
    sp_PitchAngle = setpoint;
}
void setRollAngleSetpoint(int setpoint){
    if (limitSetpoint) {
        constrain(&setpoint, -MAX_ROLL_ANGLE, MAX_ROLL_ANGLE);
    }
    sp_RollAngle = setpoint;
}
void setPitchRateSetpoint(int setpoint){
    if (limitSetpoint) {
        constrain(&setpoint, -MAX_PITCH_RATE, MAX_PITCH_RATE);
    }
    sp_PitchRate = setpoint;
}
void setRollRateSetpoint(int setpoint){
    if (limitSetpoint) {
        constrain(&setpoint, -MAX_ROLL_RATE, MAX_ROLL_RATE);
    }
    sp_RollRate = setpoint;
}
void setYawRateSetpoint(int setpoint){
    if (limitSetpoint) {
        constrain(&setpoint, -MAX_YAW_RATE, MAX_YAW_RATE);
    }
    sp_YawRate = setpoint;
}
void setThrottleSetpoint(int setpoint){
    sp_Throttle = setpoint;
}
void setAltitudeSetpoint(int setpoint){
    sp_Altitude = setpoint;
}
void setHeadingSetpoint(int setpoint){
    sp_Heading = setpoint;
}