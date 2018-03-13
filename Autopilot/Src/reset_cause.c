
#include "debug.h"
#include "stm32f7xx_hal_rcc.h"

void reset_cause_check(){
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) debug("Pin reset");
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) debug("POR/PWD reset");
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) debug("Software reset");
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) debug("Independent watch dog rest");
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) debug("Window watch dog reset");
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) debug("Low power reset");
}