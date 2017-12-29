/*
 * File:   ProgramStatus.c
 * Author: Chris Hajduk
 *
 * Created on March 21, 2016, 3:40 PM
 */
#include "main.h"
#include "ProgramStatus.h"
#include "debug.h"

char sensorState[NUM_SENSORS];
int programState;

void setSensorStatus(char sensor, char status){
    if (sensor < NUM_SENSORS){
        sensorState[(int)sensor] = status;

#ifdef DEBUG
        if (status & SENSOR_CONNECTED){
            debug("Sensor %d is connected", sensor);
        }
        else if (status & SENSOR_INITIALIZED){
            debug("Sensor %d is initialized", sensor);
        }
#endif
    }
    else{
    //Display Error Message
#ifdef DEBUG
        debug("Invalid Sensor ID");
#endif
    }
}

char getSensorStatus(char sensor){
    if (sensor < NUM_SENSORS){
        return sensorState[(int)sensor];
    }
    else{
    //Display Error Message
#ifdef DEBUG
        debug("Invalid Sensor ID");
#endif
        return -1;
    }
}

void setProgramStatus(int status){
    programState = status;

#ifdef DEBUG
    if (status == INITIALIZATION) {
        debug("Attitude Manager Initialization");
    } else if (status == UNARMED) {
        debug("Vehicle is Unarmed. Waiting for arm.");
    } else if (status == ARMING) {
        debug("Arming: Motor Startup Procedure Started");
    } else if (status == MAIN_EXECUTION) {
        debug("Motor Startup Procedure Complete");
        debug("Autopilot Running...");
    } else if (status == KILL_MODE_WARNING) {
        debug("I am contemplating suicide.");
    } else if (status == KILL_MODE) {
        debug("I am attempting to destroy myself now.");
    }

#endif
}
int getProgramStatus(){
    return programState;
}
