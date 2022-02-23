/**
 * Implements abstracted PWM driver for the stm32F0. For pin mappings, look at the .cpp file
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 *
 * Implementation details for PWM generation on our STM32F0:
 *
 * What timer controls what PWM channels:
 *
 * TIM16_CH1 - PWM 1
 * TIM17_CH1 - PWM 2
 * TIM15_CH1-CH2 - PWM 3-4
 * TIM3_CH1-CH4 - PWM 5-8
 * TIM1_CH1-CH4 - PWM 9-12
 * Timer14 - PPM
 *
 * The above means we can only set the frequencies of a group of PWMs controlled by a single timer!
 * We can't individually control frequencies of channels!
 */

#pragma once

#include <stdint.h>
#include "stm32f0xx_hal.h"

#define DSHOT_DATA_FRAME_LEN 16
#define DSHOT_DMA_BUFFER_SIZE 18
#define DSHOT_MAX_THROTTLE 2047
#define DSHOT_BIT_1 240
#define DSHOT_BIT_0 120
#define NUM_DSHOT_MOTORS 4

typedef struct PWMPinConfig {
	uint16_t num;
	GPIO_TypeDef* port;
	TIM_HandleTypeDef *timer;
	uint16_t timer_channel;
  bool isUsingDshot;
  uint32_t *dshotDMABuffer;
  uint16_t timDMAHandleIndex;
  uint16_t timDMASources;

} PWMPinConfig;

typedef uint8_t PWMChannelNum;

/**
 * Represents a group of PWM outputs that map to a single timer
 * Settings can be changed on a per-group basis
 */
typedef enum PWMGroup {
	PWM_GROUP_1,
	PWM_GROUP_2,
	PWM_GROUP_3_4,
	PWM_GROUP_5_8,
	PWM_GROUP_9_12
} PWMGroup;

typedef struct PWMGroupSetting {
	uint32_t period; //period of pulse in us
	uint32_t min_length; //min pulse length in us
	uint32_t max_length; //max pulse length in us
	bool inverted = false;
} PWMGroupSetting;

typedef enum dshotType
{
    DSHOT150,
    DSHOT300,
    DSHOT600
} dshotType;

class PWMChannel {
public:
	PWMChannel();
	void set(uint8_t channel, uint8_t percent);
private:
	//values in us
	uint32_t pwmPeriod = 20000;
	uint32_t min_signal = 950; //standard for 50hz pwm signal, with a 100 Hz margin added to ensure we can reach the extreme values.
	uint32_t max_signal = 2050;

	/**
  * @brief Prepares the Dshot data frame including the data, telemetry bit, and checksum
  * @param throttlePercentage Trottle percentage output from the PID loop, 0-100%
  * @param telemetry Bool indicating whether to set the telemetry bit or not
  * @retval 16 bit dshot data frame
  */    
    uint16_t dshotPrepareFrame(uint8_t throttlePercentage, bool telemetry);

/**
  * @brief Prepares the DMA buffer using the data frame
  * @param dmaBuffer Pointer to the DMA buffer
  * @param frame DSHOT data frame
  * @retval None
  */    
    void dshotPrepareDMABuffer(uint32_t * dmaBuffer, uint8_t throttlePercentage);

/**
  * @brief Starts PWM generation of channels 1-4 on TIM1
  * @param dmaBuffer Pointer to the DMA buffer
  * @param None
  * @retval None
  */
    void dshotStartPWM();

/**
  * @brief Starts DMA peripheral 
  * @param dshotConfig which is of type struct PWMPinConfig used to start the DMA
  * @retval None
  */
    void dshotStartDMA(PWMPinConfig dshotConfig);

/**
  * @brief Enables DMA request 
  * @param dshotConfig which is of type struct PWMPinConfig used to enable the DMA request
  * @retval None
  */
    void dshotEnableDMARequests(PWMPinConfig dshotConfig);

/**
  * @brief Links the "dshotDMACompleteCallback" callback function to the specific dma channel
  * @param None
  * @retval None
  */
    void dshotSetupDMACallbacks();
};


/**
  * @brief Callback function to be called when a DMA transfer is complete 
  * @param hdma pointer to DMA handle
  * @retval None
  */
    void dshotDMACompleteCallback(DMA_HandleTypeDef *hdma);