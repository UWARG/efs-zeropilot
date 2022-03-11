#include "stm32f0xx_hal.h"
#include "main.h"
#include "PWM.hpp"
#define MAX_CHANNELS 10
#define NUM_AVAILABLE_CHANNELS 8
#define DSHOT_DATA_FRAME_LEN 16
#define DSHOT_DMA_BUFFER_SIZE 18
#define DSHOT_MAX_THROTTLE 2000
#define DSHOT_RESERVED_VALUES 47
#define DSHOT_150_BIT_1 240
#define DSHOT_150_BIT_0 120
#define NUM_DSHOT_MOTORS 4

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

static uint32_t DMAMotorBuffer1[DSHOT_DMA_BUFFER_SIZE];
static uint32_t DMAMotorBuffer2[DSHOT_DMA_BUFFER_SIZE];
static uint32_t DMAMotorBuffer3[DSHOT_DMA_BUFFER_SIZE];
static uint32_t DMAMotorBuffer4[DSHOT_DMA_BUFFER_SIZE];

static const PWMPinConfig PWM_CONFIG[MAX_CHANNELS] 
{
    {PWM1_Pin, PWM1_GPIO_Port, &htim1, TIM_CHANNEL_1, true, DMAMotorBuffer1, TIM_DMA_ID_CC1, TIM_DMA_CC1},
    {PWM2_Pin, PWM2_GPIO_Port, &htim1, TIM_CHANNEL_2, true, DMAMotorBuffer2, TIM_DMA_ID_CC2, TIM_DMA_CC2},
    {PWM3_Pin, PWM3_GPIO_Port, &htim1, TIM_CHANNEL_3, true, DMAMotorBuffer3, TIM_DMA_ID_CC3, TIM_DMA_CC3},
    {PWM4_Pin, PWM4_GPIO_Port, &htim1, TIM_CHANNEL_4, true, DMAMotorBuffer4, TIM_DMA_ID_CC4, TIM_DMA_CC4}, 
    {PWM6_Pin, PWM6_GPIO_Port, &htim3, TIM_CHANNEL_1, false, {0}, 0, 0},
    {PWM10_Pin, PWM10_GPIO_Port, &htim3, TIM_CHANNEL_2, false, {0}, 0, 0},
    {PWM11_Pin, PWM11_GPIO_Port, &htim3, TIM_CHANNEL_3, false, {0}, 0, 0},
    {PWM12_Pin, PWM12_GPIO_Port, &htim3, TIM_CHANNEL_4, false, {0}, 0, 0}
};

PWMChannel::PWMChannel()
{
    for(int i = 0; i < NUM_AVAILABLE_CHANNELS; i++)
    {
        PWMPinConfig currentChannel = PWM_CONFIG[i];
        HAL_TIM_PWM_Start(currentChannel.timer,currentChannel.timer_channel);

        if (currentChannel.isUsingDshot)
        {
                //setting up dma call back function that runs when the transfer is complete and disables the dma
                dshotSetupDMACallbacks();
        }

    }
}

void PWMChannel::set(uint8_t channel, uint8_t percent)
{
    if(channel > MAX_CHANNELS || channel < 0)
    {
        channel = 0;
    }

    const PWMPinConfig * currentChannel = &PWM_CONFIG[channel];

    if (currentChannel->isUsingDshot)
    {
        dshotPrepareDMABuffer(currentChannel->dshotDMABuffer, percent);
        dshotStartDMA(*currentChannel);
        dshotEnableDMARequests(*currentChannel);
    }

    else
    {
        uint32_t prescaler = (static_cast<TIM_HandleTypeDef *>(currentChannel->timer))->Init.Prescaler;
	    uint32_t us = ((percent * (PWMChannel::max_signal - PWMChannel::min_signal)) / 100 + PWMChannel::min_signal);
        uint32_t periodTicks = (static_cast<TIM_HandleTypeDef *>(currentChannel->timer))->Init.Period;
	    uint32_t ticks = static_cast<uint32_t>((static_cast<float>(us) / static_cast<float>(pwmPeriod)) * static_cast<float>(periodTicks));
        __HAL_TIM_SET_COMPARE((TIM_HandleTypeDef *) currentChannel->timer, currentChannel->timer_channel, (uint32_t) ticks);
    }
    
}

