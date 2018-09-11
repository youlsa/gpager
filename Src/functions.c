/*
 * functions.c
 *
 *  Created on: 2017. 10. 27.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "minmea.h"
#include "timeServer.h"
#include "lcd_interface.h"
#include "node.h"
//*****************************************************************************
//  GPS functions
//*****************************************************************************
static TimerEvent_t GPSTimer;
extern TimerEvent_t gpsTimer;
char send_buffer[1024];
int g_gps_valid = 0;

bool gps_gga_parsing(uint8_t *queue, uint8_t *data)
{
	if(minmea_sentence_id((char *)queue, false) == MINMEA_SENTENCE_GGA)
	{
		memset(data, 0, GPS_MESSAGE_MAX_SIZE);
		memcpy(data, queue, GPS_MESSAGE_MAX_SIZE);

		event_put(EVENT_GPS_MESSAGE, MINMEA_SENTENCE_GGA, NULL);

		return true;
	}

	return false;
}

bool gps_rmc_parsing(uint8_t *queue, uint8_t *data)
{
	if(minmea_sentence_id((char *)queue, false) == MINMEA_SENTENCE_RMC)
	{
		memset(data, 0, GPS_MESSAGE_MAX_SIZE);
		memcpy(data, queue, GPS_MESSAGE_MAX_SIZE);

		event_put(EVENT_GPS_MESSAGE, MINMEA_SENTENCE_RMC, NULL);

		return true;
	}

	return false;
}

bool GPS_RMC_Status(uint8_t *data)
{
	 struct minmea_sentence_rmc rmc;

	 minmea_parse_rmc(&rmc, (char *)data);

	 if(rmc.valid == 0)
	 {
		 if(global.gps_time_count >= 120)
		 {
			 global.gps_time_count = 0;
//			 GPS_ON_Control(0);
		 }
		 else
		 {
			 global.gps_time_count++;
//			 GPS_ON_Control(0);
		 }

		 return false;
	 }

	 global.gps_time_count = 0;

	 return true;
}
uint8_t g_gprmc[];
void OnGPSTimerEvent(void)
{
	//DPRINTF("=======GPS======\r\n");
	//TimerStop(&GPSTimer);
	TimerStop(&gpsTimer);
	//if(global.gps_event_time != 0)
	{
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXxx
		GPS_ON_Control(1);
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXxx
		//strcpy(send_buffer, "0037.498104:127.032776\0");
		strcpy(send_buffer, "GPS Inf.");
				//strcpy(send_buffer, "016\0");
		//DPRINTF(send_buffer);
		//DPRINTF("\r\n");




		//DPRINTF("### CAN I SEND THIS??? %s", g_gprmc);
		char pStart[255];
		strcpy(pStart, strtok(g_gprmc, ','));
		char pProtocol[255];
		strcpy(pProtocol, strtok(NULL, ","));
		char pTime[255];
		strcpy(pTime, strtok(NULL, ","));
		char pValid[255];
		strcpy(pValid, strtok(NULL, ","));
		char pLongitude[255];
		strcpy(pLongitude, strtok(NULL, ","));
		char pNorthOrSouth[255];
		strcpy(pNorthOrSouth, strtok(NULL, ","));
		char pLatitude[255];
		strcpy(pLatitude, strtok(NULL, ","));
		char pEastOrWest[255];
		strcpy(pEastOrWest, strtok(NULL, ","));
		//DPRINTF("####\n\r");
		//DPRINTF("%s, %s%s, %s%s\n\r", pValid, pNorthOrSouth, pLongitude, pEastOrWest, pLatitude);

		if(!strcmp(pValid, "A"))
		{
			g_gps_valid = 1;
			//DPRINTF("###### VALID\n\r");
			{
			char pLeft[255], pRight[255];
			char final_buffer[255];

			memset(pLeft, 0, 255);
			memset(pRight, 0, 255);
			memset(final_buffer, 0, 255);
			int nLeft = 0;
			//DPRINTF("LONG == %s\n\r", pLongitude);
			strcpy(pLeft, strtok(pLongitude, "."));
			strcat(pLeft, "\0\0\0");
			strcpy(pRight, strtok(NULL, "."));
			strcat(pRight, "\0\0\0");
			//DPRINTF("LEFT %s, RIGHT %s\n\r", pLeft, pRight);


			nLeft = strlen(pLeft);
			//DPRINTF("LEFT SIZE %n\n\r", nLeft);
			strncpy(final_buffer, pLeft, nLeft-2);
			strcat(final_buffer, ".");

			strcat(final_buffer, pLeft+nLeft-2);
			strcat(final_buffer, pRight);
			strcpy(pLongitude, final_buffer);
			//DPRINTF("%s\n\r", final_buffer);
			}
			{
			char pLeft[255], pRight[255];
			char final_buffer[255];
			memset(pLeft, 0, 255);
			memset(pRight, 0, 255);
			memset(final_buffer, 0, 255);
			int nLeft = 0;
			//DPRINTF("LONG == %s\n\r", pLatitude);
			strcpy(pLeft, strtok(pLatitude, "."));
			strcat(pLeft, "\0\0\0");
			strcpy(pRight, strtok(NULL, "."));
			strcat(pRight, "\0\0\0");
			//DPRINTF("LEFT %s, RIGHT %s\n\r", pLeft, pRight);


			nLeft = strlen(pLeft);
			//DPRINTF("LEFT SIZE %n\n\r", nLeft);
			strncpy(final_buffer, pLeft, nLeft-2);
			strcat(final_buffer, ".");

			strcat(final_buffer, pLeft+nLeft-2);
			strcat(final_buffer, pRight);
			strcpy(pLatitude, final_buffer);
			//DPRINTF("%s\n\r", final_buffer);
			}

		//	DPRINTF("***** %s, %s\n\r", pLatitude, pLongitude);

		}
		else
		{
			g_gps_valid = 0;
			//DPRINTF("########## NOT VALID\n\r");
			//TimerSetValue(&gpsTimer, 60000);
			//TimerStart(&gpsTimer);
			//return;
			strcpy(pLatitude, "00.000000");
			strcpy(pLongitude, "000.000000");
		}
		// ### SENDING 003731.0539:12705.2669
		sprintf(send_buffer, "00%s:%s",pLongitude, pLatitude);
		//DPRINTF("### SENDING %s\n\r", send_buffer);
		if(node_send(send_buffer, strlen(send_buffer)))
		{
			DPRINTF("Periodic GPS data SENT..\r\n");
			TimerSetValue(&gpsTimer, 60000);
			TimerStart(&gpsTimer);
			LCD_Main_Display(55);
		}
		else
		{
			TimerSetValue(&gpsTimer, 60000);
			TimerStart(&gpsTimer);
			DPRINTF("Periodic GPS data NOT SENT..\r\n");
			//LCD_Main_Display(55);
		}

//		TimerSetValue(&GPSTimer, (uint32_t)(global.gps_event_time * 60000.0f));
	//	TimerStart(&GPSTimer);
	}


}

void GPSTimer_Init(void)
{
	TimerInit(&GPSTimer, OnGPSTimerEvent);
}

void GPSTimer_Start(uint8_t time)
{
	if(global.gps_event_running == 0)
	{
		TimerSetValue(&GPSTimer, (uint32_t)(time * 60000.0f));
		TimerStart(&GPSTimer);

		global.gps_event_running = 1;
		global.gps_event_time    = time;
	}
}

void GPSTimer_Stop(void)
{
	TimerStop(&GPSTimer);

	global.gps_event_running = 0;
	global.gps_event_time    = 0;
}

//*****************************************************************************
//  VIBRATOR functions
//*****************************************************************************
static TimerEvent_t VibratorTimer;
static void OnVibratorTimerEvent(void);

void Vibrator_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_WritePin(VIBRATOR_PORT, VIBRATOR_PIN, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = VIBRATOR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(VIBRATOR_PORT, &GPIO_InitStruct);
}

static void OnVibratorTimerEvent(void)
{
	TimerStop(&VibratorTimer);

	Vibrator_ON_Control(VIBRATOR_OFF);
}

void VibratorTimer_Init(void)
{
	TimerInit(&VibratorTimer, OnVibratorTimerEvent);
}

void VibratorTimer_Start(uint32_t time)
{
	Vibrator_ON_Control(VIBRATOR_ON);

	TimerSetValue(&VibratorTimer, time);
	TimerStart(&VibratorTimer);
}

void Vibrator_ON_Control(uint8_t on)
{
	if(on == VIBRATOR_ON)
	{
		HAL_GPIO_WritePin(VIBRATOR_PORT, VIBRATOR_PIN, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(VIBRATOR_PORT, VIBRATOR_PIN, GPIO_PIN_RESET);
	}
}

//*****************************************************************************
//  BUZZER functions
//*****************************************************************************
static TimerEvent_t BuzzerTimer;
static void OnBuzzerTimerEvent(void);

static void OnBuzzerTimerEvent(void)
{
	TimerStop(&BuzzerTimer);

	Buzzer_ON_Control(BUZZER_OFF);
}

void BuzzerTimer_Init(void)
{
	TimerInit(&BuzzerTimer, OnBuzzerTimerEvent);
}

void BuzzerTimer_Start(uint32_t time)
{
	Buzzer_ON_Control(BUZZER_ON);

	TimerSetValue(&BuzzerTimer, time);
	TimerStart(&BuzzerTimer);
}

void Buzzer_ON_Control(uint8_t on)
{
	if(on == BUZZER_ON)
	{
		PMIC_Write(0x41, 0x7F);
		PMIC_Write(0x42, 0x7F);
	}
	else
	{
		PMIC_Write(0x41, 0x00);
		PMIC_Write(0x42, 0x00);
	}
}

//*****************************************************************************
//  Wakeup button functions
//*****************************************************************************
#define WAKEUP_CHECK_TIME			100
#define SLEEP_MODE_MIN_COUNT		3			// 0.3 sec
#define SLEEP_MODE_MAX_COUNT		19			// 1.9 sec

static uint16_t g_wakeup_count = 0;
static TimerEvent_t WakeupTimer;
static void OnWakeupTimerEvent(void);

void Wakeup_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	WAKEUP_BUTTON_CLK_ENABLE();

	GPIO_InitStruct.Pin = WAKEUP_BUTTON_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(WAKEUP_BUTTON_PORT, &GPIO_InitStruct);

	HW_GPIO_SetIrq(WAKEUP_BUTTON_PORT, WAKEUP_BUTTON_PIN, 0);
}

static void Wakeup_Mode(uint8_t mode)
{
	if(mode == WAKEUP_MODE)
	{
		global.wakeup_mode = WAKEUP_MODE;

		GPS_ON_Control(1);
		LCD_DisplayOn();
		LCD_Backlight_OnOff(1);
	}
	else
	{
		global.wakeup_mode = SLEEP_MODE;

		GPS_ON_Control(0);
		LCD_DisplayOff();
		LCD_Backlight_OnOff(0);
	}
}

static void OnWakeupTimerEvent(void)
{
	TimerStop(&WakeupTimer);

	if(HW_GPIO_Read(WAKEUP_BUTTON_PORT, WAKEUP_BUTTON_PIN))
	{
		g_wakeup_count++;
		if((global.wakeup_mode == WAKEUP_MODE) && (g_wakeup_count > SLEEP_MODE_MAX_COUNT))
		{
			g_wakeup_count = 0;
			global.wakeup_interrupt = 0;

			Power_Off_Mode();
		}
		else if((global.wakeup_mode == SLEEP_MODE) && (g_wakeup_count >= SLEEP_MODE_MIN_COUNT))
		{
			g_wakeup_count = 0;
			global.wakeup_interrupt = 0;

			Wakeup_Mode(WAKEUP_MODE);
		}
		else
		{
			TimerSetValue(&WakeupTimer, WAKEUP_CHECK_TIME);
			TimerStart(&WakeupTimer);
		}
	}
	else
	{
		if((global.wakeup_mode == WAKEUP_MODE) &&((g_wakeup_count >= SLEEP_MODE_MIN_COUNT) && (g_wakeup_count <= SLEEP_MODE_MAX_COUNT)))
		{
			Wakeup_Mode(SLEEP_MODE);
		}

		g_wakeup_count = 0;
		global.wakeup_interrupt = 0;
	}
}

void WakeupTimer_Init(void)
{
	TimerInit(&WakeupTimer, OnWakeupTimerEvent);
}

void WakeupTimer_Start(void)
{
	TimerSetValue(&WakeupTimer, WAKEUP_CHECK_TIME);
	TimerStart(&WakeupTimer);
}

void Power_Off_Mode(void)
{
	global.wakeup_mode = POWEROFF_MODE;

	LCD_Clear(LCD_COLOR_WHITE);
	LCD_DisplayStringAt(0, 48, (uint8_t *)"POWER OFF", CENTER_MODE);
	LCD_Update(NULL);

	Delay(2);
	LCD_DisplayOff();
	PMIC_Write(0x36, 0x02);
}

//*****************************************************************************
//  Charging functions
//*****************************************************************************
bool Charging_Status(uint8_t value)
{
	bool ret = false;

	if(value & PMIC_INTERRUPT_CHDET)
	{
		if(PMIC_Read(0x87) & PMIC_CHDET_STATUS)
		{
			global.charging_status = CHARGING;
		}
		else
		{
			global.charging_status = NO_CHARGING;
		}

		ret = true;
	}
	else if(value & PMIC_INTERRUPT_EOC)
	{
		global.charging_status = FULL_CHARGING;

		ret = true;
	}


	return ret;
}

//*****************************************************************************
//  Battery functions
//*****************************************************************************
#define BATTERY_CHECK_TIME		60000

static TimerEvent_t BatteryCheckTimer;
static void OnBatteryCheckTimerEvent(void);

void BAT_Level_Check_Init(void)
{
	TimerInit(&BatteryCheckTimer, OnBatteryCheckTimerEvent);

	BatteryLevel_Check_Start_IT();

	TimerSetValue(&BatteryCheckTimer, BATTERY_CHECK_TIME);
	TimerStart(&BatteryCheckTimer);
}

void BAT_Level_Check(void)
{
	TimerStop(&BatteryCheckTimer);

	TimerSetValue(&BatteryCheckTimer, 1000);
	TimerStart(&BatteryCheckTimer);
}

static void OnBatteryCheckTimerEvent(void)
{
	TimerStop(&BatteryCheckTimer);

	BatteryLevel_Check_Start_IT();

	TimerSetValue(&BatteryCheckTimer, BATTERY_CHECK_TIME);
	TimerStart(&BatteryCheckTimer);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
	float    volt = 0.0;
	uint32_t adc_value = HAL_ADC_GetValue(AdcHandle);

	volt = (float)(adc_value* 33 * 2) / 4095 / 10;

	event_put(EVENT_BAT_LEVEL_CHECK, (uint32_t)(volt * 100), NULL);
}

//*****************************************************************************
//  RDP functions
//*****************************************************************************
void RDP_SetConfig(uint8_t level)
{
	FLASH_OBProgramInitTypeDef OptionsBytes;

	HAL_FLASH_Unlock();
	HAL_FLASH_OB_Unlock();

	OptionsBytes.OptionType = OPTIONBYTE_RDP;
	OptionsBytes.RDPLevel   = level ? OB_RDP_LEVEL_1 : OB_RDP_LEVEL_0;

	if(HAL_FLASHEx_OBProgram(&OptionsBytes) != HAL_OK)
	{
		while(1)
		{

		}

		 HAL_FLASH_OB_Launch();
	}

	HAL_FLASH_OB_Lock();
}

void RDP_GetConfig(void)
{
	FLASH_OBProgramInitTypeDef OptionsBytes;

	HAL_FLASH_Unlock();
	HAL_FLASH_OB_Unlock();

	HAL_FLASHEx_OBGetConfig(&OptionsBytes);

	HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock();

	DPRINTF("Level : 0x%x\r\n", OptionsBytes.RDPLevel);
}
