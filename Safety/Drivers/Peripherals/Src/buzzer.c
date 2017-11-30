#include "buzzer.h"
#include "tim.h"
#include "debug.h"



static uint8_t state;
static uint8_t loop;

static uint8_t on_delay;
static uint8_t off_delay;

void Buzzer_init(){
    state = BUZZER_OFF;
    loop = 0;
    on_delay = 0;
    off_delay = 1;
    HAL_TIM_Base_Start_IT(&htim6);
}

void Buzzer_Set(uint8_t st){
    //check is something has changed
    if(state!= st){
        state = st;

        switch(state){
            case BUZZER_ON:
                HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
            case BUZZER_SHORT_FAST:
            off_delay = 10;
                break;
            case BUZZER_SHORT_SLOW:
            off_delay = 30;
                break;
            case BUZZER_THREE_SHORT:
                break;
            case BUZZER_LONG:
                break;
            default:
                HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
        }
    }

}
char buzzer_status = 0;

/*void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM6) {
        switch(state){
            case BUZZER_SHORT_FAST:
                if(buzzer_status == BUZZER_GPIO_OFF){
                    __HAL_TIM_SET_COUNTER(htim, 300000);
                    buzzer_status = BUZZER_GPIO_ON;
                    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
                }else{
                    __HAL_TIM_SET_COUNTER(htim, 300000);
                    buzzer_status = BUZZER_GPIO_OFF;
                    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
                }
                break;
            case BUZZER_SHORT_SLOW:
                if(buzzer_status == BUZZER_GPIO_OFF){
                    __HAL_TIM_SET_COUNTER(htim, 2000000);
                    buzzer_status = BUZZER_GPIO_ON;
                    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
                }else{
                    __HAL_TIM_SET_COUNTER(htim, 30000);
                    buzzer_status = BUZZER_GPIO_OFF;
                    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
                }
                break;
            case BUZZER_THREE_SHORT:
                if(buzzer_status == BUZZER_GPIO_OFF){
                    __HAL_TIM_SET_COUNTER(htim, 300000);
                    buzzer_status = BUZZER_GPIO_ON;
                    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
                }else{
                    __HAL_TIM_SET_COUNTER(htim, 300000);
                    buzzer_status = BUZZER_GPIO_OFF;
                    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
                }
                break;
            case BUZZER_LONG:
                if(buzzer_status == BUZZER_GPIO_OFF){
                    __HAL_TIM_SET_COUNTER(htim, 800000);
                    buzzer_status = BUZZER_GPIO_ON;
                    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
                }else{
                    __HAL_TIM_SET_COUNTER(htim, 800000);
                    buzzer_status = BUZZER_GPIO_OFF;
                    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
                }
                break;
        }
        
    }
}*/