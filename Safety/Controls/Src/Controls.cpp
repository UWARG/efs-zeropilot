//
// Controls.cpp
// Keeps the drone in the air. Updating target position will cause drone to fly
// towards position
//

#include "Controls.hpp"
#include <cmath>

#include "PPM.hpp"
#include "PWM.hpp"
#include "SensorFusion.hpp"
#include "base_pid.hpp"

/* Inputs:
 * - RC inputs
 */

/* Outputs:
 *      - PWM values
 */

/* Some notes
 * Do we need to worry about tracking drift error in the IMU? -> would it be
 * preferable if we did? Do we store current heading data? How do we store
 * this.....
 */

/***********************************************************************************************************************
 * Defs
 **********************************************************************************************************************/

static SFOutput_t curr_sf;  // current
static PID_Output_t PID_Out;

// =======================================================
// PID consts
// =======================================================
const int pid_abs_max = 100;
const int max_i_windup = 1;  // ? not sure if we need specifics for each angle

// const float roll_kp = 0.175;
// const float roll_ki = 0;
// const float roll_kd = 0.055;

// const float pitch_kp = 0.15;
// const float pitch_ki = 0;
// const float pitch_kd = 0.055;

// const float yaw_kp = 0.15;
// const float yaw_ki = 0;
// const float yaw_kd = 0;

const float roll_kp = 0.085;
const float roll_ki = 0.001;
const float roll_kd = 0.015;

const float pitch_kp = 0.085;
const float pitch_ki = 0.001;
const float pitch_kd = 0.015;

const float yaw_kp = 0.25;
const float yaw_ki = 0.01;
const float yaw_kd = 0.05;




const int PID_method = 0;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

PID_Output_t *runControlsAndGetPWM(Instructions_t *instructions, SFOutput_t *SF_pos) {
  // to use or not to use pointers?
  curr_sf = *SF_pos;

  // =================================
  // PID Code Begins here.
  // =================================

  // PIDController controller(float _kp, float _ki, float _kd, float _i_max,
  // float _min_output, float _max_output);
  PIDController pid_roll{roll_kp, roll_ki, roll_kd, max_i_windup, -pid_abs_max, pid_abs_max};

  PIDController pid_pitch{pitch_kp, pitch_ki, pitch_kd, max_i_windup, -pid_abs_max, pid_abs_max};

  PIDController pid_yaw{yaw_kp, yaw_ki, yaw_kd, max_i_windup, -pid_abs_max, pid_abs_max};

  // get PID result
  float roll = pid_roll.execute(instructions->input1, curr_sf.roll, curr_sf.rollRate);
  float pitch = pid_pitch.execute(instructions->input2, curr_sf.pitch, curr_sf.pitchRate);

  // yaw is now going to target a specific yawrate.
  // FIXME: double check that the yawRate matches direction from stick
  float yaw = pid_yaw.execute(instructions->input4, curr_sf.yawRate);
  float throttle = instructions->input3;  //

  // mix the PID's.
  PID_Out.backLeftMotorPercent   = throttle - roll - pitch - yaw;
  PID_Out.frontLeftMotorPercent  = throttle - roll + pitch + yaw;
  PID_Out.backRightMotorPercent  = throttle + roll - pitch + yaw;
  PID_Out.frontRightMotorPercent = throttle + roll + pitch - yaw;
  return &PID_Out;
}