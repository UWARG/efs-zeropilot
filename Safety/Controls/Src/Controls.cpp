//
// Controls.cpp
// Created by antho on 10/27/2021.
//

#include "Controls.hpp"

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

void run_controls(){
    // main entry point to run controls
}
