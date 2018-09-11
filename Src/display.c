/*
 * display.c
 *
 *  Created on: 2017. 9. 27.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "lcd_interface.h"
#include "display.h"
#include "bitmaps.h"
#include "timeServer.h"
 int8_t g_display = 127;

static void touch_display(ts_data_t * data);
int message_target_rider;
int message_number;
static TimerEvent_t mTimer;
int message_timer;


extern int distance1;
extern int distance2;
extern int distance3;
extern int distance4;
extern int distance5;
extern int g_gps_valid;

static void OnMessageTimer(void)
{
	Buzzer_ON_Control(0);
	//VIBRATOR_ON_Control(VIBRATOR_OFF);
	TimerStop(&mTimer);
	g_display = 1;
	LCD_Main_Display(g_display);
}

static TimerEvent_t eTimer;
static void OnTimerEvent(void)
{

	TimerStop(&eTimer);
	g_display = 1;
	LCD_Main_Display(g_display);
}


void drawDistanceNumber(int distance)
{
	char buffer[255];
	sprintf(buffer, "%d", distance);
	int length = strlen(buffer);

	int x = 65;
	if(length==2)
	{
		x = 45;
	}
	else if(length==1)
	{
		x = 25;
	}
	int i=0;
	for(i = 0; i<length; i++)
	{
		switch(buffer[i])
		{
		case '1':
			LCD_DrawBitmap_Mono(170, x, bitmap_x4_1);
			break;
		case '2':
			LCD_DrawBitmap_Mono(170, x, bitmap_x4_2);
			break;
		case '3':
			LCD_DrawBitmap_Mono(170, x, bitmap_x4_3);
			break;
		case '4':
			LCD_DrawBitmap_Mono(170, x, bitmap_x4_4);
			break;
		case '5':
			LCD_DrawBitmap_Mono(170, x, bitmap_x4_5);
			break;
		case '6':
			LCD_DrawBitmap_Mono(170, x, bitmap_x4_6);
			break;
		case '7':
			LCD_DrawBitmap_Mono(170, x, bitmap_x4_7);
			break;
		case '8':
			LCD_DrawBitmap_Mono(170, x, bitmap_x4_8);
			break;
		case '9':
			LCD_DrawBitmap_Mono(170, x, bitmap_x4_9);
			break;
		case '0':
			LCD_DrawBitmap_Mono(170, x, bitmap_x4_0);
			break;

		}

		x = x - 20;
	}
	LCD_DrawBitmap_Mono(190, 20, bitmap_x4_100m);

;
}

void LCD_Main_Display(uint8_t display)
{
	TimerInit(&eTimer,OnTimerEvent);
	TimerInit(&mTimer,OnMessageTimer);
	LCD_Clear(LCD_COLOR_WHITE);
	int a = LCD_GetXSize();
	int b = LCD_GetYSize();
	int c = a;
	c = a;
	c = b;
	switch(display)
	{


	case 2:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_DrawBitmap_Mono(20,25,bitmap_x4_course );
		LCD_DrawBitmap_Mono(80,20,bitmap_x4_rest );
		LCD_DrawBitmap_Mono(150,22,bitmap_x4_accident );
		LCD_DrawBitmap_Mono(210, 10, bitmap_x4_back);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_DrawLine(60, 20, 60, 80);
		LCD_DrawLine(130, 20, 130, 80);
		LCD_DrawLine(190, 20, 190, 80);

		LCD_DrawRect(0,0, 255,95);
		TimerSetValue(&eTimer, 5000);
		//TimerStart(&eTimer);
		//LCD_DisplayStringAt(0, 8, (uint8_t *)"메세지 보내기", CENTER_MODE);
		//LCD_DrawBitmap_Mono(0,0,ion_logo );
		//LCD_DrawBitmap_Mono(0,0, ion_bitmap );

		//LCD_DrawArrorRight(243,3);
		//LCD_DrawBitmap_Mono(100,50, battery_bitmap);

		break;
	case 3:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_DrawBitmap_Mono(20,25,bitmap_x4_flat_tire );
		LCD_DrawBitmap_Mono(80,20,bitmap_x4_drop );
		LCD_DrawBitmap_Mono(150,22,bitmap_x4_crash );
		LCD_DrawBitmap_Mono(210, 10, bitmap_x4_back);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_DrawLine(60, 20, 60, 80);
		LCD_DrawLine(130, 20, 130, 80);
		LCD_DrawLine(190, 20, 190, 80);


		LCD_DrawRect(0,0, 255,95);

		//LCD_DisplayStringAt(0, 8, (uint8_t *)"메세지 보내기", CENTER_MODE);
		//LCD_DrawBitmap_Mono(0,0,ion_logo );
		//LCD_DrawBitmap_Mono(0,0, ion_bitmap );

		//LCD_DrawArrorRight(243,3);
		//LCD_DrawBitmap_Mono(100,50, battery_bitmap);

		break;
//		LCD_DrawRect(0,0, 255,95);
//		LCD_DisplayStringAt(1, 1, (uint8_t *)"기타 정보", CENTER_MODE);
//		//LCD_DrawBitmap_Mono(0,0, ion_bitmap );
//		LCD_DrawArrorLeft(5,3);
//		LCD_DrawArrorRight(243,3);
//
//		break;
	case 4:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_DrawBitmap_Mono(20,25,bitmap_x4_rest_text );
		LCD_DrawBitmap_Mono(80,20,bitmap_x4_meal );
		LCD_DrawBitmap_Mono(150,22,bitmap_x4_waiting );
		LCD_DrawBitmap_Mono(210, 10, bitmap_x4_back);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_DrawLine(60, 20, 60, 80);
		LCD_DrawLine(130, 20, 130, 80);
		LCD_DrawLine(190, 20, 190, 80);


		LCD_DrawRect(0,0, 255,95);

		//LCD_DisplayStringAt(0, 8, (uint8_t *)"메세지 보내기", CENTER_MODE);
		//LCD_DrawBitmap_Mono(0,0,ion_logo );
		//LCD_DrawBitmap_Mono(0,0, ion_bitmap );

		//LCD_DrawArrorRight(243,3);
		//LCD_DrawBitmap_Mono(100,50, battery_bitmap);

		break;
//		LCD_DrawRect(0,0, 255,94);
//		LCD_DisplayStringAt(3, 1, (uint8_t *)"메세지", CENTER_MODE);
//		LCD_DisplayStringAt(3, 33, (uint8_t *)"(1) 펑크 발생 (0.1km)", LEFT_MODE);
//		LCD_DisplayStringAt(3, 65, (uint8_t *)"(4) 쉬었다 갑시다 (10.1km)", LEFT_MODE);
//		//LCD_DrawBitmap_Mono(150,16,ion_logo );
//		LCD_DrawArrorLeft(5,3);
//		LCD_DrawArrorRight(243,3);
//		break;
	case 5:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_DrawBitmap_Mono(20,25,bitmap_x4_uphill );
		LCD_DrawBitmap_Mono(80,20,bitmap_x4_downhill );
		LCD_DrawBitmap_Mono(150,22,bitmap_x4_curve );
		LCD_DrawBitmap_Mono(210, 10, bitmap_x4_back);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_DrawLine(60, 20, 60, 80);
		LCD_DrawLine(130, 20, 130, 80);
		LCD_DrawLine(190, 20, 190, 80);


		LCD_DrawRect(0,0, 255,95);

		//LCD_DisplayStringAt(0, 8, (uint8_t *)"메세지 보내기", CENTER_MODE);
		//LCD_DrawBitmap_Mono(0,0,ion_logo );
		//LCD_DrawBitmap_Mono(0,0, ion_bitmap );

		//LCD_DrawArrorRight(243,3);
		//LCD_DrawBitmap_Mono(100,50, battery_bitmap);

		break;
//		LCD_DrawRect(0,0, 255,95);
//
//		LCD_DrawBitmap_Mono(0,0,bitmap_x4_start );
//		LCD_DrawBitmap_Mono(160,0,bitmap_x4_rest_text );
//		LCD_DrawArrorLeft(5,3);
//		LCD_DrawArrorRight(243,3);
//		break;
	case 6:
	case 7:
	case 8:

		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);

		LCD_DrawBitmap_Mono(60,25,bitmap_x4_accident );
		if(display==6)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_flat_tire);
		else if(display==7)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_drop);
		else if(display==8)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_crash);

		LCD_DrawLine(100,20, 100, 80);

		//LCD_DrawArrorLeft(5,3);

		break;
	case 9:
	case 10:
	case 11:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);

		LCD_DrawBitmap_Mono(60,25,bitmap_x4_rest );
		if(display==9)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_rest_text);
		else if(display==10)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_meal);
		else if(display==11)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_waiting);

		LCD_DrawLine(100,20, 100, 80);

		//LCD_DrawArrorLeft(5,3);

		break;
	case 12:
	case 13:
	case 14:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);

		LCD_DrawBitmap_Mono(60,25,bitmap_x4_course );
		if(display==12)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_uphill);
		else if(display==13)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_downhill);
		else if(display==14)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_curve);;
		LCD_DrawLine(100,20, 100, 80);

		//LCD_DrawArrorLeft(5,3);

		break;

	case 46:
	case 47:
	case 48:

		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);

		LCD_DrawBitmap_Mono(60,25,bitmap_x4_accident );
		if(display==46)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_flat_tire);
		else if(display==47)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_drop);
		else if(display==48)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_crash);

		LCD_DrawLine(100,20, 100, 80);
		TimerSetValue(&eTimer, 5000);
		TimerStart(&eTimer);
		//LCD_DrawArrorLeft(5,3);

		break;
	case 49:
	case 50:
	case 51:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);

		LCD_DrawBitmap_Mono(60,25,bitmap_x4_rest );
		if(display==49)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_rest_text);
		else if(display==50)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_meal);
		else if(display==51)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_waiting);

		LCD_DrawLine(100,20, 100, 80);
		TimerSetValue(&eTimer, 5000);
		TimerStart(&eTimer);
		//LCD_DrawArrorLeft(5,3);

		break;
	case 52:
	case 53:
	case 54:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);

		LCD_DrawBitmap_Mono(60,25,bitmap_x4_course );
		if(display==52)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_uphill);
		else if(display==53)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_downhill);
		else if(display==54)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_curve);;
		LCD_DrawLine(100,20, 100, 80);
		TimerSetValue(&eTimer, 5000);
		TimerStart(&eTimer);
		//LCD_DrawArrorLeft(5,3);

		break;
	case 55:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);
		LCD_DrawBitmap_Mono(110,25,bitmap_x4_course );
		TimerSetValue(&eTimer, 5000);
		TimerStart(&eTimer);
		break;

	case 21:
	case 22:
	case 23:
	case 24:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);

		LCD_DrawBitmap_Mono(60,10,bitmap_x4_bike );
		if(g_display==21)
		{
			LCD_DrawBitmap_Mono(20, 45, bitmap_x4_1);
			drawDistanceNumber(distance1);
		}
		else if(g_display==22)
		{
			LCD_DrawBitmap_Mono(20, 45, bitmap_x4_2);
			drawDistanceNumber(distance2);
		}
		else if(g_display==23)
		{
			LCD_DrawBitmap_Mono(20, 45, bitmap_x4_3);
			drawDistanceNumber(distance3);
		}
		else if(g_display==24)
		{
			LCD_DrawBitmap_Mono(20, 45, bitmap_x4_4);
			drawDistanceNumber(distance4);
		}



//		LCD_DrawBitmap_Mono(170, 35, bitmap_x4_0);

		LCD_DrawLine(100,20, 100, 80);
		break;
	case 31:
	case 32:
	case 33:

		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);
		Buzzer_ON_Control(BUZZER_ON);

		LCD_DrawBitmap_Mono(60,25,bitmap_x4_accident );
		if(display==31)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_flat_tire);
		else if(display==32)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_drop);
		else if(display==33)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_crash);


		if(message_target_rider==1)
		{

			LCD_DrawBitmap_Mono(20, 75, bitmap_x4_1);
		}
		else if (message_target_rider==2)
		{			LCD_DrawBitmap_Mono(20, 75, bitmap_x4_2);
		}
		else if(message_target_rider==3)
		{
			LCD_DrawBitmap_Mono(20, 75, bitmap_x4_3);

		}
		else
		{
			LCD_DrawBitmap_Mono(20, 75, bitmap_x4_4);

		}

		LCD_DrawLine(100,20, 100, 80);
		TimerSetValue(&mTimer, 10000);
		TimerStart(&mTimer);
		//VIBRATOR_ON_Control(VIBRATOR_ON);
		//LCD_DrawArrorLeft(5,3);

		break;
	case 34:
	case 35:
	case 36:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);
		Buzzer_ON_Control(BUZZER_ON);

		LCD_DrawBitmap_Mono(60,25,bitmap_x4_rest );
		if(display==34)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_rest_text);
		else if(display==35)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_meal);
		else if(display==36)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_waiting);

		LCD_DrawLine(100,20, 100, 80);
		TimerSetValue(&mTimer, 10000);
		TimerStart(&mTimer);
		//Vibrator_ON_Control(VIBRATOR_ON);

		//LCD_DrawArrorLeft(5,3);

		break;
	case 37:
	case 38:
	case 39:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_DrawRect(0,0, 255,95);
		Buzzer_ON_Control(BUZZER_ON);

		LCD_DrawBitmap_Mono(60,25,bitmap_x4_course );
		if(display==37)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_uphill);
		else if(display==38)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_downhill);
		else if(display==39)
			LCD_DrawBitmap_Mono(170, 25, bitmap_x4_curve);;
		LCD_DrawLine(100,20, 100, 80);
		TimerSetValue(&mTimer, 10000);
		TimerStart(&mTimer);
		//Vibrator_ON_Control(VIBRATOR_ON);
					//Buzzer_ON_Control(BUZZER_ON);
				//Buzzer_ON_Control(BUZZER_OFF);

		//LCD_DrawArrorLeft(5,3);

		break;
	case 1:
	//default:
		LCD_Clear(LCD_COLOR_BLACK);
		//LCD_DrawRect(0,0, 255,95);
		//LCD_SetTextColor(110);
		//LCD_SetBackColor(255);
//		LCD_DisplayStringAt(3, 1, (uint8_t *)"다른 자전거들의 거리", CENTER_MODE);
//		LCD_DisplayStringAt(3, 33, (uint8_t *)"(1) 0.1km (2) 3.5km (3) 5.0km", LEFT_MODE);
//		LCD_DisplayStringAt(3, 65, (uint8_t *)"(4) 10.1km (5) 3km (6) 1.0km ", LEFT_MODE);
//
//		LCD_DrawArrorLeft(5,3);
//		LCD_DrawArrorRight(243,3);
		//		LCD_DisplayStringAt(10, 32, (uint8_t *)"X0 : ", LEFT_MODE);
//		LCD_DisplayStringAt(10, 48, (uint8_t *)"Y0 : ", LEFT_MODE);
//		LCD_DisplayStringAt(10, 64, (uint8_t *)"X1 : ", LEFT_MODE);
//		LCD_DisplayStringAt(10, 80, (uint8_t *)"Y1 : ", LEFT_MODE);
		LCD_DrawBitmap_Mono(20,0,bitmap_x4_bike );
		LCD_DrawBitmap_Mono(80,0,bitmap_x4_bike );
		LCD_DrawBitmap_Mono(140,0,bitmap_x4_bike );
		LCD_DrawBitmap_Mono(200,0,bitmap_x4_bike );

		LCD_DrawBitmap_Mono(20, 75, bitmap_x4_1);
		LCD_DrawBitmap_Mono(80, 75, bitmap_x4_2);
		LCD_DrawBitmap_Mono(140, 75, bitmap_x4_3);
		LCD_DrawBitmap_Mono(200, 75, bitmap_x4_4);
		break;
	case 126:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_DrawBitmap_Mono(100,10,bitmap_x4_start );
		break;
	case 127:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_DrawBitmap_Mono(100,10,bitmap_x4_logo_2 );
		break;
	case 81: // NO LORA
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_DrawBitmap_Mono(100,10,bitmap_x4_nocon);
		TimerSetValue(&eTimer, 3000);
		TimerStart(&eTimer);
		break;
	case 82: // NO GPS
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_DrawBitmap_Mono(100,10,bitmap_x4_nogps);
		TimerSetValue(&eTimer, 3000);
		TimerStart(&eTimer);
		break;
	default:
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_DrawBitmap_Mono(100,10,bitmap_x4_logo_2 );
		break;
	}

	LCD_Update(NULL);
}

void LCD_TS_Display(ts_data_t * data)
{
	uint8_t direction;

	direction = TS_Direction(data);

	switch(direction)
	{
	case TOUCH_POINT:
		if(g_display == 127)
		{
			g_display = 126;
			LCD_Main_Display(g_display);
		}
		else if(g_display == 126 )
		{
			g_display = 1;
			LCD_Main_Display(g_display);
		}
		else if(g_display == 81)
		{
			TimerStop(&eTimer);
			g_display = 1;
			LCD_Main_Display(g_display);
		}
		else if(g_display == 82)
		{
			TimerStop(&eTimer);

			g_display = 1;
			LCD_Main_Display(g_display);
		}
		else if(g_display ==1)
		{
			if(data->x0 <60)
			{
				if(g_gps_valid)
				{
					g_display=24;
					LCD_Main_Display(g_display);
				}
				else
				{
					g_display = 82;
					LCD_Main_Display(g_display);
				}
			}
			else if(data->x0 <120)
			{
				if(g_gps_valid)
				{
					g_display=23;
					LCD_Main_Display(g_display);
				}
				else
				{
					g_display = 82;
					LCD_Main_Display(g_display);
				}
			}
			else if(data->x0 <180)
			{
				if(g_gps_valid)
				{
					g_display=22;
					LCD_Main_Display(g_display);
				}
				else
				{
					g_display = 82;
					LCD_Main_Display(g_display);
				}
			}
			else //if(data->x0 >=18060)
			{
				if(g_gps_valid)
				{
					g_display=21;
					LCD_Main_Display(g_display);
				}
				else
				{
					g_display = 82;
					LCD_Main_Display(g_display);
				}
			}
		}
		else if(g_display == 2)
		{
			if(data->x0 < 60)
			{
				g_display = 1;
				LCD_Main_Display(g_display);
			}
			else if(data->x0 >=60 && data->x0 <=130)
			{
				g_display = 3;
				LCD_Main_Display(g_display);

			}
			else if(data->x0>130 && data->x0<=190)
			{
				g_display=4;
				LCD_Main_Display(g_display);
			}
			else //if(data->x0 >190)
			{
				g_display = 5;
				LCD_Main_Display(g_display);

			}
		}
		else if(g_display == 3)
		{
			if(data->x0 < 60)
			{
				g_display = 2;
				LCD_Main_Display(g_display);
			}
			else if(data->x0 >=60 && data->x0 <=130)
			{//8

				char send_buffer[] ="013";
				DPRINTF(send_buffer);
				DPRINTF("\r\n");
				node_send(send_buffer, strlen(send_buffer));
				strcpy(g_lora_send_buffer, send_buffer);
				g_lora_status = 0;
				g_display = 48;
				LCD_Main_Display(g_display);

			}
			else if(data->x0>130 && data->x0<=190)
			{//7

				char send_buffer[] ="012";
				DPRINTF(send_buffer);
				DPRINTF("\r\n");
				node_send(send_buffer, strlen(send_buffer));
				strcpy(g_lora_send_buffer, send_buffer);
				g_lora_status = 0;
				g_display=47;
				LCD_Main_Display(g_display);
			}
			else //if(data->x0 >190)
			{//6



				char send_buffer[] ="011";
				DPRINTF(send_buffer);
				DPRINTF("\r\n");
				node_send(send_buffer, strlen(send_buffer));
				strcpy(g_lora_send_buffer, send_buffer);
				g_lora_status = 0;
				g_display = 46;
				LCD_Main_Display(g_display);

			}
		}
		else if(g_display == 4)
		{
			if(data->x0 < 60)
			{
				g_display =2;
				LCD_Main_Display(g_display);
			}
			else if(data->x0 >=60 && data->x0 <=130)
			{//11

				char send_buffer[] ="016";
				DPRINTF(send_buffer);
				DPRINTF("\r\n");
				node_send(send_buffer, strlen(send_buffer));
				strcpy(g_lora_send_buffer, send_buffer);
				g_lora_status = 0;
				g_display = 51;
				LCD_Main_Display(g_display);
			}
			else if(data->x0>130 && data->x0<=190)
			{//10

				char send_buffer[] ="015";
				DPRINTF(send_buffer);
				DPRINTF("\r\n");
				node_send(send_buffer, strlen(send_buffer));
				strcpy(g_lora_send_buffer, send_buffer);
				g_lora_status = 0;
				g_display=50;
				LCD_Main_Display(g_display);
			}
			else //if(data->x0 >190)
			{//9

				char send_buffer[] ="014";
				DPRINTF(send_buffer);
				DPRINTF("\r\n");
				node_send(send_buffer, strlen(send_buffer));
				strcpy(g_lora_send_buffer, send_buffer);
				g_lora_status = 0;
				g_display = 49;
				LCD_Main_Display(g_display);

			}
		}
		else if(g_display == 5)
		{
			if(data->x0 < 60)
			{
				g_display = 2;
				LCD_Main_Display(g_display);
			}
			else if(data->x0 >=60 && data->x0 <=130)
			{//14

				char send_buffer[] ="019";
				DPRINTF(send_buffer);
				DPRINTF("\r\n");
				node_send(send_buffer, strlen(send_buffer));
				strcpy(g_lora_send_buffer, send_buffer);
				g_lora_status = 0;

				g_display = 54;
				LCD_Main_Display(g_display);
			}
			else if(data->x0>130 && data->x0<=190)
			{//13

				char send_buffer[] ="018";
				DPRINTF(send_buffer);
				DPRINTF("\r\n");
				node_send(send_buffer, strlen(send_buffer));
				strcpy(g_lora_send_buffer, send_buffer);
				g_lora_status = 0;

				g_display=53;
				LCD_Main_Display(g_display);
			}
			else //if(data->x0 >190)
			{//12

				char send_buffer[] ="017";
				DPRINTF(send_buffer);
				DPRINTF("\r\n");
				node_send(send_buffer, strlen(send_buffer));
				strcpy(g_lora_send_buffer, send_buffer);
				g_lora_status = 0;

				g_display = 52;
				LCD_Main_Display(g_display);

			}
		}
		else if(g_display >=6 && g_display <=14)
		{
			g_display=1;
			LCD_Main_Display(g_display);
		}
		else if(g_display >=21 && g_display <=24)
		{
			g_display=1;
			LCD_Main_Display(g_display);
		}
		break;
	case TOUCH_LEFT:
//		g_display = g_display -1;
//		if(g_display<DISPLAY_MIN_PAGE)
//			g_display = DISPLAY_MIN_PAGE;
//		LCD_Main_Display(g_display);
		break;
	case TOUCH_RIGHT:
//		g_display = g_display +1;
//		if(g_display>DISPLAY_MAX_PAGE)
//			g_display = DISPLAY_MAX_PAGE;
//		LCD_Main_Display(g_display);
		break;


	case TOUCH_UP:
		if(g_display == 1 && global.lora_status == LORA_REGISTERED)
		{
			g_display = 2;
		}
		else if(g_display ==1 && global.lora_status != LORA_REGISTERED)
		{
			g_display = 81;
		}
		LCD_Main_Display(g_display);
		break;


	case TOUCH_DOWN:
		if(g_display == 1 && global.lora_status == LORA_REGISTERED)
		{
			g_display = 2;
		}
		else if(g_display ==1 && global.lora_status != LORA_REGISTERED)
		{
			g_display = 81;
		}
		else if(g_display ==2)
		{
			g_display = 1;
		}
		else if(g_display==3 || g_display==4 || g_display==5)
		{
			g_display=2;
		}

//		g_display = g_display -1;
//		if(g_display<DISPLAY_MIN_PAGE)
//			g_display = DISPLAY_MIN_PAGE;
		LCD_Main_Display(g_display);
		break;
	}
}

static uint8_t gps_count = 0;
void send_GPGGA_Display(uint8_t *data)
{
	int len;
	uint8_t gpgga[GPS_MESSAGE_MAX_SIZE] = {0};
	uint8_t buffer[30] = {0};

	if(1)//g_display == DISPLAY_GPS_STATE)
	{
		memcpy(gpgga, data, strlen((char *)data) - 2);

		len = strlen((char *)gpgga);

		//LCD_SetTextColor(LCD_COLOR_WHITE);
		//LCD_FillRect(2, 32, 253, 63);

		//LCD_SetTextColor(LCD_COLOR_BLACK);
		//LCD_SetBackColor(LCD_COLOR_WHITE);

		sprintf((char *)buffer, "count : %d", gps_count++);
		//LCD_DisplayStringAt(2, 32, buffer, LEFT_MODE);
		//DPRINTF(buffer);
		if(gps_count >= 30) {
			gps_count = 0;
		}
		else
		{
			return;
		}
		DPRINTF(gpgga);
		char send_buffer[] ="0037.498104:127.032776";
		DPRINTF(send_buffer);
		DPRINTF("\r\n");
		node_send(send_buffer, strlen(send_buffer));


//		if(len <= 30)
//		{
//			//LCD_DisplayStringAt(2, 48, gpgga, LEFT_MODE);
//			DPRINTF(gpgga);
//		}
//		else if(len > 30 && len <= 60)
//		{
//			memset(buffer, 0, 30);
//			memcpy(buffer, gpgga, 30);
//			DPRINTF(buffer);//LCD_DisplayStringAt(2, 48, buffer, LEFT_MODE);
//
//			memset(buffer, 0, 30);
//			memcpy(buffer, &gpgga[30], len - 30);
//			DPRINTF(buffer);//LCD_DisplayStringAt(2, 64, buffer, LEFT_MODE);
//		}
//		else if (len > 60)
//		{
//			memcpy(buffer, gpgga, 30);
//			DPRINTF(buffer);//LCD_DisplayStringAt(2, 48, buffer, LEFT_MODE);
//
//			memset(buffer, 0, 30);
//			memcpy(buffer, &gpgga[30], 30);
//			DPRINTF(buffer);//LCD_DisplayStringAt(2, 64, buffer, LEFT_MODE);
//
//			memset(buffer, 0, 30);
//			memcpy(buffer, &gpgga[60], len - 60);
//			DPRINTF(buffer);//LCD_DisplayStringAt(2, 80, buffer, LEFT_MODE);
//		}

		//LCD_Update(NULL);
	}

}
void LCD_GPGGA_Display(uint8_t *data)
{
	return;
	int len;
	uint8_t gpgga[GPS_MESSAGE_MAX_SIZE] = {0};
	uint8_t buffer[30] = {0};

	if(g_display == DISPLAY_GPS_STATE)
	{
		memcpy(gpgga, data, strlen((char *)data) - 2);

		len = strlen((char *)gpgga);

		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_FillRect(2, 32, 253, 63);

		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_SetBackColor(LCD_COLOR_WHITE);

		sprintf((char *)buffer, "count : %d", gps_count++);
		LCD_DisplayStringAt(2, 32, buffer, LEFT_MODE);

		if(gps_count >= 100) gps_count = 0;

		if(len <= 30)
		{
			LCD_DisplayStringAt(2, 48, gpgga, LEFT_MODE);
		}
		else if(len > 30 && len <= 60)
		{
			memset(buffer, 0, 30);
			memcpy(buffer, gpgga, 30);
			LCD_DisplayStringAt(2, 48, buffer, LEFT_MODE);

			memset(buffer, 0, 30);
			memcpy(buffer, &gpgga[30], len - 30);
			LCD_DisplayStringAt(2, 64, buffer, LEFT_MODE);
		}
		else if (len > 60)
		{
			memcpy(buffer, gpgga, 30);
			LCD_DisplayStringAt(2, 48, buffer, LEFT_MODE);

			memset(buffer, 0, 30);
			memcpy(buffer, &gpgga[30], 30);
			LCD_DisplayStringAt(2, 64, buffer, LEFT_MODE);

			memset(buffer, 0, 30);
			memcpy(buffer, &gpgga[60], len - 60);
			LCD_DisplayStringAt(2, 80, buffer, LEFT_MODE);
		}

		LCD_Update(NULL);
	}
}

static void touch_display(ts_data_t * data)
{
	uint8_t buffer[128] = {0};

	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_FillRect(10, 32, 245, 63);

	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_WHITE);

	sprintf((char *)buffer, "X0 : %d", data->x0);
	LCD_DisplayStringAt(10, 32, buffer, LEFT_MODE);

	memset(buffer, 0, 128);
	sprintf((char *)buffer, "Y0 : %d", data->y0);
	LCD_DisplayStringAt(10, 48, buffer, LEFT_MODE);

	memset(buffer, 0, 128);
	sprintf((char *)buffer, "X1 : %d", data->x1);
	LCD_DisplayStringAt(10, 64, buffer, LEFT_MODE);

	memset(buffer, 0, 128);
	sprintf((char *)buffer, "Y1 : %d", data->y1);
	LCD_DisplayStringAt(10, 80, buffer, LEFT_MODE);

	LCD_Update(NULL);
}


void LCD_BAT_LEVLE_Display(void)
{
	uint8_t buf[30] = {0};

	if(0)//(g_display == DISPLAY_BATTERY_STATE)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_FillRect(0, 32, 256, 16);

		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_SetBackColor(LCD_COLOR_WHITE);

		sprintf((char *)buf, "LEVEL : %d.%02d V", global.battery_level/100, global.battery_level%100);
		LCD_DisplayStringAt(10, 32, buf, LEFT_MODE);

		LCD_Update(NULL);
	}
}

void LCD_CHARGING_Display(void)
{
	if(0)//(g_display == DISPLAY_BATTERY_STATE)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_FillRect(0, 48, 256, 16);

		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_SetBackColor(LCD_COLOR_WHITE);

		if(global.charging_status == NO_CHARGING)
			LCD_DisplayStringAt(10, 48, (uint8_t *)"NO CHARGING", LEFT_MODE);
		else if(global.charging_status == CHARGING)
			LCD_DisplayStringAt(10, 48, (uint8_t *)"CHARGING", LEFT_MODE);
		else
			LCD_DisplayStringAt(10, 48, (uint8_t *)"FULL CHARGING", LEFT_MODE);

		LCD_Update(NULL);
	}
}

void LCD_LoRa_Display(void)
{
	LCD_DrawBitmap_Mono(10, 10, bitmap_antena);
	LCD_Update(NULL);
	if(0)//(g_display == DISPLAY_LORA_STATE)
	{
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_FillRect(0, 48, 256, 16);

		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_SetBackColor(LCD_COLOR_WHITE);

		LCD_DisplayStringAt(10, 48,  (global.lora_status == LORA_REGISTERED) ? (uint8_t *)"Joined" : (uint8_t *)"Not Joined", LEFT_MODE);

		LCD_Update(NULL);
	}
}


uint8_t g_gprmc[GPS_MESSAGE_MAX_SIZE] = {0};
void LCD_GPRMC_Display(uint8_t *data)
{
	int len;
	uint8_t gprmc[GPS_MESSAGE_MAX_SIZE] = {0};
	uint8_t buffer[30] = {0};
	memcpy(gprmc, data, strlen((char *)data) - 2);
	memcpy(g_gprmc, data, strlen((char *)data) - 2);
	//DPRINTF("GGGGGGGG\n\r%s\n\r", gprmc);

	if(0)//(g_display == DISPLAY_GPS_STATE)
	{
		memcpy(gprmc, data, strlen((char *)data) - 2);
		//DPRINTF("GGGGGGGG\n\r%s\n\r", gprmc);

		len = strlen((char *)gprmc);

		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_FillRect(2, 32, 253, 63);

		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_SetBackColor(LCD_COLOR_WHITE);

		sprintf((char *)buffer, "count : %d",  global.gps_time_count);
		LCD_DisplayStringAt(2, 32, buffer, LEFT_MODE);

		if(len <= 30)
		{
			LCD_DisplayStringAt(2, 48, gprmc, LEFT_MODE);
		}
		else if(len > 30 && len <= 60)
		{
			memset(buffer, 0, 30);
			memcpy(buffer, gprmc, 30);
			LCD_DisplayStringAt(2, 48, buffer, LEFT_MODE);

			memset(buffer, 0, 30);
			memcpy(buffer, &gprmc[30], len - 30);
			LCD_DisplayStringAt(2, 64, buffer, LEFT_MODE);
		}
		else if (len > 60)
		{
			memcpy(buffer, gprmc, 30);
			LCD_DisplayStringAt(2, 48, buffer, LEFT_MODE);

			memset(buffer, 0, 30);
			memcpy(buffer, &gprmc[30], 30);
			LCD_DisplayStringAt(2, 64, buffer, LEFT_MODE);

			memset(buffer, 0, 30);
			memcpy(buffer, &gprmc[60], len - 60);
			LCD_DisplayStringAt(2, 80, buffer, LEFT_MODE);
		}

		LCD_Update(NULL);
	}
}

