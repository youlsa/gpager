/*
 * lcd_interface.h
 *
 *  Created on: 2017. 9. 27.
 *      Author: baekjg
 */

#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>

#include "fonts.h"

#define __IO    volatile

#define COLOR_RGB(r, g ,b)  ((unsigned long) (((unsigned char) (r) |  ((unsigned short) (g) << 8)) | (((unsigned long) (unsigned char) (b)) << 16)))

/**
* @brief  Draw Properties structures definition
*/
typedef struct
{
	uint32_t  TextColor;
	uint32_t  BackColor;
	sFONT    *pFont;    /*!< Specifies the font used for the text */
} LCD_DrawPropTypeDef;

/**
* @brief  Point structures definition
*/
typedef struct
{
	int16_t X;
	int16_t Y;
} Point, * pPoint;

/**
* @brief  Line mode structures definition
*/
typedef enum
{
	CENTER_MODE             = 0x01,    /*!< Center mode */
	RIGHT_MODE              = 0x02,    /*!< Right mode  */
	LEFT_MODE               = 0x03     /*!< Left mode   */
} Line_ModeTypdef;


/**
* @brief  LCD status structure definition
*/
#define LCD_OK         0x00
#define LCD_ERROR      0x01
#define LCD_TIMEOUT    0x02

/**
* @brief  LCD color (RGB 565)
*/
#define LCD_COLOR_WHITE			COLOR_RGB(255, 255, 255)
#define LCD_COLOR_BLACK			COLOR_RGB(0, 0, 0)

/**
* @brief LCD default font
*/
#define LCD_DEFAULT_FONT         Efont

typedef enum
{
	LCD_BACKLIGHT_OFF,
	LCD_BACKLIGHT_ON
} lcd_backlight_t;

uint8_t  LCD_Init(void);
void     LCD_GPIO_Init(void);
void     LCD_Reset(void);
void     LCD_Backlight_OnOff(int flag);

uint16_t LCD_GetXSize(void);
uint16_t LCD_GetYSize(void);
uint32_t LCD_GetTextColor(void);
uint32_t LCD_GetBackColor(void);
void     LCD_SetTextColor(uint32_t color);
void     LCD_SetBackColor(uint32_t color);
void     LCD_SetFont(sFONT *pFonts);
sFONT    *LCD_GetFont(void);

void	 LCD_Update(void *p);
void 	 LCD_DisplayOn(void);
void 	 LCD_DisplayOff(void);
void     LCD_DrawPixel(uint16_t xpos, uint16_t ypos, uint32_t rgb_code);
void     LCD_DrawHLine(uint16_t xpos, uint16_t ypos, uint16_t length);
void     LCD_DrawVLine(uint16_t xpos, uint16_t ypos, uint16_t length);

void     LCD_Clear(uint32_t color);
void     LCD_ClearStringLine(uint16_t line);
void     LCD_DisplayEChar(uint16_t xpos, uint16_t ypos, uint8_t ascii);
void     LCD_DisplayKChar(uint16_t xpos, uint16_t ypos, uint16_t kcode);
void     LCD_DisplayStringAt(uint16_t xpos, uint16_t ypos, uint8_t *text, Line_ModeTypdef mode);
void     LCD_DisplayStringAtLine(uint16_t line, uint8_t *ptr);

void     LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void     LCD_DrawRect(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height);
void     LCD_DrawCircle(uint16_t xpos, uint16_t ypos, uint16_t radius);
void     LCD_DrawPolygon(pPoint points, uint16_t pointCount);
void     LCD_DrawEllipse(int xpos, int ypos, int xRadius, int yRadius);
void 	 LCD_DrawBitmap_Mono(uint16_t xpos, uint16_t ypos, uint8_t *pbmp);

void     LCD_FillRect(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height);
void     LCD_FillCircle(uint16_t xpos, uint16_t ypos, uint16_t radius);
void     LCD_FillPolygon(pPoint points, uint16_t pointCount);
void     LCD_FillEllipse(int xpos, int ypos, int xRadius, int yRadius);
void LCD_DrawArrorLeft(uint16_t xpos, uint16_t ypos);
void LCD_DrawArrorRight(uint16_t xpos, uint16_t ypos);
#ifdef __cplusplus
}
#endif
#endif /* LCD_INTERFACE_H_ */
