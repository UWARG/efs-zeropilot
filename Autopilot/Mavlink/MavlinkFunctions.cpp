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

PIGO_Message_IDs_t decoded_message_type = MESSAGE_ID_NONE;


mavlink_decoding_status_t Mavlink_airside_decoder(int channel, uint8_t incomingByte, uint8_t *telemetryData)
{
    decoded_message_type = MESSAGE_ID_NONE; //init

    mavlink_status_t status;
    mavlink_message_t decoded_msg;
    uint8_t parsingStatus = mavlink_parse_char(channel, incomingByte, &decoded_msg, &status);
    //printf("inside decoder\n");
    if (status.msg_received)
    {
        //printf("message receieved\n");
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
                    uint32_t warg_ID = global_position.time_boot_ms; /*< [ms] Timestamp (time since system boot).*/
                    int32_t latitude = global_position.lat; /*< [degE7] Latitude, expressed*/
                    int32_t longitude = global_position.lon; /*< [degE7] Longitude, expressed*/
                    int32_t altitude = global_position.alt; /*< [mm] Altitude (MSL). Note that virtually all GPS modules provide both WGS84 and MSL.*/
                    int32_t relative_altitude = global_position.relative_alt; /*< [mm] Altitude above ground*/
                    //int16_t Vx = global_position.vx; /*< [cm/s] Ground X Speed (Latitude, positive north)*/
                    //int16_t Vy = global_position.vy; /*< [cm/s] Ground Y Speed (Longitude, positive east)*/
                    //int16_t Vz = global_position.vz; /*< [cm/s] Ground Z Speed (Altitude, positive down)*/
                    uint16_t Hdg = global_position.hdg; /*< [cdeg] Vehicle heading (yaw angle), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX*/
                    
                    switch(warg_ID)
                    {
                        case MESSAGE_ID_GPS_LANDING_SPOT:
                            {
                                PIGO_GPS_LANDING_SPOT_t landing_spot;
                                landing_spot.latitude = latitude;
                                landing_spot.longitude = longitude;
                                landing_spot.altitude = altitude;
                                landing_spot.landingDirection = relative_altitude;

                                printf("valid data\n");
                                memcpy((void*) telemetryData, (void*) &landing_spot, sizeof(PIGO_GPS_LANDING_SPOT_t));
                                decoded_message_type = MESSAGE_ID_GPS_LANDING_SPOT;

                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_WAYPOINTS:
                            {
                                PIGO_WAYPOINTS_t waypoints;
                                waypoints.latitude = latitude;
                                waypoints.longitude = longitude;
                                waypoints.altitude = altitude;
                                waypoints.turnRadius = relative_altitude;
                                waypoints.waypointType = Hdg;

                                memcpy((void*) telemetryData, (void*) &waypoints, sizeof(PIGO_WAYPOINTS_t));
                                decoded_message_type = MESSAGE_ID_WAYPOINTS;

                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_HOMEBASE:
                            {
                                PIGO_WAYPOINTS_t waypoints;
                                waypoints.latitude = latitude;
                                waypoints.longitude = longitude;
                                waypoints.altitude = altitude;
                                waypoints.turnRadius = relative_altitude;
                                waypoints.waypointType = Hdg;

                                memcpy((void*) telemetryData, (void*) &waypoints, sizeof(PIGO_WAYPOINTS_t));
                                decoded_message_type = MESSAGE_ID_HOMEBASE;

                                return MAVLINK_DECODING_OKAY;
                            }
                            break;

                        case MESSAGE_ID_NUM_WAYPOINTS:
                            {
                                four_bytes_int_cmd_t command;
                                command.cmd = latitude;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(four_bytes_int_cmd_t));
                                decoded_message_type = MESSAGE_ID_NUM_WAYPOINTS;

                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_HOLDING_ALTITUDE:
                            {
                                four_bytes_int_cmd_t command;
                                command.cmd = latitude;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(four_bytes_int_cmd_t));
                                decoded_message_type = MESSAGE_ID_HOLDING_ALTITUDE;

                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_HOLDING_TURN_RADIUS:
                            {
                                four_bytes_int_cmd_t command;
                                command.cmd = latitude;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(four_bytes_int_cmd_t));
                                decoded_message_type = MESSAGE_ID_HOLDING_TURN_RADIUS;

                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_PATH_MODIFY_NEXT_LD:
                            {
                                four_bytes_int_cmd_t command;
                                command.cmd = latitude;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(four_bytes_int_cmd_t));
                                decoded_message_type = MESSAGE_ID_PATH_MODIFY_NEXT_LD;

                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_PATH_MODIFY_PREV_LD:
                            {
                                four_bytes_int_cmd_t command;
                                command.cmd = latitude;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(four_bytes_int_cmd_t));
                                decoded_message_type = MESSAGE_ID_PATH_MODIFY_PREV_LD;

                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_PATH_MODIFY_LD:
                            {
                                four_bytes_int_cmd_t command;
                                command.cmd = latitude;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(four_bytes_int_cmd_t));
                                decoded_message_type = MESSAGE_ID_PATH_MODIFY_LD;

                                return MAVLINK_DECODING_OKAY;
                            }
                            break;

                            case MESSAGE_ID_GROUND_CMD:
                            {                 
                                PIGO_GIMBAL_t command;
                                command.pitch = latitude; // TODO type casting
                                command.yaw = longitude;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(PIGO_GIMBAL_t));
                                decoded_message_type = MESSAGE_ID_GROUND_CMD;

                                return MAVLINK_DECODING_OKAY;                               
                            }
                            break;

                            case MESSAGE_ID_GIMBAL_CMD:
                            {                 
                                PIGO_GROUND_COMMAND_t command;
                                command.heading = latitude;
                                command.latestDistance = longitude;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(PIGO_GROUND_COMMAND_t));
                                decoded_message_type = MESSAGE_ID_GIMBAL_CMD;

                                return MAVLINK_DECODING_OKAY;                               
                            }
                            break;

                        default:
                        return MAVLINK_DECODING_FAIL;
                    }
                }
                break;

            case MAVLINK_MSG_ID_ATTITUDE: 
                {
                    mavlink_attitude_t gimbal_command;

                    mavlink_msg_attitude_decode(&decoded_msg, &gimbal_command);
                              
                    uint32_t warg_gimbal_ID = gimbal_command.time_boot_ms; // cmd ID
                    //float roll; /< [rad] Roll angle (-pi..+pi)/
                    float cmdA = gimbal_command.pitch;
                    float cmdB = gimbal_command.yaw; 
                    //float rollspeed;
                    //float pitchspeed; 
                    //float yawspeed; 
                    
                    switch(warg_gimbal_ID)
                    {
                        case MESSAGE_ID_GROUND_CMD:
                            {                 
                                PIGO_GROUND_COMMAND_t command;
                                command.heading = cmdA;
                                command.latestDistance = cmdB;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(PIGO_GROUND_COMMAND_t));
                                decoded_message_type = MESSAGE_ID_GROUND_CMD;

                                return MAVLINK_DECODING_OKAY;                               
                            }
                            break;
                        case MESSAGE_ID_GIMBAL_CMD:
                            {
                                PIGO_GIMBAL_t command;
                                command.pitch = cmdA;
                                command.yaw = cmdB;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(PIGO_GIMBAL_t));
                                decoded_message_type = MESSAGE_ID_GIMBAL_CMD;

                                return MAVLINK_DECODING_OKAY;
                            }
                            break;

                        default:
                            return MAVLINK_DECODING_FAIL;
                            break;
                    }
                }
                break;
