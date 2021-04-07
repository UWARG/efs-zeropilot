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


/*
mavlink_message_t msg;
int channel = MAVLINK_COMM_0; //0, assume only one MAVlink stream exists
*/

mavlink_decoding_status_t Mavlink_decoder(int channel, uint8_t incomingByte, uint8_t *telemetryData) //need to add decoded message here
{
    mavlink_status_t status; //TODO make use of this to get detailed status for parsing
    mavlink_message_t decoded_msg;
    mavlink_parse_char(channel, incomingByte, &decoded_msg, &status);

    if (status.msg_received)
    //if (mavlink_parse_char(channel, incomingByte, msg, &status)) // the msg is the decoded message on success
    {
        // complete message receieved, decoding starts
        printf("message receieved! msg id is: %d\n", decoded_msg.msgid);
        /*
        if (decoded_msg == NULL)
        {
            printf("null ptr warning\n");
        */
        switch(decoded_msg.msgid)
        {
            printf("pre decoding\n");
            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: // ID for GLOBAL_POSITION_INT, 33
                {
                    //printf("start decoding GPS for byte: %hhx\n", incomingByte);
                    // Get all fields in payload (into global_position)
                    mavlink_global_position_int_t global_position;
                    
                    mavlink_msg_global_position_int_decode(&decoded_msg, &global_position); //void
                    printf("varifying decoding\n");

                    //TODO add GPS valid check

                    if (telemetryData == NULL) // || *telemetryData == NULL 
                    {
                        printf("null pointer warning");
                        return MAVLINK_DECODING_NULL_PTR;
                    }
                    memcpy((void*) telemetryData, (void*) &global_position, sizeof(mavlink_global_position_int_t)); //added (void*) for telemetry for test

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
                    printf("timestamp: %d, latitude: %d longitude %d, hdg: %d\n",timestamp_ms, latitude, longitude, Hdg);
                    

                    if (timestamp_ms == 1 && Hdg == 9) // insert GPS data check here, such as valid data range
                    {
                        return MAVLINK_DECODING_OKAY;
                    }
                    else
                    {
                        return MAVLINK_DECODING_FAIL;
                    }
                }

                break;
            case MAVLINK_MSG_ID_GPS_STATUS:
                {
                    // Get just one field from payload
                    uint8_t visible_sats = mavlink_msg_gps_status_get_satellites_visible(&decoded_msg);
                }
                break;
            default:
                break;
        }
        
    }

    //the same return with parsing crc mismatch and bad signature (according to the mavlink_parse_char function)
    return MAVLINK_DECODING_INCOMPLETE;
}

uint16_t Mavlink_encoder(Message_IDs_t type, mavlink_message_t **msg, const uint8_t *struct_ptr) //TODO change this msg handle
{
    uint8_t system_id = 1;
    uint8_t component_id = 1;
    mavlink_message_t encoded_msg_original;

    switch(type)
    {
        case MESSAGE_ID_GPS:
        {
            //return mavlink_msg_global_position_int_encode(system_id, component_id, msg, &global_position);
            uint16_t message_len = mavlink_msg_global_position_int_encode(system_id, component_id, &encoded_msg_original, (mavlink_global_position_int_t*) struct_ptr);

            mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE;
            mavlink_global_position_int_t global_position_decoded;

            unsigned char* ptr = (unsigned char *) &encoded_msg_original;
            for( int i = 2; i < message_len+2; i++)
            {
                char current_byte = ptr[i];
                printf("encoding byte: %d / %d   |   current byte : %hhx\n", i-2, message_len,current_byte);
            }

            unsigned char* ptr_in_byte = (unsigned char *) &encoded_msg_original;
            char message_buffer[message_len];
            for( int i = 0; i < message_len; i++)
            {
                if (ptr_in_byte == NULL)
                {
                    //null ptr 
                }
                else if (ptr_in_byte[i] == 0xfd) //0xfd, starting byte
                {
                    //ptr_in_byte += i;
                    printf("shifting : %d\n", i);
                    //char current_byte = message_buffer[i];
                    //memcpy(message_buffer, &ptr_in_byte[i], sizeof(message_len));
                    for(int r = 0; r < message_len; r++)
                    {
                        message_buffer[r] = ptr_in_byte[r+i];
                        printf("copying byte: %d / %d   |   current byte : %hhx\n", r, message_len,message_buffer[r]);
                        decoderStatus = Mavlink_decoder(MAVLINK_COMM_0, message_buffer[r], (uint8_t*) &global_position_decoded);
                    }
                    break;
                }
            }

            *msg = (mavlink_message_t*) &message_buffer;

            //return message_len;
///*

            if (decoderStatus == MAVLINK_DECODING_OKAY)
            {
                printf("decoding complete\n");
                int32_t altitude = global_position_decoded.alt;
                printf("altitude is: %d\n", altitude);
            }

            else
            {
                printf("decoding unsuccessful, error message: %d\n",decoderStatus);
            }
            return message_len;
//*/
        
        }
        break;

        default:
            break;
    }
}

