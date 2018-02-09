/**
 * @file PID.h
 * @author Ian Frosst
 * @date  March 26, 2017
 * @copyright Waterloo Aerial Robotics Group 2017 \n
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */

#ifndef PID_H
#define	PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define PID_RESET_TIME (500000) // timeout to reset I and D terms (us)

class PIDController {
 public:
  PIDController(float kp, float ki, float kd, float _scale, int16_t i_max);
  float PIDControl(float error);

 private:
  float kp, kd, ki;
  int16_t i_max;

  uint64_t last_time;  // for derivative control
  float last_err;
  float last_der;  // last derivative, for filtering
  float integral;
  float scale;
};

#ifdef __cplusplus
}
#endif

#endif	/* PID_H */

