/*
 * debug.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

extern UART_HandleTypeDef huart1;

void dbg_puts(char *msg, uint16_t len)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)msg, len, 300);
}

uint32_t dbg_printf(char *fmt, ...)
{
	char buffer[256];
	uint16_t iw;

	va_list ap;
	va_start(ap, fmt);

	iw = vsprintf(buffer, fmt, ap);

	dbg_puts(buffer, iw);

	va_end(ap);

	return iw;
}

