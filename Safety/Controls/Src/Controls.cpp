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
static SFOutput_t temp_sf;  // temp new targs
static SFError_t sf_err;    // sf err
static PID_Output_t PID_Out;

static PositionTargets pos_targ;

long double x_targ;  // x target
long double y_targ;  // y target
float a_targ;        // altitude target
double h_targ;       // heading target

/** Determines whether to use P-loop, or PID, or something else.
 * 0 = base_pid
 * 1 = simple_p
 */
const int PID_method = 0;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

StickDistance *translatePPM(Instructions_t *instructions) {
  // translates PPM into distances to move in each direction wrt % max
}

void updateTargets(StickDistance *stick) {
  // update target position to go to.
  // todo: make inputs not temporary floats (maybe a struct?)

  /** Drone coordinates:
   * positive x forward
   * positive y left
   ** SF Coordinates:
   * positive North, East, Up.
   * newx = + forward cos(heading) + leftright sin(heading)
   * newy = + forward sin(heading) + leftright cos(heading)
   * ~ might have to flip LR to match SF? ~
   */
  int f_stick = stick->f_stick;
  int lr_stick = stick->lr_stick;
  int a_stick = stick->a_stick;
  int h_stick = stick->h_stick;

  updatePosition();

  pos_targ.latitude = f_stick * cos(curr_sf.heading) +
                      lr_stick * sin(curr_sf.heading) + curr_sf.latitude;
  pos_targ.longitude = f_stick * sin(curr_sf.heading) +
                       lr_stick * cos(curr_sf.heading) + curr_sf.longitude;

  pos_targ.altitude = curr_sf.altitude + a_stick;

  pos_targ.heading = curr_sf.heading + h_stick;
}

void updatePosition() {
  // update the current position of the drone from SF

  sf_err = SF_GetResult(&temp_sf);

  if (sf_err.errorCode == 0) {
    // double checks that the SF output is correct before assigning new values.
    curr_sf = temp_sf;
  }
}

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

  StickDistance *internal_targets = translatePPM(instructions);

  // updateTargets(internal_targets);

  /**
   * Now we need to get distances and run pids.
   * dist lat
   * dist lon
   * dist alt
   * angl hdn
   * ===============
   * pid lat
   * pid lon
   * pid alt
   * pid hdn
   * ===============
   * mux signals and send back out
   */

  static float pid_x;
  static float pid_y;
  static float pid_a;
  static float pid_h;

  static float dist_lat;  // latitude
  static float dist_lon;  // longitude
  static float dist_alt;  // altitude
  static float angl_hdn;  // heading

  static float translate_lat;
  static float translate_lon;

  dist_lat = pos_targ.latitude - curr_sf.latitude;
  dist_lon = pos_targ.longitude - curr_sf.longitude;
  dist_alt = pos_targ.altitude - curr_sf.altitude;
  angl_hdn = pos_targ.heading - curr_sf.heading;

  translate_lat =
      -dist_lon * sin(curr_sf.heading) + dist_lat * cos(curr_sf.heading);
  translate_lon =
      dist_lon * cos(curr_sf.heading) + dist_lat * sin(curr_sf.heading);

  // PIDController controller(float _kp, float _ki, float _kd, float _i_max,
  // float _min_output, float _max_output);

  PIDController x_pid{0.7, 0.3, 0.2, 100, 10, 100};
  PIDController y_pid{0.7, 0.3, 0.2, 100, 10, 100};
  PIDController a_pid{1, 0.2, 0.2, 100, 20, 100};
  PIDController h_pid{1, 0.2, 0.2, 100, 20, 100};

  // {...}

  // calculate and run through PID's or just simple difference....?
  // ensure some safety somewhere?

  if (PID_method == 0) {
    // use the base pid written without extensive scaling.

    // currents are all 0 because we reset the frame with reference to the
    // quadcopter.

    pid_x = x_pid.execute(translate_lon, 0);
    pid_y = -y_pid.execute(
        translate_lat,
        0);  // global and airframe positive left/right are flipped
    pid_a = a_pid.execute(dist_alt, 0);
    pid_h = h_pid.execute(angl_hdn, 0);
  } else if (PID_method == 1) {
    // use a simple p-loop integrator
  } else {
    // not sure yet
  }

  //    // to return PID
  //    TEMPORARY_OUT_REMOVE_THIS.backLeftMotorPercent = instructions->input3;
  //    TEMPORARY_OUT_REMOVE_THIS.frontLeftMotorPercent = instructions->input3;
  //    TEMPORARY_OUT_REMOVE_THIS.backRightMotorPercent = instructions->input3;
  //    TEMPORARY_OUT_REMOVE_THIS.frontRightMotorPercent = instructions->input3;

  // =================================
  // PID Testing Code Begins here.
  // =================================

  // PIDController controller(float _kp, float _ki, float _kd, float _i_max,
  // float _min_output, float _max_output);
  PIDController pid_test{
      0.4, 0, 0.1, 80,
      -100,  100};  // defining our test PID with a lower max output for now (bring
                // this up as testing needs).

  // get PID result
  float test_pid = pid_test.execute(
      0, curr_sf.roll, curr_sf.rollRate); // change yaw and yawRate to correct measurement.
  // yawRate may not be usable. If you don't put in yawRate PID will automatically calculate derivative

  // scalars that we will use to adjust bias linearly.
  float M_Base = instructions->input3;    // some amount of BASE pid to keepmotors above threshold
  float M_Scale = 0.5;  // some amount of scaling for the PID result (scale this
                        // down as PID output range increases)

  // mix the PID's. Not sure yet if there wants to be a "soft limit" of how
  // negative the PID can be, e.g. M_Base - M_Scale * Max(test_pid, SET_VALUE)
  // in order to keep motors spinning. Won't be an issue with DSHOT
  PID_Out.backLeftMotorPercent = M_Base - M_Scale * test_pid;
  PID_Out.frontLeftMotorPercent = M_Base - M_Scale * test_pid;
  PID_Out.backRightMotorPercent = M_Base + M_Scale * test_pid;
  PID_Out.frontRightMotorPercent = M_Base + M_Scale * test_pid;
  return &PID_Out;
}