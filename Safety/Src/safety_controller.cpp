#include "safety_controller.hpp"
#include "PWM.hpp"
#include "stdlib.h"
//#include "Interchip.h"
#include "interchip_S.hpp"

static void setPWMChannel(PWMChannel &pwm, int channel, int percentage);
static int getPPM(PPMChannel &ppm, int channel);



//static Interchip_AtoS_Packet *dataRX;
//static Interchip_StoA_Packet *dataTX;

void safety_controller_init()
{

    //dataTX = (Interchip_StoA_Packet*) malloc(sizeof(Interchip_StoA_Packet));
    //dataRX = (Interchip_AtoS_Packet*) malloc(sizeof(Interchip_AtoS_Packet));
    //Interchip_Init(dataTX, dataRX);
}

void safety_run(PWMChannel &pwm, PPMChannel &ppm)
{
    if(!isSafetyManual())
    {
        for(int channel = 0; channel < 8; channel++)
        {
            //setPWMChannel(pwm, channel, dataRX->PWM[channel]);
        }
    }
    else
    {
        for(int channel = 0; channel < 8; channel++)
        {
            setPWMChannel(pwm, channel, (uint32_t) getPPM(ppm, channel));
        }
    }

}


/*
* Sets an individual PWM channel to the desired output.
*/
static void setPWMChannel(PWMChannel &pwm, int channel, int percentage)
{
    //Input protection
    if(percentage > 100) {percentage = 100;}
    if(percentage < 0) {percentage = 0;}
    pwm.set(channel, percentage);
}

bool isSafetyManual()
{
    /*
    *   TODO: ADD SYSTEM TO MAKE SAFETY MANUAL!
    */
    //Not sure what the implementation is for now...?
    return true;
}

static int getPPM(PPMChannel &ppm, int channel)
{
    return ppm.get(channel);
}
