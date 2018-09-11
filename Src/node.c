/*
 * node.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "event.h"
#include "config.h"
#include "low_power.h"
#include "lora.h"
#include "timeServer.h"
#include "version.h"
#include "node.h"
#include "display.h"
#include "lcd_interface.h"
#include "bitmaps.h"
extern  int8_t g_display;
extern int message_target_rider;
extern int message_number;

TimerEvent_t lTimer;
char g_lora_send_buffer[255];
int g_lora_status = 0; /* 0: initialized, 1: Failed, 2: Success */

int distance1 = 0;
int distance2 = 0;
int distance3 = 0;
int distance4 = 0;
int distance5 = 0;
/*!
 * Number of trials for the join request.
 */
#define JOINREQ_NBTRIALS	3

static void    LoraTxData(lora_AppData_t *AppData, FunctionalState* IsTxConfirmed);
static void    LoraRxData(lora_AppData_t *AppData);
static void    LoraSetState(uint8_t State);
static void    LoraDevTime(uint32_t Time);
static void    LoraLinkCheck(uint8_t Gateways);
static void    LoraTxDone(bool bSuccess);
static void    LoraJoined(void);
static void    LoraCertified(uint8_t *key);
static void    LoraTxTimeout(void);
static int8_t  LoraGetChPower(uint8_t Channel);
static uint8_t LoraGetCfmTrials(void);

/* load call backs*/
static LoRaMainCallback_t LoRaMainCallbacks;

/* !
 * Initialise the Lora Parameters
 */
static  LoRaParam_t LoRaParamInit;

void node_init(void)
{
	if(memzero(config.DevEui, 8))
	{
		DBG_PRINTF("DevEui is Null\r\n");
	    return;
	}

	DBG_PRINTF("\r\nNODE INIT START\r\n");
	DBG_PRINTF("LoRa Class %s\r\n", !config.Class ? "A" : "C");

	memcpy1(LoRaParamInit.DevEui, config.DevEui, 8);
	memcpy1(LoRaParamInit.AppEui, config.AppEui, 8);

	LoRaParamInit.IsCertified = !memzero(config.RealAppKey, 16);
	if(!LoRaParamInit.IsCertified)
	{
		DBG_PRINTF("PSUEDO Join-request\r\n");
		memcpy1(LoRaParamInit.AppKey, config.AppKey, 16);
	}
	else
	{
		DBG_PRINTF("REAL Join-request\r\n");
		memcpy1(LoRaParamInit.AppKey, config.RealAppKey, 16);
	}

	// Initialize the Lora Parameters

//	LoRaParamInit.IsCertified = !memzero(config.RealAppKey, 16);
//	memcpy1(LoRaParamInit.AppKey, LoRaParamInit.IsCertified ? config.RealAppKey : config.AppKey, 16);
	LoRaParamInit.TxPower = config.TxPower;
  	for(int i=0; i<8; i++) LoRaParamInit.ChPower[i] = config.ChPower[i];
	LoRaParamInit.TxConfirmed = config.TxConfirmed;
	LoRaParamInit.DebugEnable = config.DebugEnable;
	LoRaParamInit.CfmNbTrials = config.CfmMsgTrials;
	LoRaParamInit.UncfmNbTrials = config.UncfmMsgTrials;
	LoRaParamInit.AppPort = config.AppPort;
	LoRaParamInit.TxEvent = config.TxEvent;
	LoRaParamInit.TxDutyCycleTime = config.TxDutyCycleTime*1000;
	LoRaParamInit.Class = config.Class;
	LoRaParamInit.AdrEnable = config.AdrEnable;
	LoRaParamInit.TxDatarate = config.TxDatarate;
	LoRaParamInit.EnablePublicNetwork = config.PublicNetwork;
	LoRaParamInit.JoinNbTrials = JOINREQ_NBTRIALS;

	// Initialize callbacks
	LoRaMainCallbacks.BoardGetBatteryLevel = BoardGetBatteryLevel;
	LoRaMainCallbacks.BoardGetUniqueId = BoardGetUniqueId;
	LoRaMainCallbacks.BoardGetRandomSeed = BoardGetRandomSeed;
	LoRaMainCallbacks.LoraTxData = LoraTxData;
	LoRaMainCallbacks.LoraRxData = LoraRxData;
	LoRaMainCallbacks.LoraSetState = LoraSetState;
  	LoRaMainCallbacks.LoraTxDone = LoraTxDone;
	LoRaMainCallbacks.LoraDevTime = LoraDevTime;
	LoRaMainCallbacks.LoraLinkCheck = LoraLinkCheck;
	LoRaMainCallbacks.LoraJoined = LoraJoined;
	LoRaMainCallbacks.LoraCertified = LoraCertified;
	LoRaMainCallbacks.LoraTxTimeout = LoraTxTimeout;
	LoRaMainCallbacks.LoraGetChPower = LoraGetChPower;
	LoRaMainCallbacks.LoraGetCfmTrials = LoraGetCfmTrials;
  
	HW_Init();

	/* Configure the Lora Stack*/
	lora_init(&LoRaMainCallbacks, &LoRaParamInit);

	global.lora_status = LORA_INITIALIZED;
}

