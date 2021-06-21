/*
* Author: Dhruv Rawat
*/

#include <gtest/gtest.h>
#include <fff.h>

#include "airsideMavlinkDecodingEncoding.hpp"
#include "Airside_Functions.hpp"
#include "Mavlink2_lib/common/mavlink.h"


using namespace std; 
using ::testing::Test;


/*
    Cases we want to test:

        - Decoding
        - Encoding
*/

FAKE_VALUE_FUNC(mavlink_decoding_status_t, Mavlink_airside_decoder, PIGO_Message_IDs_e*, uint8_t, uint8_t*);
FAKE_VALUE_FUNC(mavlink_encoding_status_t, Mavlink_airside_encoder, POGI_Message_IDs_e, mavlink_message_t*, const uint8_t*);

class AirsideMavlinkDecodingEncoding : public ::testing::Test
{
	public:

		virtual void SetUp()
		{
			RESET_FAKE(Mavlink_airside_decoder);
            RESET_FAKE(Mavlink_airside_encoder);
		}

		virtual void TearDown()
		{
			FFF_RESET_HISTORY();
		}

};

TEST(AirsideMavlinkDecodingEncoding, Decoding) {
    EXPECT_EQ(1, 1);
}

TEST(AirsideMavlinkDecodingEncoding, Encoding) {
    EXPECT_EQ(1, 1);
}


