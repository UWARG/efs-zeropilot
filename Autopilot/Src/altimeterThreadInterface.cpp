/**
 * Interface implementation from altimeter driver (written in C++) to freertos (C).
 * Author(s): Dhruv Upadhyay
 */


#include "altimeterThreadInterface.h"
#include "altimeter.hpp"

// TODO: The implementations of the functions and integration into threads will be done once the altimeter driver is fully implemented

void AltimeterThreadInterfaceInit(void) {
    static Altimeter& altObj = MPL3115A2::getInstance();
    altObj.Begin_Measuring();

}

void AltimeterThreadInterfaceExecute() {
    static Altimeter& altObj = MPL3115A2::getInstance();
    altObj.Begin_Measuring();
} 