bool node_process(uint8_t state)
{
	return lora_fsm( state );
}

static void LoraTxData(lora_AppData_t *AppData, FunctionalState* IsTxConfirmed)
{
	/* LoRa Txmode Event Data */
	AppData->Port = LoRaParamInit.AppPort;
	*IsTxConfirmed = LoRaParamInit.TxConfirmed;
	AppData->NbTrials = LoRaParamInit.CfmNbTrials;
	memset(AppData->Buff, 0, 16);
	AppData->BuffSize = 16;

	global.number++;
	AppData->Buff[7] = global.number & 0xff;

	DBG_PRINTF("TX DATA[%d]:", AppData->BuffSize);
	for(int i=0; i<AppData->BuffSize; i++) DBG_PRINTF(" %02X", AppData->Buff[i]); DBG_PRINTF("\r\n");

	//AppData->Port = LoRaParamInit.AppPort;
	//AppData->NbTrials = LoRaParamInit.CfmNbTrials;
	//*IsTxConfirmed = LoRaParamInit.TxConfirmed;
	//sprintf((char *)AppData->Buff, "%08x", (unsigned int)global.number++);
	//AppData->BuffSize = 8;
}

static void LoraRxData(lora_AppData_t *AppData)
{
	int i;
//	DBG_PRINTF("\r\n######### LoraRxData!!");

	if(AppData->Buff[1] == 0x00 && AppData->Port == 0xDE)
	{
//		DBG_PRINTF("\r\nExtDevMgmt : ");
//		for(i=0; i<AppData->BuffSize; i++)
//		{
//			DBG_PRINTF("%02x", AppData->Buff[i]);
//		}
//		DBG_PRINTF("\r\n");
		if(AppData->Buff[3] =='1' && AppData->Buff[4] == '1')//AppData->Buff[5] == '1' && AppData->Buff[7]=='1')
		{
			char buffer[256];
			Buzzer_ON_Control(BUZZER_ON);

			sprintf(buffer, "%c\n", AppData->Buff[5]);
			message_target_rider = atoi(buffer);

			sprintf(buffer, "%c\n", AppData->Buff[7]);

			int message_number = atoi(buffer);
			g_display = 30+message_number;
			LCD_Main_Display(g_display);
		}
		else if(AppData->Buff[3] == '1' && AppData->Buff[4] == '2')
		{
//			char buffer[256];
//			sprintf(buffer, "%s\n", AppData->Buff[5]);
//			DBG_PRINTF("\r\n** DISTANCE DATA : %s", buffer);
		}
		else if(AppData->Buff[3] =='1' && AppData->Buff[4] == '0')//AppData->Buff[5] == '1' && AppData->Buff[7]=='1')
		{
			char buffer[255];
			DPRINTF("GPS DIST INFO  RCVD..\n\r");
//			sprintf(buffer, "%s\n", AppData->Buff[5]);
			DPRINTF("%x %x %x %x %x\n", AppData->Buff[5], AppData->Buff[6], AppData->Buff[7], AppData->Buff[8], AppData->Buff[9]);
			//DPRINTF("[ %s ]\n\r", buffer);

			memset(buffer, 0, 255);
			sprintf(buffer, "%c", AppData->Buff[5]);
			int me = atoi(buffer);

			memset(buffer, 0, 255);
			sprintf(buffer, "%c", AppData->Buff[7]);
			int you = atoi(buffer);

			memset(buffer, 0, 255);
			if(AppData->Buff[9] == '0')
			{
				sprintf(buffer, "%c", AppData->Buff[9]);
			}
			else
			{
				sprintf(buffer, "%c%c", AppData->Buff[9], AppData->Buff[10]);
			}
			int distance = atoi(buffer);


			//sprintf(buffer, "%c%c", AppData->Buff[9], AppData->Buff[10]);
			//int distance = AppData->Buff[9];

			DPRINTF("########## me is %d, you are %d, distance is %d\n\r", me, you, distance);



			if(me < you)
			{
				you = you - 1;
			}

			if(you == 1)
				distance1 = distance;
			else if(you==2)
				distance2 = distance;
			else if (you==3)
				distance3 = distance;
			else if(you==4)
				distance4 = distance;
//			char buffer[256];
//			Buzzer_ON_Control(BUZZER_ON);
//
//			sprintf(buffer, "%c\n", AppData->Buff[5]);
//			message_target_rider = atoi(buffer);
//
//			sprintf(buffer, "%c\n", AppData->Buff[7]);
//
//			int message_number = atoi(buffer);
//			g_display = 30+message_number;
//			LCD_Main_Display(g_display);
		}
	}
	else //if(AppData->Buff[0] == '0' && AppData->Buff[1] == '1')
	{
//		int i;
//
//		DBG_PRINTF("\r\nJUST Message : ");
//		for(i=0; i<AppData->BuffSize; i++)
//		{
//			DBG_PRINTF("%02x", AppData->Buff[i]);
//		}
//		DBG_PRINTF("\r\n");

	}
}

