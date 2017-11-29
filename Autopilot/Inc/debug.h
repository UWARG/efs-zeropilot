#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "usart.h"
#include "cmsis_os.h"

#define DBG_UART 0
#define DBG_ITM  1

#ifndef DBG_TYPE
#define DBG_TYPE DBG_UART
#endif

#ifndef NDEBUG
#define DEBUG
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

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */
