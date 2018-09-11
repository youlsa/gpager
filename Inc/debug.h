/*
 * debug.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef DEBUG_H_
#define DEBUG_H_
#ifdef __cplusplus
 extern "C" {
#endif

void     dbg_puts(char *msg, uint16_t len);
uint32_t dbg_printf(char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif /* DEBUG_H_ */
