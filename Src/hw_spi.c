/*
 * hw_spi.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

//*****************************************************************************
// @LORA SPI
//*****************************************************************************
SPI_HandleTypeDef hspi1;

static uint32_t SpiFrequency(uint32_t hz);

void HW_LORA_SPI_Init(void)
{
	/*##-1- Configure the SPI peripheral */
	/* Set the SPI parameters */
	hspi1.Instance = SPI1;

//	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi1.Init.BaudRatePrescaler = SpiFrequency(10000000);
	hspi1.Init.Direction      = SPI_DIRECTION_2LINES;
	hspi1.Init.Mode           = SPI_MODE_MASTER;
	hspi1.Init.CLKPolarity    = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase       = SPI_PHASE_1EDGE;
	hspi1.Init.DataSize       = SPI_DATASIZE_8BIT;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial  = 7;
	hspi1.Init.FirstBit       = SPI_FIRSTBIT_MSB;
	hspi1.Init.NSS            = SPI_NSS_SOFT;
	hspi1.Init.TIMode         = SPI_TIMODE_DISABLE;

	SPI_CLK_ENABLE();

	if(HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		/* Initialization Error */
		 Error_Handler();
	}

	/*##-2- Configure the SPI GPIOs */
	HW_LORA_SPI_IoInit();
}

void HW_LORA_SPI_DeInit(void)
{
	/*##-1- Reset peripherals ####*/
	HAL_SPI_DeInit(&hspi1);

	/*##-2- Configure the SPI GPIOs */
	HW_LORA_SPI_IoDeInit();
}

void HW_LORA_SPI_IoInit(void)
{
	GPIO_InitTypeDef initStruct={0};

	initStruct.Mode = GPIO_MODE_AF_PP;
	initStruct.Pull = GPIO_NOPULL;
	initStruct.Speed = GPIO_SPEED_HIGH;
	initStruct.Alternate= SPI1_AF;
	HW_GPIO_Init(RADIO_MISO_PORT, RADIO_MISO_PIN, &initStruct);
	HW_GPIO_Init(RADIO_MOSI_PORT, RADIO_MOSI_PIN, &initStruct);
	HW_GPIO_Init(RADIO_SCLK_PORT, RADIO_SCLK_PIN, &initStruct);

	initStruct.Mode = GPIO_MODE_OUTPUT_PP;
	initStruct.Pull = GPIO_NOPULL;
	HW_GPIO_Init(RADIO_NSS_PORT, RADIO_NSS_PIN, &initStruct);
	HW_GPIO_Write(RADIO_NSS_PORT, RADIO_NSS_PIN, 1);
}

void HW_LORA_SPI_IoDeInit(void)
{
//	GPIO_InitTypeDef initStruct={0};
//
//	initStruct.Mode = GPIO_MODE_OUTPUT_PP;
//
//	initStruct.Pull = GPIO_NOPULL;
//	HW_GPIO_Init(RADIO_MOSI_PORT, RADIO_MOSI_PIN, &initStruct);
//	HW_GPIO_Write(RADIO_MOSI_PORT, RADIO_MOSI_PIN, 0);
//
//	initStruct.Pull = GPIO_NOPULL;
//	HW_GPIO_Init(RADIO_MISO_PORT, RADIO_MISO_PIN, &initStruct);
//	HW_GPIO_Write(RADIO_MISO_PORT, RADIO_MISO_PIN, 0);
//
//	initStruct.Pull = GPIO_NOPULL;
//	HW_GPIO_Init(RADIO_SCLK_PORT, RADIO_SCLK_PIN, &initStruct);
//	HW_GPIO_Write( RADIO_SCLK_PORT, RADIO_SCLK_PIN, 0);
//
//	initStruct.Pull = GPIO_NOPULL;
//	HW_GPIO_Init(RADIO_NSS_PORT, RADIO_NSS_PIN , &initStruct);
//	HW_GPIO_Write(RADIO_NSS_PORT, RADIO_NSS_PIN , 1);

	HAL_GPIO_DeInit(GPIOA, RADIO_SCLK_PIN|RADIO_MISO_PIN|RADIO_MOSI_PIN);
	HAL_GPIO_DeInit(RADIO_NSS_PORT,RADIO_NSS_PIN);
}

/*!
 * @brief Sends outData and receives inData
 *
 * @param [IN] outData Byte to be sent
 * @retval inData      Received byte.
 */
uint16_t HW_LORA_SPI_InOut(uint16_t txData)
{
	uint16_t rxData ;

	HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&txData, (uint8_t*) &rxData, 1, HAL_MAX_DELAY);

	return rxData;
}

static uint32_t SpiFrequency(uint32_t hz)
{
  uint32_t divisor = 0;
  uint32_t SysClkTmp = SystemCoreClock;
  uint32_t baudRate;

  while (SysClkTmp > hz)
  {
    divisor++;
    SysClkTmp = (SysClkTmp >> 1);

    if (divisor >= 7)
    {
      break;
    }
  }

  assert_param(((SPI_CR1_BR_0 == (0x1U << SPI_CR1_BR_Pos)) &&
                (SPI_CR1_BR_1 == (0x2U << SPI_CR1_BR_Pos)) &&
                (SPI_CR1_BR_2 == (0x4U << SPI_CR1_BR_Pos))));
  baudRate = divisor << SPI_CR1_BR_Pos;

  return baudRate;
}

