/*
 * lcd.h
 *
 *  Created on: 2017. 9. 27.
 *      Author: baekjg
 */

#ifndef LCD_H_
#define LCD_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @defgroup LCD_Driver_structure  LCD Driver structure
* @{
*/
typedef struct
{
	void	 (*Init)(void);
	void	 (*DrawPixel)(uint16_t, uint16_t, uint32_t);
	void	 (*DrawHLine)(uint16_t, uint16_t, uint16_t, uint32_t);
	void	 (*DrawVLine)(uint16_t, uint16_t, uint16_t, uint32_t);
	uint16_t (*GetLcdPixelWidth)(void);
	uint16_t (*GetLcdPixelHeight)(void);
	void     (*DisplayOn)(void);
	void     (*DisplayOff)(void);
	void	 (*Update)(void*);
}LCD_DrvTypeDef;

#ifdef __cplusplus
}
#endif
#endif /* LCD_H_ */
