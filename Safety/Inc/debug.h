#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "usart.h"

#define DBG_UART 0
#define DBG_ITM  1

#ifndef DBG_TYPE
#define DBG_TYPE DBG_UART
#endif

#ifdef DEBUG
#define debug(fmt, ...) printf(fmt "\r\n", ## __VA_ARGS__);
#else
#define debug(fmt, ...)
#endif /* DEBUG */

#endif
