/*
 * hw_i2c.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

I2C_HandleTypeDef hi2c1;

void HW_I2C_Init(void)
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 400000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	if(HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		/* Initialization Error */
		 Error_Handler();
	}
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(i2cHandle->Instance == I2C1)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();

		/**I2C1 GPIO Configuration
		PB8     ------> I2C1_SCL
		PB9     ------> I2C1_SDA
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* Peripheral clock enable */
		__HAL_RCC_I2C1_CLK_ENABLE();
	}
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{
	if(i2cHandle->Instance == I2C1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_I2C1_CLK_DISABLE();

		/**I2C1 GPIO Configuration
		PB8     ------> I2C1_SCL
		PB9     ------> I2C1_SDA
		*/
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);
	}
}

//*****************************************************************************
// @ PMIC
//*****************************************************************************
void HW_PMIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	BAT_CHARGING_CLK_ENABLE();

	GPIO_InitStruct.Pin = BAT_CHARGING_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(BAT_CHARGING_PORT, &GPIO_InitStruct);

	HW_GPIO_SetIrq(BAT_CHARGING_PORT, BAT_CHARGING_PIN, 0);

	/* AS3701 ID */
	DEBUG("\r\n[ID]           0x90 = %02x\r\n", PMIC_Read(0x90));

	/* POWER ON */
	PMIC_Write(0x03, 0xcc); // LDO2, 0xcc:1.8V, 0xea:3.3V
	PMIC_Write(0x01, 0x7e); // SD1,  0x50:1.8V, 0x7e:3.3V

	/* Interrupt Mask */
	PMIC_Write(0x74, 0xe7);	// interrupt mask1 settings (CHDET, EOC)
	PMIC_Write(0x75, 0xff);	// interrupt mask2 settings

	/* BATTERY CHARING */
	PMIC_Write(0x80, 0x38);	// Normal battery charging, USB charger input, USB input : 470mA
	PMIC_Write(0x81, 0x51);	// End of charge voltage 4.16V
	PMIC_Write(0x82, 0x88); // Trickle Current 106mA, Constant Current 403mA

	/* Interrupt generation */
	PMIC_Write(0x85, 0x00);	// No NTC Input
	PMIC_Write(0x85, 0x04);	// NTC Input
	PMIC_Write(0x85, 0x00);	// No NTC Input

	/* Buzzer */
	PMIC_Write(0x09, 0x1E);		// PWM Output
	PMIC_Write(0x41, 0x00);
	PMIC_Write(0x42, 0x00);

	/* backlight */
	PMIC_Write(0x0a, 0x10);		// GPIO2 Output
	PMIC_Write(0x44, 0x02);

	if(PMIC_Read(0x87) & 0x04)	global.charging_status = CHARGING;
}

uint8_t PMIC_Read(uint8_t addr)
{
	uint8_t data;

	/*  Check the current state of the peripheral */
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

	if(HAL_I2C_Mem_Read(&hi2c1, 0x81, (uint16_t)addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000) != HAL_OK)
	{
		Error_Handler();
	}

	return data;
}

void PMIC_Write(uint8_t addr, uint8_t data)
{
	/*  Check the current state of the peripheral */
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

	if(HAL_I2C_Mem_Write(&hi2c1, 0x80, (uint16_t)addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000) != HAL_OK)
	{
		Error_Handler();
	}
}

//*****************************************************************************
// @ EEPROM
//*****************************************************************************
void EEPROM_Write(uint8_t addr, uint8_t *data, uint16_t size)
{
	uint8_t index = 0;

	for(uint16_t i=0; i<(size/8); i++)
	{
		while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

		if(HAL_I2C_Mem_Write(&hi2c1, 0xA0, (uint16_t)(addr+index), I2C_MEMADD_SIZE_8BIT, &data[index], 8, 1000) != HAL_OK)
		{
			Error_Handler();
		}

		index += 8;
		DelayMs(10);
	}

	if(size%8)
	{
		while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

		if(HAL_I2C_Mem_Write(&hi2c1, 0xA0, (uint16_t)(addr+index), I2C_MEMADD_SIZE_8BIT, &data[index], (size%8), 1000) != HAL_OK)
		{
			Error_Handler();
		}

		DelayMs(10);
	}
}

void EEPROM_Read(uint8_t addr, uint8_t *data, uint16_t size)
{
	uint8_t index = 0;

	for(uint16_t i=0; i<(size/8); i++)
	{
		while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

		if(HAL_I2C_Mem_Read(&hi2c1, 0xA1, (uint16_t)(addr+index), I2C_MEMADD_SIZE_8BIT, &data[index], 8, 1000) != HAL_OK)
		{
			Error_Handler();
		}

		index += 8;
		DelayMs(10);
	}

	if(size%8)
	{
		while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

		if(HAL_I2C_Mem_Read(&hi2c1, 0xA1, (uint16_t)(addr+index), I2C_MEMADD_SIZE_8BIT, &data[index], (size%8), 1000) != HAL_OK)
		{
			Error_Handler();
		}
		DelayMs(10);
	}
}

//*****************************************************************************
// @ TOUCH SCREEN
//*****************************************************************************
#define FT3267_WRITE_ADDR       0x70
#define FT3267_READ_ADDR		0x71

void TS_Read(uint8_t reg, uint8_t *buf, uint16_t size)
{
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)FT3267_WRITE_ADDR, &reg, 1, 10);

	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
	HAL_I2C_Master_Receive(&hi2c1, (uint16_t)FT3267_READ_ADDR, buf, size, 10);
}
