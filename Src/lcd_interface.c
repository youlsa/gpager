/*
 * lcd_interface.c
 *
 *  Created on: 2017. 9. 27.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "lcd.h"
#include "lcd_interface.h"
#include "st75256.h"

#define POLY_X(Z)             ((int32_t)((points + (Z))->X))
#define POLY_Y(Z)             ((int32_t)((points + (Z))->Y))

#define MAX_HEIGHT_FONT			16
#define MAX_WIDTH_FONT			16

#define ABS(X) ((X) > 0 ? (X) : -(X))

LCD_DrawPropTypeDef DrawProp;
static LCD_DrvTypeDef  *lcd_drv;

static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3);
static uint16_t KS_code_conversion(uint16_t KSSM);

uint8_t LCD_Init(void)
{
	uint8_t ret = LCD_ERROR;

	/* Default value for draw propriety */
	DrawProp.BackColor = LCD_COLOR_WHITE;
	DrawProp.TextColor = LCD_COLOR_BLACK;
	DrawProp.pFont     = &Efont;

	LCD_GPIO_Init();
	LCD_Reset();

	lcd_drv = &st75256_drv;

	lcd_drv->Init();

	ret = LCD_OK;

	LCD_Backlight_OnOff(LCD_BACKLIGHT_ON);

	return ret;
}

void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	LCD_RESET_CLK_ENABLE();

	GPIO_InitStruct.Pin = LCD_RESET_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(LCD_RESET_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(LCD_RESET_PORT, LCD_RESET_PIN, GPIO_PIN_RESET);

	LCD_Delay(15);
}

void LCD_Reset(void)
{
	HAL_GPIO_WritePin(LCD_RESET_PORT, LCD_RESET_PIN, GPIO_PIN_SET);
	LCD_Delay(150);
}

void LCD_Backlight_OnOff(int flag)
{
	if(flag == LCD_BACKLIGHT_ON)
	{
		PMIC_Write(0x20, 0x00);

	}
	else
	{
		PMIC_Write(0x20, 0x02);

	}
}

uint16_t LCD_GetXSize(void)
{
	return (lcd_drv->GetLcdPixelWidth());
}

uint16_t LCD_GetYSize(void)
{
	return (lcd_drv->GetLcdPixelHeight());
}

uint32_t LCD_GetTextColor(void)
{
	return DrawProp.TextColor;
}

uint32_t LCD_GetBackColor(void)
{
	return DrawProp.BackColor;
}

void LCD_SetTextColor(uint32_t color)
{
	DrawProp.TextColor = color;
}

void LCD_SetBackColor(uint32_t color)
{
	DrawProp.BackColor = color;
}

void LCD_SetFont(sFONT *pFonts)
{
	DrawProp.pFont = pFonts;
}

sFONT *LCD_GetFont(void)
{
	return DrawProp.pFont;
}

void LCD_Update(void *p)
{
	if(lcd_drv->Update != NULL)
	{
		lcd_drv->Update(p);
	}
}

void LCD_DisplayOn(void)
{
	if(lcd_drv->DisplayOn != NULL)
	{
		lcd_drv->DisplayOn();
	}
}

void LCD_DisplayOff(void)
{
	if(lcd_drv->DisplayOff != NULL)
	{
		lcd_drv->DisplayOff();
	}
}

void LCD_DrawPixel(uint16_t xpos, uint16_t ypos, uint32_t rgb_code)
{
	if(lcd_drv->DrawPixel != NULL)
	{
		lcd_drv->DrawPixel(xpos, ypos, rgb_code);
	}
}

void LCD_DrawHLine(uint16_t xpos, uint16_t ypos, uint16_t length)
{
	uint16_t index = 0;

	if(lcd_drv->DrawHLine != NULL)
	{
		lcd_drv->DrawHLine(xpos, ypos, length, DrawProp.TextColor);
	}
	else
	{
		for(index=0; index<length; index++)
		{
			LCD_DrawPixel((xpos + index), ypos, DrawProp.TextColor);
		}
	}
}

