// Author: Jingting Liu

#ifndef ENCODER_HPP
#define ENCODER_HPP

#include "Mavlink2_lib/common/mavlink.h"

typedef enum {
    MESSAGE_ID_GPS,
} Message_IDs_t;

uint16_t Mavlink_encoder(Message_IDs_t type, mavlink_message_t ** msg, const uint64_t *struct_ptr);

#endif