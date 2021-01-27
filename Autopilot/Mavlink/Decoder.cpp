// Author: Jingting Liu
#include <common/mavlink.h>

mavlink_system_t mavlink_system = {
    1, //System ID (1-255)
    1 // Component ID (a MAV_COMPONENT value, most info refer to common.xml)
}

// https://mavlink.io/en/mavgen_c/

//receiving messages in two phases, parse streams into packets, then decode each packet payload into a C struct (mapping the XML definition)

MAVLINK_HELPER uint8_t mavlink_parse_char(uint8_t channelID, uint8_t theCharToParse, mavlink_message_t* r_message, mavlink_status_t* r_mavlink_status)
// return 1 is the packet successfully decoded, 0 if not


mavlink_status_t status;
mavlink_message_t msg;
int chan = MAVLINK_COMM_0;

while(serial.bytesAvailable > 0)
{
    uint8_t byte = serial.getNextByte();
    if (mavlink_parse_char(chan, byte, &msg, &status))
    {
        printf("Received message with ID %d, sequence: %d from component %d of system %d\n", msg.msgid, msg.seq, msg.compid, msg.sysid);
        // ... DECODE THE MESSAGE PAYLOAD HERE ...
        switch(msg.msgid)
        {
            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: // ID for GLOBAL_POSITION_INT
                {
                // Get all fields in payload (into global_position)
                mavlink_msg_global_position_int_decode(&msg, &global_position);

                }
                break;
            case MAVLINK_MSG_ID_GPS_STATUS:
                {
                // Get just one field from payload
                visible_sats = mavlink_msg_gps_status_get_satellites_visible(&msg);
                }
                break;
            default:
                break;
        }   
    }
}

// use mavlink_msg_command_int_decode()/mavlink_msg_command_long_decode() for command decodings
// use MAVLINK_CHECK_MESSAGE_LENGTH to check for invalid messages

// example
// https://mavlink.io/en/mavgen_c/example_c_uart.html