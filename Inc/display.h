/*
 * display.h
 *
 *  Created on: 2017. 9. 27.
 *      Author: baekjg
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include "ts.h"
uint8_t g_gprmc[];
typedef enum
{
	DISPLAY_TOUCH_STATE,
	DISPLAY_GPS_STATE,
	DISPLAY_BATTERY_STATE,
	DISPLAY_LORA_STATE,
	DISPLAY_END_STATE,
} display_state_t;

void LCD_Main_Display(uint8_t display);
void LCD_TS_Display(ts_data_t * data);
void LCD_GPGGA_Display(uint8_t *data);
void LCD_GPRMC_Display(uint8_t *data);

void LCD_BAT_LEVLE_Display(void);
void LCD_CHARGING_Display(void);
void LCD_LoRa_Display(void);

#ifdef __cplusplus
}
#endif
#endif /* DISPLAY_H_ */
