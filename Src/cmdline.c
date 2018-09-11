/*
 * cmdline.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "node.h"
#include "lcd_interface.h"

/******************************************************************************
 * @PROCESS
 *****************************************************************************/
#define CMDLINE_MAX_ARGS        8
static char *g_ppcArgv[CMDLINE_MAX_ARGS + 1];
static bool bAT = false;
static bool bSET = false;

#define CMDLINE_ID_ARGS			"ioncom"
#define CMDLINE_PASSWORD_ARGS 	"gpager10"
static char g_tmpID[128];
//static char g_tmpPASS[128];

int CmdLineProcess(char *pcCmdLine)
{
	char *pcChar;
	uint_fast8_t ui8Argc;
	bool bFindArg = true;
	tCmdLineEntry *psCmdEntry;
	tCmdInvisibleEntry *psCmdInvisibleEntry;

	// Initialize the argument counter, and point to the beginning of the
	// command line string.
	ui8Argc = 0;
	pcChar = pcCmdLine;

	// Advance through the command line until a zero character is found.
	while(*pcChar)
	{
		// If there is a space, then replace it with a zero, and set the flag
		// to search for the next argument.
		if(*pcChar == ' ')
		{
			*pcChar = 0;
			bFindArg = true;
		}

		// Otherwise it is not a space, so it must be a character that is part
		// of an argument.
		else
		{
			// If bFindArg is set, then that means we are looking for the start
			// of the next argument.
			if(bFindArg)
			{
				// As long as the maximum number of arguments has not been
				// reached, then save the pointer to the start of this new arg
				// in the argv array, and increment the count of args, argc.
				if(ui8Argc < CMDLINE_MAX_ARGS)
				{
					g_ppcArgv[ui8Argc] = pcChar;
					ui8Argc++;
					bFindArg = false;
				}

				// The maximum number of arguments has been reached so return the error.
				else
				{
					return(CMDLINE_TOO_MANY_ARGS);
				}
			}
		}

		// Advance to the next character in the command line.
		pcChar++;
	}

	// If one or more arguments was found, then process the command.
	if(global.account == ID_MODE)
	{
		if(ui8Argc)
		{
			int id_count = strlen(g_ppcArgv[0]);

			if(id_count < 128)
			{
				memset(g_tmpID, 0, 128);
				memcpy(g_tmpID, g_ppcArgv[0], id_count);
				global.account = PASSWORD_MODE;
				return(CMDLINE_ACCOUNT_PASSWORD);
			}
			else
			{
				return(CMDLINE_TOO_MANY_ARGS);
			}
		}

		return(CMDLINE_ACCOUNT_ID);
	}
	else if(global.account == PASSWORD_MODE)
	{
		if(ui8Argc)
		{
			int pass_count = strlen(g_ppcArgv[0]);

			if(pass_count < 128)
			{
				char pwdbuffer[128];
				memset(pwdbuffer, 0, 128);
				//uprintf("WHAT? %02X %02X %s\n\r", config.DevEui[6], config.DevEui[7], config.DevEui);
				sprintf(pwdbuffer, "gpager%02X%02X", config.DevEui[6], config.DevEui[7]);
				//uprintf("ui? %s\n\r", pwdbuffer);
				//if(!strcmp(CMDLINE_ID_ARGS, g_tmpID) && !strcmp(CMDLINE_PASSWORD_ARGS, g_ppcArgv[0]))
				if(!strcmp(CMDLINE_ID_ARGS, g_tmpID) && !strcmp(pwdbuffer, g_ppcArgv[0]))

				{
					global.account = ACCOUNT_MODE;
					return(CMDLINE_OK);
				}
				else
				{
					global.account = ID_MODE;
					return(CMDLINE_ERROR);
				}
			}
			else
			{
				global.account = ID_MODE;
				return(CMDLINE_ERROR);
			}
		}

		return(CMDLINE_ACCOUNT_PASSWORD);
	}
	else if(ui8Argc && (global.account == ACCOUNT_MODE))
	{
		// Start at the beginning of the command table, to look for a matching command.
		psCmdEntry = &g_psCmdTable[0];
		psCmdInvisibleEntry = &g_psCmdInvisibleTable[0];

		strlwr(g_ppcArgv[0]);
		bAT = (strncmp(g_ppcArgv[0], "at+", 3) == 0);
		char *argv = bAT ? &g_ppcArgv[0][3] : g_ppcArgv[0];

		// Search through the command table until a null command string is
		// found, which marks the end of the table.
		while(psCmdEntry->pcCmd)
		{
			// If this command entry command string matches argv[0], then call
			// the function for this command, passing the command line arguments.
			if((!strcmp(argv, psCmdEntry->pcCmd)) /* && (global.account == ACCOUNT_MODE) */)
			{
				return(psCmdEntry->pfnCmd(ui8Argc, g_ppcArgv));
			}

			// Not found, so advance to the next entry.
			psCmdEntry++;
		}

		bSET = (strncmp(g_ppcArgv[0], "set+", 4) == 0);
		char *argv1 = bSET ? &g_ppcArgv[0][4] : g_ppcArgv[0];

		while(psCmdInvisibleEntry->pcCmd)
		{
			if((!strcmp(argv1, psCmdInvisibleEntry->pcCmd)) /* && (global.account == ACCOUNT_MODE) */)
			{
				return(psCmdInvisibleEntry->pfnCmd(ui8Argc, g_ppcArgv));
			}

			psCmdInvisibleEntry++;
		}
	}

	// Fall through to here means that no matching command was found, so return an error.
	return(CMDLINE_BAD_CMD);
}

