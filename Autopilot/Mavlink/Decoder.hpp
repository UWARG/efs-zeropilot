// Author: Jingting Liu

#ifndef DECODER_HPP
#define DECODER_HPP

#include "Mavlink2_lib/common/mavlink.h"

typedef enum {
    MAVLINK_DECODING_INCOMPLETE=0,
    MAVLINK_DECODING_OKAY=1,
    MAVLINK_DECODING_FAIL=2,
    MAVLINK_DECODING_NULL_PTR=3,
} mavlink_decoding_status_t;


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
 * uint8_t *telemetryData;
 * int channel = MAVLINK_COMM_0; //0, assume only one MAVlink stream exists
 * 
 * //the following part should exist in the state machine
 * while(serial.bytesAvailable > 0) // should make the xbee send one byte at a time from the serial port
 * {
 *   uint8_t byte = serial.getNextByte(); //this is just a space filler arduino function
 *   mavlink_decoding_status_t decoderStatus = Mavlink_decoder(channel, byte ,&msg, &status, &telemetryData)
 *   
 * }
 **/
mavlink_decoding_status_t Mavlink_decoder(int channel, uint8_t incomingByte, mavlink_message_t *msg, uint8_t **telemetryData) ;

#endif