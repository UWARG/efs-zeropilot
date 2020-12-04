/*
* Interchip packets
* Packets should be a multiple of 16 bits
*/


typedef struct {
	int16_t PWM[12];
	uint16_t autonomous_level;
} Interchip_StoA_Packet;    //Safety to Autopilot packet

typedef struct {
	int16_t PWM[12];
	uint16_t autonomous_level;
} Interchip_AtoS_Packet;    //Autopilot to Safety packet

