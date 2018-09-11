/*
 * console.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_
#ifdef __cplusplus
 extern "C" {
#endif

void     uputs(char *msg, uint16_t len);
int      upeek(unsigned char ucChar);
int      ugets(char *pcBuf, uint32_t ui32Len);
void     uread(char *msg, uint16_t len);
uint32_t uprintf(char *fmt, ...);
void     uoutput(uint8_t * buffer, int length, int indent);

/* Queue */
#define UART_QUEUE_NUM		20
#define UART_QUEUE_BUFFER	256

void    serial_init(void);
uint8_t serial_put(uint8_t *data, uint16_t *size);
uint8_t serial_get(uint8_t *data, uint16_t *size);

#ifdef __cplusplus
}
#endif
#endif /* CONSOLE_H_ */