//*****************************************************************************
// @LCD SPI
//*****************************************************************************
uint32_t SpiTimeout = LCD_SPI_TIMEOUT_MAX; /*<! Value of Timeout when SPI communication fails */
static SPI_HandleTypeDef lcd_spi;

static void LCD_SPI_Init(void);
static void LCD_SPI_Write(uint8_t Value);
static void LCD_SPI_Error(void);
static void LCD_SPI_MspInit(SPI_HandleTypeDef *hspi);

static void LCD_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/*** Configure the GPIOs ***/
	GPIO_InitStruct.Pin       = LCD_SPI_SCK_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = LCD_SPI_SCK_AF;
	HW_GPIO_Init(LCD_SPI_SCK_GPIO_PORT, LCD_SPI_SCK_PIN, &GPIO_InitStruct);

	/* Configure SPI MOSI */
	GPIO_InitStruct.Pin       = LCD_SPI_MOSI_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = LCD_SPI_MOSI_AF;
	HW_GPIO_Init(LCD_SPI_MOSI_GPIO_PORT, LCD_SPI_MOSI_PIN, &GPIO_InitStruct);

	/*** Configure the SPI peripheral ***/
	/* Enable SPI clock */
	LCD_SPI_CLK_ENABLE();
}

static void LCD_SPI_Init(void)
{
	if(HAL_SPI_GetState(&lcd_spi) == HAL_SPI_STATE_RESET)
	{
		/* SPI Config */
		lcd_spi.Instance = LCD_SPI;

		lcd_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
		lcd_spi.Init.Direction         = SPI_DIRECTION_2LINES;
		lcd_spi.Init.CLKPhase          = SPI_PHASE_2EDGE;
		lcd_spi.Init.CLKPolarity       = SPI_POLARITY_HIGH;
		lcd_spi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
		lcd_spi.Init.CRCPolynomial     = 7;
		lcd_spi.Init.DataSize          = SPI_DATASIZE_8BIT;
		lcd_spi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
		lcd_spi.Init.NSS               = SPI_NSS_SOFT;
		lcd_spi.Init.TIMode            = SPI_TIMODE_DISABLED;
		lcd_spi.Init.Mode              = SPI_MODE_MASTER;

		LCD_SPI_MspInit(&lcd_spi);
		HAL_SPI_Init(&lcd_spi);
	}
}

static void LCD_SPI_Write(uint8_t Value)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t data;

	status = HAL_SPI_TransmitReceive(&lcd_spi, (uint8_t*) &Value, &data, 1, SpiTimeout);

	/* Check the communication status */
	if(status != HAL_OK)
	{
		/* Execute user timeout callback */
		LCD_SPI_Error();
	}
}

static void LCD_SPI_Error(void)
{
	/* De-initialize the SPI communication BUS */
	HAL_SPI_DeInit(&lcd_spi);

	/* Re-Initiaize the SPI communication BUS */
	LCD_SPI_Init();
}

/********************************* LINK LCD ***********************************/
/**
  * @brief  Initializes the LCD.
  */
void LCD_IO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* LCD_CS_GPIO and LCD_DC_GPIO Periph clock enable */
	LCD_CS_GPIO_CLK_ENABLE();
	LCD_DC_GPIO_CLK_ENABLE();

	/* Configure LCD_CS_PIN pin: LCD Card CS pin */
	GPIO_InitStruct.Pin   = LCD_CS_PIN;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(LCD_CS_GPIO_PORT, &GPIO_InitStruct);

	/* Configure LCD_DC_PIN pin: LCD Card DC pin */
	GPIO_InitStruct.Pin   = LCD_DC_PIN;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(LCD_DC_GPIO_PORT, &GPIO_InitStruct);

	/* LCD chip select high */
	LCD_CS_LOW();
	LCD_CS_HIGH();

	/* LCD SPI Config */
	LCD_SPI_Init();
}

/**
  * @brief  Writes command to select the LCD register.
  * @param  LCDReg: Address of the selected register.
  */
void LCD_IO_WriteReg(uint8_t LCDReg)
{
	/* Reset LCD control line CS */
	LCD_CS_LOW();

	/* Set LCD data/command line DC to Low */
	LCD_DC_LOW();

	/* Send Command */
	LCD_SPI_Write(LCDReg);

	/* Deselect : Chip Select high */
	LCD_CS_HIGH();
}

/**
  * @brief  Writes data to select the LCD register.
  *         This function must be used after st7735_WriteReg() function
  * @param  Data: data to write to the selected register.
  */
void LCD_IO_WriteData(uint8_t Data)
{
	/* Reset LCD control line CS */
	LCD_CS_LOW();

	/* Set LCD data/command line DC to High */
	LCD_DC_HIGH();

	/* Send Data */
	LCD_SPI_Write(Data);

	/* Deselect : Chip Select high */
	LCD_CS_HIGH();
}

void LCD_Delay(uint32_t Delay)
{
	DelayMs(Delay);
}
