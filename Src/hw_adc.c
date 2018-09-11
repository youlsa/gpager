/*
 * hw_adc.c
 *
 *  Created on: 2017. 11. 7.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

ADC_HandleTypeDef hadc1;

void HW_ADC_Init(void)
{
	ADC_ChannelConfTypeDef sConfig;

	/**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if(HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}

	/**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(adcHandle->Instance == ADC1)
	{
		/* Peripheral clock enable */
		__HAL_RCC_ADC1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**ADC1 GPIO Configuration
    	PA2     ------> ADC1_IN2
		 */
		GPIO_InitStruct.Pin = BAT_ADC_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(BAT_ADC_PORT, &GPIO_InitStruct);

		/* ADC1 interrupt Init */
		HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(ADC_IRQn);
	}
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
	if(adcHandle->Instance == ADC1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_ADC1_CLK_DISABLE();

		/**ADC1 GPIO Configuration
    	PA2     ------> ADC1_IN2
		 */
		HAL_GPIO_DeInit(BAT_ADC_PORT, BAT_ADC_PIN);

		/* ADC1 interrupt Deinit */
		HAL_NVIC_DisableIRQ(ADC_IRQn);
	}
}

void HW_ADC_IrqHandler(void)
{
	HAL_ADC_IRQHandler(&hadc1);
}

HAL_StatusTypeDef BatteryLevel_Check_Start_IT(void)
{
	HAL_StatusTypeDef status;

	status = HAL_ADC_Start_IT(&hadc1);

	return status;
}

HAL_StatusTypeDef BatteryLevel_Check_Stop_IT(void)
{
	HAL_StatusTypeDef status;

	status = HAL_ADC_Stop_IT(&hadc1);

	return status;
}