/******************************************************************************
 * @COMMAND
 *****************************************************************************/
#define CMD_BUF_SIZE      128
static char g_cCmdBuf[CMD_BUF_SIZE];

int Cmd_help(int argc, char *argv[])
{
	tCmdLineEntry *psEntry;

	uprintf("\nAvailable commands\r\n");
	uprintf("------------------\r\n");

	psEntry = &g_psCmdTable[0];

	while(psEntry->pcCmd != NULL)
	{
    	uprintf("%-12s: %s\r\n", psEntry->pcCmd, psEntry->pcHelp);
		psEntry++;
	}

	return (CMDLINE_OK);
}

int Cmd_ver(int argc, char *argv[])
{
	uprintf("%.4s\r\n", global.version.s);

	return (CMDLINE_OK);
}

int Cmd_reset(int argc, char *argv[])
{
	uprintf("OK\r\n");
	DelayMs(500);
	_RESET();

	return (CMDLINE_OK);
}

int Cmd_gps(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 2)
	{
		unsigned int d;

		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 1) )
		{
			GPS_ON_Control((uint8_t)d);
				ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_gps_log(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 2)
	{
		unsigned int d;

		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 1) )
		{
			global.gps_log = (uint8_t)d;
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_gps_event(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 2)
	{
		unsigned int d;

		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0) )
		{
			GPSTimer_Stop();

			ret = true;
		}
	}
	else if(argc == 3)
	{
		unsigned int d, p;

		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 1) && (sscanf(argv[2], "%u", &p) == 1) && (p >= 5 && p <= 20) )
		{
			GPSTimer_Start((uint8_t)p);

			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_backlight(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 2)
	{
		unsigned int d;

		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 1) )
		{
			LCD_Backlight_OnOff((int)d);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_vibrator(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 2)
	{
		unsigned int d;

		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 1) )
		{
			Vibrator_ON_Control((uint8_t)d);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_buzzer(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 2)
	{
		unsigned int d;

		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 1) )
		{
			Buzzer_ON_Control((uint8_t)d);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_pmic_read(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 2)
	{
		unsigned int d;

		if( (sscanf(argv[1], "%x", &d) == 1) && (d <= 0xff) )
		{
			uprintf("0x%02x = 0x%02x\r\n", d, PMIC_Read((uint8_t)d));
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_pmic_write(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 3)
	{
		unsigned int d, p;

		if( (sscanf(argv[1], "%x", &d) == 1) && (d <= 0xff) && (sscanf(argv[2], "%x", &p) == 1) && (p <= 0xff) )
		{
			PMIC_Write((uint8_t)d, (uint8_t)p);
			DelayMs(20);
			uprintf("0x%02x = 0x%02x\r\n", d, PMIC_Read((uint8_t)d));
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_ble(int argc, char *argv[])
{
	bool ret = false;
	char buffer[128] = {0};

	if(argc == 2)
	{
		if(strcmp(argv[1], "$$$") == 0)
		{
			ble_put(argv[1], 3);
			ret = true;
		}
		else
		{
			sprintf(buffer, "%s\r\n", argv[1]);
			ble_put(buffer, strlen(buffer));
			ret = true;
		}

	}

	return (ret ? CMDLINE_BLE_OK : CMDLINE_ERROR);
}

void Encrypt_Lora_Key(void)
{
	config_t enconfig;

	EEPROM_Read(EEPROM_CONFIG_ADDR, (uint8_t *)&enconfig, sizeof(config_t));

	uprintf("%-16s ", "EnDevEui"); for(int i=0; i<16; i++) uprintf("%02X", enconfig.DevEui[i]); uprintf("\r\n");
	uprintf("%-16s ", "EnAppEui"); for(int i=0; i<16; i++) uprintf("%02X", enconfig.AppEui[i]); uprintf("\r\n");
	uprintf("%-16s ", "EnAppKey"); for(int i=0; i<16; i++) uprintf("%02X", enconfig.AppKey[i]); uprintf("\r\n");
	uprintf("%-16s ", "EnRealAppKey"); for(int i=0; i<16; i++) uprintf("%02X", enconfig.RealAppKey[i]); uprintf("\r\n");
}

int Cmd_lora(int argc, char *argv[])
{
	uprintf("%-16s ", "DevEui"); for(int i=0; i<8; i++) uprintf("%02X", config.DevEui[i]); uprintf("\r\n");
	uprintf("%-16s ", "AppEui"); for(int i=0; i<8; i++) uprintf("%02X", config.AppEui[i]); uprintf("\r\n");
	uprintf("%-16s ", "AppKey"); for(int i=0; i<16; i++) uprintf("%02X", config.AppKey[i]); uprintf("\r\n");
//	uprintf("%-16s ", "Debug");  uprintf("%d\r\n", config.DebugEnable);
	uprintf("%-16s ", "Class");  uprintf("%d\r\n", config.Class);
	uprintf("%-16s ", "Public");  uprintf("%d\r\n", config.PublicNetwork);
	uprintf("%-16s ", "AppPort");  uprintf("%d\r\n", config.AppPort);
	uprintf("%-16s ", "ADR");  uprintf("%d\r\n", config.AdrEnable);
	uprintf("%-16s ", "TxPower");  uprintf("%d\r\n", config.TxPower);
	uprintf("%-16s ", "ChPower");  for(int i=0; i<8; i++) uprintf("%d ", config.ChPower[i]); uprintf("\r\n");
	uprintf("%-16s ", "Datarate");  uprintf("%d\r\n", config.TxDatarate);
	uprintf("%-16s ", "TxCfm");  uprintf("%d\r\n", config.TxConfirmed);
	uprintf("%-16s ", "CREP");  uprintf("%d\r\n", config.CfmMsgTrials);
	uprintf("%-16s ", "UREP");  uprintf("%d\r\n", config.UncfmMsgTrials);
	uprintf("%-16s ", "TxMode");  uprintf("%d\r\n", config.TxEvent);
	uprintf("%-16s ", "DutyCycle");  uprintf("%d\r\n", config.TxDutyCycleTime);

#ifdef CONFIG_ENCRYT_ENABLE
	Encrypt_Lora_Key();
#endif

	return (CMDLINE_OK);
}

int Cmd_rst(int argc, char *argv[])
{
	bool ret = false;
	if(bAT)
	{
		event_put(EVENT_LORA_NODE, 0, NULL);
		ret = true;
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

bool STR2HEX(char *src, uint8_t *dst, uint8_t len)
{
	for(int i=0; i<len; i++)
	{
		uint8_t upper, lower;

		if(src[i*2] >= '0' && src[i*2] <= '9') upper = src[i*2] - '0';
		else if(src[i*2] >= 'a' && src[i*2] <= 'f') upper = src[i*2] - 'a' + 10;
		else if(src[i*2] >= 'A' && src[i*2] <= 'F') upper = src[i*2] - 'A' + 10;
		else return false;

		if(src[i*2+1] >= '0' && src[i*2+1] <= '9') lower = src[i*2+1] - '0';
		else if(src[i*2+1] >= 'a' && src[i*2+1] <= 'f') lower = src[i*2+1] - 'a' + 10;
		else if(src[i*2+1] >= 'A' && src[i*2+1] <= 'F') lower = src[i*2+1] - 'A' + 10;
		else return false;

		dst[i] = (upper << 4) + lower;
	}

	return true;
}

int Cmd_deveui(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		for(int i=0; i<8; i++)
		{
			uprintf("%02X", config.DevEui[i]);
		}
		uprintf("\r\n");
		ret = true;
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_appeui(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		for(int i=0; i<8; i++)
		{
			uprintf("%02X", config.AppEui[i]);
		}
		uprintf("\r\n");
		ret = true;
	}
	else if(argc == 2)
	{
		uint8_t tmp[8];
		if(strlen(argv[1]) == 16 && STR2HEX(argv[1], tmp, 8) == true)
		{
			memcpy(config.AppEui, tmp, 8);
			CONFIG_WRITE(config.AppEui, 8);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_appkey(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		for(int i=0; i<16; i++)
		{
			uprintf("%02X", config.AppKey[i]);
		}
		uprintf("\r\n");
		ret = true;
	}
	else if(argc == 2)
	{
		uint8_t tmp[16];
		if(strlen(argv[1]) == 32 && STR2HEX(argv[1], tmp, 16) == true)
		{
			memcpy(config.AppKey, tmp, 16);
			CONFIG_WRITE(config.AppKey, 16);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_ps(int argc, char *argv[])
{
	bool ret = false;
	if(bAT)
	{
		memset(config.RealAppKey, 0, 16);
		CONFIG_WRITE(config.RealAppKey, 16);
		ret = true;
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_adr(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		bool data;
		if(bAT)
		{
			ret = node_get(NODE_ADR, (void *)&data);
		}
		else {
			data = config.AdrEnable;
			ret = true;
		}
		if(ret) uprintf("%d\r\n", data);
	}
	else if(argc == 2)
	{
		unsigned int d;
		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 1) ) {
			if(bAT)
			{
				ret = node_set(NODE_ADR, (void *)&d);
			}
			else
			{
				config.AdrEnable = (bool)d;
				CONFIG_WRITE(&config.AdrEnable, 1);
				ret = true;
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_class(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		uprintf("%u\r\n", config.Class);
		ret = true;
	}
	else if(argc == 2)
	{
		unsigned int d;
		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 2) ) {
			config.Class = (uint8_t)d;
			CONFIG_WRITE(&config.Class, 1);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_dr(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		int8_t data;
		if(bAT)
		{
			ret = node_get(NODE_CHANNELS_DATARATE, (void *)&data);
		}
		else
		{
			data = config.TxDatarate;
			ret = true;
		}
		if(ret) uprintf("%d\r\n", data);
	}
	else if(argc == 2)
	{
		int d;
		if( (sscanf(argv[1], "%d", &d) == 1) && (d >= 0 && d <= 5) )
		{
			if(bAT)
			{
				ret = node_set(NODE_CHANNELS_DATARATE, (void *)&d);
			}
			else
			{
				config.TxDatarate = (int8_t)d;
				CONFIG_WRITE(&config.TxDatarate, 1);
				ret = true;
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_txp(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		int8_t data;
		if(bAT)
		{
			ret = node_get(NODE_CHANNELS_TX_POWER, (void *)&data);
		}
		else
		{
			data = config.TxPower;
			ret = true;
		}
		if(ret) uprintf("%d\r\n", data);
	}
	else if(argc == 2)
	{
		int d;
		if( (sscanf(argv[1], "%d", &d) == 1) && (d >= 0 && d <= 10) )
		{
			if(bAT)
			{
				ret = node_set(NODE_CHANNELS_TX_POWER, (void *)&d);
			}
			else
			{
				config.TxPower = (int8_t)d;
				CONFIG_WRITE(&config.TxPower, 1);
				ret = true;
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_chp(int argc, char *argv[])
{
	bool ret = false;
	int8_t data[2];

	if(argc == 1) {
		int8_t pwr[8];
		if(bAT) {
			for(int i=0; i<8; i++)
			{
				data[0] = (int8_t)i;
				ret = node_get(NODE_CHANNEL_POWER, (void *)data);
				if(ret) pwr[i] = data[1];
				else break;
			}
		}
		else {
			for(int i=0; i<8; i++) pwr[i] = config.ChPower[i];
			ret = true;
		}

		if(ret)
		{
			for(int i=0; i<8; i++)
			{
				uprintf("%d ", pwr[i]);
			}
			uprintf("\r\n");
		}
	}
	else if(argc == 2)
	{
		int c;
		if( (sscanf(argv[1], "%d", &c) == 1) && (c >= 0 && c <= 7) )
		{
			data[0] = (int8_t)c;
			if(bAT)
			{
				ret = node_get(NODE_CHANNEL_POWER, (void *)data);
			}
			else
			{
				data[1] = config.ChPower[c];
				ret = true;
			}
			if(ret)
			{
				uprintf("ch : %d  txpower : %d\r\n", data[0], data[1]);
			}
		}
	}
	else if(argc == 3)
	{
		int c, p;
		if( (sscanf(argv[1], "%d", &c) == 1) && (c >=0 && c <= 7) && (sscanf(argv[2], "%d", &p) == 1) && (p >= 0 && p <= 14) && !(c == 3 && p > 10) )
		{
			data[0] = (int8_t)c;
			data[1] = (int8_t)p;
			if(bAT)
			{
				ret = node_set(NODE_CHANNEL_POWER, (void *)data);
			}
			else
			{
				config.ChPower[c] = data[1];
				CONFIG_WRITE(&config.ChPower[c], 1);
				ret = true;
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_txm(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		bool data;
		if(bAT) {
			ret = node_get(NODE_TX_MODE, (void *)&data);
		}
		else
		{
			data = config.TxEvent;
			ret = true;
		}
		if(ret) uprintf("%d\r\n", data);
	}
	else if(argc == 2)
	{
		int d;
		if( (sscanf(argv[1], "%d", &d) == 1) && (d == 0 || d == 1) )
		{
			if(bAT)
			{
				ret = node_set(NODE_TX_MODE, (void *)&d);
			}
			else
			{
				config.TxEvent = (bool)d;
				CONFIG_WRITE(&config.TxEvent, 1);
				ret = true;
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_cfm(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		bool data;
		if(bAT)
		{
			ret = node_get(NODE_TX_CONFIRMED, (void *)&data);
		}
		else
		{
			data = config.TxConfirmed;
			ret = true;
		}
		if(ret) uprintf("%d\r\n", data);
	}
	else if(argc == 2)
	{
		int d;
		if( (sscanf(argv[1], "%d", &d) == 1) && (d == 0 || d == 1) )
		{
			if(bAT) {
				ret = node_set(NODE_TX_CONFIRMED, (void *)&d);
			}
			else {
				config.TxConfirmed = (bool)d;
				CONFIG_WRITE(&config.TxConfirmed, 1);
				ret = true;
			}
		}
	}

	return(ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_crep(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		uint8_t data;
		if(bAT)
		{
			ret = node_get(NODE_CFM_REPETITIONS, (void *)&data);
		}
		else
		{
			data = config.CfmMsgTrials;
			ret = true;
		}
		if(ret) uprintf("%u\r\n", data);
	}
	else if(argc == 2)
	{
		unsigned int d;
		if( (sscanf(argv[1], "%u", &d) == 1) && (d >= 0 && d <= 8) )
		{
			if(bAT) {
				ret = node_set(NODE_CFM_REPETITIONS, (void *)&d);
			}
			else
			{
				config.CfmMsgTrials = (uint8_t)d;
				CONFIG_WRITE(&Config.cfmMsgTrials, 1);
				ret = true;
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_urep(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		uint8_t data;
		if(bAT)
		{
			ret = node_get(NODE_CHANNELS_NB_REP, (void *)&data);
		}
		else
		{
			data = config.UncfmMsgTrials;
			ret = true;
		}
		if(ret) uprintf("%u\r\n", data);
	}
	else if(argc == 2)
	{
		unsigned int d;
		if( (sscanf(argv[1], "%u", &d) == 1) && (d >= 0 && d <= 8) )
		{
			if(bAT)
			{
				ret = node_set(NODE_CHANNELS_NB_REP, (void *)&d);
			}
			else
			{
				config.UncfmMsgTrials = (uint8_t)d;
				CONFIG_WRITE(&config.UncfmMsgTrials, 1);
				ret = true;
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_dc(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		uint32_t data;
		if(bAT)
		{
			ret = node_get(NODE_TX_PERIOD, (void *)&data);
		}
		else
		{
			data = config.TxDutyCycleTime;
			ret = true;
		}
		if(ret) uprintf("%u\r\n", data);
	}
	else if(argc == 2)
	{
		unsigned int d;
		if( (sscanf(argv[1], "%u", &d) == 1) && (d >= 10 && d <= 2592000) )
		{
			if(bAT)
			{
				ret = node_set(NODE_TX_PERIOD, (void *)&d);
			}
			else
			{
				config.TxDutyCycleTime = (uint32_t)d;
				CONFIG_WRITE(config.TxDutyCycleTime, 1);
				ret = true;
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_ap(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		uint8_t data;
		if(bAT) {
			ret = node_get(NODE_APP_PORT, (void *)&data);
		}
		else
		{
			data = config.AppPort;
			ret = true;
		}
		if(ret) uprintf("%u\r\n", data);
	}
	else if(argc == 2)
	{
		unsigned int d;
		if( (sscanf(argv[1], "%u", &d) == 1) && (d >= 1 && d <= 127) )
		{
			if(bAT)
			{
				ret = node_set(NODE_APP_PORT, (void *)&d);
			}
			else
			{
				config.AppPort = (uint8_t)d;
				CONFIG_WRITE(Config.AppPort, 1);
				ret = true;
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_debug(int argc, char *argv[])
{
	bool ret = false;

	if(argc == 1)
	{
		uprintf("%u\r\n", config.DebugEnable);
		ret = true;
	}
	else if(argc == 2)
	{
		unsigned int d;
		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 1) )
		{
			config.DebugEnable = (bool)d;
			CONFIG_WRITE(&config.DebugEnable, 1);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_send(int argc, char *argv[])
{

	bool ret = false;

	if(bAT)
	{
		if(argc == 1)
		{
			char *tmp = "0123456789ABCDEF";
			ret = node_send((uint8_t *)tmp, strlen(tmp));
		}
		else if(argc == 2)
		{
			ret = node_send((uint8_t *)argv[1], strlen(argv[1]));
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_lchk(int argc, char *argv[])
{
	bool ret = false;

	if(bAT)
	{
		ret = node_linkcheck();
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_devt(int argc, char *argv[])
{
	  bool ret = false;

	  if(bAT)
	  {
	    ret = node_devtime();
	  }

	  return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_rssi(int argc, char *argv[])
{
	bool ret = false;

	if(bAT)
	{
		int16_t data;
		ret = node_get(NODE_RX_RSSI, (void *)&data);
		if(ret) uprintf("%d\r\n", data);
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_snr(int argc, char *argv[])
{
	bool ret = false;

	if(bAT)
	{
		int8_t data;
		ret = node_get(NODE_RX_SNR, (void *)&data);
		if(ret) uprintf("%d\r\n", data);
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_ucnt(int argc, char *argv[])
{
	bool ret = false;

	if(bAT)
	{
		if(argc == 1)
		{
			uint32_t data;
			ret = node_get(NODE_UPLINK_COUNTER, (void *)&data);
			if(ret)
			{
				uprintf("%u\r\n", data);
			}
		}
		else if(argc == 2)
		{
			unsigned int d;
			if(sscanf(argv[1], "%u", &d) == 1 && (d >= 1 && d <= 0xffff) )
			{
				ret = node_set(NODE_UPLINK_COUNTER, (void *)&d);
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

int Cmd_dcnt(int argc, char *argv[])
{
	bool ret = false;

	if(bAT)
	{
		if(argc == 1)
		{
			uint32_t data;
			ret = node_get(NODE_DOWNLINK_COUNTER, (void *)&data);
			if(ret)
			{
				uprintf("%u\r\n", data);
			}
		}
		else if(argc == 2)
		{
			unsigned int d;
			if(sscanf(argv[1], "%u", &d) == 1 && (d >= 1 && d <= 0xffff) )
			{
				ret = node_set(NODE_DOWNLINK_COUNTER, (void *)&d);
			}
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_ERROR);
}

tCmdLineEntry g_psCmdTable[] =
{
	{ "help",     	Cmd_help,			"Print list of commands" },
	{ "h",        	Cmd_help,     		"alias for help" },
	{ "reset",    	Cmd_reset,    		"Reset the system" },
	{ "ver",        Cmd_ver,        	"Print firmware version" },
	{ "gps",    	Cmd_gps,     		"GPS 1:on 0:off" },
	{ "gpslog",   	Cmd_gps_log,        "GPS log 1:on 0:ff" },
	{ "gpsevent",   Cmd_gps_event,		"GPS check event 5 ~ 20 minute" },
	{ "backlight",  Cmd_backlight,  	"Backlight 1:on 0:off" },
	{ "vibrator",   Cmd_vibrator,  	    "Vibrator 1:on 0:off" },
	{ "buzzer",     Cmd_buzzer,  	    "BUZZER 1:on 0:off" },
	{ "read",		Cmd_pmic_read,		"PMIC read " },
	{ "write",		Cmd_pmic_write,		"PMIC write" },
	{ "ble",      	Cmd_ble,		   	"BLE Command" },
	{ "lora",       Cmd_lora,       	"Print lora information" },
	{ "deveui",     Cmd_deveui,     	"Set deveui" },
	{ "appeui",     Cmd_appeui,     	"Set appeui" },
	{ "appkey",     Cmd_appkey,     	"Set appkey" },
	{ "ps" ,        Cmd_ps,         	"Clear realappkey" },
	{ "adr",        Cmd_adr,        	"Set ADR, 1:on" },
	{ "cls",        Cmd_class,      	"Set class, 0:A 2:C" },
	{ "dr",         Cmd_dr,         	"Set datarate, 0~5" },
	{ "txp",        Cmd_txp,        	"Set txpower, 0~10" },
	{ "chp",        Cmd_chp,        	"Set channel txpower, 0~14dBm" },
	{ "txm",        Cmd_txm,        	"Set txmode, 0:timer,1:event" },
	{ "cfm",        Cmd_cfm,        	"Set message type, 1:Confirmed" },
	{ "crep",       Cmd_crep,       	"Set cfm msg repetition, 1~8" },
	{ "urep",       Cmd_urep,       	"Set uncfm msg repetition, 1~8" },
	{ "dc",         Cmd_dc,         	"Set dutycycle time, second" },
	{ "ap",         Cmd_ap,         	"Set app port, 1~127" },
//	{ "dbg",        Cmd_debug,      	"Set debug output" },
	{ "send",       Cmd_send,       	"Send data" },
	{ "lchk",       Cmd_lchk,       	"Do linkcheck" },
	{ "devt",       Cmd_devt,       	"Do devtime" },
	{ "rst",        Cmd_rst,        	"Reset lora module" },
	{ "rssi",       Cmd_rssi,       	"Get RSSI" },
	{ "snr",        Cmd_snr,        	"Get SNR" },
	{ "ucnt",       Cmd_ucnt,       	"Set uplink counter" },
	{ "dcnt",       Cmd_dcnt,       	"Set downlink counter" },
	{ 0, 0, 0 }
};

int Cmd_invisible_deveui(int argc, char *argv[])
{
	bool ret = false;

	if(bSET && (argc == 1))
	{
		for(int i=0; i<8; i++)
		{
			uprintf("%02X", config.DevEui[i]);
		}
		uprintf("\r\n");
		ret = true;
	}
	else if(bSET && (argc == 2))
	{
		uint8_t tmp[8];
		if(strlen(argv[1]) == 16 && STR2HEX(argv[1], tmp, 8) == true)
		{
			memcpy(config.DevEui, tmp, 8);
			CONFIG_WRITE(config.DevEui, 8);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_BAD_CMD);
}

int Cmd_invisible_realkey(int argc, char *argv[])
{
	bool ret = false;

	if(bSET && (argc == 1))
	{
		for(int i=0; i<16; i++)
		{
			uprintf("%02X", config.RealAppKey[i]);
		}
		uprintf("\r\n");
		ret = true;
	}
	else if(bSET && (argc == 2))
	{
		uint8_t tmp[16];
		if(strlen(argv[1]) == 32 && STR2HEX(argv[1], tmp, 16) == true)
		{
			memcpy(config.RealAppKey, tmp, 16);
			CONFIG_WRITE(config.RealAppKey, 16);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_BAD_CMD);
}


int Cmd_invisible_rdp(int argc, char *argv[])
{
	bool ret = false;

	if(bSET && (argc == 1))
	{
		RDP_GetConfig();
		ret = true;
	}
	else if(bSET && (argc == 2))
	{
		unsigned int d;

		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 1) )
		{
			RDP_SetConfig(d);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_BAD_CMD);
}

int Cmd_invisible_public(int argc, char *argv[])
{
	bool ret = false;

	if(bSET && (argc == 1))
	{
		bool data;
		data = config.PublicNetwork;
		ret = true;

		if(ret) uprintf("%d\r\n", data);
	}
	else if(bSET && (argc == 2))
	{
		unsigned int d;

		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 1) )
		{
			config.PublicNetwork = (bool)d;
			CONFIG_WRITE(&config.PublicNetwork, 1);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_BAD_CMD);
}

int Cmd_invisible_debug(int argc, char *argv[])
{
	bool ret = false;

	if(bSET && (argc == 1))
	{
		bool data;
		data = config.DebugEnable;
		ret = true;

		if(ret) uprintf("%d\r\n", data);
	}
	else if(bSET && (argc == 2))
	{
		unsigned int d;

		if( (sscanf(argv[1], "%u", &d) == 1) && (d == 0 || d == 1) )
		{
			config.DebugEnable = (bool)d;
			CONFIG_WRITE(&config.DebugEnable, 1);
			ret = true;
		}
	}

	return (ret ? CMDLINE_OK : CMDLINE_BAD_CMD);
}

tCmdInvisibleEntry g_psCmdInvisibleTable[] =
{
	{ "deveui",      Cmd_invisible_deveui },
	{ "realkey",     Cmd_invisible_realkey },
	{ "rdp",         Cmd_invisible_rdp },
	{ "public",	     Cmd_invisible_public },
	{ "debug",		 Cmd_invisible_debug },
	{ 0, 0 }
};

void CheckForUserCommands(void)
{
	int iStatus;

	if(upeek('\r') == -1)
	{
		// If not, return so other functions get a chance to run.
		return;
	}

	// If we do have commands, process them immediately in the order they were received.
	 while(upeek('\r') != -1)
	 {
		// Get a user command back.
		ugets(g_cCmdBuf, sizeof(g_cCmdBuf));

		// Process the received command.
		iStatus = CmdLineProcess(g_cCmdBuf);

		// Handle the case of bad command.
		if(iStatus == CMDLINE_BAD_CMD)
		{
			uprintf("Bad command! Type 'h' for help!\r\n");
		}

		// Handle the case of too many arguments.
		else if(iStatus == CMDLINE_TOO_MANY_ARGS)
		{
			uprintf("Too many arguments for command processor!\r\n");
		}

		else if(iStatus == CMDLINE_OK)
		{
			uprintf("OK\r\n");
		}
		else if(iStatus == CMDLINE_ERROR)
		{
			uprintf("ERROR\r\n");
		}

		else if(iStatus == CMDLINE_ACCOUNT_ID)
		{
			uprintf("Enter ID\r\n");
		}
		else if(iStatus == CMDLINE_ACCOUNT_PASSWORD)
		{
			uprintf("Enter PASSWORD\r\n");
		}
	 }
}
