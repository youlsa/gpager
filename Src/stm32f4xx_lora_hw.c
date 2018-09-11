/*
 * stm32f4xx_lora_hw.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "radio.h"

/*!
 *  \brief Unique Devices IDs register set ( STM32L0xxx )
 */
#define ID1                                 ( 0x1FF80050 )
#define ID2                                 ( 0x1FF80054 )
#define ID3                                 ( 0x1FF80064 )

/*!
 * Flag to indicate if the MCU is Initialized
 */
static bool McuInitialized = false;

void HW_Init(void)
{
	if(McuInitialized == false)
	{
		HW_LORA_SPI_Init();
		Radio.IoInit();

		McuInitialized = true;
	}
}

void HW_DeInit(void)
{
	HW_LORA_SPI_DeInit();
	Radio.IoDeInit();
	McuInitialized = false;
}

/**
  * @brief This function Initializes the hardware Ios
  * @param None
  * @retval None
  */
static void HW_IoInit( void )
{
	HW_LORA_SPI_IoInit();
	Radio.IoInit();
}

/**
  * @brief This function Deinitializes the hardware Ios
  * @param None
  * @retval None
  */
static void HW_IoDeInit( void )
{
	HW_LORA_SPI_IoDeInit();
	Radio.IoDeInit();
}

/**
  * @brief This function return a random seed
  * @note based on the device unique ID
  * @param None
  * @retval see
  */
uint32_t HW_GetRandomSeed( void )
{
	return ((*( uint32_t* )ID1) ^ (*( uint32_t* )ID2) ^ (*( uint32_t* )ID3));
}

/**
  * @brief This function return a unique ID
  * @param unique ID
  * @retval none
  */
void HW_GetUniqueId( uint8_t *id )
{
	id[7] = ((*(uint32_t*)ID1)+ (*(uint32_t*)ID3)) >> 24;
	id[6] = ((*(uint32_t*)ID1)+ (*(uint32_t*)ID3)) >> 16;
	id[5] = ((*(uint32_t*)ID1)+ (*(uint32_t*)ID3)) >> 8;
	id[4] = ((*(uint32_t*)ID1)+ (*(uint32_t*)ID3));
	id[3] = ((*(uint32_t*)ID2)) >> 24;
	id[2] = ((*(uint32_t*)ID2)) >> 16;
	id[1] = ((*(uint32_t*)ID2)) >> 8;
	id[0] = ((*(uint32_t*)ID2));
}

/**
  * @brief Enters Low Power Stop Mode
  * @note ARM exists the function when waking up
  * @param none
  * @retval none
  */
void HW_EnterStopMode(void)
{
	BACKUP_PRIMASK();

	DISABLE_IRQ();

	HW_IoDeInit();

	/*clear wake up flag*/
	SET_BIT(PWR->CR, PWR_CR_CWUF);

	RESTORE_PRIMASK();

	/* Enter Stop Mode */
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

/**
  * @brief Exists Low Power Stop Mode
  * @note Enable the pll at 32MHz
  * @param none
  * @retval none
  */
void HW_ExitStopMode(void)
{
	/* Disable IRQ while the MCU is not running on HSI */

	BACKUP_PRIMASK();

	DISABLE_IRQ();

	/* After wake-up from STOP reconfigure the system clock */
	/* Enable HSI */
	__HAL_RCC_HSI_ENABLE();

	/* Wait till HSI is ready */
	while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET) {}

	/* Enable PLL */
	__HAL_RCC_PLL_ENABLE();
	/* Wait till PLL is ready */
	while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET) {}

	/* Select PLL as system clock source */
	__HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);

	/* Wait till PLL is used as system clock source */
	while(__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK) {}

	/*initilizes the peripherals*/
	HW_IoInit();

	RESTORE_PRIMASK();
}

/**
  * @brief Enters Low Power Sleep Mode
  * @note ARM exits the function when waking up
  * @param none
  * @retval none
  */
void HW_EnterSleepMode(void)
{
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}
