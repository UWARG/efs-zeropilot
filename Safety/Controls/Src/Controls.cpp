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
    float h_stick;
} StickDistance;


// temporary structs which will be imported from AM soon.
typedef struct
{
    float motor1Percent;
    float motor2Percent;
    float motor3Percent;
    float motor4Percent;

} PID_Output_t;

typedef struct
{
    float input1;
    float input2;
    float input3;
    float input4;
} Instruction_t;

static SFOutput_t curr_sf; // current
static SFOutput_t temp_sf; // temp new targs
static SFError_t sf_err;   // sf err

static PositionTargets pos_targ;

long double x_targ; // x target
long double y_targ; // y target
float a_targ;       // altitude target
double h_targ;      // heading target

StickDistance *translatePPM(Instruction_t * instructions){
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
    int f_stick = *stick->f_stick;
    int lr_stick = *stick->lr_stick;
    int a_stick = *stick->a_stick;
    int h_stick = *stick->h_stick;

    updatePosition();

    pos_targ.latitude   = f_stick * cos(curr_sf.heading) + lr_stick * sin(curr_sf.heading) + curr_sf.latitude;
    pos_targ.longitude  = f_stick * sin(curr_sf.heading) + lr_stick * cos(curr_sf.heading) + curr_sf.longitude;

    pos_targ.altitude   = curr_sf.altitude + a_stick;

    pos_targ.heading    = curr_sf.heading + h_stick;
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

    // to be used in the future!
}

/*
 * pwmValues = runControlsAndGetPWM(instructions, SF_position)
 */

PID_Output_t *runControlsAndGetPWM(Instruction_t * instructions, SFOutput_t * SF_pos, bool NEWDATA) {
    // to use or not to use pointers?
    curr_sf = *SF_pos;

    static float dist_lat; // latitude
    static float dist_lon; // longitude
    static float dist_alt; // altitude
    static float angl_hdn; // heading

    if (NEWDATA):
        StickDistance *internal_targets = translatePPM(instructions);

    updateTargets(internal_targets);

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
     * pid head
     * ===============
     * mux signals and send back out
     */

    dist_lat = pos_targ.latitude - curr_sf.latitude;
    dist_lon = pos_targ.longitude - curr_sf.longitude;
    dist_alt = pos_targ.altitude - curr_sf.altitude;
    angl_hdn = pos_targ.heading - curr_sf.heading;

    // calculate and run through PID's or just simple difference....?
    // ensure some safety somewhere?
}