void LCD_DrawVLine(uint16_t xpos, uint16_t ypos, uint16_t length)
{
	uint16_t index = 0;

	if(lcd_drv->DrawVLine != NULL)
	{
		lcd_drv->DrawVLine(xpos, ypos, length, DrawProp.TextColor);
	}
	else
	{
		for(index=0; index<length; index++)
		{
			LCD_DrawPixel(xpos, (ypos + index), DrawProp.TextColor);
		}
	}
}

void LCD_Clear(uint32_t color)
{
	uint16_t counter = 0;
	uint32_t color_backup = DrawProp.TextColor;

	DrawProp.TextColor = color;

	for(counter=0; counter<LCD_GetYSize(); counter++)
	{
		LCD_DrawHLine(0, counter, LCD_GetXSize());
	}

	DrawProp.TextColor = color_backup;
	LCD_SetTextColor(DrawProp.TextColor);
}

void LCD_ClearStringLine(uint16_t line)
{
	uint32_t color_backup = DrawProp.TextColor;
	DrawProp.TextColor = DrawProp.BackColor;;

	/* Draw a rectangle with background color */
	LCD_FillRect(0, (line * DrawProp.pFont->Height), LCD_GetXSize(), DrawProp.pFont->Height);

	DrawProp.TextColor = color_backup;
	LCD_SetTextColor(DrawProp.TextColor);
}

void LCD_DisplayEChar(uint16_t xpos, uint16_t ypos, uint8_t ascii)
{
	uint32_t i = 0, j = 0;
	uint16_t height, width;
	uint8_t pixel;

	height = DrawProp.pFont->Height;
	width  = DrawProp.pFont->Width;

	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			pixel = Efont_table[ascii][i] & (0x01 << j);

			if(i < 7)
			{
				if(pixel)
				{
					LCD_DrawPixel(xpos, (ypos+j*2), DrawProp.TextColor);
					LCD_DrawPixel(xpos, (ypos+j*2+1), DrawProp.TextColor);
				}
				else
				{
					LCD_DrawPixel(xpos, (ypos+j*2), DrawProp.BackColor);
					LCD_DrawPixel(xpos, (ypos+j*2+1), DrawProp.BackColor);
				}
			}
			else
			{
				if(pixel)
				{
					LCD_DrawPixel(xpos-8, (ypos+(8+j)*2), DrawProp.TextColor);
					LCD_DrawPixel(xpos-8, (ypos+(8+j)*2+1), DrawProp.TextColor);
				}
				else
				{
					LCD_DrawPixel(xpos-8, (ypos+(8+j)*2), DrawProp.BackColor);
					LCD_DrawPixel(xpos-8, (ypos+(8+j)*2+1), DrawProp.BackColor);
				}
			}
		}

		xpos++;
	}

}

