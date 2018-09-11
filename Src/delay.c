/*
 * delay.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

void DelayMs(uint32_t ms)
{
	HW_RTC_DelayMs(ms);
}

void Delay(float s)
{
    DelayMs((uint32_t)(s * 1000.0f));
}