static void LoraSetState(uint8_t State)
{
	event_put(EVENT_LORA_NODE, State, NULL);
}

static void LoraDevTime(uint32_t Time)
{
	//1455287071 -  1139322288 = 315964783
	Time += 315964783; 	// GPS epoch -> LINUX epoch
	Time += 32400; 		// +9:00

	DBG_PRINTF("DEVTIME %s\r\n",  date2str(Time));
}

static void LoraLinkCheck( uint8_t Gateways )
{
	DBG_PRINTF("LINKCHECK %u\r\n", Gateways);
}

static void LoraJoined(void)
{
	DBG_PRINTF("JOINED\r\n");
	global.lora_status = LORA_REGISTERED;
	event_put(EVENT_LORA_JOINED, 0, NULL);
	LCD_DrawBitmap_Mono(10, 10, bitmap_antena);
	LCD_Update(NULL);
}



static void LoraTxDone(bool bSuccess)
{
	if(LoRaParamInit.DebugEnable)
	{
		return;
	}

	if(bSuccess)
	{
		g_lora_status = 2;
	  global.lora_success += 1;
	  DBG_PRINTF("TX_SUCCESS [%u,%u]\r\n", global.lora_success, global.lora_failure);
	}
	else
	{
		g_lora_status = 1;
		global.lora_failure += 1;
	  DBG_PRINTF("TX_FAILURE [%u,%u]\r\n", global.lora_success, global.lora_failure);
	  int random_delay = randr(0,5000);
		TimerSetValue(&lTimer, 10000+random_delay);
		TimerStart(&lTimer);
		DBG_PRINTF("RETRY IN %d msec\n\r", 10000+random_delay);

	}

	if(LoRaParamInit.TxEvent == TX_ON_TIMER )
	{
	  lora_ontimer( LoRaParamInit.TxDutyCycleTime );
	}
}

static void LoraCertified(uint8_t* key)
{
	DBG_PRINTF("CERTIFIED\r\n");

	memcpy1(config.RealAppKey, key, 16);
	CONFIG_WRITE(config.RealAppKey, 16);
}

static void LoraTxTimeout( void )
{
	DBG_PRINTF("TX_TIMEOUT\r\n");

	if(LoRaParamInit.TxEvent == TX_ON_TIMER )
	{
	  lora_ontimer( LoRaParamInit.TxDutyCycleTime );
	}
}

static int8_t LoraGetChPower(uint8_t Channel)
{
	return LoRaParamInit.ChPower[Channel];
}

static uint8_t LoraGetCfmTrials(void)
{
	return config.CfmMsgTrials;
}

bool node_send(uint8_t *data, uint8_t size)
{
	if(global.lora_status >= LORA_REGISTERED && (size > 0 && size <= 65))
	{
		return lora_send(data, size, LoRaParamInit.AppPort, LoRaParamInit.TxConfirmed, LoRaParamInit.CfmNbTrials);
	}

	return false;
}

bool node_linkcheck(void)
{
	if(global.lora_status >= LORA_REGISTERED)
	{
		return lora_command(MLME_LINK_CHECK);
	}

	return false;
}

bool node_devtime(void)
{
	if(global.lora_status >= LORA_REGISTERED)
	{
		return lora_command(MLME_DEV_TIME);
	}

	return false;
}

