/*
 * utilities.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "utilities.h"

/*!
 * Redefinition of rand() and srand() standard C functions.
 * These functions are redefined in order to get the same behavior across
 * different compiler toolchains implementations.
 */
// Standard random functions redefinition start
#define RAND_LOCAL_MAX 2147483647L

static uint32_t next = 1;

int32_t rand1(void)
{
	return ((next = next * 1103515245L + 12345L) % RAND_LOCAL_MAX);
}

void srand1(uint32_t seed)
{
	next = seed;
}
// Standard random functions redefinition end

int32_t randr(int32_t min, int32_t max)
{
	return (int32_t)rand1( ) % (max - min + 1) + min;
}

void memcpy1(uint8_t *dst, const uint8_t *src, uint16_t size)
{
    while(size--)
	{
    	*dst++ = *src++;
	}
}

void memcpyr(uint8_t *dst, const uint8_t *src, uint16_t size)
{
    dst = dst + (size - 1);
    while(size--)
    {
        *dst-- = *src++;
    }
}

void memset1(uint8_t *dst, uint8_t value, uint16_t size)
{
	while(size--)
	{
		*dst++ = value;
	}
}

int8_t Nibble2HexChar(uint8_t a)
{
	if(a < 10)
	{
		return '0' + a;
	}
	else if(a < 16)
	{
		return 'A' + ( a - 10 );
	}
	else
	{
		return '?';
	}
}

bool memzero(uint8_t *data, uint8_t size)
{
	for(int i=0; i<size; i++)
	{
		if(data[i] != 0) return false;
	}

	return true;
}

char *date2str(uint32_t time)
{
  static char tp[20];
  struct tm *tm1 = localtime((time_t *)&time);
  sprintf(tp, "%04u-%02u-%02u %02u:%02u:%02u", 2000+tm1->tm_year-100, tm1->tm_mon+1, tm1->tm_mday, tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
  return tp;
}
