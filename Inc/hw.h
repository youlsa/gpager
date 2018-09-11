/*
 * hw.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef HW_H_
#define HW_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_lora_msp.h"
#include "main.h"

#include "hw_usb.h"
#include "hw_i2c.h"
#include "hw_gpio.h"
#include "hw_rtc.h"
#include "hw_spi.h"
#include "hw_usart.h"
#include "hw_adc.h"
#include "board.h"

#include "event.h"
#include "console.h"
#include "config.h"
#include "ts.h"
#include "delay.h"
#include "cmdline.h"

#include "functions.h"

#ifdef __cplusplus
}
#endif
#endif /* HW_H_ */
