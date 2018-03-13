
#include "debug.h"
#include "stm32f7xx_hal_rcc.h"

void reset_cause_check(){
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) debug("Pin reset");
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) debug("POR/PWD reset");
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) debug("Software reset");
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) debug("Independent watch dog rest");
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) debug("Window watch dog reset");
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) debug("Low power reset");
}