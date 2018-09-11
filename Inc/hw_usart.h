/*
 * hw_usart.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef HW_USART_H_
#define HW_USART_H_
#ifdef __cplusplus
 extern "C" {
#endif

void HW_UART_Init(void);
void BLE_Init(void);
void GPS_Init(void);
void DEBUG_Init(void);

void HW_BT_IrqHandler(void);
void HW_GPS_IrqHandler(void);
void HW_DEBUG_IrqHandler(void);

void GPS_ON_Control(uint8_t on);

void gps_message_init(void);
bool gps_message_put(uint8_t *theItemValue);
bool gps_message_get(uint8_t *theItemValue, uint8_t theSize);

uint8_t ble_message_init(void);
uint8_t ble_message_put(uint8_t theItemValue);
uint8_t ble_message_get(uint8_t *theItemValue);
void    ble_put(char *msg, int len);

#ifdef __cplusplus
}
#endif
#endif /* HW_USART_H_ */
