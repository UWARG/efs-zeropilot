// Author: Jingting Liu
#include "Mavlink2_lib/common/mavlink.h"
#include "MavlinkFunctions.hpp"

//#include <mavlink_helpers.h>

mavlink_system_t mavlink_system = {
    1, //System ID (1-255)
    1 // Component ID (MAV_COMPONENT) "1" for System flight controller component ("autopilot")
};

//receiving messages in two phases, parse streams into packets, then decode each packet payload into a C struct (mapping the XML definition)

//MAVLINK_HELPER uint8_t mavlink_parse_char(uint8_t channelID, uint8_t theCharToParse, mavlink_message_t* r_message, mavlink_status_t* r_mavlink_status);
// return 1 is the packet successfully decoded, 0 if not



mavlink_message_t msg;
int channel = MAVLINK_COMM_0; //0, assume only one MAVlink stream exists

bool IsByteAvailable = true;
uint8_t sampleByte = 32;


mavlink_decoding_status_t Mavlink_decoder(int channel, uint8_t incomingByte, mavlink_message_t *msg, uint8_t **telemetryData) //need to add decoded message here
{
    mavlink_status_t status; //TODO make use of this to get detailed status for parsing
    if (mavlink_parse_char(channel, incomingByte, msg, &status))
    {
        // complete message receieved, decoding starts
        switch(msg->msgid)
        {
            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: // ID for GLOBAL_POSITION_INT
                {
                    // Get all fields in payload (into global_position)
                    mavlink_global_position_int_t global_position;
                    mavlink_msg_global_position_int_decode(msg, &global_position); //void

                    //TODO add GPS valid check

                    if (telemetryData == NULL || *telemetryData == NULL )
                    {
                        return MAVLINK_DECODING_NULL_PTR;
                    }
                    memcpy(*telemetryData, (uint8_t*) &global_position, sizeof(mavlink_global_position_int_t));

                    // these are only for a demonstration of how to access the data from global_position, use this to check if GPS is valid
                    uint32_t timestamp_ms = global_position.time_boot_ms; /*< [ms] Timestamp (time since system boot).*/
                    int32_t latitude = global_position.lat; /*< [degE7] Latitude, expressed*/
                    int32_t longitude = global_position.lon; /*< [degE7] Longitude, expressed*/
                    int32_t altitude = global_position.alt; /*< [mm] Altitude (MSL). Note that virtually all GPS modules provide both WGS84 and MSL.*/
                    int32_t relative_altitude = global_position.relative_alt; /*< [mm] Altitude above ground*/
                    int16_t Vx = global_position.vx; /*< [cm/s] Ground X Speed (Latitude, positive north)*/
                    int16_t Vy = global_position.vy; /*< [cm/s] Ground Y Speed (Longitude, positive east)*/
                    int16_t Vz = global_position.vz; /*< [cm/s] Ground Z Speed (Altitude, positive down)*/
                    uint16_t Hdg = global_position.hdg; /*< [cdeg] Vehicle heading (yaw angle), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX*/

                    if (false) // insert GPS data check here, such as valid data range
                    {
                        return MAVLINK_DECODING_FAIL;
                    }
                    else
                    {
                        return MAVLINK_DECODING_OKAY;
                    }
                }

                break;
            case MAVLINK_MSG_ID_GPS_STATUS:
                {
                    // Get just one field from payload
                    uint8_t visible_sats = mavlink_msg_gps_status_get_satellites_visible(msg);
                }
                break;
            default:
                break;
        }   
    }
    //the same return with parsing crc mismatch and bad signature (according to the mavlink_parse_char function)
    return MAVLINK_DECODING_INCOMPLETE;
}

uint16_t Mavlink_encoder(Message_IDs_t type, mavlink_message_t ** msg, const uint64_t *struct_ptr)
{
    uint8_t system_id = 1;
    uint8_t component_id = 1;

    switch(type)
    {
        case MESSAGE_ID_GPS:
        {
            //return mavlink_msg_global_position_int_encode(system_id, component_id, *msg, &global_position);
            return mavlink_msg_global_position_int_encode(system_id, component_id, *msg, (mavlink_global_position_int_t*) struct_ptr);
        
        }
        break;

        default:
            break;
    }
}

int main(void) // TODO: this needs to be removed once integrated
{
    // encoding starts

    mavlink_message_t *encoded_msg;
    mavlink_global_position_int_t global_position = 
    {
        1, // timestamp
        1, // latitude
        1, // longitude
        1, // altitude
        1, // altitude above ground
        1, // x speed, latitude positive north
        1, // y speed, longitude, positive east
        1, // z speed, positive down
        1 // hdg
    };

    uint16_t message_len = Mavlink_encoder(MESSAGE_ID_GPS, &encoded_msg, (const uint64_t*) &global_position);

    // decoding starts

    uint8_t *telemetryData;
    mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE;
    unsigned char* ptr_in_byte = (unsigned char *) encoded_msg; // prepare to read the message one byte at a time
    mavlink_message_t received_encoded_msg;

    for( int i = 0; i < message_len; i++) // assume message_len is in bytes
    {
        uint8_t current_byte = ptr_in_byte[i]; //to get the encoded message one byte at a time, in real application this would each byte received from xbee
        decoderStatus = Mavlink_decoder(MAVLINK_COMM_0, current_byte, &received_encoded_msg, &telemetryData);
    }

    mavlink_global_position_int_t global_position_revieved;
    if (decoderStatus == MAVLINK_DECODING_OKAY)
    {
        printf("decoding complete");
        memcpy(&global_position_revieved, (mavlink_global_position_int_t*) telemetryData, sizeof(mavlink_global_position_int_t));
        int32_t altitude = global_position.alt;
        printf("altitude is: %d", altitude);
    }

    else
    {
        printf("decoding unsuccessful, error message: %d",decoderStatus);
    }
    return 0;

}




// use mavlink_msg_command_int_decode()/mavlink_msg_command_long_decode() for command decodings
// use MAVLINK_CHECK_MESSAGE_LENGTH to check for invalid messages
// unit test: https://uwarg-docs.atlassian.net/wiki/spaces/TUT/pages/1058078728/test+driven+development

//https://stackoverflow.com/questions/484357/trying-to-copy-struct-members-to-byte-array-in-c
// https://stackoverflow.com/questions/8680220/how-to-get-the-value-of-individual-bytes-of-a-variable

//The minimum packet length is 12 bytes for acknowledgment packets without payload.
//The maximum packet length is 280 bytes for a signed message that uses the whole payload.

// mavlink encoded message divided in bytes https://mavlink.io/en/guide/serialization.html#payload