//TODO after all cmds are implemented, give cmd ID to seperate the ones that uses the same mavlink encoding functions
//TODO update the decoder to use the corresponsing cmd IDs
            case MAVLINK_MSG_ID_CAMERA_SETTINGS: // simple Warg cmd goes here - Pogi (plane out ground in)
                {
                    mavlink_camera_settings_t warg_command;
                    //all simple commands sits under here

                    //typedef struct __mavlink_camera_settings_t {
                    uint32_t warg_cmd_ID = warg_command.time_boot_ms;         //command ID      
                    // uint8_t mode_id;
                    // float zoomLevel; 
                    // float focusLevel;            
                    //}) mavlink_camera_settings_t;

                    mavlink_msg_camera_settings_decode(&decoded_msg, &warg_command);
                    printf("reached here decoding is LANDED\n");

                    switch(warg_cmd_ID)
                    {
                        case MESSAGE_ID_WAYPOINT_MODIFY_PATH_CMD:
                            {
                                decoded_message_type = MESSAGE_ID_WAYPOINT_MODIFY_PATH_CMD;
                                one_byte_uint_cmd_t command;
                                command.cmd = warg_command.mode_id;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(one_byte_uint_cmd_t));
                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_WAYPOINT_NEXT_DIRECTIONS_CMD:
                            {
                                decoded_message_type = MESSAGE_ID_WAYPOINT_NEXT_DIRECTIONS_CMD;
                                one_byte_uint_cmd_t command;
                                command.cmd = warg_command.mode_id;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(one_byte_uint_cmd_t));
                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_HOLDING_TURN_DIRECTION:
                            {
                                decoded_message_type = MESSAGE_ID_HOLDING_TURN_DIRECTION;
                                one_byte_uint_cmd_t command;
                                command.cmd = warg_command.mode_id;

                                memcpy((void*) telemetryData, (void*) &command, sizeof(one_byte_uint_cmd_t));
                                return MAVLINK_DECODING_OKAY;
                            }
                            break;

                        case MESSAGE_ID_BEGIN_LANDING:
                            {
                                printf("reached here decoding is LANDED\n");
                                decoded_message_type = MESSAGE_ID_BEGIN_LANDING;
                                single_bool_cmd_t isLanded;
                                isLanded.cmd = warg_command.mode_id;

                                memcpy((void*) telemetryData, (void*) &isLanded, sizeof(single_bool_cmd_t));
                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_BEGIN_TAKEOFF:
                            {
                                decoded_message_type = MESSAGE_ID_BEGIN_TAKEOFF;
                                single_bool_cmd_t isTakenoff;
                                isTakenoff.cmd = warg_command.mode_id;

                                memcpy((void*) telemetryData, (void*) &isTakenoff, sizeof(single_bool_cmd_t));
                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        case MESSAGE_ID_INITIALIZING_HOMEBASE:
                            {
                                decoded_message_type = MESSAGE_ID_INITIALIZING_HOMEBASE;
                                single_bool_cmd_t isInitialized;
                                isInitialized.cmd = warg_command.mode_id;

                                memcpy((void*) telemetryData, (void*) &isInitialized, sizeof(single_bool_cmd_t));
                                return MAVLINK_DECODING_OKAY;
                            }
                            break;
                        
                        default:
                            return MAVLINK_DECODING_FAIL;
                            break;
                    }

                }
                break;

            case MAVLINK_MSG_ID_SET_HOME_POSITION: // longer Warg cmd goes here
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

                }
                break;
    
            default:
                break;
        }
    }

    return MAVLINK_DECODING_INCOMPLETE;
}