int main(void) // TODO: this main needs to be removed once integrated
{
    // encoding starts

    mavlink_message_t *encoded_msg = 0;
    mavlink_global_position_int_t global_position =  //sizeof(global_position) = 28
    {
        1, // timestamp
        2, // latitude
        3, // longitude
        4, // altitude
        5, // altitude above ground
        6, // x speed, latitude positive north
        7, // y speed, longitude, positive east
        8, // z speed, positive down
        9 // hdg
    };

    uint16_t message_len = Mavlink_encoder(MESSAGE_ID_GPS, &encoded_msg, (const uint8_t*) &global_position);
/*
    // decoding starts
    printf("decoding starts");
    //uint8_t *telemetryData;
    mavlink_global_position_int_t global_position_decoded;
    mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE;

    //mavlink_message_t encoded_msg_new;
    unsigned char* ptr_in_byte = (unsigned char *) encoded_msg; // prepare to read the message one byte at a time
    //mavlink_message_t received_encoded_msg;

    for( int i = 0; i < message_len; i++) // message_len in bytes by default
    {
        char current_byte = ptr_in_byte[i]; //to get the encoded message one byte at a time, in real application this would each byte received from xbee
        printf("decoding byte: %d, out of %d |||| current byte: %hhx\n",i, message_len,current_byte);
        
        decoderStatus = Mavlink_decoder(MAVLINK_COMM_0, current_byte, (uint8_t*) &global_position_decoded);

    }

    //mavlink_global_position_int_t global_position_revieved;
    if (decoderStatus == MAVLINK_DECODING_OKAY)
    {
        printf("decoding complete\n");
        //memcpy(&global_position_revieved, (mavlink_global_position_int_t*) telemetryData, sizeof(mavlink_global_position_int_t));
        int32_t altitude = global_position_decoded.alt;
        printf("altitude is: %d\n", altitude);
    }

    else
    {
        //printf("decoding unsuccessful, error message: %d\n",decoderStatus);
    }
*/
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


// to run the program:
//gcc -g MavlinkFunctions.cpp
//a.out



/*

    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_global_position_int_t packet_in = {
        963497464,963497672,963497880,963498088,963498296,18275,18379,18483,18587
    };
    mavlink_global_position_int_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_boot_ms = packet_in.time_boot_ms;
        packet1.lat = packet_in.lat;
        packet1.lon = packet_in.lon;
        packet1.alt = packet_in.alt;
        packet1.relative_alt = packet_in.relative_alt;
        packet1.vx = packet_in.vx;
        packet1.vy = packet_in.vy;
        packet1.vz = packet_in.vz;
        packet1.hdg = packet_in.hdg;
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_global_position_int_encode(1, 1, &msg, &packet1);
    mavlink_msg_global_position_int_decode(&msg, &packet2);
    int ret = memcmp(&packet1, &packet2, sizeof(packet1));
    printf ("return value for decoder: %d", ret);
    uint8_t message_len = sizeof(packet1);

*/