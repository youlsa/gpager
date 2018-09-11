/*
 * delay.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef DELAY_H_
#define DELAY_H_
#ifdef __cplusplus
 extern "C" {
#endif

/*!
* Blocking delay of "s" seconds
*/
void Delay(float s);

/*!
* Blocking delay of "ms" milliseconds
*/
void DelayMs(uint32_t ms);

#ifdef __cplusplus
}
#endif
#endif /* DELAY_H_ */