void LCD_DisplayKChar(uint16_t xpos, uint16_t ypos, uint16_t kcode)
{
	uint8_t i, j, pixel;
	uint8_t cho_5bit, joong_5bit, jong_5bit;
	uint8_t cho_bul, joong_bul, jong_bul=0, jong_flag;
	uint16_t character;
	uint8_t Korean_buffer[32];		// 32 byte Korean font buffer;

	// get cho, joong, jong 5 bit
	cho_5bit = table_cho[(kcode >> 10) & 0x001F];//pgm_read_byte(&table_cho[(Kcode >> 10) & 0x001F]);
	joong_5bit = table_joong[(kcode >> 5) & 0x001F];//pgm_read_byte(&table_joong[(Kcode >> 5) & 0x001F]);
	jong_5bit = table_jong[kcode & 0x001F];//pgm_read_byte(&table_jong[Kcode & 0x001F]);

	if(jong_5bit == 0)
	{
		jong_flag = 0;
		cho_bul = bul_cho1[joong_5bit];

		if((cho_5bit == 1) || (cho_5bit == 16)) joong_bul = 0;
		else									joong_bul = 1;
	}
	else
	{
		jong_flag = 1;
		cho_bul = bul_cho2[joong_5bit];

		if((cho_5bit == 1) || (cho_5bit == 16)) joong_bul = 2;
		else                                    joong_bul = 3;

		jong_bul = bul_jong[joong_5bit];
	}

	character = cho_bul*20 + cho_5bit;		// copy chosung font
	for(i = 0; i <= 31; i++)
		Korean_buffer[i] = K_font[character][i];

	character = 8*20 + joong_bul*22 + joong_5bit;
	for(i = 0; i <= 31; i++)
		Korean_buffer[i] |= K_font[character][i];

	if(jong_flag == 1)		// if jongsung, OR jongsung font
	{
		character = 8*20 + 4*22 +jong_bul*28 + jong_5bit;
		for(i = 0; i <= 31; i++)
			Korean_buffer[i] |= K_font[character][i];
	}

	for(i=0; i<8; i++)	// // display upper left row
	{
		for(j=0; j<8; j++)
		{
			pixel = Korean_buffer[i] & (0x01 << j);

			if(pixel)
			{
				LCD_DrawPixel((xpos+i), (ypos+j*2), DrawProp.TextColor);
				LCD_DrawPixel((xpos+i), (ypos+j*2+1), DrawProp.TextColor);
			}
			else
			{
				LCD_DrawPixel((xpos+i), (ypos+j*2), DrawProp.BackColor);
				LCD_DrawPixel((xpos+i), (ypos+j*2+1), DrawProp.BackColor);
			}
		}
	}

	for(i=16; i<24; i++)	// display lower left row
	{
		for(j=0; j<8; j++)
		{
			pixel = Korean_buffer[i] & (0x01 << j);

			if(pixel)
			{
				LCD_DrawPixel((xpos+i-16), (ypos+(j+8)*2), DrawProp.TextColor);
				LCD_DrawPixel((xpos+i-16), (ypos+(j+8)*2+1), DrawProp.TextColor);
			}
			else
			{
				LCD_DrawPixel((xpos+i-16), (ypos+(j+8)*2), DrawProp.BackColor);
				LCD_DrawPixel((xpos+i-16), (ypos+(j+8)*2+1), DrawProp.BackColor);
			}
		}
	}

	for(i=8; i<16; i++)	// display upper right row
	{
		for(j=0; j<8; j++)
		{
			pixel = Korean_buffer[i] & (0x01 << j);

			if(pixel)
			{
				LCD_DrawPixel((xpos+i), (ypos+j*2), DrawProp.TextColor);
				LCD_DrawPixel((xpos+i), (ypos+j*2+1), DrawProp.TextColor);
			}
			else
			{
				LCD_DrawPixel((xpos+i), (ypos+j*2), DrawProp.BackColor);
				LCD_DrawPixel((xpos+i), (ypos+j*2+1), DrawProp.BackColor);
			}
		}
	}

	for(i=24; i<32; i++)	// display lower right row
	{
		for(j=0; j<8; j++)
		{
			pixel = Korean_buffer[i] & (0x01 << j);

			if(pixel)
			{
				LCD_DrawPixel((xpos+i-16), (ypos+(j+8)*2), DrawProp.TextColor);
				LCD_DrawPixel((xpos+i-16), (ypos+(j+8)*2+1), DrawProp.TextColor);
			}
			else
			{
				LCD_DrawPixel((xpos+i-16), (ypos+(j+8)*2), DrawProp.BackColor);
				LCD_DrawPixel((xpos+i-16), (ypos+(j+8)*2+1), DrawProp.BackColor);
			}
		}
	}
}

