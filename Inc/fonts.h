/*
 * fonts.h
 *
 *  Created on: 2017. 9. 27.
 *      Author: baekjg
 */

#ifndef FONTS_H_
#define FONTS_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup Utilities
* @{
*/

/** @addtogroup STM32_EVAL
* @{
*/

/** @addtogroup Common
* @{
*/

/** @addtogroup FONTS
* @{
*/

/** @defgroup FONTS_Exported_Types
* @{
*/
typedef struct _tFont
{
	uint16_t Width;
	uint16_t Height;
} sFONT;

extern sFONT Efont;
extern const uint8_t Efont_table[128][16];

extern const uint8_t table_cho[21];
extern const uint8_t table_joong[30];
extern const uint8_t table_jong[30];
extern const uint8_t bul_cho1[22];
extern const uint8_t bul_cho2[22];
extern const uint8_t bul_jong[22];
extern const uint8_t K_font[360][32];
extern const uint8_t KS_Table[2350][2];
/**
* @}
*/

/** @defgroup FONTS_Exported_Constants
* @{
*/
#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))

/**
* @}
*/

/** @defgroup FONTS_Exported_Macros
* @{
*/
/**
* @}
*/

/** @defgroup FONTS_Exported_Functions
* @{
*/
/**
* @}
*/

#ifdef __cplusplus
}
#endif
#endif /* FONTS_H_ */