PIGO_Message_IDs_t Mavlink_airside_decoder_get_message_type(void)
{
    return decoded_message_type;
}


mavlink_encoding_status_t Mavlink_airside_encoder(POGI_Message_IDs_t msgID, mavlink_message_t *message, const uint8_t *struct_ptr) 
{
    uint8_t system_id = 1;
    uint8_t component_id = 1;
    mavlink_message_t encoded_msg_original;
    uint16_t message_len;

    switch(msgID)
    {
        case MESSAGE_ID_GPS:
        {
            POGI_GPS_t* warg_GPS_cmd = (POGI_GPS_t*) struct_ptr;

            mavlink_global_position_int_t global_position;
            global_position.time_boot_ms = MESSAGE_ID_GPS_LANDING_SPOT;
            global_position.lat = warg_GPS_cmd ->latitude;
            global_position.lon = warg_GPS_cmd ->longitude;
            global_position.alt = warg_GPS_cmd ->altitude;
            global_position.relative_alt = 4;
            global_position.vx = 6;
            global_position.vy = 7;
            global_position.vz = 8;
            global_position.hdg = 9;

            message_len = mavlink_msg_global_position_int_encode(system_id, component_id, &encoded_msg_original, &global_position);
            //message_len = mavlink_msg_global_position_int_encode(system_id, component_id, &encoded_msg_original, (mavlink_global_position_int_t*) struct_ptr);
        } 
        break;

        case MESSAGE_ID_EULER_ANGLE_PLANE:
        case MESSAGE_ID_EULER_ANGLE_CAM:
        {
            printf("reached here\n");
            POGI_Euler_Angle_t* gimbal_cmd = (POGI_Euler_Angle_t*) struct_ptr;

            mavlink_global_position_int_t global_position;
            global_position.time_boot_ms = MESSAGE_ID_GIMBAL_CMD;
            global_position.lat = 1;//(uint32_t)gimbal_cmd ->roll;
            global_position.lon = 2;//(uint32_t)gimbal_cmd ->pitch;
            global_position.alt = 3;//(uint32_t)gimbal_cmd ->yaw;
            global_position.relative_alt = 4;
            global_position.vx = 6;
            global_position.vy = 7;
            global_position.vz = 8;
            global_position.hdg = 9;

            message_len = mavlink_msg_global_position_int_encode(system_id, component_id, &encoded_msg_original, &global_position);

/*
            mavlink_attitude_t attitude_cmd;
            attitude_cmd.time_boot_ms = MESSAGE_ID_GROUND_CMD; //TODO use appropriate ground side decoder cmd ID here
            attitude_cmd.roll = gimbal_cmd ->roll;
            attitude_cmd.pitch = gimbal_cmd ->pitch;
            attitude_cmd.yaw = gimbal_cmd ->yaw;
            attitude_cmd.rollspeed = 1;
            attitude_cmd.pitchspeed = 1;
            attitude_cmd.yawspeed = 1;

            message_len = mavlink_msg_attitude_encode(system_id, component_id, &encoded_msg_original, &attitude_cmd);
*/      
        }
        break;

        case MESSAGE_ID_IS_LANDED:
        case MESSAGE_ID_HOMEBASE_INITIALIZED:
        {
            single_bool_cmd_t* warg_cmd = (single_bool_cmd_t*) struct_ptr;

            mavlink_camera_settings_t camera_setting;
            camera_setting.time_boot_ms = MESSAGE_ID_BEGIN_LANDING; //TODO use appropriate ID here for decoder PIGO
            camera_setting.mode_id = (uint8_t) warg_cmd ->cmd; // 1 byte
            camera_setting.zoomLevel = 8; //4 bytes
            camera_setting.focusLevel = 8; // 4bytes
            printf(" encoded IS LANDED \n");
            message_len = mavlink_msg_camera_settings_encode(system_id, component_id, &encoded_msg_original, &camera_setting);
        }
        break;

        case MESSAGE_ID_AIR_SPEED:
        case MESSAGE_ID_CURRENT_WAYPOINT_LD:
        case MESSAGE_ID_CURRENT_WAYPOINT_INDEX:
        {
            four_bytes_int_cmd_t* numWaypoint_cmd = (four_bytes_int_cmd_t*) struct_ptr;

            mavlink_global_position_int_t global_position;
            global_position.time_boot_ms = 1; //TODO  use appropriate ID here
            global_position.lat = numWaypoint_cmd ->cmd;
            global_position.lon = 1;
            global_position.alt = 1;
            global_position.relative_alt = 1;
            global_position.vx = 6;
            global_position.vy = 7;
            global_position.vz = 8;
            global_position.hdg = 0;

            message_len = mavlink_msg_global_position_int_encode(system_id, component_id, &encoded_msg_original, &global_position);
            //message_len = mavlink_msg_global_position_int_encode(system_id, component_id, &encoded_msg_original, (mavlink_global_position_int_t*) struct_ptr);
        } 
        break;

        case MESSAGE_ID_ERROR_CODE:
        case MESSAGE_ID_EDITING_FLIGHT_PATH_ERROR_CODE:
        case MESSAGE_ID_FLIGHT_PATH_FOLLOWING_ERROR_CODE:
        {
            one_byte_uint_cmd_t* warg_cmd = (one_byte_uint_cmd_t*) struct_ptr;

            mavlink_camera_settings_t camera_setting;
            camera_setting.time_boot_ms = MESSAGE_ID_WAYPOINT_MODIFY_PATH_CMD; //TODO use appripriate ID here
            camera_setting.mode_id = warg_cmd ->cmd; // 1 byte
            camera_setting.zoomLevel = 1.0; //4 bytes
            camera_setting.focusLevel = 2.0; // 4 bytes

            message_len = mavlink_msg_camera_settings_encode(system_id, component_id, &encoded_msg_original, &camera_setting);
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
    /*
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
    */
   PIGO_GPS_LANDING_SPOT_t landing_spot = {
       1,
       2,
       3,
       4,
   };
   POGI_GPS_t GPS_Test = {
       1,2,3,
   };
    PIGO_WAYPOINTS_t warg_GPS = 
    {
        2, //int32_t latitude;
        3, //int32_t longitude;
        4, //int32_t altitude;
        5, //int32_t turnpoint;
        9,
    };

    PIGO_GIMBAL_t gimbal_cmd = 
    {
        2, //pitch
        1, //yaw
    };
    POGI_Euler_Angle_t angle_cmd = {
        1, //yaw
        2, //pitch
        3, //roll
    };
/*
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
*/
 
    one_byte_uint_cmd_t uint8_cmd = 
    {
        8,
    };

    single_bool_cmd_t bool_cmd = 
    {
        1,
    };


    mavlink_message_t encoded_msg;

    //uint8_t encoderStatus = Mavlink_airside_encoder(MESSAGE_ID_GPS_LANDING_SPOT, &encoded_msg, (const uint8_t*) &global_position);
    //uint8_t encoderStatus = Mavlink_airside_encoder(MESSAGE_ID_ERROR_CODE, &encoded_msg, (const uint8_t*) &uint8_cmd);
    
    //uint8_t encoderStatus = Mavlink_airside_encoder(MESSAGE_ID_GPS, &encoded_msg, (const uint8_t*) &landing_spot);
    uint8_t encoderStatus = Mavlink_airside_encoder(MESSAGE_ID_EULER_ANGLE_CAM, &encoded_msg, (const uint8_t*) &gimbal_cmd);

    if (encoderStatus == MAVLINK_ENCODING_FAIL)
    {
        printf("encoding failed");
        return 0;
    }

    //---------------------------------- decoding starts ---------------------------------- 

    mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE;
    //mavlink_attitude_t gimbal_command_decoded;

    char decoded_message_buffer[256]; //256 is the max payload length

    // the following few lines imitates how a decoder is used when it gets one byte at a time from a serial port
    unsigned char* ptr_in_byte = (unsigned char *) &encoded_msg;
    char message_buffer[50];
    for( int i = 0; i < 50; i++) // 50 is just a random number larger than message length (for GPS message length is 39)
    {
        if (decoderStatus != MAVLINK_DECODING_OKAY)
        {
            message_buffer[i] = ptr_in_byte[i];
                printf("copying byte: %d / %d   |   current byte : %hhx\n", i, 50, message_buffer[i]);
            //decoderStatus = Mavlink_airside_decoder(MAVLINK_COMM_0, ptr_in_byte[i], (uint8_t*) &global_position_decoded);
            decoderStatus = Mavlink_airside_decoder(MAVLINK_COMM_0, ptr_in_byte[i], (uint8_t*) &decoded_message_buffer);
            

        }
    }

    if (decoderStatus == MAVLINK_DECODING_OKAY)
    {
        printf("initial decoding okay\n");
        PIGO_Message_IDs_t message_type = Mavlink_airside_decoder_get_message_type();
        int result = 1;

        switch (message_type) // those types need to match ground side decoder's type, currently use the airside only for the purpose of testing
        {
            case MESSAGE_ID_GPS_LANDING_SPOT:
                {
                    PIGO_GPS_LANDING_SPOT_t landing_spot_decoded;
                    memcpy(&landing_spot_decoded, &decoded_message_buffer, sizeof(PIGO_GPS_LANDING_SPOT_t));
                    
                    result = memcmp(&landing_spot_decoded, &landing_spot, sizeof(PIGO_GPS_LANDING_SPOT_t) );

                    //mavlink_global_position_int_t global_position_decoded;
                    //memcpy(&global_position_decoded, &decoded_message_buffer, sizeof(mavlink_global_position_int_t));
                    //result = memcmp(&global_position_decoded, &global_position, sizeof(mavlink_global_position_int_t) );
                }
                break;

            case MESSAGE_ID_GIMBAL_CMD:
                {
                    printf("in gimbal cmd decode\n");
                    PIGO_GIMBAL_t cmd_received;
                    memcpy(&cmd_received, &decoded_message_buffer, sizeof(PIGO_GIMBAL_t));
                    printf("pitch: %f, yaw: %f\n", cmd_received.pitch, cmd_received.yaw);
                    result = memcmp(&cmd_received, &gimbal_cmd, sizeof(PIGO_GIMBAL_t) );
                }
                break;

            case MESSAGE_ID_BEGIN_LANDING: //test for all one byte cmd
                {
                    printf("error code gets here\n");
                    single_bool_cmd_t islanded;
                    memcpy(&islanded, &decoded_message_buffer, sizeof(single_bool_cmd_t));
                    result = memcmp(&islanded, &bool_cmd, sizeof(single_bool_cmd_t) );
                }
                break;

            case MESSAGE_ID_WAYPOINT_MODIFY_PATH_CMD: //test for all one byte cmd
                {
                    printf("error code gets here\n");
                    one_byte_uint_cmd_t cmd_decoded;
                    memcpy(&cmd_decoded, &decoded_message_buffer, sizeof(one_byte_uint_cmd_t));
                    result = memcmp(&cmd_decoded, &uint8_cmd, sizeof(one_byte_uint_cmd_t) );
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
    else{
        printf("decoding failed\n");
    }
    return 0;
}


int main(void) // TODO: this main needs to be removed once integrated
{
    test__encode_then_decode();

    return 0;
}


