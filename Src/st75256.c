/*
 * st75256.c
 *
 *  Created on: 2017. 9. 27.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "st75256.h"

LCD_DrvTypeDef st75256_drv =
{
	st75256_Init,
	st75256_DrawPixel,
	st75256_DrawHLine,
	st75256_DrawVLine,
	st75256_GetLcdPixelWidth,
	st75256_GetLcdPixelHeight,
	st75256_DisplayOn,
	st75256_DisplayOff,
	st75256_Update,
};

static uint8_t vop = 0xB9;

static uint8_t 	lcdview[ST75256_LCD_WIDTH][ST75256_LCD_HEIGHT];
static uint8_t 	lcdview_keep[ST75256_LCD_WIDTH][ST75256_LCD_HEIGHT];
static uint32_t tcount = 0;

static void st75256_Write_To_Hardware(void);
static void st75256_Clean_Screen(void);

static void st75256_Clean_Screen(void)
{
	int size;
	uint16_t page, seg;

	for(page=0; page<ST75256_LCD_HEIGHT; page++)
	{
		LCD_IO_WriteReg(0x75);
		LCD_IO_WriteData(page);

		for(seg=0; seg<ST75256_LCD_WIDTH; seg++)
		{
			LCD_IO_WriteReg(0x15);
			LCD_IO_WriteData(seg);

			LCD_IO_WriteReg(0x5C);
			LCD_IO_WriteData(0x00);
		}
	}

	size = sizeof(lcdview);
	memset(lcdview, 0x0, size);
	memset(lcdview_keep, 0x0, size);
}

void st75256_Init(void)
{
	LCD_IO_Init();

	LCD_IO_WriteReg(0x30);		// Extension Command 1

	LCD_IO_WriteReg(0x6E);		// Enable Master

	LCD_IO_WriteReg(0x31);		// Extension Command 2

	LCD_IO_WriteReg(0xD7);		// Disable Auto Read
	LCD_IO_WriteData(0x9F);

	LCD_IO_WriteReg(0xE0);		// Enable OTP Read
	LCD_IO_WriteData(0x00);
	LCD_Delay(10);

	LCD_IO_WriteReg(0xE3);		// OTP Up-Load
	LCD_Delay(20);

	LCD_IO_WriteReg(0xE1);		// OTP Control Out

	LCD_IO_WriteReg(0x30);		// Extension Command 1

	LCD_IO_WriteReg(0x94);		// Sleep out

	LCD_IO_WriteReg(0xAE);		// Display off

	LCD_IO_WriteReg(0xD1);		// Osc on
	LCD_Delay(50);

	LCD_IO_WriteReg(0x20);		// Power Control
	LCD_IO_WriteData(0x0B);   	// VB,VR,VF All on

	LCD_IO_WriteReg(0x81);		// Set vop=11.0V
	LCD_IO_WriteData(0x3F & vop);
	LCD_IO_WriteData(0x07 & (vop>>6));

	LCD_IO_WriteReg(0x31); 		// Extension Command 2

	LCD_IO_WriteReg(0x32); 		// Analog Circuit Set
	LCD_IO_WriteData(0x00);
	LCD_IO_WriteData(0x01);    	// Booster Efficiency
	LCD_IO_WriteData(0x03);    	// Bias=1/11

	LCD_IO_WriteReg(0x51);		// Booster Level x10
	LCD_IO_WriteData(0xFB);

	LCD_IO_WriteReg(0x30);		// Extension Command 1

	LCD_IO_WriteReg(0xF0);		// Display Mode
	LCD_IO_WriteData(0x10);   	// Mono Mode

	LCD_IO_WriteReg(0xCA);		// Display Control
	LCD_IO_WriteData(0x00);   	// CL Dividing Ratio-not divide
	LCD_IO_WriteData(0x5F);   	// Duty Set: 1/96
	LCD_IO_WriteData(0x00);   	// Frame Inversion

	LCD_IO_WriteReg(0xBC);		//Data Scan Direction
	LCD_IO_WriteData(0x00);

	LCD_IO_WriteReg(0xA6);		// Normal Display

	LCD_IO_WriteReg(0x31);		// Extension Command 2

	LCD_IO_WriteReg(0x40);		// Intermal Power Supply

	LCD_IO_WriteReg(0x30); 		// Extension Command 1

	LCD_IO_WriteReg(0x76);		// Disable ICON RAM

	LCD_IO_WriteReg(0x15);		// Column Address Setting
	LCD_IO_WriteData(0x00);
	LCD_IO_WriteData(0xFF);		// SEG0-SEG255

	LCD_IO_WriteReg(0x75);		// Page address setting
	LCD_IO_WriteData(0x00);
	LCD_IO_WriteData(0x0B);		// COM0-COM95

	LCD_IO_WriteReg(0xAF);		// Display on

	st75256_Clean_Screen();
}

void st75256_DrawPixel(uint16_t x, uint16_t y, uint32_t color)
{
	uint8_t *pc, c;

	if(y>=0 && y<8)
		pc = &lcdview[x][0];	/*page 0*/
	else if(y>=8 && y<16)
		pc = &lcdview[x][1]; 	/*page 1*/
	else if(y>=16 && y<24)
		pc = &lcdview[x][2]; 	/*page 2*/
	else if(y>=24 && y<32)
		pc = &lcdview[x][3]; 	/*page 3*/
	else if(y>=32 && y<40)
		pc = &lcdview[x][4]; 	/*page 4*/
	else if(y>=40 && y<48)
		pc = &lcdview[x][5]; 	/*page 5*/
	else if(y>=48 && y<56)
		pc = &lcdview[x][6]; 	/*page 6*/
	else if(y>=56 && y<64)
		pc = &lcdview[x][7]; 	/*page 7*/
	else if(y>=64 && y<72)
		pc = &lcdview[x][8]; 	/*page 8*/
	else if(y>=72 && y<80)
		pc = &lcdview[x][9]; 	/*page 9*/
	else if(y>=80 && y<88)
		pc = &lcdview[x][10]; 	/*page 10*/
	else if(y>=88 && y<96)
		pc = &lcdview[x][11]; 	/*page 11*/

	c = *pc;

	if(color == 0)	/*black*/
	{
		c |= 0x80 >> (y%8);
	}
	else
	{
		c &= ~(0x80 >> (y%8));
	}

	*pc = c;

	/* write back to lcdview */
	tcount++;
}

