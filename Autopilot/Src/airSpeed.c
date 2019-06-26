#include "airSpeed.h"
#include "adc.h"


/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void airSpeed_Init(void)
{
	MX_ADC3_Init();
	HAL_ADC_MspInit(&hadc3);
}

airSpeedState_t airSpeed_GetAirSpeed(float *airSpeed)
{

	HAL_StatusTypeDef status;

	status = HAL_ADC_Start(&hadc3);

	if (status != HAL_OK)
	{
		return AIR_SPEED_FAILED;
	}

	status = HAL_ADC_PollForConversion(&hadc3, 55);

	if (status != HAL_OK)
	{
		return AIR_SPEED_FAILED;
	}

	HAL_ADC_GetValue(&hadc3);

	status = HAL_ADC_Stop(&hadc3);

	if (status != HAL_OK)
	{
		return AIR_SPEED_FAILED;
	}

	return AIR_SPEED_SUCCEEDED;
}
