/**
 * @file PID.cpp
 * @author Ian Frosst
 * @date  March 26, 2017
 * @copyright Waterloo Aerial Robotics Group 2017 \n
 *   https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */

#include "PID.h"

/**
 * Filtering constant for derivative. Between 0 and 1.
 * The larger this is, the more twitchy D control is.
 */
#define FILTER (0.4f)

/* Generic PID functions. Can be used to PID other things (flaps, etc) */


PIDController::PIDController(float _kp, float _ki, float _kd, float _scale, int16_t _i_max){
  kp = _kp;
  kd = _kd;
  ki = _ki;
  i_max = _i_max;
  scale = _scale;

  integral = 0;
  last_time = 0;
  last_err = 0;
  last_der = 0;
}

// PID loop function. error is (setpointValue - currentValue)
float PIDController::PIDControl(float error) {
  float output = 0;

  uint64_t now = 0;  // getTimeUs(); TODO
  uint32_t delta_usec = (now - last_time);

  // check if we've gone too long without updating (keeps the I and D from
  // freaking out)
  if (delta_usec > PID_RESET_TIME || last_time == 0) {
    delta_usec = 0;
    integral = 0;
    last_err = error;
  }

  last_time = now;

  output += kp * error;  // Proportional control

  if (delta_usec > 500) {  // only compute time-sensitive control if time has
                           // elapsed (more then 500 us)
    float dTime = delta_usec / 1e6f;  // elapsed time in seconds

    if (fabsf(ki) > 0) {  // Integral control
      integral += (ki * error) * dTime;

      if (integral < -i_max) {  // ensure integral stays manageable
        integral = -i_max;
      } else if (integral > i_max) {
        integral = i_max;
      }
      output += integral;
    }

    if (fabsf(kd) > 0) {  // Derivative control
      float derivative = (error - last_err) / dTime;
      derivative =
          derivative * FILTER +
          last_der * (1 - FILTER);  // reduce jitter in derivative by averaging
      last_err = error;
      last_der = derivative;
      output += kd * derivative;
    }
  }
  return output * scale;
}