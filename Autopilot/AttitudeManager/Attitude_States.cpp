/*
* Attitude State CPP file
*/
#include "Attitude_States.hpp"
#include "IMU.hpp"
#include "airspeed.hpp"
#include "PID.h"
#include "OutputMixing.hpp"
#include "SendInstructionsToSafety.hpp"
#include "SensorFusion.hpp"
/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
enum attitudeManagerState {
    pathManagerInputMode,
    PIDloopMode,
    sendToSafetyMode,
    failedMode
};
attitudeManagerState currentState;
/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/
static attitudeManagerState pathManagerInput(void);
static attitudeManagerState PIDloop(void);
static attitudeManagerState sendToSafety(void);
static attitudeManagerState failed(void);
/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/
void initAttitudeState()
{
    //Initialize sensor classes via smart pointer 
    //Initialize PID class
    PIDController PID; 
    //Initialize SendtoSafety Module
    SendToSafety_Init();
}