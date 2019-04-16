
#include "debug.h"

int _write(int file, char *ptr, int len) {
#if DBG_TYPE == DBG_UART
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, 0xFF);
#elif DBG_TYPE == DBG_ITM
    for (uint8_t i = 0; i < len; i++) {
        ITM_SendChar(ptr[i]);
    }
#endif
    return len;
}
