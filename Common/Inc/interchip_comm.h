/*
* Interchip packets
* Packets should be a multiple of 16 bits
*/

typedef struct
{
  uint16_t PWM[12];
  uint16_t safety_level;
} Interchip_StoA_Packet;    //Safety to Autopilot packet

typedef struct
{
  uint16_t PWM[12];
  uint16_t autonomous_level;
} Interchip_AtoS_Packet;    //Autopilot to Safety packet