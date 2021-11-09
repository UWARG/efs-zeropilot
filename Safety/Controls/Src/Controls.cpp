//
// Controls.cpp
// Created by antho on 10/27/2021.
//

/* Inputs:
 * - Global position -> not sure who will track diff from prev
 * - New set values
 * - New set value tick
*/

/* Outputs:
 * Literally 4 pwm values :D
*/

/* Some notes
 * Do we need to worry about tracking drift error in the IMU? -> would it be preferable if we did?
 * Do we store current heading data? How do we store this.....
 *  -> would we use this data later to generate better curves and splines?
*/