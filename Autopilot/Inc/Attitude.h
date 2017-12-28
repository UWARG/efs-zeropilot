#include "main.h"

void Attitude_Run(void const * argument);

//Bit Mask Bit Shifts
typedef enum {
    PITCH_CONTROL_TYPE      = 0,
    PITCH_CONTROL_SOURCE    = 1,
    ROLL_CONTROL_TYPE       = 2,
    ROLL_CONTROL_SOURCE     = 3,
    THROTTLE_CONTROL_SOURCE = 4,
    UNUSED_FIVE             = 5, // throttle is 2 bits
    ALTITUDE_CONTROL_SOURCE = 6,
    ALTITUDE_CONTROL        = 7,
    HEADING_CONTROL_SOURCE  = 8,
    HEADING_CONTROL         = 9,
    FLAP_CONTROL_SOURCE     = 10,
} CtrlType;

//Bit Mask Resultant Values
#define RATE_CONTROL 0
#define ANGLE_CONTROL 1

#define RC_SOURCE 0
#define GS_SOURCE 1
#define AP_SOURCE 2

#define HEADING_GS_SOURCE 0
#define HEADING_AP_SOURCE 1
#define ALTITUDE_GS_SOURCE 0
#define ALTITUDE_AP_SOURCE 1

#define CONTROL_ON 1
#define CONTROL_OFF 0

//Defined in the opposite order for rates of rotation
//Be careful not to confuse the constants
#define IMU_YAW_RATE     2
#define IMU_PITCH_RATE   1
#define IMU_ROLL_RATE    0

//Misc
#define COMMAND_HISTORY_SIZE 4

int getRollAngleSetpoint();
int getPitchAngleSetpoint();
int getPitchRateSetpoint();
int getRollRateSetpoint();
int getYawRateSetpoint();
int getThrottleSetpoint();
int getAltitudeSetpoint();
int getHeadingSetpoint();

void setPitchAngleSetpoint(int16_t setpoint);
void setRollAngleSetpoint(int16_t setpoint);
void setPitchRateSetpoint(int16_t setpoint);
void setRollRateSetpoint(int16_t setpoint);
void setYawRateSetpoint(int16_t setpoint);
void setThrottleSetpoint(int16_t setpoint);
void setAltitudeSetpoint(int16_t setpoint);
void setHeadingSetpoint(int16_t setpoint);

int getPitchAngleInput(char source);
int getRollAngleInput(char source);
int getPitchRateInput(char source);
int getRollRateInput(char source);
int getYawRateInput(char source);
int getThrottleInput(char source);
int getAltitudeInput(char source);
int getHeadingInput(char source);
int getFlapInput(char source);

uint8_t getControlValue(CtrlType type);