void LCD_DisplayStringAt(uint16_t xpos, uint16_t ypos, uint8_t *text, Line_ModeTypdef mode)
{
	uint16_t refcolumn = 1, i = 0;
	uint16_t size = 0, xsize = 0;
	uint8_t  character1;
	uint16_t character2;
	uint8_t  *ptr = text;

	/* Get the text size */
	while (*ptr++) size ++ ;

	/* Characters number per line */
	xsize = (LCD_GetXSize()/DrawProp.pFont->Width);

	switch(mode)
	{
	case CENTER_MODE:
		{
			refcolumn = xpos + ((xsize - size)* DrawProp.pFont->Width) / 2;
			break;
		}
	case LEFT_MODE:
		{
			refcolumn = xpos;
			break;
		}
	case RIGHT_MODE:
		{
			refcolumn =  -xpos + ((xsize - size)*DrawProp.pFont->Width);
			break;
		}
	default:
		{
			refcolumn = xpos;
			break;
		}
	}

	/* Send the string character by character on lCD */
	while((*text != 0) & (((LCD_GetXSize() - (i*DrawProp.pFont->Width)) & 0xFFFF) >= DrawProp.pFont->Width))
	{
		character1 = *text;

		/* Point on the next character */
		text++;
		i++;

		if(character1 < 0x80)
		{
			/* Display one character on LCD */
			LCD_DisplayEChar(refcolumn, ypos, character1);
			refcolumn += DrawProp.pFont->Width;
		}
		else
		{
			character2 = (256 * character1) + (*text & 0xFF);
			character2 = KS_code_conversion(character2);

			/* Display one character on LCD */
			LCD_DisplayKChar(refcolumn, ypos, character2);
			refcolumn += (DrawProp.pFont->Width * 2);
			text++;
			i++;
		}
	}
}



void LCD_DisplayStringAt_H(uint16_t xpos, uint16_t ypos, uint8_t *text, Line_ModeTypdef mode)
{
	uint16_t refcolumn = 1, i = 0;
	uint16_t size = 0, xsize = 0;
	uint8_t  character1;
	uint16_t character2;
	uint8_t  *ptr = text;

	/* Get the text size */
	while (*ptr++) size ++ ;

	/* Characters number per line */
	xsize = (LCD_GetXSize()/DrawProp.pFont->Width);

	switch(mode)
	{
	case CENTER_MODE:
		{
			refcolumn = xpos + ((xsize - size)* DrawProp.pFont->Width) / 2;
			break;
		}
	case LEFT_MODE:
		{
			refcolumn = xpos;
			break;
		}
	case RIGHT_MODE:
		{
			refcolumn =  -xpos + ((xsize - size)*DrawProp.pFont->Width);
			break;
		}
	default:
		{
			refcolumn = xpos;
			break;
		}
	}

	/* Send the string character by character on lCD */
	while((*text != 0) & (((LCD_GetXSize() - (i*DrawProp.pFont->Width)) & 0xFFFF) >= DrawProp.pFont->Width))
	{
		character1 = *text;

		/* Point on the next character */
		text++;
		i++;

		if(character1 < 0x80)
		{
			/* Display one character on LCD */
			LCD_DisplayEChar(refcolumn, ypos, character1);
			refcolumn += DrawProp.pFont->Width;
		}
		else
		{
			character2 = (256 * character1) + (*text & 0xFF);
			character2 = KS_code_conversion(character2);

			/* Display one character on LCD */
			LCD_DisplayKChar(refcolumn, ypos, character2);
			refcolumn += (DrawProp.pFont->Width * 2);
			text++;
			i++;
		}
	}
}

void LCD_DisplayStringAtLine(uint16_t line, uint8_t *ptr)
{
	LCD_DisplayStringAt(0, LINE(line), ptr, LEFT_MODE);
}

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = ABS(x2 - x1);        /* The difference between the x's */
	deltay = ABS(y2 - y1);        /* The difference between the y's */
	x = x1;                       /* Start x off at the first pixel */
	y = y1;                       /* Start y off at the first pixel */

	if (x2 >= x1)                 /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else                          /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1)                 /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else                          /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)         /* There is at least one x-value for every y-value */
	{
		xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
		yinc2 = 0;                  /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;         /* There are more x-values than y-values */
	}
	else                          /* There is at least one y-value for every x-value */
	{
		xinc2 = 0;                  /* Don't change the x for every iteration */
		yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;         /* There are more y-values than x-values */
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		LCD_DrawPixel(LCD_GetXSize()-x, LCD_GetYSize()-y, DrawProp.TextColor);  /* Draw the current pixel */
		num += numadd;                            /* Increase the numerator by the top of the fraction */
		if (num >= den)                           /* Check if numerator >= denominator */
		{
			num -= den;                             /* Calculate the new numerator value */
			x += xinc1;                             /* Change the x as appropriate */
			y += yinc1;                             /* Change the y as appropriate */
		}

		x += xinc2;                               /* Change the x as appropriate */
		y += yinc2;                               /* Change the y as appropriate */
	}
}

