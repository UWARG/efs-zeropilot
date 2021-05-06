/**************************************************************************************************/
// Author: Jingting Liu
// April 9th, 2021

// to run the test without cmake:
// gcc -g MavlinkFunctions.cpp
// a.out

// refer to this page for the stucture of mavlink messages 
// https://mavlink.io/en/guide/serialization.html

// The decoder and encoder only support GPS and gimbal control, other simpler commands will be taken
// care of by the Xbee communication directly
/**************************************************************************************************/

#include "Mavlink2_lib/common/mavlink.h"
#include "MavlinkFunctions.hpp"

POGI_Message_IDs_t decoded_message_type = MESSAGE_ID_NONE;


mavlink_decoding_status_t Mavlink_airside_decoder(int channel, uint8_t incomingByte, uint8_t *telemetryData)
{
    decoded_message_type = MESSAGE_ID_NONE; //init

    mavlink_status_t status;
    mavlink_message_t decoded_msg;
    uint8_t parsingStatus = mavlink_parse_char(channel, incomingByte, &decoded_msg, &status);

    if (status.msg_received)
    {
        if (parsingStatus != MAVLINK_FRAMING_OK )
        {
            return MAVLINK_DECODING_BAD_PARSING;
        }

        if (telemetryData == NULL)
        {
            return MAVLINK_DECODING_FAIL;
        }

        switch(decoded_msg.msgid)
        {
            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: // ID for GLOBAL_POSITION_INT, 33
                {
                    mavlink_global_position_int_t global_position;
                    
                    mavlink_msg_global_position_int_decode(&decoded_msg, &global_position);

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
                    
                    // someone needs to change this to an actual GPS check, e.g. check for a valid range
                    if (Hdg == 9) //to be changed
                    {
                        printf("valid data\n");
                        memcpy((void*) telemetryData, (void*) &global_position, sizeof(mavlink_global_position_int_t));
                        decoded_message_type = MESSAGE_ID_GPS;
                        return MAVLINK_DECODING_OKAY;
                    }
                    else
                    {
                        return MAVLINK_DECODING_FAIL;
                    }
                }

                break;

            case MAVLINK_MSG_ID_ATTITUDE: 
                {
                    mavlink_attitude_t gimbal_command;

                    mavlink_msg_attitude_decode(&decoded_msg, &gimbal_command);
                    /*            
                    uint32_t time_boot_ms; /< [ms] Timestamp (time since system boot)./
                    float roll; /< [rad] Roll angle (-pi..+pi)/
                    float pitch; /< [rad] Pitch angle (-pi..+pi)/
                    float yaw; /< [rad] Yaw angle (-pi..+pi)/
                    float rollspeed; /< [rad/s] Roll angular speed/
                    float pitchspeed; /< [rad/s] Pitch angular speed/
                    float yawspeed; /< [rad/s] Yaw angular speed/
                    */

                    // to be changed
                    if (gimbal_command.time_boot_ms == MESSAGE_ID_EULER_ANGLE_PLANE)
                    {
                        decoded_message_type = MESSAGE_ID_EULER_ANGLE_PLANE;
                        Warg_Euler_Angle_t angle_command;
                        angle_command.yaw = gimbal_command.yaw;
                        angle_command.pitch = gimbal_command.pitch;
                        angle_command.roll = gimbal_command.roll;

                        memcpy((void*) telemetryData, (void*) &angle_command, sizeof(Warg_Euler_Angle_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    if (gimbal_command.time_boot_ms == MESSAGE_ID_EULER_ANGLE_CAM)
                    {
                        decoded_message_type = MESSAGE_ID_EULER_ANGLE_CAM;
                        memcpy((void*) telemetryData, (void*) &gimbal_command, sizeof(mavlink_attitude_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    else
                    {
                        return MAVLINK_DECODING_FAIL;
                    }
                }
                break;

            case MAVLINK_MSG_ID_CAMERA_SETTINGS: // simple Warg cmd goes here - Pogi (plane out ground in)
                {
                    mavlink_camera_settings_t warg_command;
                    //static inline void mavlink_msg_camera_settings_decode(const mavlink_message_t* msg, mavlink_camera_settings_t* camera_settings)
                    //typedef struct __mavlink_camera_settings_t {
                    // uint32_t time_boot_ms;               
                    // uint8_t mode_id;
                    // float zoomLevel; 
                    // float focusLevel;            //command ID
                    //}) mavlink_camera_settings_t;

                    mavlink_msg_camera_settings_decode(&decoded_msg, &warg_command);

                    if (warg_command.focusLevel == 1.0) // errorCode
                    {
                        //mode_id carries the errorcode
                        memcpy((void*) telemetryData, (void*) &warg_command, sizeof(mavlink_camera_settings_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    if (warg_command.focusLevel == 2.0)
                    {
                        memcpy((void*) telemetryData, (void*) &warg_command, sizeof(mavlink_camera_settings_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    if (warg_command.focusLevel == 3.0)
                    {
                        memcpy((void*) telemetryData, (void*) &warg_command, sizeof(mavlink_camera_settings_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    if (warg_command.focusLevel == 4.0)
                    {
                        memcpy((void*) telemetryData, (void*) &warg_command, sizeof(mavlink_camera_settings_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    if (warg_command.focusLevel == 5.0)
                    {
                        memcpy((void*) telemetryData, (void*) &warg_command, sizeof(mavlink_camera_settings_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    if (warg_command.focusLevel == 1.0)
                    {
                        memcpy((void*) telemetryData, (void*) &warg_command, sizeof(mavlink_camera_settings_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    if (warg_command.focusLevel == 1.0)
                    {
                        memcpy((void*) telemetryData, (void*) &warg_command, sizeof(mavlink_camera_settings_t));
                        return MAVLINK_DECODING_OKAY;
                    }

                    else
                    {
                        return MAVLINK_DECODING_FAIL;
                    }

                }
                break;

            case MAVLINK_MSG_ID_SET_HOME_POSITION: // more of Warg cmd goes here
                {
                    mavlink_set_home_position_t direction_command;
                    /*
                    typedef struct __mavlink_set_home_position_t {
                    int32_t latitude; 
                    int32_t longitude; 
                    int32_t altitude; 
                    float x; 
                    float y; 
                    float z; 
                    float q[4]; 
                    float approach_x; 
                    float approach_y; 
                    float approach_z; 
                    uint8_t target_system;
                    uint64_t time_usec;                 // direction command ID
                    }) mavlink_set_home_position_t;
                    */

                    mavlink_msg_set_home_position_decode(&decoded_msg, &direction_command);

                    if (direction_command.time_usec == 1) //homebase
                    {
                        memcpy((void*) telemetryData, (void*) &direction_command, sizeof(mavlink_set_home_position_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    else if (direction_command.time_usec == 2) //waypoint
                    {
                        memcpy((void*) telemetryData, (void*) &direction_command, sizeof(mavlink_set_home_position_t));
                        return MAVLINK_DECODING_OKAY;
                    }
                    else
                    {
                        return MAVLINK_DECODING_FAIL;
                    }

                }
                break;
            case MAVLINK_MSG_ID_TAKEOFF_CMD:
                {
                    mavlink_custom_cmd_takeoff_t takeoff_command;
                    custom_mavlink_msg__begin_takeoff_command_decode(&decoded_msg, &takeoff_command);

                    // to be changed
                    if (takeoff_command.beginTakeoff == 1)
                    {
                        memcpy((void*) telemetryData, (void*) &takeoff_command, sizeof(mavlink_custom_cmd_takeoff_t));

                        return MAVLINK_DECODING_OKAY;
                    }
                    else
                    {
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


POGI_Message_IDs_t Mavlink_airside_decoder_get_message_type(void)
{
    return decoded_message_type;
}


mavlink_encoding_status_t Mavlink_airside_encoder(PIGO_Message_IDs_t msgID, mavlink_message_t *message, const uint8_t *struct_ptr) 
{
    uint8_t system_id = 1;
    uint8_t component_id = 1;
    mavlink_message_t encoded_msg_original;
    uint16_t message_len;

    switch(msgID)
    {
        case MESSAGE_ID_GPS_LANDING_SPOT:
        {
            PIGO_GPS_LANDING_SPOT_t* warg_GPS_cmd = (PIGO_GPS_LANDING_SPOT_t*) struct_ptr;

            mavlink_global_position_int_t global_position;
            global_position.time_boot_ms = 1;
            global_position.lat = warg_GPS_cmd ->latitude;
            global_position.lon = warg_GPS_cmd ->longitude;
            global_position.alt = warg_GPS_cmd ->altitude;
            global_position.relative_alt = warg_GPS_cmd ->landingDirection;
            global_position.vx = 6;
            global_position.vy = 7;
            global_position.vz = 8;
            global_position.hdg = 9;

            message_len = mavlink_msg_global_position_int_encode(system_id, component_id, &encoded_msg_original, &global_position);
            //message_len = mavlink_msg_global_position_int_encode(system_id, component_id, &encoded_msg_original, (mavlink_global_position_int_t*) struct_ptr);
        } 
        break;

        case MESSAGE_ID_GIMBAL_CMD:
        {
            /*
            mavlink_attitude_t gimbal_command =
            {
                1, //time_boot_ms

                1, //roll
                2, //pitch
                3, //yaw

                4, // roll speed
                5, // pitch speed
                6, // yaw speed
            }*/
            message_len = mavlink_msg_attitude_encode(system_id, component_id, &encoded_msg_original, (mavlink_attitude_t*) struct_ptr);
        }
        break;

        default:
            break;
    }

    if (message_len == 0)
    {
        return MAVLINK_ENCODING_FAIL;
    }

    // the following loop is supposed to fix an inconsistency the original mavlink encoder, where the first byte is always shifted.
    // this code lets the encoder to send out a message (byte array) that starts exactly with the starting byte
    unsigned char* ptr_in_byte = (unsigned char *) &encoded_msg_original;
    char message_buffer[message_len];

    for( int i = 0; i < message_len; i++)
    {

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

    memcpy(message, message_buffer, message_len);
    printf("encoding successful\n");
    return MAVLINK_ENCODING_OKAY;
}

//--------------------------------------custom decoder and encoders for specific functions---------------------------------

uint16_t custom_mavlink_msg__begin_takeoff_command_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* message, const mavlink_custom_cmd_takeoff_t* struct_ptr)
{

    mavlink_custom_cmd_takeoff_t cmd;
    cmd.beginTakeoff = struct_ptr->beginTakeoff;

    memcpy(_MAV_PAYLOAD_NON_CONST(message), &cmd, MAVLINK_MSG_ID_TAKEOFF_CMD_LEN); //TODO look into the length calculation

    message->msgid = MAVLINK_MSG_ID_TAKEOFF_CMD;

    uint16_t message_len = mavlink_finalize_message(    message, 
                                                        system_id, 
                                                        component_id, 
                                                        MAVLINK_MSG_ID_TAKEOFF_CMD_MIN_LEN, 
                                                        MAVLINK_MSG_ID_TAKEOFF_CMD_LEN, 
                                                        MAVLINK_MSG_ID_GLOBAL_POSITION_INT_CRC);

    return message_len;
}


void custom_mavlink_msg__begin_takeoff_command_decode(const mavlink_message_t* message, mavlink_custom_cmd_takeoff_t* takeoff_command)
{
    uint8_t len = message->len < MAVLINK_MSG_ID_TAKEOFF_CMD_LEN? message->len : MAVLINK_MSG_ID_TAKEOFF_CMD_LEN;
    memset(takeoff_command, 0, MAVLINK_MSG_ID_TAKEOFF_CMD_LEN);
    memcpy(takeoff_command, _MAV_PAYLOAD(message), len);
}

//this needs a proper crc_extra to work
uint16_t custom_fcn__calculate_crc(mavlink_message_t* msg, uint8_t crc_extra)
{
    uint8_t buf[MAVLINK_CORE_HEADER_LEN+1];
    uint8_t header_len = MAVLINK_CORE_HEADER_LEN+1;

	// form the header as a byte array for the crc
	buf[0] = msg->magic;
	buf[1] = msg->len;
    buf[2] = msg->incompat_flags;
    buf[3] = msg->compat_flags;
    buf[4] = msg->seq;
    buf[5] = msg->sysid;
    buf[6] = msg->compid;
    buf[7] = msg->msgid & 0xFF;
    buf[8] = (msg->msgid >> 8) & 0xFF;
    buf[9] = (msg->msgid >> 16) & 0xFF;

	
	uint16_t checksum = crc_calculate(&buf[1], header_len-1);
	crc_accumulate_buffer(&checksum, _MAV_PAYLOAD(msg), msg->len);
	crc_accumulate(crc_extra, &checksum);
	mavlink_ck_a(msg) = (uint8_t)(checksum & 0xFF);
	mavlink_ck_b(msg) = (uint8_t)(checksum >> 8);

	return checksum;
}
//---------------------------------------- tests -----------------------------------------------------------------------------
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

    PIGO_GPS_LANDING_SPOT_t warg_GPS = 
    {
        2, //int32_t latitude;
        3, //int32_t longitude;
        4, //int32_t altitude;
        5, //int32_t landingDirection;
    };

    mavlink_attitude_t gimbal_command =
    {
        MESSAGE_ID_EULER_ANGLE_PLANE, //time_boot_ms
        1, //roll
        2, //pitch
        3, //yaw
        0, // roll speed
        0, // pitch speed
        0, // yaw speed
    };

    Warg_Euler_Angle_t angle_command = 
    {
        3,
        2,
        1,
    };

    mavlink_custom_cmd_takeoff_t takeoff_command = 
    {
        1,
    };

    mavlink_message_t encoded_msg;

    //uint8_t encoderStatus = Mavlink_airside_encoder(MESSAGE_ID_GPS_LANDING_SPOT, &encoded_msg, (const uint8_t*) &global_position);
    uint8_t encoderStatus = Mavlink_airside_encoder(MESSAGE_ID_GPS_LANDING_SPOT, &encoded_msg, (const uint8_t*) &warg_GPS);
    //uint8_t encoderStatus = Mavlink_airside_encoder(MESSAGE_ID_EULER_ANGLE_PLANE, &encoded_msg, (const uint8_t*) &gimbal_command);

    if (encoderStatus == MAVLINK_ENCODING_FAIL)
    {
        printf("encoding failed");
        return 0;
    }

    //---------------------------------- decoding starts ---------------------------------- 
    mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE;
    mavlink_global_position_int_t global_position_decoded;
    mavlink_attitude_t gimbal_command_decoded;
    mavlink_custom_cmd_takeoff_t takeoff_command_decoded;

    char decoded_message_buffer[256]; //256 is the max payload length

    // the following few lines imitates how a decoder is used when it gets one byte at a time from a serial port
    unsigned char* ptr_in_byte = (unsigned char *) &encoded_msg;
    for( int i = 0; i < 50; i++) // 50 is just a random number larger than message length (for GPS message length is 39)
    {
        if (decoderStatus != MAVLINK_DECODING_OKAY)
        {
            //decoderStatus = Mavlink_airside_decoder(MAVLINK_COMM_0, ptr_in_byte[i], (uint8_t*) &global_position_decoded);
            decoderStatus = Mavlink_airside_decoder(MAVLINK_COMM_0, ptr_in_byte[i], (uint8_t*) &decoded_message_buffer);

        }
    }

    if (decoderStatus == MAVLINK_DECODING_OKAY)
    {
        POGI_Message_IDs_t message_type = Mavlink_airside_decoder_get_message_type();
        int result = 1;

        switch (message_type)
        {
            case MESSAGE_ID_GPS:
                {
                    memcpy(&global_position_decoded, &decoded_message_buffer, sizeof(mavlink_global_position_int_t));
                    result = memcmp(&global_position_decoded, &global_position, sizeof(mavlink_global_position_int_t) );
                }
                break;

            case MESSAGE_ID_EULER_ANGLE_PLANE:
                {
                    Warg_Euler_Angle_t angle_received;
                    result = memcmp(&angle_received, &decoded_message_buffer, sizeof(Warg_Euler_Angle_t) );
                }
                break;

            case MESSAGE_ID_ERROR_CODE:
                {
                    result = memcmp(&global_position_decoded, &global_position, sizeof(mavlink_global_position_int_t) );
                }
                break;
            default:
                break;
        }

        if (result == 0)
        {
            printf("test passed!\n");
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


