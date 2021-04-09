// Author: Jingting Liu
#include "Mavlink2_lib/common/mavlink.h"
#include "MavlinkFunctions.hpp"


mavlink_decoding_status_t Mavlink_decoder(int channel, uint8_t incomingByte, uint8_t *telemetryData)
{
    mavlink_status_t status;
    mavlink_message_t decoded_msg;
    uint8_t parsingStatus = mavlink_parse_char(channel, incomingByte, &decoded_msg, &status);

    if (status.msg_received)
    {
        if (parsingStatus != MAVLINK_FRAMING_OK )
        {
            return MAVLINK_DECODING_BAD_PARSING;
        }

        switch(decoded_msg.msgid)
        {
            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: // ID for GLOBAL_POSITION_INT, 33
                {

                    mavlink_global_position_int_t global_position;
                    
                    mavlink_msg_global_position_int_decode(&decoded_msg, &global_position);

                    if (telemetryData == NULL)
                    {
                        return MAVLINK_DECODING_FAIL;
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
                        memcpy((void*) telemetryData, (void*) &global_position, sizeof(mavlink_global_position_int_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    else
                    {
                        return MAVLINK_DECODING_FAIL;
                    }
                }

                break;

            case MAVLINK_MSG_ID_GIMBAL_MANAGER_SET_PITCHYAW: 
                {
                    mavlink_gimbal_manager_set_pitchyaw_t gimbal_command;

                    mavlink_msg_gimbal_manager_set_pitchyaw_decode(&decoded_msg, &gimbal_command);

                    if (telemetryData == NULL)
                    {
                        return MAVLINK_DECODING_FAIL;
                    }
                    
                    //uint32_t flags; /*<  High level gimbal manager flags to use.*/
                    //float pitch; /*< [rad] Pitch angle (positive: up, negative: down, NaN to be ignored).*/
                    //float yaw; /*< [rad] Yaw angle (positive: to the right, negative: to the left, NaN to be ignored).*/
                    //float pitch_rate; /*< [rad/s] Pitch angular rate (positive: up, negative: down, NaN to be ignored).*/
                    //float yaw_rate; /*< [rad/s] Yaw angular rate (positive: to the right, negative: to the left, NaN to be ignored).*/
                    uint8_t target_system; /*<  System ID*/
                    //uint8_t target_component; /*<  Component ID*/
                    //uint8_t gimbal_device_id; /*<  Component ID of gimbal device to address (or 1-6 for non-MAVLink gimbal), 0 for all gimbal device components. Send command multiple times for more than one gimbal (but not all gimbals).*/
                    
                    printf("flags: %d\n", gimbal_command.flags);
                    printf("pitch: %f\n", gimbal_command.pitch);
                    printf("yaw: %f\n", gimbal_command.yaw);
                    printf("pitch_rate: %f\n", gimbal_command.pitch_rate);
                    printf("yaw_rate: %f\n", gimbal_command.yaw_rate);
                    printf("target_system: %d\n", gimbal_command.target_system);
                    printf("target_component: %d\n", gimbal_command.target_component);
                    printf("gimbal_device_id: %d\n", gimbal_command.gimbal_device_id);
                    // to be changed
                    if (gimbal_command.target_system == 1)
                    {
                        memcpy((void*) telemetryData, (void*) &gimbal_command, sizeof(mavlink_gimbal_manager_set_pitchyaw_t));
                        printf("decoding successful!\n");
                        return MAVLINK_DECODING_OKAY;
                    }
                    else
                    {
                        printf("decoding NOT successful!\n");
                        return MAVLINK_DECODING_FAIL;
                    }
                }
                break;

            default:
                break;
        }
    }

    return MAVLINK_DECODING_INCOMPLETE;
}


mavlink_encoding_status_t Mavlink_encoder(Message_IDs_t type, mavlink_message_t *message, const uint8_t *struct_ptr) 
{
    uint8_t system_id = 1;
    uint8_t component_id = 1;
    mavlink_message_t encoded_msg_original;
    uint16_t message_len;

    switch(type)
    {
        case MESSAGE_ID_GPS:
        {
            message_len = mavlink_msg_global_position_int_encode(system_id, component_id, &encoded_msg_original, (mavlink_global_position_int_t*) struct_ptr);
        } 
        break;

        case MESSAGE_ID_GIMBAL:
        {
            message_len = mavlink_msg_gimbal_manager_set_pitchyaw_encode(system_id, component_id, &encoded_msg_original, (mavlink_gimbal_manager_set_pitchyaw_t*) struct_ptr);
        }
        break;

        default:
            break;
    }

    if (message_len == 0)
    {
        return MAVLINK_ENCODING_FAIL;
    }

    // the following loop is supposed to fix a bug from the original mavlink encoder, where the first byte is always shifted.
    // this code lets the encoder to send out a message (byte array) that starts exactly with the starting byte
    unsigned char* ptr_in_byte = (unsigned char *) &encoded_msg_original;
    char message_buffer[message_len];

    for( int i = 0; i < message_len; i++)
    {
        //printf("pre bytes: %d / %d   |   current byte : %hhx\n", i, message_len, ptr_in_byte[i]);
        if (ptr_in_byte[i] == 0xfd) //0xfd, starting byte
        {
            for(int r = 0; r < message_len; r++)
            {
                message_buffer[r] = ptr_in_byte[r+i];
                printf("copying byte: %d / %d   |   current byte : %hhx\n", r, message_len, message_buffer[r]);
            }
            break;
        }
        else if (i == message_len-1)
        {
            return MAVLINK_ENCODING_FAIL;
        }
    }
    printf("message length: %d\n", message_len);
    memcpy(message, message_buffer, message_len);

    return MAVLINK_ENCODING_OKAY;
}

int Custom_fcn_command_encode(int cmd_type, bool command, mavlink_message_t* message)
{
    //mavlink_finalize_message_buffer();
    return 0;
}
//an example of how to use the encoder and decoder
int test__encode_then_decode(void)
{
    mavlink_global_position_int_t global_position = 
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

    mavlink_gimbal_manager_set_pitchyaw_t gimbal_command =
    {
        8, //flag
        7, //pitch
        6, //yaw
        5, //pitch rate
        4, // yaw rate
        1, // system ID
        1, // component ID
        1  //gimbal device component ID
    };


    mavlink_message_t encoded_msg;

    uint8_t encoderStatus = Mavlink_encoder(MESSAGE_ID_GPS, &encoded_msg, (const uint8_t*) &global_position);
    //uint8_t encoderStatus = Mavlink_encoder(MESSAGE_ID_GIMBAL, &encoded_msg, (const uint8_t*) &gimbal_command);
    if (encoderStatus == MAVLINK_ENCODING_FAIL)
    {
        return 0;
    }

    //---------------------------------- decoding starts ---------------------------------- 
    mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE;
    mavlink_global_position_int_t global_position_decoded;
    mavlink_gimbal_manager_set_pitchyaw_t gimbal_command_decoded;

    // the following few lines imitates how a decoder is used when it gets one byte at a time from a serial port
    unsigned char* ptr_in_byte = (unsigned char *) &encoded_msg;
    for( int i = 0; i < 50; i++) // 50 is just a random number larger than message length (for GPS message length is 39)
    {
        if (decoderStatus != MAVLINK_DECODING_OKAY)
        {
            printf("copying byte: %d / %d   |   current byte : %hhx\n", i, 50, ptr_in_byte[i]);
            decoderStatus = Mavlink_decoder(MAVLINK_COMM_0, ptr_in_byte[i], (uint8_t*) &global_position_decoded);
            //decoderStatus = Mavlink_decoder(MAVLINK_COMM_0, ptr_in_byte[i], (uint8_t*) &gimbal_command_decoded);
        }
    }

    if (decoderStatus == MAVLINK_DECODING_OKAY)
    {
        int result = memcmp(&global_position_decoded, &global_position, sizeof(mavlink_global_position_int_t) );
        //int result = memcmp(&gimbal_command_decoded, &gimbal_command, sizeof(mavlink_gimbal_manager_set_pitchyaw_t) );
        if (result == 0)
        {
            printf("decoding successful\n");
            return 1;
        }
    }
    return 0;
}


int main(void) // TODO: this main needs to be removed once integrated
{
    test__encode_then_decode();
    return 0;
}


// to run the test without cmake:
// gcc -g MavlinkFunctions.cpp
// a.out

// refer to this page for the stucture of mavlink messages 
// https://mavlink.io/en/guide/serialization.html