void LCD_DrawRect(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height)
{
	/* Draw horizontal lines */
	LCD_DrawHLine(xpos, ypos, width);
	LCD_DrawHLine(xpos, (ypos+ height), width);

	/* Draw vertical lines */
	LCD_DrawVLine(xpos, ypos, height);
	LCD_DrawVLine((xpos + width), ypos, height);
}

void LCD_DrawCircle(uint16_t xpos, uint16_t ypos, uint16_t radius)
{
	int32_t  D;       /* Decision Variable */
	uint32_t  CurX;   /* Current X Value */
	uint32_t  CurY;   /* Current Y Value */

	D = 3 - (radius << 1);
	CurX = 0;
	CurY = radius;

	while (CurX <= CurY)
	{
		LCD_DrawPixel((xpos + CurX), (ypos - CurY), DrawProp.TextColor);

		LCD_DrawPixel((xpos - CurX), (ypos - CurY), DrawProp.TextColor);

		LCD_DrawPixel((xpos + CurY), (ypos - CurX), DrawProp.TextColor);

		LCD_DrawPixel((xpos - CurY), (ypos - CurX), DrawProp.TextColor);

		LCD_DrawPixel((xpos + CurX), (ypos + CurY), DrawProp.TextColor);

		LCD_DrawPixel((xpos - CurX), (ypos + CurY), DrawProp.TextColor);

		LCD_DrawPixel((xpos + CurY), (ypos + CurX), DrawProp.TextColor);

		LCD_DrawPixel((xpos - CurY), (ypos + CurX), DrawProp.TextColor);

		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}

		CurX++;
	}
}

void LCD_DrawPolygon(pPoint points, uint16_t pointCount)
{
	int16_t x = 0, y = 0;

	if(pointCount < 2)
	{
		return;
	}

	LCD_DrawLine(points->X, points->Y, (points+pointCount-1)->X, (points+pointCount-1)->Y);

	while(--pointCount)
	{
		x = points->X;
		y = points->Y;
		points++;
		LCD_DrawLine(x, y, points->X, points->Y);
	}
}

void LCD_DrawEllipse(int xpos, int ypos, int xRadius, int yRadius)
{
	int x = 0, y = -yRadius, err = 2-2*xRadius, e2;
	float K = 0, rad1 = 0, rad2 = 0;

	rad1 = xRadius;
	rad2 = yRadius;

	K = (float)(rad2/rad1);

	do
	{
		LCD_DrawPixel((xpos-(uint16_t)(x/K)), (ypos+y), DrawProp.TextColor);
		LCD_DrawPixel((xpos+(uint16_t)(x/K)), (ypos+y), DrawProp.TextColor);
		LCD_DrawPixel((xpos+(uint16_t)(x/K)), (ypos-y), DrawProp.TextColor);
		LCD_DrawPixel((xpos-(uint16_t)(x/K)), (ypos-y), DrawProp.TextColor);

		e2 = err;
		if (e2 <= x)
		{
			err += ++x*2+1;
			if (-y == x && e2 <= y) e2 = 0;
		}
		if (e2 > y) err += ++y*2+1;
	} while (y <= 0);
}

void LCD_DrawArrorLeft(uint16_t xpos, uint16_t ypos)
{
	LCD_DrawLine(xpos, ypos+4, xpos+10, ypos+4);
	LCD_DrawLine(xpos, ypos+5, xpos+10, ypos+5);
	LCD_DrawLine(xpos, ypos+6, xpos+10, ypos+6);

	LCD_DrawLine(xpos,ypos+5, xpos+4, ypos);
	LCD_DrawLine(xpos, ypos+5, xpos+4, ypos+9);
	LCD_DrawLine(xpos+1,ypos+5, xpos+4, ypos+1);
	LCD_DrawLine(xpos+1, ypos+5, xpos+4, ypos+8);
}

