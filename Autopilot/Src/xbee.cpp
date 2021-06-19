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





