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

xbee_in_t GetResult() {
	uint8_t holder;
	xbee_in_t xbeeIn;
	HAL_USART_Receive(&husart2, &holder, 1, 100);
	HAL_USART_Receive(&husart2, &xbeeIn.len, 1, 100);
	uint8_t payload[xbeeIn.len];
	for(int i = 2; i < xbeeIn.len; i++) {
		HAL_USART_Receive_DMA(&husart2, &payload[i], 1);
	}
	xbeeIn.payload = payload;
	return xbeeIn;
}

void XBEE::SendResult(Telemetry_POGI_t* data) {
	for(unsigned int i = 0; i < sizeof(data); i++) {
		HAL_USART_Transmit(&husart2, &data[i], 1, 100);
	}
}




