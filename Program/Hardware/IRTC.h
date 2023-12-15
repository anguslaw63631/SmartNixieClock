/*
 * IRTC.h
 *
 *  Created on: Nov 2, 2023
 *      Author: Angus
 */
#ifndef __IRTC_H__
#define __IRTC_H__

#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "HardwareConfig.h"

typedef struct
{
  uint8_t enableAutoBrightness;
  uint8_t brightness;

  uint8_t timeZone;
  uint8_t timeZonePosNeg;

  uint8_t enableAlarm;
  uint8_t alarmDay;

  uint8_t alarmHour;
  uint8_t alarmMinute;
  

  uint8_t enableNetworkSync;
  uint8_t networkTimeSyncInterval;

  uint8_t nixieTubeRefreshInterval;

  uint8_t aht10RefreshInterval;

  uint8_t enableAida64Monitor;

  uint8_t otherSetting;
  
} Clock_User_Setting;

typedef struct
{
  uint8_t operatingMode;       
  uint8_t countdownModeStatus; 
  uint8_t alarmModeStatus;     
} Clock_Current_Status;

enum ClockOperatingMode {
    ClockMode_Normal,
    ClockMode_Countdown,
    ClockMode_Alarm,
    ClockMode_Setting,
    ClockMode_Debug
};

enum CountdownModeStatus {
    Counter_Pause,
    Counter_Running
};

enum AlarmModeStatus {
    Alarm_OFF,
    Alarm_ON
};

typedef struct
{
  uint8_t WeekDay; /*!< Specifies the RTC Date WeekDay (not necessary for HAL_RTC_SetDate).
                    This parameter can be a value of @ref RTC_WeekDay_Definitions */

  uint8_t Month; /*!< Specifies the RTC Date Month (in BCD format).
                      This parameter can be a value of @ref RTC_Month_Date_Definitions */

  uint8_t Date; /*!< Specifies the RTC Date.
                     This parameter must be a number between Min_Data = 1 and Max_Data = 31 */

  uint8_t Year;  /*!< Specifies the RTC Date Year.
                      This parameter must be a number between Min_Data = 0 and Max_Data = 99 */
  uint8_t Hours; /*!< Specifies the RTC Time Hour.
This parameter must be a number between Min_Data = 0 and Max_Data = 23 */

  uint8_t Minutes; /*!< Specifies the RTC Time Minutes.
                        This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

  uint8_t Seconds; /*!< Specifies the RTC Time Seconds.
                        This parameter must be a number between Min_Data = 0 and Max_Data = 59 */
} IRTC_DateTime;

extern uint8_t newSetupFlag;
extern Clock_User_Setting clockUserSetting;
extern Clock_Current_Status clockCurrentStatus;

void IRTC_Init(void);

void IRTC_SetRTCDateTime(uint32_t unixTimeStamp);

void IRTC_GetRTCDateTime(IRTC_DateTime *iRTC_dateTIme);

void IRTC_SaveUserSetting(void);

void IRTC_ReadUserSetting(void);

void IRTC_ResetUserSetting(void);

uint8_t IRTC_GetAlarm();

void IRTC_SetAlarm(uint8_t day, uint8_t hour, uint8_t minute);

void IRTC_SetCountdownCounter(uint8_t minute, uint8_t second);

int IRTC_GetCountdownRemainingTime();

void IRTC_StartCountdown();

void IRTC_StopCountdown();

HAL_StatusTypeDef IRTC_EnterInitMode_Custom(RTC_HandleTypeDef *hrtc);

HAL_StatusTypeDef IRTC_ExitInitMode_Custom(RTC_HandleTypeDef *hrtc);

uint32_t IRTC_ReadTimeCounter_Custom(RTC_HandleTypeDef *hrtc);

HAL_StatusTypeDef IRTC_WriteTimeCounter_Custom(RTC_HandleTypeDef *hrtc, uint32_t TimeCounter);

#endif