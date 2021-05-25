/**
 * The c++ standard does not include pi, e, or a bunch of other useful math constants.
 * This file contains all there definitions, yet is safe to use even in cases where particular compilers have some, or all these constants already defined.
 * This file also includes a few useful math functions, like converting radians to degrees and vice versa.
 * Author: Anthony Berbari
 */

#pragma once

/***********************************************************************************************************************
 * Constants
 **********************************************************************************************************************/

#ifndef ZP_PI 
    constexpr double ZP_PI = 3.14159265358979311599796346854;
#endif

#ifndef ZP_E 
    constexpr double ZP_E = 2.71828182845904523536028747135;
#endif

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

#ifndef DEG_TO_RAD
    #define DEG_TO_RAD(angleInDegrees) ((angleInDegrees) * ZP_PI / 180.0)
#endif

#ifndef RAD_TO_DEG
    #define RAD_TO_DEG(angleInRadians) ((angleInRadians) * 180.0 / ZP_PI)
#endif
