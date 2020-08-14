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

static void pathManagerInput()
{
    PMCommands Commands;
    int pmErrorState = PM_Commands(Commands); //dummy commands call for now?
    currentState = PIDloopMode;
}

static void PIDloop()
{
    int error = 0;
    PID->PIDControl(error);
    currentState = sendToSafetyMode;
}

static void sendToSafety()
{
    float channelOut = 0; //default value
    int errorState = OutputMixing_Execute(); //requires arguements!!
    SendToSafety_Execute(channelOut); //sends the instructions to safety. Dumb q: why is it not liking me passing in the float?
    currentState = pathManagerInputMode;
}