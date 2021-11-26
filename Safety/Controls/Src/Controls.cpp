//
// Controls.cpp
// Keeps the drone in the air. Updating target position will cause drone to fly towards position
//

#include "Controls.hpp"
#include <cmath>

#include "../../Inc/PPM.hpp"
#include "../../Inc/PWM.hpp"

#include "../../SensorFusion/Inc/sensorFusion.hpp"

#include "base_pid.hpp"


/* Inputs:
 * - Global position -> not sure who will track diff from prev
 * - RC inputs
*/

/* Outputs:
 *      - PWM values
*/

/* Some notes
 * Do we need to worry about tracking drift error in the IMU? -> would it be preferable if we did?
 * Do we store current heading data? How do we store this.....
 *  -> would we use this data later to generate better curves and splines?
 * Define traditional XYZ coordinates: x forward, y left, z up, cw +
*/

// static volatile global position.
// follows formats from SFOutput_t
typedef struct {
    long double latitude, longitude;    //meters from where we started originally
    float altitude;                     // meters above start point (hopefully the ground?)
    double heading;                     // degree from true north, cw
} PositionTargets;

typedef struct {
    float f_stick, lr_stick;
    float a_stick;
    float a_heading;
} StickDistance;

static SFOutput_t curr_sf; // current
static SFOutput_t temp_sf; // temp new targs
static SFError_t sf_err;   // sf err

static PositionTargets pos_targ;

long double x_targ; // x target
long double y_targ; // y target
float a_targ;       // altitude target
double h_targ;      // heading target

StickDistance translatePPM(){
    // translates PPM into distances to move in each direction wrt % max
}

void updateTargets(float f_stick, float lr_stick, float a_stick, float a_heading) {
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

    updatePosition();

    pos_targ.latitude   = f_stick * cos(curr_sf.heading) + lr_stick * sin(curr_sf.heading) + curr_sf.latitude;
    pos_targ.longitude  = f_stick * sin(curr_sf.heading) + lr_stick * cos(curr_sf.heading) + curr_sf.longitude;

    pos_targ.altitude   = curr_sf.altitude + a_stick;

    pos_targ.heading    = curr_sf.heading + a_heading;
}

void updatePosition() {
    // update the current position of the drone from SF

    sf_err = SF_GetResult(&temp_sf);

    if(sf_err.errorCode == 0) {
        // double checks that the SF output is correct before assigning new values.
        curr_sf = temp_sf;
    }
}

void evalControls(){
    // main entry point to run/evaluate controls.

    // run PID's on our target positions.

}
