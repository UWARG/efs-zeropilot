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
 * - Global position -> not sure who will track diff from prev
 * - RC inputs
 */

/* Outputs:
 *      - PWM values
 */

/* Some notes
 * Do we need to worry about tracking drift error in the IMU? -> would it be
 * preferable if we did? Do we store current heading data? How do we store
 * this.....
 *  -> would we use this data later to generate better curves and splines?
 * Define traditional XYZ coordinates: x forward, y left, z up, cw +
 */

/***********************************************************************************************************************
 * Defs
 **********************************************************************************************************************/

static SFOutput_t curr_sf;  // current
static PID_Output_t PID_Out;

// PIDController controller(float _kp, float _ki, float _kd, float _i_max,
// float _min_output, float _max_output);

const int pid_abs_max = 100;
const int max_i_windup = 5;  // ? not sure if we need specifics for each angle -> especially yaw

const float roll_kp = 0.175;
const float roll_kd = 0.055;
const float roll_ki = 0;

const float pitch_kp = 0.15;
const float pitch_kd = 0.055;
const float pitch_ki = 0;

const float yaw_kp = 0;
const float yaw_kd = 0;
const float yaw_ki = 0;

/** Determines whether to use P-loop, or PID, or something else.
 * 0 = base_pid
 * 1 = simple_p
 */
const int PID_method = 0;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void evalControls() {
  // main entry point to run/evaluate controls.

  // run PID's on our target positions.

  // to be used in the future!
}

/*
 * pwmValues = runControlsAndGetPWM(instructions, SF_position)
 */

PID_Output_t *runControlsAndGetPWM(Instructions_t *instructions,
                                   SFOutput_t *SF_pos) {
  // to use or not to use pointers?
  curr_sf = *SF_pos;

  // =================================
  // PID Code Begins here.
  // =================================

  // PIDController controller(float _kp, float _ki, float _kd, float _i_max,
  // float _min_output, float _max_output);
  PIDController pid_roll{
      roll_kp,      roll_ki,    roll_kd, max_i_windup,
      -pid_abs_max, pid_abs_max};  // defining our test PID with a lower max
                                   // output for now (bring this up as testing
                                   // needs).
  PIDController pid_pitch{pitch_kp,     pitch_ki,     pitch_kd,
                          max_i_windup, -pid_abs_max, pid_abs_max};

  PIDController pid_yaw{yaw_kp,       yaw_ki,       yaw_kd,
                        max_i_windup, -pid_abs_max, pid_abs_max};

  // get PID result
  float roll = pid_roll.execute(
      instructions->input1, curr_sf.roll,
      curr_sf.rollRate);  // change yaw and yawRate to correct measurement.
  // yawRate may not be usable. If you don't put in yawRate PID will
  // automatically calculate derivative

  float pitch =
      pid_pitch.execute(instructions->input2, curr_sf.pitch, curr_sf.pitchRate);

  // yaw is now going to target a specific yawrate.
  // FIXME: double check that the yawRate matches direction from stick
  float yaw = pid_yaw.execute(instructions->input4, curr_sf.yawRate);

  float throttle = instructions->input3;  //

  // mix the PID's.
  PID_Out.backLeftMotorPercent = throttle - roll - pitch;    // - yaw;
  PID_Out.frontLeftMotorPercent = throttle - roll + pitch;   // + yaw;
  PID_Out.backRightMotorPercent = throttle + roll - pitch;   // + yaw;
  PID_Out.frontRightMotorPercent = throttle + roll + pitch;  // - yaw;
  return &PID_Out;
}