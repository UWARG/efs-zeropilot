
#include "debug.h"

int __io_putchar(int ch) {
#if DBG_TYPE == DBG_UART
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFF);
#elif DBG_TYPE == DBG_ITM
    ITM_SendChar(ch);
#endif
    return ch;
}
