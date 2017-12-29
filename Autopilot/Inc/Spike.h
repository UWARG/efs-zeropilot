

#ifndef FIXEDWING_H
#define	FIXEDWING_H

#include "main.h"
// Function Prototypes
void Vehicle_Initialization();
void Vehicle_Arm();
void Vehicle_Dearm();
void Vehicle_GetInputRC(int16_t* roll, int16_t* pitch, int16_t* throttle, int16_t* yaw);
void Vehicle_HighLevelControl();
void Vehicle_LowLevelControl();

// Constants
#define MAX_ROLL_PWM (PWM_MAX - 87) //This was - 20
#define MIN_ROLL_PWM (PWM_MIN+ 174) //This was + 40
#define MAX_L_TAIL_PWM (PWM_MAX - 109) //This was - 25
#define MIN_L_TAIL_PWM (PWM_MIN + 109) //This was + 25
#define MAX_R_TAIL_PWM (PWM_MAX - 109) //This was - 25
#define MIN_R_TAIL_PWM (PWM_MIN + 109) //This was + 25

#define STALL_SPEED 48.15 //km/h

#define RUDDER_PROPORTION 0.75
#define ELEVATOR_PROPORTION 0.75

#define STANDARD_TAIL 0
#define V_TAIL 1
#define INV_V_TAIL 2

// Set airplane tail type
#define TAIL_TYPE INV_V_TAIL 

#if USE_PPM // Can't use physical channel 8 
#define FLAP_IN_CHANNEL 8
#else
#define FLAP_IN_CHANNEL 5
#endif

#if TAIL_TYPE == STANDARD_TAIL
// Inputs
#define THROTTLE_IN_CHANNEL 1
#define ROLL_IN_CHANNEL 2
#define PITCH_IN_CHANNEL 3
#define YAW_IN_CHANNEL 4

// Outputs
#define THROTTLE_OUT_CHANNEL 1
#define ROLL_OUT_CHANNEL 2
#define PITCH_OUT_CHANNEL 3
#define YAW_OUT_CHANNEL 4
#define FLAP_OUT_CHANNEL 5

#elif TAIL_TYPE == V_TAIL || TAIL_TYPE == INV_V_TAIL
// Inputs
#define THROTTLE_IN_CHANNEL 1
#define ROLL_IN_CHANNEL 2
#define L_TAIL_IN_CHANNEL 3
#define R_TAIL_IN_CHANNEL 4

// Outputs
#define THROTTLE_OUT_CHANNEL 1
#define ROLL_OUT_CHANNEL 2
#define L_TAIL_OUT_CHANNEL 3
#define R_TAIL_OUT_CHANNEL 4
#define FLAP_OUT_CHANNEL 5

#endif

#endif	/* FIXEDWING_H */
