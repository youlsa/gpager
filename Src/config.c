/*
 * config.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "aes.h"
#include "utilities.h"

static aes_context AesContext;
static const uint8_t lora_key[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00 };

static void encrypt(const uint8_t *buffer, uint16_t size, uint8_t *decBuffer)
{
    memset1(AesContext.ksch, '\0', 240);
    aes_set_key(lora_key, 16, &AesContext);
    aes_encrypt(buffer, decBuffer, &AesContext);

    if(size >=16)
    {
        aes_encrypt(buffer + 16, decBuffer + 16, &AesContext);
    }
}

static void decrypt(const uint8_t *buffer, uint16_t size, uint8_t *decBuffer)
{
    memset1(AesContext.ksch, '\0', 240);
    aes_set_key(lora_key, 16, &AesContext);
    aes_decrypt(buffer, decBuffer, &AesContext);

    if(size >=16)
    {
        aes_decrypt(buffer + 16, decBuffer + 16, &AesContext);
    }
}

void CONFIG_Init(void)
{
	for(int i=0; i<16; i++)  config.DevEui[i] = 0;
	for(int i=0; i<16; i++)  config.AppEui[i] = 0;
	for(int i=0; i<16; i++) config.AppKey[i] = 0;
	for(int i=0; i<16; i++) config.RealAppKey[i] = 0;

	config.DebugEnable = false;
	config.Class = 2; // CLASS_C
	config.PublicNetwork = true; // SKT
	config.AppPort = 2;
	config.AdrEnable = true;
	config.TxPower = 1; // MAX
	config.TxDatarate = 0;
	config.TxConfirmed = true;
	config.CfmMsgTrials = 8;
	config.UncfmMsgTrials = 1;
	config.TxEvent = 1; // EVENT
	config.TxDutyCycleTime = 60;
	for(int i=0; i<8; i++) config.ChPower[i] = 10;

	config.marker = CONFIG_MARKER;
}

void CONFIG_Write(void)
{
	uint8_t tmp_deveui[16], tmp_appeui[16];
	uint8_t tmp_appkey[16], tmp_realappkey[16];

	memcpy(tmp_deveui, config.DevEui, 16);
	memcpy(tmp_appeui, config.AppEui, 16);
	memcpy(tmp_appkey, config.AppKey, 16);
	memcpy(tmp_realappkey, config.RealAppKey, 16);

	encrypt(tmp_deveui, sizeof(tmp_deveui) - 1, config.DevEui);
	encrypt(tmp_appeui, sizeof(tmp_appeui) - 1, config.AppEui);
	encrypt(tmp_appkey, sizeof(tmp_appkey) - 1, config.AppKey);
	encrypt(tmp_realappkey, sizeof(tmp_realappkey) - 1, config.RealAppKey);

	EEPROM_Write(EEPROM_CONFIG_ADDR, (uint8_t *)&config, sizeof(config));

	memcpy(config.DevEui, tmp_deveui, 16);
	memcpy(config.AppEui, tmp_appeui, 16);
	memcpy(config.AppKey, tmp_appkey, 16);
	memcpy(config.RealAppKey, tmp_realappkey, 16);
}

void CONFIG_Read(void)
{
	uint8_t tmp_deveui[16], tmp_appeui[16];
	uint8_t tmp_appkey[16], tmp_realappkey[16];

	EEPROM_Read(EEPROM_CONFIG_ADDR, (uint8_t *)&config, sizeof(config_t));

	if(config.marker == CONFIG_MARKER)
	{
		decrypt(config.DevEui, sizeof(config.DevEui) - 1, tmp_deveui);
		decrypt(config.AppEui, sizeof(config.AppEui) - 1, tmp_appeui);
		decrypt(config.AppKey, sizeof(config.AppKey) - 1, tmp_appkey);
		decrypt(config.RealAppKey, sizeof(config.RealAppKey) - 1, tmp_realappkey);

		memcpy(config.DevEui, tmp_deveui, 16);
		memcpy(config.AppEui, tmp_appeui, 16);
		memcpy(config.AppKey, tmp_appkey, 16);
		memcpy(config.RealAppKey, tmp_realappkey, 16);
	}
	else
	{
		DEBUG("CONFIG Init\r\n");
		CONFIG_Init();
		CONFIG_Write();
	}
}
