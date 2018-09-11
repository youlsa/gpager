/*
 * hw_spi.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef HW_SPI_H_
#define HW_SPI_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* LoRa */
void     HW_LORA_SPI_Init(void);
void     HW_LORA_SPI_DeInit(void);
void     HW_LORA_SPI_IoInit(void);
void     HW_LORA_SPI_IoDeInit(void);
uint16_t HW_LORA_SPI_InOut(uint16_t outData);

/* LCD */
void LCD_IO_Init(void);
void LCD_IO_WriteReg(uint8_t LCDReg);
void LCD_IO_WriteData(uint8_t Data);
void LCD_Delay(uint32_t Delay);

#ifdef __cplusplus
}
#endif
#endif /* HW_SPI_H_ */
