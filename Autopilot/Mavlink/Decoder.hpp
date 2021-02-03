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


int Mavlink_decoder(int chan, uint8_t sampleByte, mavlink_message_t &msg, mavlink_status_t &status, uint8_t* telemetryData);

#endif