bool node_get(uint8_t type, void *data)
{
	bool ret = true;

	if(global.lora_status >= LORA_INITIALIZED)
	{
		switch(type)
		{
		case NODE_NETWORK_JOINED:
			ret = lora_getMib(MIB_NETWORK_JOINED, data);
			break;
		case NODE_ADR:
			ret = lora_getMib(MIB_ADR, data);
			break;
		case NODE_CHANNELS_NB_REP:
			ret = lora_getMib(MIB_CHANNELS_NB_REP, data);
			break;
		case NODE_CHANNELS_DATARATE:
			ret = lora_getMib(MIB_CHANNELS_DATARATE, data);
			break;
		case NODE_CHANNELS_TX_POWER:
			ret = lora_getMib(MIB_CHANNELS_TX_POWER, data);
			break;
		case NODE_TX_MODE:
			*(bool *)data = LoRaParamInit.TxEvent;
			break;
		case NODE_TX_CONFIRMED:
			*(bool *)data = LoRaParamInit.TxConfirmed;
			break;
		case NODE_CFM_REPETITIONS:
			*(uint8_t *)data = LoRaParamInit.CfmNbTrials;
			break;
		case NODE_RX_RSSI:
			ret = lora_getMib(MIB_RX_RSSI, data);
			break;
		case NODE_RX_SNR:
			ret = lora_getMib(MIB_RX_SNR, data);
			break;
		case NODE_TX_RUNNING:
			ret = lora_getMib(MIB_TX_RUNNING, data);
			break;
		case NODE_UPLINK_COUNTER:
			ret = lora_getMib(MIB_UPLINK_COUNTER, data);
			break;
		case NODE_DOWNLINK_COUNTER:
			ret = lora_getMib(MIB_DOWNLINK_COUNTER, data);
			break;
		case NODE_APP_PORT:
			*(uint8_t *)data = LoRaParamInit.AppPort;
			break;
		case NODE_TX_PERIOD:
			*(uint32_t *)data = LoRaParamInit.TxDutyCycleTime/1000;
			break;
		case NODE_CHANNEL_POWER:
		{
			int8_t *tmp = (int8_t *)data;
			tmp[1] = LoRaParamInit.ChPower[tmp[0]];
			break;
		}
		default:
			ret = false;
			break;
		}
	}

	return ret;
}

bool node_set(uint8_t type, void *data)
{
	bool ret = true;

	if(global.lora_status >= LORA_INITIALIZED)
	{
		switch(type)
		{
		case NODE_ADR:
			ret = lora_setMib(MIB_ADR, data);
			break;
		case NODE_CHANNELS_NB_REP:
			ret = lora_setMib(MIB_CHANNELS_NB_REP, data);
			break;
		case NODE_CHANNELS_DATARATE:
			ret = lora_setMib(MIB_CHANNELS_DATARATE, data);
			break;
		case NODE_CHANNELS_TX_POWER:
			ret = lora_setMib(MIB_CHANNELS_TX_POWER, data);
			break;
		case NODE_TX_MODE:
			LoRaParamInit.TxEvent = *(bool *)data;
			ret = lora_ontimer(3000);
			break;
		case NODE_TX_CONFIRMED:
			LoRaParamInit.TxConfirmed = *(bool *)data;
			break;
		case NODE_CFM_REPETITIONS:
			if(*(uint8_t *)data >= 1 && *(uint8_t *)data <= 8)
			{
				LoRaParamInit.CfmNbTrials = *(uint8_t *)data;
			}
			else
			{
				ret = false;
			}
			break;
		case NODE_APP_PORT:
			if(*(uint8_t *)data >= 1 && *(uint8_t *)data <= 0xdd)
			{
				LoRaParamInit.AppPort = *(uint8_t *)data;
			}
			else
			{
				ret = false;
			}
			break;
		case NODE_UPLINK_COUNTER:
			ret = lora_setMib(MIB_UPLINK_COUNTER, data);
			break;
		case NODE_DOWNLINK_COUNTER:
			ret = lora_setMib(MIB_DOWNLINK_COUNTER, data);
			break;
		case NODE_TX_PERIOD:
			LoRaParamInit.TxDutyCycleTime = *(uint32_t *)data * 1000;
			break;
		case NODE_CHANNEL_POWER:
		{
			int8_t *tmp = (int8_t *)data;
			LoRaParamInit.ChPower[tmp[0]] = tmp[1];
			break;
		}
		default:
			ret = false;
			break;
		}
	}

	return ret;
}
