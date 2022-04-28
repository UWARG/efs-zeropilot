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
static SFOutput_t prev_sf;  // previous
static PID_Output_t PID_Out;

// =======================================================
// PID consts
// =======================================================
const int pid_abs_max = 100;
const int max_i_windup = 5;  // ? not sure if we need specifics for each angle

// const float roll_kp = 0.175;
// const float roll_ki = 0;
// const float roll_kd = 0.055;

// const float pitch_kp = 0.15;
// const float pitch_ki = 0;
// const float pitch_kd = 0.055;

// const float yaw_kp = 0.15;
// const float yaw_ki = 0;
// const float yaw_kd = 0;

// FIXME: this is to be defined in the code later depending on switch positions
int PID_method = 0;  // 0 = acro, 1 = hold altitude, 2 = hold location

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

PID_Output_t *runControlsAndGetPWM(Instructions_t *instructions, SFOutput_t *SF_pos) {
  // to use or not to use pointers?
  curr_sf = *SF_pos;

  int prev_PID_method = 0;  // default is acro
  // =================================
  // PID Code Begins here.
  // =================================
  if (PID_method < 0 || PID_method > 2) {
    PID_method = 0;  // set to default acro mode
  }
  if (PID_method == 0) {
    // acro mode
    // PIDController controller(float _kp, float _ki, float _kd, float _i_max,
    // float _min_output, float _max_output);

    // ? is redifining these everytime costly? should we leave them at the top? Compiler should take care of this right?
    const float roll_kp = 0.05;
    const float roll_ki = 0;
    const float roll_kd = 0.05;

    const float pitch_kp = 0.05;
    const float pitch_ki = 0;
    const float pitch_kd = 0.05;

    const float yaw_kp = 0.25;
    const float yaw_ki = 0;
    const float yaw_kd = 0.05;

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
    PID_Out.backLeftMotorPercent = throttle - roll - pitch - yaw;
    PID_Out.frontLeftMotorPercent = throttle - roll + pitch + yaw;
    PID_Out.backRightMotorPercent = throttle + roll - pitch + yaw;
    PID_Out.frontRightMotorPercent = throttle + roll + pitch - yaw;
  } else if (PID_method == 1) {
    // PID method 1 = Hold current altitude
    if (prev_PID_method != PID_method) {
      // we have just toggled the PID method, so save height location
      prev_sf = curr_sf;  // not sure about the pointer operation...
    }
    // hold altitude
    const float roll_kp = 0.05;
    const float roll_ki = 0;
    const float roll_kd = 0.05;

    const float pitch_kp = 0.05;
    const float pitch_ki = 0;
    const float pitch_kd = 0.05;

    const float yaw_kp = 0.25;
    const float yaw_ki = 0;
    const float yaw_kd = 0.05;

    const float alt_kp = 0.025;
    const float alt_ki = 0;
    const float alt_kd = 0.0125;

    PIDController pid_roll(roll_kp, roll_ki, roll_kd, max_i_windup, -pid_abs_max, pid_abs_max);

    PIDController pid_pitch(pitch_kp, pitch_ki, pitch_kd, max_i_windup, -pid_abs_max, pid_abs_max);

    PIDController pid_yaw(yaw_kp, yaw_ki, yaw_kd, max_i_windup, -pid_abs_max, pid_abs_max);

    PIDController pid_altitude(alt_kp, alt_ki, alt_kd, max_i_windup, -pid_abs_max, pid_abs_max);

    // get PID result
    float roll = pid_roll.execute(instructions->input1, curr_sf.roll, curr_sf.rollRate);
    float pitch = pid_pitch.execute(instructions->input2, curr_sf.pitch, curr_sf.pitchRate);

    // yaw is now going to target a specific yawrate.
    // FIXME: double check that the yawRate matches direction from stick
    float yaw = pid_yaw.execute(instructions->input4, curr_sf.yawRate);
    float throttle = pid_altitude.execute(prev_sf.altitude, curr_sf.altitude);  // desired is the one assigned on toggle

    // mix the PID's.
    PID_Out.backLeftMotorPercent = throttle - roll - pitch - yaw;
    PID_Out.frontLeftMotorPercent = throttle - roll + pitch + yaw;
    PID_Out.backRightMotorPercent = throttle + roll - pitch + yaw;
    PID_Out.frontRightMotorPercent = throttle + roll + pitch - yaw;
  } else if (PID_method == 2) {
    // Pid method 2 = HOLD LOCATION
    // EXTREMELY UNTESTED, probably not usable
    // lat = roll = left/right ; lon = pitc = front/back ; alt = throttle = up/down ; yaw = yaw = rotation
    if (prev_PID_method != PID_method) {
      // we have just toggled the PID method, so save height location
      prev_sf = curr_sf;  // not sure about the pointer operation...
    }

    const float lat_kp = 0.04;
    const float lat_ki = 0;
    const float lat_kd = 0.01;

    const float lon_kp = 0.04;
    const float lon_ki = 0;
    const float lon_kd = 0.01;

    const float yaw_kp = 0.25;
    const float yaw_ki = 0;
    const float yaw_kd = 0.05;

    const float alt_kp = 0.025;
    const float alt_ki = 0;
    const float alt_kd = 0.0125;

    PIDController pid_lat(lat_kp, lat_ki, lat_kd, max_i_windup, -pid_abs_max, pid_abs_max);

    PIDController pid_lon(lon_kp, lon_ki, lon_kd, max_i_windup, -pid_abs_max, pid_abs_max);

    PIDController pid_yaw(yaw_kp, yaw_ki, yaw_kd, max_i_windup, -pid_abs_max, pid_abs_max);

    PIDController pid_altitude(alt_kp, alt_ki, alt_kd, max_i_windup, -pid_abs_max, pid_abs_max);

    float lat = pid_lat.execute(prev_sf.latitude, curr_sf.latitude);
    float lon = pid_lon.execute(prev_sf.longitude, curr_sf.longitude);

    // yaw is now going to target a specific yawrate.
    // FIXME: double check that the yawRate matches direction from stick
    float yaw = pid_yaw.execute(0, curr_sf.yawRate);                       // no yaw rate
    float alt = pid_altitude.execute(prev_sf.altitude, curr_sf.altitude);  // desired is the one assigned on toggle

    // mix the PID's.
    PID_Out.backLeftMotorPercent = alt - lat - lon - yaw;
    PID_Out.frontLeftMotorPercent = alt - lat + lon + yaw;
    PID_Out.backRightMotorPercent = alt + lat - lon + yaw;
    PID_Out.frontRightMotorPercent = alt + lat + lon - yaw;
  } else if (PID_method == 3) {
    // some other mode not yet implemented
  }
  return &PID_Out;
}