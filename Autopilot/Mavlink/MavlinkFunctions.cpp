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

mavlink_decoding_status_t Mavlink_decoder(int channel, uint8_t incomingByte, uint8_t *telemetryData)
{
    mavlink_status_t status;
    mavlink_message_t decoded_msg;
    mavlink_parse_char(channel, incomingByte, &decoded_msg, &status);

    if (status.msg_received)
    {
        // complete message receieved, decoding starts
        //printf("message receieved! msg id is: %d\n", decoded_msg.msgid);
        
        switch(decoded_msg.msgid)
        {
            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: // ID for GLOBAL_POSITION_INT, 33
                {

                    mavlink_global_position_int_t global_position;
                    
                    mavlink_msg_global_position_int_decode(&decoded_msg, &global_position);

                    if (telemetryData == NULL) // || *telemetryData == NULL 
                    {
                        printf("null pointer warning");
                        return MAVLINK_DECODING_NULL_PTR;
                    }
                    
                    // the following are only for a demonstration of how to access the data from global_position
                    //uint32_t timestamp_ms = global_position.time_boot_ms; /*< [ms] Timestamp (time since system boot).*/
                    //int32_t latitude = global_position.lat; /*< [degE7] Latitude, expressed*/
                    //int32_t longitude = global_position.lon; /*< [degE7] Longitude, expressed*/
                    //int32_t altitude = global_position.alt; /*< [mm] Altitude (MSL). Note that virtually all GPS modules provide both WGS84 and MSL.*/
                    //int32_t relative_altitude = global_position.relative_alt; /*< [mm] Altitude above ground*/
                    //int16_t Vx = global_position.vx; /*< [cm/s] Ground X Speed (Latitude, positive north)*/
                    //int16_t Vy = global_position.vy; /*< [cm/s] Ground Y Speed (Longitude, positive east)*/
                    //int16_t Vz = global_position.vz; /*< [cm/s] Ground Z Speed (Altitude, positive down)*/
                    uint16_t Hdg = global_position.hdg; /*< [cdeg] Vehicle heading (yaw angle), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX*/
                    //printf("timestamp: %d, latitude: %d longitude %d, hdg: %d\n",timestamp_ms, latitude, longitude, Hdg);
                    
                    // someone needs to change this to an actual GPS check, e.g. check for a valid range
                    if (Hdg == 9) //to be changed
                    {
                        printf("decoding successful\n");
                        memcpy((void*) telemetryData, (void*) &global_position, sizeof(mavlink_global_position_int_t));
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


//uint16_t Mavlink_encoder(Message_IDs_t type, mavlink_message_t **msg, const uint8_t *struct_ptr) 
mavlink_message_t * Mavlink_encoder(Message_IDs_t type, const uint8_t *struct_ptr)
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

            printf("size of the message_t: %lu, length of the message: %d\n", sizeof(mavlink_message_t), message_len);

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

            mavlink_message_t* msg = (mavlink_message_t*) message_buffer;
            //mavlink_message_t* msg = 0;
            //memcpy(msg, message_buffer, sizeof(message_len));
            return msg;
        }
        break;

        default:
            break;
    }
}

int main(void) // TODO: this main needs to be removed once integrated
{
    // encoding starts

    //mavlink_message_t *encoded_msg ;
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

    //uint16_t message_len = Mavlink_encoder(MESSAGE_ID_GPS, &encoded_msg, (const uint8_t*) &global_position);
    mavlink_message_t * encoded_msg = Mavlink_encoder(MESSAGE_ID_GPS, (const uint8_t*) &global_position);
    

    printf("++++++++++++++++++start decoding process++++++++++++++++++\n");
    mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE;
    mavlink_global_position_int_t global_position_decoded;

    unsigned char* ptr_in_byte = (unsigned char *) encoded_msg;
    for( int i = 0; i < 39; i++)
    {
        if (ptr_in_byte == NULL)
        {
            //null ptr 
        }
        
        printf("copying byte: %d / %d   |   current byte : %hhx\n", i, 39,ptr_in_byte[i]);
        decoderStatus = Mavlink_decoder(MAVLINK_COMM_0, ptr_in_byte[i], (uint8_t*) &global_position_decoded);

    }

    if (decoderStatus == MAVLINK_DECODING_OKAY)
    {
        int32_t altitude = global_position_decoded.alt;
        printf("altitude is: %d\n", altitude);
    }

    else
    {
        printf("decoding unsuccessful, error message: %d\n",decoderStatus);
    }
    return 0;

}

// to run the mavlink functions only:
//gcc -g MavlinkFunctions.cpp
//a.out
