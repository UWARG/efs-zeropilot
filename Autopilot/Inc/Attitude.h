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

int16_t getRollAngleSetpoint();
int16_t getPitchAngleSetpoint();
int16_t getPitchRateSetpoint();
int16_t getRollRateSetpoint();
int16_t getYawRateSetpoint();
int16_t getThrottleSetpoint();
int16_t getAltitudeSetpoint();
int16_t getHeadingSetpoint();

void setPitchAngleSetpoint(int16_t setpoint);
void setRollAngleSetpoint(int16_t setpoint);
void setPitchRateSetpoint(int16_t setpoint);
void setRollRateSetpoint(int16_t setpoint);
void setYawRateSetpoint(int16_t setpoint);
void setThrottleSetpoint(int16_t setpoint);
void setAltitudeSetpoint(int16_t setpoint);
void setHeadingSetpoint(int16_t setpoint);

int16_t getPitchAngleInput(uint8_t source);
int16_t getRollAngleInput(uint8_t source);
int16_t getPitchRateInput(uint8_t source);
int16_t getRollRateInput(uint8_t source);
int16_t getYawRateInput(uint8_t source);
int16_t getThrottleInput(uint8_t source);
int16_t getAltitudeInput(uint8_t source);
int16_t getHeadingInput(uint8_t source);
int16_t getFlapInput(uint8_t source);

uint8_t getControlValue(CtrlType type);