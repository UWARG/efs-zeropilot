// Author: Jingting Liu
#include "Mavlink2_lib/common/mavlink.h"
#include "Decoder.hpp"
//#include <mavlink_helpers.h>

mavlink_system_t mavlink_system = {
    1, //System ID (1-255)
    1 // Component ID (MAV_COMPONENT) "1" for System flight controller component ("autopilot")
};

//receiving messages in two phases, parse streams into packets, then decode each packet payload into a C struct (mapping the XML definition)

//MAVLINK_HELPER uint8_t mavlink_parse_char(uint8_t channelID, uint8_t theCharToParse, mavlink_message_t* r_message, mavlink_status_t* r_mavlink_status);
// return 1 is the packet successfully decoded, 0 if not


mavlink_status_t status;
mavlink_message_t msg;
int channel = MAVLINK_COMM_0; //0, assume only one MAVlink stream exists

bool IsByteAvailable = true;
uint8_t sampleByte = 32;

/**
 * This decoder is consists of two parts, parser and decoder.
 * The parser handles mavlink messages one byte at a time. 
 * Once the complete packet could be successfully decoded, the decoder would translate the message into telemetry data.
 * 
 * An example of how this function should be called is demonstrated below:
 * 
 * #include <mavlink.h>
 *
 * mavlink_status_t status;
 * mavlink_message_t msg;
 * int channel = MAVLINK_COMM_0; //0, assume only one MAVlink stream exists
 *
 * //the following part should exist in the state machine
 * while(serial.bytesAvailable > 0) // should make the xbee send one byte at a time from the serial port
 * {
 *   uint8_t byte = serial.getNextByte(); //this is just a space filler arduino function
 *   decoderStatus = Mavlink_decoder(channel, byte ,&msg, &status)
 *   printf("Received message with ID %d, sequence: %d from component %d of system %d\n", msg.msgid, msg.seq, msg.compid, msg.sysid);
 * }
 **/
int Mavlink_decoder(int channel, uint8_t incomingByte, mavlink_message_t &msg, mavlink_status_t &status, uint8_t **telemetryData) //need to add decoded message here
{

    if (mavlink_parse_char(channel, incomingByte, &msg, &status))
    {
        // complete message receieved, decoding starts
        switch(msg.msgid)
        {
            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: // ID for GLOBAL_POSITION_INT
                {
                // Get all fields in payload (into global_position)
                mavlink_global_position_int_t global_position;
                mavlink_msg_global_position_int_decode(&msg, &global_position); //void

                //TODO add GPS valid check

                if (telemetryData == NULL || *telemetryData == NULL )
                {
                    return MAVLINK_DECODING_NULL_PTR;
                }
                memcpy(*telemetryData, (uint8_t*) &global_position, sizeof(mavlink_global_position_int_t))
                // insert GPS check


                
                }
                break;
            case MAVLINK_MSG_ID_GPS_STATUS:
                {
                // Get just one field from payload
                uint8_t visible_sats = mavlink_msg_gps_status_get_satellites_visible(&msg);
                }
                break;
            default:
                break;
        }   
    }
    else
    {
        return MAVLINK_DECODING_INCOMPLETE;
        //the same return with parsing crc mismatch and bad signature (according to the mavlink_parse_char function)
    }
    
}
int main(void)
{

}
// use mavlink_msg_command_int_decode()/mavlink_msg_command_long_decode() for command decodings
// use MAVLINK_CHECK_MESSAGE_LENGTH to check for invalid messages

// example
// https://mavlink.io/en/mavgen_c/example_c_uart.html