/*
 * hw_usart.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;

static void BLE_SetConfig(void);
static void BLE_Reset(void);
static void GPS_SetConfig(void);
static void DEBUG_SetConfig(void);

static void uart_queue_init(uart_queue_t *theQueue);
static bool uart_queue_put(uart_queue_t *theQueue, uint8_t *theItemValue);
static bool uart_queue_get(uart_queue_t *theQueue, uint8_t *theItemValue, uint8_t theSize);

#ifdef UART_LOG_ENABLE
static uint8_t g_SerialRxBuffer = 0;
#endif

// GPS
static uint8_t g_GPSRxBuffer = 0;
static uint8_t g_GPSMsgBuffer[GPS_MESSAGE_MAX_SIZE];

// BT
static uint8_t g_BLERxBuffer = 0;

void HW_UART_Init(void)
{
	DEBUG_Init();
	BLE_Init();
	GPS_SetConfig();
}

void BLE_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	BLE_RESET_CLK_ENABLE();
	GPIO_InitStruct.Pin = BLE_RESET_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BLE_RESET_PORT, &GPIO_InitStruct);

	BLE_SetConfig();

	if(HAL_UART_Receive_IT(&huart4, (uint8_t *)&g_BLERxBuffer, 1) != HAL_OK)
	{
		DEBUG("BLE Rx IT Error\r\n");
		return;
	}

	DelayMs(10);

	BLE_Reset();
}

void GPS_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	HAL_GPIO_WritePin(GPS_ON_PORT, GPS_ON_PIN, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = GPS_ON_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPS_ON_PORT, &GPIO_InitStruct);

	DelayMs(100);

	if(HAL_UART_Receive_IT(&huart5, (uint8_t *)&g_GPSRxBuffer, 1) != HAL_OK)
	{
		DEBUG("GPS Rx IT Error\r\n");
		return;
	}

	HAL_GPIO_WritePin(GPS_ON_PORT, GPS_ON_PIN, GPIO_PIN_SET);
	DelayMs(200);
	HAL_GPIO_WritePin(GPS_ON_PORT, GPS_ON_PIN, GPIO_PIN_RESET);

	global.gps_status = GPS_ON;
}

void DEBUG_Init(void)
{
	DEBUG_SetConfig();

#ifdef UART_LOG_ENABLE
	if(HAL_UART_Receive_IT(&huart1, (uint8_t *)&g_SerialRxBuffer, 1) != HAL_OK)
	{
		Error_Handler();
	}
#endif
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(uartHandle->Instance == UART4)
	{
		/* Peripheral clock enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_UART4_CLK_ENABLE();

		/**UART4 GPIO Configuration
		PA0     ------> UART4_TX
		PA1     ------> UART4_RX
		*/
		GPIO_InitStruct.Pin = BT_TX_PIN|BT_RX_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* UART4 interrupt Init */
		HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(UART4_IRQn);
	}
	else if(uartHandle->Instance == UART5)
	{
		/* Peripheral clock enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_UART5_CLK_ENABLE();

		/**UART5 GPIO Configuration
		PB12     ------> UART5_RX
		PB13     ------> UART5_TX
		*/
		GPIO_InitStruct.Pin = GPS_RX_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF11_UART5;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* UART5 interrupt Init */
		HAL_NVIC_SetPriority(UART5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(UART5_IRQn);
	}
	else if(uartHandle->Instance == USART1)
	{
		/* Peripheral clock enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART1_CLK_ENABLE();

		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		GPIO_InitStruct.Pin = DEBUG_TX_PIN|DEBUG_RX_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
	if(uartHandle->Instance == UART4)
	{
		/* Peripheral clock disable */
		__HAL_RCC_UART4_CLK_DISABLE();

		/**UART4 GPIO Configuration
		PA0     ------> UART4_TX
		PA1     ------> UART4_RX
		*/
		HAL_GPIO_DeInit(GPIOA, BT_TX_PIN|BT_RX_PIN);

		/* UART4 interrupt Deinit */
		HAL_NVIC_DisableIRQ(UART4_IRQn);
	}
	else if(uartHandle->Instance == UART5)
	{
		/* Peripheral clock disable */
		__HAL_RCC_UART5_CLK_DISABLE();

		/**UART5 GPIO Configuration
		PB12     ------> UART5_RX
		PB13     ------> UART5_TX
		*/
		HAL_GPIO_DeInit(GPIOB, GPS_RX_PIN);

		/* UART5 interrupt Deinit */
		HAL_NVIC_DisableIRQ(UART5_IRQn);
	}
	else if(uartHandle->Instance==USART1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();

		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		HAL_GPIO_DeInit(GPIOA, DEBUG_TX_PIN|DEBUG_RX_PIN);

		/* USART1 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART1_IRQn);
	}
}

void HW_BT_IrqHandler(void)
{
	UART_HandleTypeDef *huart = &huart4;

	HAL_UART_IRQHandler(huart);
}

void HW_GPS_IrqHandler(void)
{
	UART_HandleTypeDef *huart = &huart5;

	HAL_UART_IRQHandler(huart);
}

void HW_DEBUG_IrqHandler(void)
{
	UART_HandleTypeDef *huart = &huart1;

	HAL_UART_IRQHandler(huart);
}

void GPS_ON_Control(uint8_t on)
{
	if(on)
	{
		if(global.gps_status == GPS_OFF)
		{
			HAL_UART_MspInit(&huart5);
			HAL_UART_Receive_IT(&huart5, (uint8_t *)&g_GPSRxBuffer, 1);

			HAL_GPIO_WritePin(GPS_ON_PORT, GPS_ON_PIN, GPIO_PIN_SET);
			DelayMs(200);
			HAL_GPIO_WritePin(GPS_ON_PORT, GPS_ON_PIN, GPIO_PIN_RESET);
		}

		global.gps_status = GPS_ON;
	}
	else
	{
		if(global.gps_status == GPS_ON)
		{
			HAL_UART_MspDeInit(&huart5);

			g_GPSRxBuffer = 0;
			memset(g_GPSMsgBuffer, 0, GPS_MESSAGE_MAX_SIZE);

			HAL_GPIO_WritePin(GPS_ON_PORT, GPS_ON_PIN, GPIO_PIN_SET);
			DelayMs(200);
			HAL_GPIO_WritePin(GPS_ON_PORT, GPS_ON_PIN, GPIO_PIN_RESET);
		}

		global.gps_status = GPS_OFF;
	}

	global.gps_time_count = 0;
}

static void BLE_SetConfig(void)
{
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&huart4) != HAL_OK)
	{
		Error_Handler();
	}
}

static void BLE_Reset(void)
{
	HAL_GPIO_WritePin(BLE_RESET_PORT, BLE_RESET_PIN, GPIO_PIN_SET);
	DelayMs(100);
}

static void GPS_SetConfig(void)
{
	huart5.Instance = UART5;
	huart5.Init.BaudRate = 4800;
	huart5.Init.WordLength = UART_WORDLENGTH_8B;
	huart5.Init.StopBits = UART_STOPBITS_1;
	huart5.Init.Parity = UART_PARITY_NONE;
	huart5.Init.Mode = UART_MODE_RX;
	huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart5.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&huart5) != HAL_OK)
	{
		Error_Handler();
	}
}

static void DEBUG_SetConfig(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}

//*****************************************************************************
//  UART queue functions
//*****************************************************************************
static void uart_queue_init(uart_queue_t *theQueue)
{
	int i;

	theQueue->front = 0;
	theQueue->rear = 0;

	for(i=0; i<UART_QUEUE_ITEMS; i++)
	{
		memset(&theQueue->data[i], 0, UART_QUEUE_MAX_SIZE);
	}
}

static bool uart_queue_put(uart_queue_t *theQueue, uint8_t *theItemValue)
{
	int16_t len;

	if((theQueue->rear + 1) % UART_QUEUE_ITEMS == theQueue->front)
	{
		return false;
	}

	len = strlen((char *)theItemValue);

	memset(&theQueue->data[theQueue->rear], 0, UART_QUEUE_MAX_SIZE);
	memcpy(&theQueue->data[theQueue->rear], theItemValue, len);

	theQueue->rear = (theQueue->rear + 1) % UART_QUEUE_ITEMS;

	return true;
}

static bool uart_queue_get(uart_queue_t *theQueue, uint8_t *theItemValue, uint8_t theSize)
{
	if(theQueue->front == theQueue->rear)
	{
		//DEBUG("The serial queue is empty\r\n");
		return false;
	}

	memset(theItemValue, 0, theSize);
	memcpy(theItemValue, &theQueue->data[theQueue->front], theSize);

	theQueue->front = (theQueue->front +1) % UART_QUEUE_ITEMS;

	return true;
}

//*****************************************************************************
//  GPS queue functions
//*****************************************************************************
static uart_queue_t g_GPSQueue;

void gps_message_init(void)
{
	uart_queue_init(&g_GPSQueue);
}

bool gps_message_put(uint8_t *theItemValue)
{
	return uart_queue_put(&g_GPSQueue, theItemValue);
}

bool gps_message_get(uint8_t *theItemValue, uint8_t theSize)
{
	return uart_queue_get(&g_GPSQueue, theItemValue, theSize);
}

//*****************************************************************************
//  BT queue functions
//*****************************************************************************
typedef struct
{
	uint8_t    first;
	uint8_t    last;
	uint8_t    valid;
	uint8_t    items;
	uint8_t    *data;
} queue_t;

static queue_t g_BLEQueue;

static uint8_t queue_init(queue_t *theQueue, uint8_t items)
{
	theQueue->valid =  0;
	theQueue->first =  0;
	theQueue->last  =  0;
	theQueue->data  = (uint8_t *)malloc(sizeof(uint8_t)*items);

	if (NULL == theQueue->data)
	{
		theQueue->items =  0;
		return(0);
	}
	else
	{
		theQueue->items =  items;
		return(1);
	}
}

static uint8_t queue_empty(queue_t *theQueue)
{
	if(theQueue->valid==0)
		return(1);
	else
		return(0);
}

static uint8_t queue_full(queue_t *theQueue)
{
	if(theQueue->valid >= theQueue->items)
		return(1);
	else
		return(0);
}

static uint8_t queue_put(queue_t *theQueue, uint8_t theItemValue)
{
	if(queue_full(theQueue))
	{
		//DEBUG("The queue is full\r\n");
		return(0);
	}
	else
	{
		theQueue->valid++;
		theQueue->data[theQueue->last] = theItemValue;
		theQueue->last = (theQueue->last+1) % theQueue->items;
		return(1);
	}
}

static uint8_t queue_get(queue_t *theQueue, uint8_t *theItemValue)
{
	if(queue_empty(theQueue))
	{
		//DEBUG("The queue is empty\r\n");
		*theItemValue = 0;
		return(0);
	}
	else
	{
		*theItemValue=theQueue->data[theQueue->first];
		theQueue->first=(theQueue->first+1) % theQueue->items;
		theQueue->valid--;
		return(1);
	}
}

uint8_t ble_message_init(void)
{
	return queue_init(&g_BLEQueue, BLE_MESSAGE_MAX_SIZE);
}

uint8_t ble_message_put(uint8_t theItemValue)
{
	return queue_put(&g_BLEQueue, theItemValue);
}

uint8_t ble_message_get(uint8_t *theItemValue)
{
	return queue_get(&g_BLEQueue, theItemValue);
}

void ble_put(char *msg, int len)
{
	HAL_UART_Transmit(&huart4, (uint8_t *)msg, (uint16_t)len, 300);
}

//*****************************************************************************
//  UART Callback function
//*****************************************************************************
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	int16_t len;

#ifdef UART_LOG_ENABLE
	if(huart->Instance == USART1)
	{
		uread((char *)&g_SerialRxBuffer, 1);

		g_SerialRxBuffer = 0;
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&g_SerialRxBuffer, 1);
	}
	else if(huart->Instance == UART5)
#else
	if(huart->Instance == UART5)
#endif
	{
		// GPS Message
		len = strlen((char *)g_GPSMsgBuffer);

		if(len >= GPS_MESSAGE_MAX_SIZE)
		{
			memset(g_GPSMsgBuffer, 0, GPS_MESSAGE_MAX_SIZE);
		}
		else
		{
			strncat((char *)g_GPSMsgBuffer, (char *)&g_GPSRxBuffer, 1);

			if(g_GPSMsgBuffer[len-1] == 0x0D && g_GPSMsgBuffer[len] == 0x0A)
			{
				gps_message_put(g_GPSMsgBuffer);
				memset(g_GPSMsgBuffer, 0, GPS_MESSAGE_MAX_SIZE);
			}

			g_GPSRxBuffer = 0;
			HAL_UART_Receive_IT(&huart5, (uint8_t *)&g_GPSRxBuffer, 1);
		}
	}
	else if(huart->Instance == UART4)
	{
		ble_message_put(g_BLERxBuffer);

		g_BLERxBuffer = 0;
		HAL_UART_Receive_IT(&huart4, (uint8_t *)&g_BLERxBuffer, 1);
	}
}
