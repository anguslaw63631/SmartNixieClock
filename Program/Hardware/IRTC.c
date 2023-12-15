/*
 * IRTC.c
 *
 *  Created on: Nov 2, 2023
 *      Author: Angus
 */
#include "IRTC.h"

Clock_User_Setting clockUserSetting = {0};
Clock_Current_Status clockCurrentStatus = {0};
uint16_t backupRegisterData[9] = {0};
struct tm tempDateTime = {0};

uint32_t countdownCounterTime = 0;
uint8_t countdownCounterMinute = 0;
uint8_t countdownCounterSecond = 0;

uint32_t unixTimeStamp = 0;
uint8_t newSetupFlag = 2;
/// @brief Init intern RTC and check the backup register for user setting
void IRTC_Init(void)
{
  // Check VBAT BK Settings
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR10) != 0x3030)
  {
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR10, 0x3030);
    HAL_PWR_DisableBkUpAccess();

    uint32_t defaultDateTime = 1704038370; //Sun Dec 31 2023 23:59:30 GMT+0800 (Hong Kong Standard Time)
    HAL_PWR_EnableBkUpAccess();
    IRTC_WriteTimeCounter_Custom(&hrtc, defaultDateTime);
    HAL_PWR_DisableBkUpAccess();

    IRTC_ResetUserSetting();
    newSetupFlag = 1;
  }
  else // If the backup register is not empty, read the user settings
  {
    IRTC_ReadUserSetting();
    newSetupFlag = 0;
  }
}

/// @brief Save user setting to backup register
void IRTC_SaveUserSetting(void)
{
  HAL_PWR_EnableBkUpAccess();
  backupRegisterData[0] = u8_to_u16(clockUserSetting.enableAutoBrightness, clockUserSetting.brightness);
  backupRegisterData[1] = u8_to_u16(clockUserSetting.timeZone, clockUserSetting.timeZonePosNeg);
  backupRegisterData[2] = u8_to_u16(clockUserSetting.enableAlarm, clockUserSetting.alarmDay);
  backupRegisterData[3] = u8_to_u16(clockUserSetting.alarmHour, clockUserSetting.alarmMinute);
  backupRegisterData[4] = u8_to_u16(clockUserSetting.enableNetworkSync, clockUserSetting.networkTimeSyncInterval);
  backupRegisterData[5] = u8_to_u16(clockUserSetting.nixieTubeRefreshInterval, 0);
  backupRegisterData[6] = u8_to_u16(clockUserSetting.aht10RefreshInterval, 0);
  backupRegisterData[7] = u8_to_u16(clockUserSetting.enableAida64Monitor, 0);
  backupRegisterData[8] = u8_to_u16(0, clockUserSetting.otherSetting);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, backupRegisterData[0]);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, backupRegisterData[1]);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, backupRegisterData[2]);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, backupRegisterData[3]);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, backupRegisterData[4]);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR6, backupRegisterData[5]);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR7, backupRegisterData[6]);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR8, backupRegisterData[7]);
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR9, backupRegisterData[8]);
  HAL_PWR_DisableBkUpAccess();
}

