#include "xbee.hpp"

XBEE* XBEE::xbeeInstance = nullptr;

XBEE* XBEE::getInstance() {
    if (!xbeeInstance) {
        xbeeInstance = new XBEE();
    }

    return xbeeInstance;
}

XBEE::XBEE() {}

void XBEE::Receive_GS_Data() {}

void XBEE::Send_GS_Data() {}

void XBEE::GetResult(uint8_t* data) {
	uint8_t array_until_len[2];
	HAL_USART_Receive(&husart2, &array_until_len[0], 1, 100);
	HAL_USART_Receive(&husart2, &array_until_len[0], 1, 100);
	uint8_t data[11 + array_until_len[1]];
	data[0] = array_until_len[0];
	data[1] = array_until_len[1];
	for(int i = 2; i < array_until_len[1] + 2; i++) {
		HAL_USART_Receive(&husart2, &data[i], 1, 100);
	}
}

void XBEE::SendResult(Telemetry_POGI_t* data) {
	for(unsigned int i = 0; i < sizeof(data); i++) {
		HAL_USART_Transmit(&husart2, &data[i], 1, 100);
	}
}




