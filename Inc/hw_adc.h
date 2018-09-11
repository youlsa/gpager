/*
 * hw_adc.h
 *
 *  Created on: 2017. 11. 7.
 *      Author: baekjg
 */

#ifndef HW_ADC_H_
#define HW_ADC_H_
#ifdef __cplusplus
 extern "C" {
#endif

void HW_ADC_Init(void);
void HW_ADC_IrqHandler(void);

HAL_StatusTypeDef BatteryLevel_Check_Start_IT(void);
HAL_StatusTypeDef BatteryLevel_Check_Stop_IT(void);

#ifdef __cplusplus
}
#endif
#endif /* HW_ADC_H_ */
