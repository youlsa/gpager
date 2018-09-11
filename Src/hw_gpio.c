/*
 * hw_gpio.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

/* Private variables ---------------------------------------------------------*/
//static GpioIrqHandler *GpioIrq[16] = { NULL };

/* Private function prototypes -----------------------------------------------*/

static uint8_t HW_GPIO_GetBitPos(uint16_t GPIO_Pin);

/* Exported functions ---------------------------------------------------------*/
/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HW_GPIO_IrqHandler(GPIO_Pin);
}

/**
  * @brief  Gets IRQ number as a finction of the GPIO_Pin.
  * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
  * @retval IRQ number
  */
IRQn_Type MSP_GetIRQn(uint16_t GPIO_Pin)
{
	switch( GPIO_Pin )
	{
	case GPIO_PIN_0:  return EXTI0_IRQn;
	case GPIO_PIN_1:  return EXTI1_IRQn;
	case GPIO_PIN_2:  return EXTI2_IRQn;
	case GPIO_PIN_3:  return EXTI3_IRQn;
	case GPIO_PIN_4:  return EXTI4_IRQn;
	case GPIO_PIN_5:
	case GPIO_PIN_6:
	case GPIO_PIN_7:
	case GPIO_PIN_8:
	case GPIO_PIN_9:  return EXTI9_5_IRQn;
	case GPIO_PIN_10:
	case GPIO_PIN_11:
	case GPIO_PIN_12:
	case GPIO_PIN_13:
	case GPIO_PIN_14:
	case GPIO_PIN_15:
	default: return EXTI15_10_IRQn;
	}
}

/*!
 * @brief Initializes the given GPIO object
 *
 * @param  GPIOx: where x can be (A..E and H)
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @param [IN] initStruct  GPIO_InitTypeDef intit structure
 * @retval none
 */
void HW_GPIO_Init(GPIO_TypeDef* port, uint16_t GPIO_Pin, GPIO_InitTypeDef* initStruct)
{

  RCC_GPIO_CLK_ENABLE((uint32_t) port);

  initStruct->Pin = GPIO_Pin ;

  HAL_GPIO_Init(port, initStruct);
}

/*!
 * @brief Records the interrupt handler for the GPIO  object
 *
 * @param  GPIOx: where x can be (A..E and H)
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @param [IN] prio       NVIC priority (0 is highest)
 * @retval none
 */
void HW_GPIO_SetIrq(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t prio)
{
	IRQn_Type IRQnb;

	IRQnb = MSP_GetIRQn(GPIO_Pin);

	HAL_NVIC_SetPriority(IRQnb , prio, 0);

	HAL_NVIC_EnableIRQ(IRQnb);
}

void HW_GPIO_DisableIrq(uint16_t GPIO_Pin)
{
	IRQn_Type IRQnb;

	IRQnb = MSP_GetIRQn(GPIO_Pin);

	HAL_NVIC_DisableIRQ(IRQnb);
}

/*!
 * @brief Execute the interrupt from the object
 *
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @retval none
 */
void HW_GPIO_IrqHandler(uint16_t GPIO_Pin)
{
	uint32_t BitPos = HW_GPIO_GetBitPos( GPIO_Pin );

//	DPRINTF("[HW_GPIO_IrqHandler] BitPos : %d\r\n", BitPos);

	if(BitPos <= 2) event_put(EVENT_LORA_INTERRUPT, BitPos, NULL);
	else if(BitPos == 13) event_put(EVENT_TOUCH_INTERRUPT, 0, NULL);
	else if(BitPos == 7) event_put(EVENT_WAKEUP_BUTTON_INTERRUPT, 0, NULL);
	else if(BitPos == 10) event_put(EVENT_BAT_CHARGING, 0, NULL);
}

/*!
 * @brief Writes the given value to the GPIO output
 *
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @param [IN] value New GPIO output value
 * @retval none
 */
void HW_GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t value)
{
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, (GPIO_PinState)value);
}

/*!
 * @brief Reads the current GPIO input value
 *
 * @param  GPIOx: where x can be (A..E and H)
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @retval value   Current GPIO input value
 */
uint32_t HW_GPIO_Read(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}

/* Private functions ---------------------------------------------------------*/

/*!
 * @brief Get the position of the bit set in the GPIO_Pin
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @retval the position of the bit
 */
static uint8_t HW_GPIO_GetBitPos(uint16_t GPIO_Pin)
{
	uint8_t PinPos=0;

	if((GPIO_Pin & 0xFF00) != 0) { PinPos |= 0x8; }
	if((GPIO_Pin & 0xF0F0) != 0) { PinPos |= 0x4; }
	if((GPIO_Pin & 0xCCCC) != 0) { PinPos |= 0x2; }
	if((GPIO_Pin & 0xAAAA) != 0) { PinPos |= 0x1; }

	return PinPos;
}
