/*
* Interchip packets
* Packets should be a multiple of 16 bits
*/
#include "Common.h"

typedef struct
{
  int16_t PWM[PWM_NUM_CHANNELS];
  uint16_t safety_level;
} Interchip_StoA_Packet;    //Safety to Autopilot packet

typedef struct
{
  int16_t PWM[PWM_NUM_CHANNELS];
  uint16_t autonomous_level;
} Interchip_AtoS_Packet;    //Autopilot to Safety packet