void LCD_DrawArrorRight(uint16_t xpos, uint16_t ypos)
{
	LCD_DrawLine(xpos, ypos+4, xpos+10, ypos+4);
	LCD_DrawLine(xpos, ypos+5, xpos+10, ypos+5);
	LCD_DrawLine(xpos, ypos+6, xpos+10, ypos+6);

	LCD_DrawLine(xpos+10,ypos+5, xpos+6, ypos);
	LCD_DrawLine(xpos+10, ypos+5, xpos+6, ypos+9);
	LCD_DrawLine(xpos+9,ypos+5, xpos+6, ypos+1);
	LCD_DrawLine(xpos+9, ypos+5, xpos+6, ypos+8);
}
void LCD_DrawBitmap_Mono(uint16_t xpos, uint16_t ypos, uint8_t *pbmp)
{
	uint8_t c, mask;
	uint16_t i, j, k;
	uint16_t type;
	uint32_t index, width, height;

	type = *(volatile uint16_t *)(pbmp);

	if(type != 0x4D42)	/* bitmap file type */
	{
		return;
	}

	index = *(volatile uint16_t *)(pbmp + 10);
	index |= (*(volatile uint16_t *)(pbmp + 12)) << 16;

	width = *(volatile uint16_t *)(pbmp + 18);
	width |= (*(volatile uint16_t *)(pbmp + 20)) << 16;

	height = *(volatile uint16_t *)(pbmp + 22);
	height |= (*(volatile uint16_t *)(pbmp + 24)) << 16;

	k = 0;
// 왜 위아래가 반대로 나옴?
	for(j=0; j<(uint16_t)height; j++)
	{
		k = 0;
		//if((ypos+height-j)>LCD_GetYSize()) continue;
		mask = 0x80;

		for(i=0; i<(uint16_t)width; i++)
		{
			//if((xpos+i>LCD_GetXSize())) continue;
			c = (pbmp[index] & mask) > 0 ? 1:0;

			if(c == 1)
				LCD_DrawPixel(LCD_GetXSize()-(xpos+i), LCD_GetYSize()-(ypos+(height-j-1)), COLOR_RGB(255,255,255));
				//LCD_DrawPixel(xpos+i, ypos+(height-j), COLOR_RGB(255,255,255));
			else
				LCD_DrawPixel(LCD_GetXSize()-(xpos+i), LCD_GetYSize()-(ypos+(height-j-1)), COLOR_RGB(0,0,0));
			//LCD_DrawPixel(xpos+i, ypos+(height-j), COLOR_RGB(0,0,0));

			mask = mask >> 1;

			if(mask == 0x00)
			{
				index++;
				k++;
				mask = 0x80;
//
//				if((k%4) == 3)
//				{
//					k++;
//					index++;
//				}
			}

		}
		if(k%4)
		{
			index += (4-(k%4));
			k=0;
		}
	}

}

void LCD_FillRect(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height)
{
	LCD_SetTextColor(DrawProp.TextColor);
	do
	{
		LCD_DrawHLine(xpos, ypos++, width);
	} while(height--);
}

void LCD_FillCircle(uint16_t xpos, uint16_t ypos, uint16_t radius)
{
	int32_t  D;        /* Decision Variable */
	uint32_t  CurX;    /* Current X Value */
	uint32_t  CurY;    /* Current Y Value */

	D = 3 - (radius << 1);

	CurX = 0;
	CurY = radius;

	LCD_SetTextColor(DrawProp.TextColor);

	while (CurX <= CurY)
	{
		if(CurY > 0)
		{
			LCD_DrawHLine(xpos - CurY, ypos + CurX, 2*CurY);
			LCD_DrawHLine(xpos - CurY, ypos - CurX, 2*CurY);
		}

		if(CurX > 0)
		{
			LCD_DrawHLine(xpos - CurX, ypos - CurY, 2*CurX);
			LCD_DrawHLine(xpos - CurX, ypos + CurY, 2*CurX);
		}
		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}

		CurX++;
	}

	LCD_SetTextColor(DrawProp.TextColor);
	LCD_DrawCircle(xpos, ypos, radius);
}

