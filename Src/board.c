/*
 * board.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

/*!
 * Unique Devices IDs register set ( STM32L )
 */
#define         ID1                                 0x0FE081F0
#define         ID2                                 0x0FE081F4

static uint8_t batterylevel(void);

void _RESET(void)
{
	NVIC_SystemReset();
}

uint32_t BoardGetRandomSeed(void)
{
	return ((*(uint32_t*)ID1) ^ (*(uint32_t*)ID2));
}

void BoardGetUniqueId(uint8_t *id)
{
	id[7] = ((*(uint32_t*)ID1)) >> 24;
	id[6] = ((*(uint32_t*)ID1)) >> 16;
	id[5] = ((*(uint32_t*)ID1)) >> 8;
	id[4] = ((*(uint32_t*)ID1));
	id[3] = ((*(uint32_t*)ID2)) >> 24;
	id[2] = ((*(uint32_t*)ID2)) >> 16;
	id[1] = ((*(uint32_t*)ID2)) >> 8;
	id[0] = ((*(uint32_t*)ID2));
}

uint8_t BoardGetBatteryLevel(void)
{
	return batterylevel();
}


static uint8_t batterylevel(void)
{
	uint8_t level = 0;
	if(global.battery_level > 340 && global.battery_level <= 344)      level = 10;
	else if(global.battery_level > 344 && global.battery_level <= 350) level = 50;
	else if(global.battery_level > 350 && global.battery_level <= 360) level = 80;
	else if(global.battery_level > 360 && global.battery_level <= 370) level = 100;
	else if(global.battery_level > 370 && global.battery_level <= 380) level = 130;
	else if(global.battery_level > 380 && global.battery_level <= 390) level = 160;
	else if(global.battery_level > 390 && global.battery_level <= 400) level = 180;
	else if(global.battery_level > 400 && global.battery_level <= 410) level = 210;
	else if(global.battery_level > 410 && global.battery_level <= 414) level = 230;
	else if(global.battery_level > 414)                                level = 255;

	DBG_PRINTF("device bat : %d \r\n", level);

	return level;
}
