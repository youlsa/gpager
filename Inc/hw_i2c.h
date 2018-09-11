/*
 * hw_i2c.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef HW_I2C_H_
#define HW_I2C_H_
#ifdef __cplusplus
 extern "C" {
#endif

void HW_I2C_Init(void);

/* PMIC */
void    HW_PMIC_Init(void);
uint8_t PMIC_Read(uint8_t addr);
void    PMIC_Write(uint8_t addr, uint8_t data);

/* EXTERNAL EEPROM */
void EEPROM_Write(uint8_t addr, uint8_t *data, uint16_t size);
void EEPROM_Read(uint8_t addr, uint8_t *data, uint16_t size);

/* TOUCH SCREEN */
void TS_Read(uint8_t reg, uint8_t *buf, uint16_t size);

#ifdef __cplusplus
}
#endif
#endif /* HW_I2C_H_ */