void PWMChannel::dshotPrepareDMABuffer(uint32_t * dmaBuffer, uint8_t throttlePercentage)
{
    uint16_t frame = dshotPrepareFrame(throttlePercentage, false);

    for (int i = 0; i < DSHOT_DATA_FRAME_LEN; i++)
    {
        //Using the frame, populate the buffer with the duty cycle value corresponding to a 1 and 0
        //We are masking out everything except the most significant bit (far left bit) and setting the proper value in the buffer depending if it's a 1 or 0
        dmaBuffer[i] = (frame & 0x8000 ? DSHOT_150_BIT_1 : DSHOT_150_BIT_0);

        //Left shifting the current frame over by 1 to set the next frame bit in the buffer
        frame <<= 1;
    }

    //Setting the last two indexes in the buffer to 0 to ensure the last part of the signal is low
    dmaBuffer[16] = 0;
    dmaBuffer[17] = 0;
}

uint16_t PWMChannel::dshotPrepareFrame(uint8_t throttlePercentage, bool telemetry)
{
    /* DSHOT data frame (16 bits total):
     *
     *           b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
     *           |-------------------------------| |-| |-------------| 
     *                      Throttle Data           ^     Checksum
     *                                              |
     *                                           Telemetry
     */

    uint16_t frame;

    frame = (((uint16_t)(DSHOT_MAX_THROTTLE * (float)throttlePercentage / 100) + DSHOT_RESERVED_VALUES) << 1) | (telemetry ? 1 : 0); //throttle and telemetry bits

    uint8_t checksum = (frame ^ (frame >> 4) ^ (frame >> 8)) & 0x00F; //calculating checksum... splitting first 12 bits into 3 nibbles and XORing

    frame = (frame << 4) | checksum; //adding the checksum to the frame
    
    return frame;
}

void PWMChannel::dshotStartDMA(PWMPinConfig dshotConfig)
{
    uint32_t destinationBuffer;

    switch (dshotConfig.timer_channel)
    {
        case TIM_CHANNEL_1:
        destinationBuffer = (uint32_t)&dshotConfig.timer->Instance->CCR1;
        break;

        case TIM_CHANNEL_2:
        destinationBuffer = (uint32_t)&dshotConfig.timer->Instance->CCR2;
        break;

        case TIM_CHANNEL_3:
        destinationBuffer = (uint32_t)&dshotConfig.timer->Instance->CCR3;
        break;

        case TIM_CHANNEL_4:
        destinationBuffer = (uint32_t)&dshotConfig.timer->Instance->CCR4;
        break;

    }
    HAL_DMA_Start_IT(dshotConfig.timer->hdma[dshotConfig.timDMAHandleIndex], (uint32_t)dshotConfig.dshotDMABuffer, destinationBuffer, DSHOT_DMA_BUFFER_SIZE);
}

void PWMChannel::dshotEnableDMARequests(PWMPinConfig dshotConfig)
{
    __HAL_TIM_ENABLE_DMA(dshotConfig.timer, dshotConfig.timDMASource);
}

void PWMChannel::dshotSetupDMACallbacks()
{
    for (uint8_t i = 0; i < NUM_DSHOT_MOTORS; i++)
    {
        PWM_CONFIG[i].timer->hdma[PWM_CONFIG[i].timDMAHandleIndex]->XferCpltCallback = dshotDMACompleteCallback;
    }
}

void dshotDMACompleteCallback(DMA_HandleTypeDef *hdma)
{
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

	for (uint8_t i = 0; i < NUM_DSHOT_MOTORS; i++)
    {
        if (hdma == htim->hdma[PWM_CONFIG[i].timDMAHandleIndex])
	    {
		    __HAL_TIM_DISABLE_DMA(htim, PWM_CONFIG[i].timDMASource);
	    }
    }
}