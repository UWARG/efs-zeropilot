/**************************************************************************************************/
// Author: Jingting Liu
// April 9th, 2021

// refer to this page for the stucture of mavlink messages 
// https://mavlink.io/en/guide/serialization.html

// The decoder and encoder only support GPS and gimbal control, other simpler commands will be taken
// care of by the Xbee communication directly
/**************************************************************************************************/

#ifndef MAVLINKFUNCTIONS_HPP
#define MAVLINKFUNCTIONS_HPP

#include "Mavlink2_lib/common/mavlink.h"

typedef enum {
    MAVLINK_DECODING_INCOMPLETE=0,
    MAVLINK_DECODING_OKAY=1,
    MAVLINK_DECODING_FAIL=2,
    MAVLINK_DECODING_BAD_PARSING=3,
} mavlink_decoding_status_t;

typedef enum {
    MAVLINK_ENCODING_OKAY=1,
    MAVLINK_ENCODING_FAIL=2,
} mavlink_encoding_status_t;

//airside encoder, Plane In Ground Out (PIGO)
typedef enum {
    MESSAGE_ID_EMPTY,
    MESSAGE_ID_GPS_LANDING_SPOT,
    MESSAGE_ID_GROUND_COMMAND,
    MESSAGE_ID_GIMBAL_CMD,
    MESSAGE_ID_BEGIN_LANDING,
    MESSAGE_ID_BEGIN_TAKEOFF,
    MESSAGE_ID_NUM_WAYPOINTS,
    MESSAGE_ID_WAYPOINT_MODIFY_PATH_CMD,
    MESSAGE_ID_WAYPOINT_NEXT_DIRECTIONS_CMD,
    MESSAGE_ID_INITIALIZING_HOMEBASE,
    MESSAGE_ID_HOLDING_ALTITUDE,
    MESSAGE_ID_HOLDING_TURN_RADIUS,
    MESSAGE_ID_HOLDING_TURN_DIRECTION,
    MESSAGE_ID_PATH_MODIFY_NEXT_LD,
    MESSAGE_ID_PATH_MODIFY_PREV_LD,
    MESSAGE_ID_PATH_MODIFY_LD,
    MESSAGE_ID_WAYPOINTS,
    MESSAGE_ID_HOMEBASE,
} PIGO_Message_IDs_t;

//airside decoder, Plane Out Ground In (POGI)
typedef enum {
    MESSAGE_ID_NONE,
    MESSAGE_ID_GPS,
    MESSAGE_ID_GIMBAL,
    MESSAGE_ID_ERROR_CODE,
    MESSAGE_ID_AIR_SPEED,
    MESSAGE_ID_EULER_ANGLE_PLANE,
    MESSAGE_ID_EULER_ANGLE_CAM,
    MESSAGE_ID_IS_LANDED,
    MESSAGE_ID_EDITING_FLIGHT_PATH_ERROR_CODE,
    MESSAGE_ID_FLIGHT_PATH_FOLLOWING_ERROR_CODE,
    MESSAGE_ID_CURRENT_WAYPOINT_LD,
    MESSAGE_ID_CURRENT_WAYPOINT_INDEX,
    MESSAGE_ID_HOMEBASE_INITIALIZED,
} POGI_Message_IDs_t;

//-------------------------- Customized WARG Command Structs ---------------------------------------------------------------

typedef struct PIGO_GPS_LANDING_SPOT_t {
    int32_t latitude;
    int32_t longitude;
    int32_t altitude;
    int32_t landingDirection;
} PIGO_GPS_LANDING_SPOT_t;


typedef struct Warg_Euler_Angle_t {
    float yaw;
    float pitch;
    float roll;
} Warg_Euler_Angle_t;


//-------------------------- Prototypes ---------------------------------------------------------------

/**
 * This decoder is consists of two parts, parser and decoder.
 * The parser handles mavlink messages one byte at a time. 
 * Once the complete packet could be successfully decoded, the decoder would translate the message into telemetry data.
 * 
 * mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE;
    mavlink_global_position_int_t global_position_decoded;

    while(byteavailable == true) // 50 is just a random number larger than message length (for GPS message length is 39)
    {
        // get incoming byte here, decoder can pick up the starting byte by itself

        if (decoderStatus != MAVLINK_DECODING_OKAY) //this make sure it stops after a whole message is received
        {
            decoderStatus = Mavlink_airside_decoder(MAVLINK_COMM_0, current_byte, (uint8_t*) &global_position_decoded);
        }
    }
 * 
 **/
mavlink_decoding_status_t Mavlink_airside_decoder(int channel, uint8_t incomingByte, uint8_t *telemetryData);

/**
 * @brief Encode an selected struct
 *
 * @param type The type of telemetry data e.g. GPS or takeoff or landing...
 * @param message The MAVLink message to compress the data into, this is guarenteed to be a full message starts from byte 0xfd
 * @param struct_ptr C-struct to read the message contents from
 * 
 * @return the status of encoding
 * 
 * Example usage, give the address of the struct you want to encode
 *     mavlink_global_position_int_t global_position = 
    {
        ...
    };

    mavlink_message_t encoded_msg;

    uint8_t encoderStatus = Mavlink_airside_encoder(MESSAGE_ID_GPS, &encoded_msg, (const uint8_t*) &global_position);
 */
mavlink_encoding_status_t Mavlink_airside_encoder(POGI_Message_IDs_t id, mavlink_message_t *message, const uint8_t *struct_ptr);


int test__encode_then_decode(void);

// functions created while attampting to create warg customed Mavlink messages
uint16_t custom_mavlink_msg__begin_takeoff_command_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* message, const mavlink_custom_cmd_takeoff_t* struct_ptr);
void custom_mavlink_msg__begin_takeoff_command_decode(const mavlink_message_t* message, mavlink_custom_cmd_takeoff_t* takeoff_command);
uint16_t custom_fcn__calculate_crc(mavlink_message_t* msg, uint8_t crc_extra);

POGI_Message_IDs_t Mavlink_airside_decoder_get_message_type(void);

#endif //MAVLINKFUNCTIONS_HPP