/// @brief Read user setting from backup register
void IRTC_ReadUserSetting(void)
{
  backupRegisterData[0] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
  backupRegisterData[1] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
  backupRegisterData[2] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
  backupRegisterData[3] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
  backupRegisterData[4] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
  backupRegisterData[5] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR6);
  backupRegisterData[6] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR7);
  backupRegisterData[7] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR8);
  backupRegisterData[8] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR9);

  uint8_t temp[2] = {0};
  u16_to_u8(backupRegisterData[0], temp);
  clockUserSetting.enableAutoBrightness = temp[0];
  clockUserSetting.brightness = temp[1];
  memset(temp, 0, sizeof(temp));

  u16_to_u8(backupRegisterData[1], temp);
  clockUserSetting.timeZone = temp[0];
  clockUserSetting.timeZonePosNeg = temp[1];
  memset(temp, 0, sizeof(temp));

  u16_to_u8(backupRegisterData[2], temp);
  clockUserSetting.enableAlarm = temp[0];
  clockUserSetting.alarmDay = temp[1];
  memset(temp, 0, sizeof(temp));

  u16_to_u8(backupRegisterData[3], temp);
  clockUserSetting.alarmHour = temp[0];
  clockUserSetting.alarmMinute = temp[1];
  memset(temp, 0, sizeof(temp));

  u16_to_u8(backupRegisterData[4], temp);
  clockUserSetting.enableNetworkSync = temp[0];
  clockUserSetting.networkTimeSyncInterval = temp[1];
  memset(temp, 0, sizeof(temp));

  u16_to_u8(backupRegisterData[5], temp);
  clockUserSetting.nixieTubeRefreshInterval = temp[0];
  memset(temp, 0, sizeof(temp));

  u16_to_u8(backupRegisterData[6], temp);
  clockUserSetting.aht10RefreshInterval = temp[0];
  memset(temp, 0, sizeof(temp));

  u16_to_u8(backupRegisterData[7], temp);
  clockUserSetting.enableAida64Monitor = temp[0];
  memset(temp, 0, sizeof(temp));

  u16_to_u8(backupRegisterData[8], temp);
  clockUserSetting.otherSetting = temp[1];
  memset(temp, 0, sizeof(temp));
}

/// @brief Reset user setting to default
void IRTC_ResetUserSetting(void)
{
  // default setting
  clockUserSetting.enableAutoBrightness = 1; // AutoBrightess On
  clockUserSetting.brightness = 0;

  clockUserSetting.timeZone = 8;       // GMT+8
  clockUserSetting.timeZonePosNeg = 0; // Pos = 0, Neg = 1

  clockUserSetting.enableAlarm = 0; // Alarm Off
  clockUserSetting.alarmDay = 0;

  clockUserSetting.alarmHour = 0;
  clockUserSetting.alarmMinute = 0;

  clockUserSetting.enableNetworkSync = 1;
  clockUserSetting.networkTimeSyncInterval = 10;

  clockUserSetting.nixieTubeRefreshInterval = 5;

  clockUserSetting.aht10RefreshInterval = 5;

  clockUserSetting.enableAida64Monitor = 0;

  clockUserSetting.otherSetting = 0;
  IRTC_SaveUserSetting();
}

/// @brief Set RTC date time using unix time stamp
/// @param unixTimeStamp
void IRTC_SetRTCDateTime(uint32_t unixTimeStamp)
{
  HAL_PWR_EnableBkUpAccess();
  IRTC_WriteTimeCounter_Custom(&hrtc, unixTimeStamp);
  HAL_PWR_DisableBkUpAccess();
}

/// @brief Get RTC date time
/// @param iRTC_dateTime variable to store date time
void IRTC_GetRTCDateTime(IRTC_DateTime *iRTC_dateTime)
{
  unixTimeStamp = IRTC_ReadTimeCounter_Custom(&hrtc);

  time_t Count = 0;
  if (clockUserSetting.timeZonePosNeg == 1)
  {
    Count = unixTimeStamp - clockUserSetting.timeZone * 3600;
  }else{
    Count = unixTimeStamp + clockUserSetting.timeZone * 3600;
  }

  tempDateTime = *gmtime(&Count);

  iRTC_dateTime->Year = tempDateTime.tm_year - 100;
  iRTC_dateTime->Month = tempDateTime.tm_mon + 1;
  iRTC_dateTime->Date = tempDateTime.tm_mday;
  iRTC_dateTime->WeekDay = tempDateTime.tm_wday;

  iRTC_dateTime->Hours = tempDateTime.tm_hour;
  iRTC_dateTime->Minutes = tempDateTime.tm_min;
  iRTC_dateTime->Seconds = tempDateTime.tm_sec;
}

