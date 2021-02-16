// Author: Jingting Liu

#include "Mavlink2_lib/common/mavlink.h"
#include "Encoder.hpp"

// search keyword " _int_encode " to see all the encoders in the mavlink lib

mavlink_message_t outgoing_msg;
mavlink_global_position_int_t global_position = {
    1, // timestamp
    1, // latitude
    1, // longitude
    1, // altitude
    1, // altitude above ground
    1, // X speed, latitude positive north
    1, // y speed, longitude, positive east
    1, // z speed, positive down
    1 // hdg
};

/**
 * @brief Encode an selected struct
 *
 * @param type The type of telemetry data
 * @param msg The MAVLink message to compress the data into
 * @param global_position_int C-struct to read the message contents from
 * 
 * @return the length of the finalized message: msg->len + header_len + signature_len
 */
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