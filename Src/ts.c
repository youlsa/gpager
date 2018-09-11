/*
 * ts.c
 *
 *  Created on: 2017. 9. 27.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

#define TOUCH_DIRECTION_LEN	30

static uint16_t g_ts_count = 0;

void TS_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	TS_RESET_CLK_ENABLE();
	GPIO_InitStruct.Pin = TS_RESET_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TS_RESET_PORT, &GPIO_InitStruct);

	TS_INT_CLK_ENABLE();
	GPIO_InitStruct.Pin = TS_INT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(TS_INT_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(TS_RESET_PORT, TS_RESET_PIN, GPIO_PIN_RESET);

	HW_GPIO_SetIrq(TS_INT_PORT, TS_INT_PIN, 0);

	DelayMs(10);

	TS_Reset();
}

void TS_Reset(void)
{
	HAL_GPIO_WritePin(TS_RESET_PORT, TS_RESET_PIN, GPIO_PIN_SET);
	DelayMs(100);
}

void TS_Event(ts_data_t *data)
{
	uint16_t x = 0, y = 0;

	if(g_ts_count == 0)
	{
		memset(data, 0, sizeof(ts_data_t));
	}

	TS_GetXY(&x, &y);

	if(x == 0 && y == 0)
	{
		if(g_ts_count != 0)
		{
			event_put(EVENT_TOUCH_POINT, 1, NULL); // Touch point event end
		}

		g_ts_count = 0;
	}
	else
	{
		if(g_ts_count == 0)
		{
			data->x0 = x;
			data->y0 = y;
			event_put(EVENT_TOUCH_POINT, 0, NULL); // Touch point event start
		}
		else
		{
			data->x1 = x;
			data->y1 = y;
		}

		g_ts_count++;
	}
}

uint8_t TS_Direction(ts_data_t *data)
{
	uint8_t direction = TOUCH_POINT;

	if(data->x1 == 0 && data->y1 == 0)
	{
		return direction;
	}

	if(data->x0 > data->x1)
	{
		if((data->x0 - data->x1) >= TOUCH_DIRECTION_LEN)
		{
			direction = TOUCH_RIGHT;
		}
	}

	if(data->x0 < data->x1)
	{
		if((data->x1 - data->x0) >= TOUCH_DIRECTION_LEN)
		{
			direction = TOUCH_LEFT;
		}
	}
	if(data->y0 > data->y1)
	{
		if((data->y0 - data->y1) >= TOUCH_DIRECTION_LEN)
		{
			direction = TOUCH_UP;
		}
	}

	if(data->y0 < data->y1)
	{
		if((data->y1 - data->y0) >= TOUCH_DIRECTION_LEN)
		{
			direction = TOUCH_DOWN;
		}
	}
	return (direction);
}

void TS_GetXY(uint16_t *x, uint16_t *y)
{
	uint8_t i, touches;
	uint8_t buffer[33] = {0};
	uint16_t touchX[2], touchY[2];

	TS_Read(0, buffer, 33);

	touches = buffer[0x2];

	if(touches > 2)
	{
		touches = 0;
		*x = *y = 0;
	}

	if(touches == 0)
	{
		*x = *y = 0;
		return;
	}

	for(i=0; i<2; i++)
	{
		touchX[i] =   buffer[0x03 + i*6] & 0x0F;
		touchX[i] <<= 8;
		touchX[i] |=  buffer[0x04 + i*6];
		touchY[i] =   buffer[0x05 + i*6] & 0x0F;
		touchY[i] <<= 8;
		touchY[i] |=  buffer[0x06 + i*6];
	}

	*x = touchX[0];
	*y = touchY[0];
}
