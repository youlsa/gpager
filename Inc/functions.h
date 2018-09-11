/*
 * fuctions.h
 *
 *  Created on: 2017. 10. 27.
 *      Author: baekjg
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#ifdef __cplusplus
 extern "C" {
#endif

enum {
	VIBRATOR_OFF,
	VIBRATOR_ON,
};

enum {
	BUZZER_OFF,
	BUZZER_ON,
};

// GPS
bool gps_gga_parsing(uint8_t *queue, uint8_t *data);
bool gps_rmc_parsing(uint8_t *queue, uint8_t *data);
bool GPS_RMC_Status(uint8_t *data);

void GPSTimer_Init(void);
void GPSTimer_Start(uint8_t time);
void GPSTimer_Stop(void);
void OnGPSTimerEvent(void);

// Vibrator
void Vibrator_Init(void);
void VibratorTimer_Init(void);
void VibratorTimer_Start(uint32_t time);
void Vibrator_ON_Control(uint8_t on);

// Buzzer
void BuzzerTimer_Init(void);
void BuzzerTimer_Start(uint32_t time);
void Buzzer_ON_Control(uint8_t on);

// Wakeup Button
void Wakeup_Init(void);
void WakeupTimer_Init(void);
void WakeupTimer_Start(void);
void Power_Off_Mode(void);

// Charging
bool Charging_Status(uint8_t value);

// Battery Check
void BAT_Level_Check_Init(void);
void BAT_Level_Check(void);

// RDP
void RDP_SetConfig(uint8_t level);
void RDP_GetConfig(void);

#ifdef __cplusplus
}
#endif
#endif /* FUNCTIONS_H_ */
