/*
 * config.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#ifdef __cplusplus
 extern "C" {
#endif

#define CONFIG_MARKER    0x9a3471d2

#define EEPROM_CONFIG_ADDR        0

void CONFIG_Init(void);
void CONFIG_Write(void);
void CONFIG_Read(void);

#define CONFIG_WRITE(ITEM, SIZE)  CONFIG_Write()

#ifdef __cplusplus
}
#endif
#endif /* CONFIG_H_ */
