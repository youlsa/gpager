/*
 * low_power.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "low_power.h"

/**
 * \brief Flag to indicate if MCU can go to low power mode
 *        When 0, MCU is authorized to go in low power mode
 */
static uint32_t LowPower_State = 0;

/**
 * \brief API to set flag allowing power mode
 *
 * \param [IN] enum e_LOW_POWER_State_Id_t
 */
void LowPower_Disable(e_LOW_POWER_State_Id_t state)
{
	BACKUP_PRIMASK();

	DISABLE_IRQ();

	LowPower_State |= state;

	RESTORE_PRIMASK();
}

/**
 * \brief API to reset flag allowing power mode
 *
 * \param [IN] enum e_LOW_POWER_State_Id_t
 */
void LowPower_Enable(e_LOW_POWER_State_Id_t state)
{
	BACKUP_PRIMASK();

	DISABLE_IRQ( );

	LowPower_State &= ~state;

	RESTORE_PRIMASK();
}

/**
 * \brief API to get flag allowing power mode
 * \note When flag is 0, low power mode is allowed
 * \param [IN] state
 * \retval flag state
 */
uint32_t LowPower_GetState(void)
{
	return LowPower_State;
}

/**
 * @brief  Handle Low Power
 * @param  None
 * @retval None
 */

void LowPower_Handler(void)
{
	if(LowPower_State == 0)
	{
		DBG_PRINTF("dz\r\n");

		HW_EnterStopMode();

		/* mcu dependent. to be implemented by user*/
		HW_ExitStopMode();

		HW_RTC_setMcuWakeUpTime();
	}
	else
	{
		DBG_PRINTF("z\r\n");
		HW_EnterSleepMode();
	}

}