void LCD_FillPolygon(pPoint points, uint16_t pointCount)
{
	int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
	uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

	IMAGE_LEFT = IMAGE_RIGHT = points->X;
	IMAGE_TOP= IMAGE_BOTTOM = points->Y;

	for(counter = 1; counter < pointCount; counter++)
	{
		pixelX = POLY_X(counter);
		if(pixelX < IMAGE_LEFT)
		{
			IMAGE_LEFT = pixelX;
		}
		if(pixelX > IMAGE_RIGHT)
		{
			IMAGE_RIGHT = pixelX;
		}

		pixelY = POLY_Y(counter);
		if(pixelY < IMAGE_TOP)
		{
			IMAGE_TOP = pixelY;
		}
		if(pixelY > IMAGE_BOTTOM)
		{
			IMAGE_BOTTOM = pixelY;
		}
	}

	if(pointCount < 2)
	{
		return;
	}

	X_center = (IMAGE_LEFT + IMAGE_RIGHT)/2;
	Y_center = (IMAGE_BOTTOM + IMAGE_TOP)/2;

	X_first = points->X;
	Y_first = points->Y;

	while(--pointCount)
	{
		X = points->X;
		Y = points->Y;
		points++;
		X2 = points->X;
		Y2 = points->Y;

		FillTriangle(X, X2, X_center, Y, Y2, Y_center);
		FillTriangle(X, X_center, X2, Y, Y_center, Y2);
		FillTriangle(X_center, X2, X, Y_center, Y2, Y);
	}

	FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
	FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
	FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);
}

void LCD_FillEllipse(int xpos, int ypos, int xRadius, int yRadius)
{
	int x = 0, y = -yRadius, err = 2-2*xRadius, e2;
	float K = 0, rad1 = 0, rad2 = 0;

	rad1 = xRadius;
	rad2 = yRadius;

	K = (float)(rad2/rad1);

	do
	{
		LCD_DrawHLine((xpos-(uint16_t)(x/K)), (ypos+y), (2*(uint16_t)(x/K) + 1));
		LCD_DrawHLine((xpos-(uint16_t)(x/K)), (ypos-y), (2*(uint16_t)(x/K) + 1));

		e2 = err;
		if (e2 <= x)
		{
			err += ++x*2+1;
			if (-y == x && e2 <= y) e2 = 0;
		}

		if (e2 > y) err += ++y*2+1;
	} while (y <= 0);
}

/*******************************************************************************
                            Static Functions
*******************************************************************************/
static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = ABS(x2 - x1);        /* The difference between the x's */
	deltay = ABS(y2 - y1);        /* The difference between the y's */
	x = x1;                       /* Start x off at the first pixel */
	y = y1;                       /* Start y off at the first pixel */

	if (x2 >= x1)                 /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else                          /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1)                 /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else                          /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)         /* There is at least one x-value for every y-value */
	{
		xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
		yinc2 = 0;                  /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;         /* There are more x-values than y-values */
	}
	else                          /* There is at least one y-value for every x-value */
	{
		xinc2 = 0;                  /* Don't change the x for every iteration */
		yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;         /* There are more y-values than x-values */
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		LCD_DrawLine(x, y, x3, y3);

		num += numadd;              /* Increase the numerator by the top of the fraction */
		if (num >= den)             /* Check if numerator >= denominator */
		{
			num -= den;               /* Calculate the new numerator value */
			x += xinc1;               /* Change the x as appropriate */
			y += yinc1;               /* Change the y as appropriate */
		}

		x += xinc2;                 /* Change the x as appropriate */
		y += yinc2;                 /* Change the y as appropriate */
	}
}

static uint16_t KS_code_conversion(uint16_t KSSM)
{
	uint8_t HB, LB;
	uint16_t index, KS;

	HB = KSSM >> 8;
	LB = KSSM & 0x00FF;

	if(KSSM >= 0xB0A1 && KSSM <= 0xC8FE)
	{
		index = (HB - 0xB0)*94 + LB - 0xA1;
		KS = KS_Table[index][0] * 256;
		KS |= KS_Table[index][1];

		return KS;
	}
	else
	{
		return 1;
	}
}