void st75256_DrawHLine(uint16_t x, uint16_t y, uint16_t length, uint32_t color)
{
	uint16_t i;

	if(x + length > ST75256_LCD_WIDTH) return;

	for(i=x; i<x+length; i++)
		st75256_DrawPixel(i, y, color);
}

void st75256_DrawVLine(uint16_t x, uint16_t y, uint16_t length, uint32_t color)
{
	uint16_t i;

	if(y + length > ST75256_LCD_HEIGHT_PIXEL) return;

	for(i=y; i<y+length; i++)
		st75256_DrawPixel(x, i, color);
}

uint16_t st75256_GetLcdPixelWidth(void)
{
	return ST75256_LCD_WIDTH;
}

uint16_t st75256_GetLcdPixelHeight(void)
{
	return ST75256_LCD_HEIGHT_PIXEL;
}

void st75256_DisplayOn(void)
{
	LCD_IO_WriteReg(0xAF);		// Display on
}

void st75256_DisplayOff(void)
{
	LCD_IO_WriteReg(0xAE);		// Display off
}

typedef struct update {
	uint16_t x;
	uint16_t y;
	uint8_t  data;
} UPDATE;

static UPDATE update[3072];
static uint32_t update_count = 0;
void st75256_Update(void *p)
{
	uint16_t x, y;

	if(tcount)
	{
		/* compare the lcdview and lcdview keep */
		for(x=0; x<ST75256_LCD_WIDTH; x++)
		{
			for(y=0; y<ST75256_LCD_HEIGHT; y++)
			{
				if(lcdview[x][y] != lcdview_keep[x][y])
				{
					/* this byte need to write to driver */
					update[update_count].x = x;
					update[update_count].y = y;
					update[update_count].data = lcdview[x][y];
					update_count++;
				}
			}
		}

		st75256_Write_To_Hardware();
		memcpy(lcdview_keep, lcdview, sizeof(lcdview));
		tcount = 0;
	}
}

static void st75256_Write_To_Hardware(void)
{
	int i, page;
	UPDATE *pd;

	page = -1;

	for(i=0; i<update_count; i++)
	{
		pd = &update[i];

		if(page != pd->y)	/*page changed*/
		{
			LCD_IO_WriteReg(0x75);
			LCD_IO_WriteData(pd->y);

			page = (int)pd->y;
		}

		/* pd point a segment */
		LCD_IO_WriteReg(0x15);
		LCD_IO_WriteData(pd->x);

		LCD_IO_WriteReg(0x5C);
		LCD_IO_WriteData(pd->data);
	}

	update_count = 0;
}
