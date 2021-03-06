/*
 * hw_rtc.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef HW_RTC_H_
#define HW_RTC_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "utilities.h"

/*!
* @brief Initializes the RTC timer
* @note The timer is based on the RTC
* @param none
* @retval none
*/
void HW_RTC_Init(void);

/*!
* @brief Stop the Alarm
* @param none
* @retval none
*/
void HW_RTC_StopAlarm(void);

/*!
* @brief Return the minimum timeout the RTC is able to handle
* @param none
* @retval minimum value for a timeout
*/
uint32_t HW_RTC_GetMinimumTimeout(void);

/*!
* @brief Set the alarm
* @note The alarm is set at Reference + timeout
* @param timeout Duration of the Timer in ticks
*/
void HW_RTC_SetAlarm(uint32_t timeout);

/*!
* @brief Get the RTC timer elapsed time since the last Reference was set
* @retval RTC Elapsed time in ticks
*/
uint32_t HW_RTC_GetTimerElapsedTime(void);

/*!
* @brief Get the RTC timer value
* @retval none
*/
uint32_t HW_RTC_GetTimerValue(void);

/*!
* @brief Set the RTC timer Reference
* @retval  Timer Reference Value in  Ticks
*/
uint32_t HW_RTC_SetTimerContext(void);

/*!
* @brief Get the RTC timer Reference
* @retval Timer Value in  Ticks
*/
uint32_t HW_RTC_GetTimerContext(void);
/*!
* @brief RTC IRQ Handler on the RTC Alarm
* @param none
* @retval none
*/
void HW_RTC_IrqHandler(void);

/*!
* @brief a delay of delay ms by polling RTC
* @param delay in ms
* @param none
* @retval none
*/
void HW_RTC_DelayMs(uint32_t delay);

/*!
* @brief calculates the wake up time between wake up and mcu start
* @note resolution in RTC_ALARM_TIME_BASE
* @param none
* @retval none
*/
void HW_RTC_setMcuWakeUpTime(void);

/*!
* @brief returns the wake up time in us
* @param none
* @retval wake up time in ticks
*/
int16_t HW_RTC_getMcuWakeUpTime(void);

/*!
* @brief converts time in ms to time in ticks
* @param [IN] time in milliseconds
* @retval returns time in timer ticks
*/
uint32_t HW_RTC_ms2Tick(TimerTime_t timeMicroSec);

/*!
* @brief converts time in ticks to time in ms
* @param [IN] time in timer ticks
* @retval returns time in timer milliseconds
*/
TimerTime_t HW_RTC_Tick2ms(uint32_t tick);

#ifdef __cplusplus
}
#endif
#endif /* HW_RTC_H_ */
