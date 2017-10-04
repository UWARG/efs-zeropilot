#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "usart.h"
#include "cmsis_os.h"

#define DBG_UART 0
#define DBG_ITM  1

#ifndef DBG_TYPE
#define DBG_TYPE DBG_UART
#endif

#ifdef DEBUG
#define debug(fmt, ...) { \
    vTaskSuspendAll(); \
    printf(fmt "\r\n", ## __VA_ARGS__); \
    xTaskResumeAll(); \
    }
#else
#define debug(fmt, ...)
#endif /* DEBUG */

void debug_init(void);


#endif