uint8_t IRTC_GetAlarm()
{

  if (clockUserSetting.alarmDay == 0)
  {
    if (clockUserSetting.alarmHour == tempDateTime.tm_hour && clockUserSetting.alarmMinute == tempDateTime.tm_min)
    {
      return 1;
    }
  }
  else if (clockUserSetting.alarmDay == 1)
  {
    if (tempDateTime.tm_wday >= 1 && tempDateTime.tm_wday <= 5)
    {
      if (clockUserSetting.alarmHour == tempDateTime.tm_hour && clockUserSetting.alarmMinute == tempDateTime.tm_min)
      {
        return 1;
      }
    }
  }
  else if (clockUserSetting.alarmDay == 2)
  {
    if (tempDateTime.tm_wday == 0 || tempDateTime.tm_wday == 6)
    {
      if (clockUserSetting.alarmHour == tempDateTime.tm_hour && clockUserSetting.alarmMinute == tempDateTime.tm_min)
      {
        return 1;
      }
    }
  }
  return 0;
}

void IRTC_SetAlarm(uint8_t day, uint8_t hour, uint8_t minute)
{
  clockUserSetting.alarmDay = day;
  clockUserSetting.alarmHour = hour;
  clockUserSetting.alarmMinute = minute;
}

void IRTC_SetCountdownCounter(uint8_t minute, uint8_t second)
{
  countdownCounterMinute = minute;
  countdownCounterSecond = second;
}

int IRTC_GetCountdownRemainingTime()
{
  return countdownCounterTime - unixTimeStamp;
}

void IRTC_StartCountdown()
{
  clockCurrentStatus.countdownModeStatus = 1;
  countdownCounterTime = unixTimeStamp + countdownCounterMinute * 60 + countdownCounterSecond;
}

void IRTC_StopCountdown()
{
  clockCurrentStatus.countdownModeStatus = 0;
}

/// @brief Enter Init Mode to config RTC counter
HAL_StatusTypeDef IRTC_EnterInitMode_Custom(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Disable the write protection for RTC registers */
  RTC->CRL |= RTC_CRL_CNF;

  return HAL_OK;
}

/**
 * @brief  Exit the RTC Initialization mode.
 * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
 *                the configuration information for RTC.
 * @retval HAL status
 */
HAL_StatusTypeDef IRTC_ExitInitMode_Custom(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/// @brief Read intern RTC counter
/// @param hrtc
/// @return unix time stamp
uint32_t IRTC_ReadTimeCounter_Custom(RTC_HandleTypeDef *hrtc)
{
  uint16_t high1 = 0U, high2 = 0U, low = 0U;
  uint32_t timecounter = 0U;

  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  low = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  if (high1 != high2)
  {
    /* In this case the counter roll over during reading of CNTL and CNTH registers,
       read again CNTL register then return the counter value */
    timecounter = (((uint32_t)high2 << 16U) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  {
    /* No counter roll over during reading of CNTL and CNTH registers, counter
       value is equal to first value of CNTL and CNTH */
    timecounter = (((uint32_t)high1 << 16U) | low);
  }

  return timecounter;
}

/**
 * @brief  Write the time counter in RTC_CNT registers.
 * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
 *                the configuration information for RTC.
 * @param  TimeCounter: Counter to write in RTC_CNT registers
 * @retval HAL status
 */
HAL_StatusTypeDef IRTC_WriteTimeCounter_Custom(RTC_HandleTypeDef *hrtc, uint32_t TimeCounter)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Set Initialization mode */
  if (IRTC_EnterInitMode_Custom(hrtc) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Set RTC COUNTER MSB word */
    WRITE_REG(hrtc->Instance->CNTH, (TimeCounter >> 16U));
    /* Set RTC COUNTER LSB word */
    WRITE_REG(hrtc->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));

    /* Wait for synchro */
    if (IRTC_ExitInitMode_Custom(hrtc) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}