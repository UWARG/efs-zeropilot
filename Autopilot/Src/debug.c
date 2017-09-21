#include <stdio.h>

#include "debug.h"
#include "usart.h"

void debug_init() {
    MX_USART3_UART_Init();
}


int __io_putchar(int ch) {
#if DBG_TYPE == DBG_UART
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFF);
#else
    ITM_SendChar(ch);
#endif
    return ch;
}

