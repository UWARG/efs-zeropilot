// Author: Jingting Liu

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

typedef enum {
    MESSAGE_ID_GPS,
} Message_IDs_t;

//-------------------------- Prototypes ---------------------------------------------------------------

/**
 * This decoder is consists of two parts, parser and decoder.
 * The parser handles mavlink messages one byte at a time. 
 * Once the complete packet could be successfully decoded, the decoder would translate the message into telemetry data.
 * 
 * mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE;
    mavlink_global_position_int_t global_position_decoded;

    // the following few lines imitates how a decoder is used when it gets one byte at a time from a serial port

    while(byteavailable == true) // 50 is just a random number larger than message length (for GPS message length is 39)
    {
        // get incoming byte here

        if (decoderStatus != MAVLINK_DECODING_OKAY) //this make sure it stops after a whole message is received
        {
            decoderStatus = Mavlink_decoder(MAVLINK_COMM_0, current_byte, (uint8_t*) &global_position_decoded);
        }
    }
 * 
 **/
mavlink_decoding_status_t Mavlink_decoder(int channel, uint8_t incomingByte, uint8_t *telemetryData);

/**
 * @brief Encode an selected struct
 *
 * @param type The type of telemetry data
 * @param msg The MAVLink message to compress the data into, this is guarenteed to be a full message starts from byte 0xfd
 * @param struct_ptr C-struct to read the message contents from
 * 
 * @return the length of the finalized message: msg->len + header_len + signature_len
 */
mavlink_encoding_status_t Mavlink_encoder(Message_IDs_t type, mavlink_message_t *message, const uint8_t *struct_ptr);


int test__encode_then_decode(void);





#endif //MAVLINKFUNCTIONS_HPP
