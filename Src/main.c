/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "hw.h"
//#include "node.h"

#include "lcd_interface.h"
#include "functions.h"
#include "display.h"
#include "minmea.h"
#include "bitmaps.h"
#include "timeServer.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
global_t global;
config_t config;

static ts_data_t ts_data;
static uint8_t g_GPSBuffer[GPS_MESSAGE_MAX_SIZE];
static uint8_t g_rmc_buffer[GPS_MESSAGE_MAX_SIZE];

static uint8_t g_gga_buffer[GPS_MESSAGE_MAX_SIZE];

static uint16_t g_battery_debug_count = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
extern void SX1276OnDio0Irq(void);
extern void SX1276OnDio1Irq(void);
extern void SX1276OnDio2Irq(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
TimerEvent_t gpsTimer;

extern TimerEvent_t lTimer;
extern char g_lora_send_buffer[255];
extern int g_lora_status;
static void OnLoraTxErrorTimer(void)
{
	//Buzzer_ON_Control(0);
	//VIBRATOR_ON_Control(VIBRATOR_OFF);
	DBG_PRINTF("RESENDING...\n\r");
	TimerStop(&lTimer);
	switch(g_lora_status)
	{
	case 0: // Initialized
		DBG_PRINTF("000\n\r");
		break;
	case 1: // Failed
		node_send(g_lora_send_buffer, strlen(g_lora_send_buffer));
		DBG_PRINTF("NODE_SENDED\n\r");
		break;
	case 2: // Success
		DBG_PRINTF("222\n\r");
		break;
	default:
		DBG_PRINTF("default\n\r");
		break;

	}
	//LCD_Main_Display(g_display);
}


static void OnGPSTimerEvent2(void)
{
	  GPS_ON_Control(1);
	  //DPRINTF("\r\nGPS TIMER %u %u\r\n", g_gga_buffer);
	TimerStop(&gpsTimer);
	DPRINTF("\r\n==============================\r\n");
	DPRINTF("\r\nGPS TIMER\r\n");


	char send_buffer[] ="0037.498104:127.032776";
	DPRINTF(send_buffer);
	DPRINTF("\r\n");
	node_send(send_buffer, strlen(send_buffer));
	DPRINTF("SENT..\r\n");



	char send_buffer2[] ="013";
	DPRINTF(send_buffer2);
	DPRINTF("\r\n");
	node_send(send_buffer2, strlen(send_buffer2));
	DPRINTF("SENT..\r\n");


	//DPRINTF(g_gga_buffer);
	//send_GPGGA_Display(g_gga_buffer);
	DPRINTF("\r\n-----------------------------\r\n");
	  //GPS_ON_Control(0);
	TimerSetValue(&gpsTimer, 60000);
	TimerStart(&gpsTimer);

}
void send_GPGGA_Display(uint8_t *data);

int main(void)
{

  /* USER CODE BEGIN 1 */
  event_t e;
  uint8_t ble_data;
#ifndef UART_LOG_ENABLE
  uint16_t size;
  uint8_t  data[UART_QUEUE_BUFFER];
#endif
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  memset(&config, 0, sizeof(config));
  memset(&global, 0, sizeof(global));

  event_init();
  serial_init();
  ble_message_init();
  gps_message_init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
#ifdef RDP_LEVEL1_ENABLE
  RDP_SetConfig(1);
#endif
  HW_USB_Init();
  HW_I2C_Init();
  HW_RTC_Init();
  HW_UART_Init();
  HW_ADC_Init();
  Wakeup_Init();
  Vibrator_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  HW_PMIC_Init();
  CONFIG_Read();
  memcpy(global.version.s, STRINGIZE(FIRMWARE_VERSION), 4);

  LCD_Init();
  TS_Init();
  GPS_Init();
  BAT_Level_Check_Init();

  //GPSTimer_Init();
  WakeupTimer_Init();
  BuzzerTimer_Init();
  VibratorTimer_Init();


  BuzzerTimer_Start(500);
  VibratorTimer_Start(1000);

  event_put(EVENT_LORA_NODE, 0, NULL);

  LCD_Main_Display(DISPLAY_TOUCH_STATE);
  /* USER CODE END 2 */
	TimerInit(&gpsTimer, OnGPSTimerEvent);
	TimerSetValue(&gpsTimer, 60000);
	TimerStart(&gpsTimer);


	TimerInit(&lTimer, OnLoraTxErrorTimer);
	//TimerSetValue(&lTimer, 60000);
	//TimerStart(&gpsTimer);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  event_get(&e, false);
	  if(e.e == EVENT_LORA_INTERRUPT || e.e ==  EVENT_LORA_NODE)
	  {
		  DPRINTF("\r\nEVENT %u %u\r\n", e.e, e.v);
	  }

	  switch(e.e)
	  {
	  case EVENT_LORA_INTERRUPT:
		  if(e.v == 0)      SX1276OnDio0Irq();
		  else if(e.v == 1) SX1276OnDio1Irq();
		  else if(e.v == 2) SX1276OnDio2Irq();
		  break;

	  case EVENT_LORA_NODE:
		  if(e.v == 0) node_init();
		  else         node_process((uint8_t)e.v);
		  break;

	  case EVENT_TIMER:
		  if(e.Callback != NULL) e.Callback();
		  break;

	  case EVENT_USB:
		  global.usb_connected = (bool)e.v;
		  global.account = ID_MODE;
		  DEBUG("USB %sCONNECTED\r\n", global.usb_connected ? "" : "DIS");
		  break;

	  case EVENT_TOUCH_INTERRUPT:
		  if(global.wakeup_mode == WAKEUP_MODE) TS_Event(&ts_data);
		  break;

	  case EVENT_TOUCH_POINT:
		  if(e.v == 1) LCD_TS_Display(&ts_data);
		  break;

	  case EVENT_GPS_MESSAGE:
		  if(e.v == MINMEA_SENTENCE_RMC)
		  {
			  GPS_RMC_Status(g_rmc_buffer);

			  if(global.gps_log)
			  {
				  DPRINTF("%s", g_rmc_buffer);
			  }

			  LCD_GPRMC_Display(g_rmc_buffer);
		  }
		  break;

	  case EVENT_WAKEUP_BUTTON_INTERRUPT:
		  if(global.wakeup_interrupt == 0)
		  {
			  global.wakeup_interrupt = 1;
			  WakeupTimer_Start();
		  }
		  break;

	  case EVENT_BAT_CHARGING:
		  if(Charging_Status(PMIC_Read(0x77)))
		  {
			  BAT_Level_Check();
			  LCD_CHARGING_Display();
		  }
		  break;

	  case EVENT_BAT_LEVEL_CHECK:
		  global.battery_level = e.v;
		  DPRINTF("VAT[%04d] %d V\r\n", g_battery_debug_count, global.battery_level);

		  if(global.battery_level < 340 && global.charging_status == NO_CHARGING)
		  {
			  // under 3.4V
			  Power_Off_Mode();
		  }
		  else
		  {
			  LCD_BAT_LEVLE_Display();
			  if(g_battery_debug_count++ >= 10000)
				  g_battery_debug_count = 0;
		  }
		  break;

	  case EVENT_LORA_JOINED:
		  LCD_LoRa_Display();
		  break;

	  default:
		  break;
	  }

	  while(event_empty())
	  {
		  if(gps_message_get(g_GPSBuffer, GPS_MESSAGE_MAX_SIZE))
		  {
			  gps_rmc_parsing(g_GPSBuffer, g_rmc_buffer);
		  }

#ifdef UART_LOG_ENABLE
		  if(ble_message_get(&ble_data))
		  {
			  DPRINTF("%c", ble_data);
		  }

		  CheckForUserCommands();
#else
		  if(global.usb_connected)
		  {
			  if(ble_message_get(&ble_data))
			  {
				DPRINTF("%c", ble_data);
			  }

			  if(serial_get(data, &size))
			  {
				 uread((char *)data, size);
			  }

			  CheckForUserCommands();
		  }
#endif
	  }
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
#ifdef USB_PLLI2S_CLOCK_ENABLE
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
#else
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
#endif
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
