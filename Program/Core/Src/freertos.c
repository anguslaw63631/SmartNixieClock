/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "HardwareConfig.h"
#include "VFD.h"
#include "AHT10.h"
#include "Button.h"
#include "Buzzer.h"
#include "Encoder.h"
#include "Wifi.h"
#include "LDR.h"
#include "NixieTube.h"
#include "IRTC.h"
#include "USBTTL.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

// Input
uint8_t input1FreezerFlag = 0;
uint8_t input2FreezerFlag = 0;
uint8_t input3FreezerFlag = 0;
uint8_t input4FreezerFlag = 0;
uint8_t input5FreezerFlag = 0;
uint8_t input6FreezerFlag = 0;

// VFD
float progressCounter = 0.0;
uint8_t vfdTestFlag = 0;

// Setting Mode
uint8_t settingModeFlag = 0;
uint8_t configDateTimeFlag = 0;
IRTC_DateTime new_iRTC_dateTime = {0};

// IRTC
IRTC_DateTime localRTCDateTime = {0};
uint8_t saveSettingFlag = 0;
uint32_t rtcCounter = 0;
uint8_t alarmFlag = 0;
uint8_t alarmNoticationCounter = 0;
uint8_t alarmModeToggleFlag = 0;
uint8_t alarmSettingMins = 0;
uint8_t alarmSettingHours = 0;

// Countdown Mode
uint8_t countdownFlag = 0;
uint8_t countdownNoticationCounter = 0;
uint8_t countdownModeToggleFlag = 0;
uint8_t countdownSettingMins = 0;
uint8_t countdownSettingSecs = 0;
uint8_t countdownRunningModePauseFlag = 0;

// NixieTube
uint8_t nixieTubeTestCounter = 0;
uint8_t nixieTubeNumber = 0;
uint8_t nixieTubeColon = 0;
uint8_t nixieTubeDutyCycle = 0;
uint8_t Tnum[6] = {0};
uint16_t nixieTubeRefreshCounter = 0;
uint8_t nixieTubeTestFlag = 0;

/* USER CODE END Variables */
osThreadId nixietubeTaskHandle;
uint32_t nixietubeControBuffer[128];
osStaticThreadDef_t nixietubeControControlBlock;
osThreadId vfdTaskHandle;
uint32_t vfdTaskBuffer[128];
osStaticThreadDef_t vfdTaskControlBlock;
osThreadId irtcTaskHandle;
uint32_t irtcTaskBuffer[128];
osStaticThreadDef_t irtcTaskControlBlock;
osThreadId aht10TaskHandle;
uint32_t aht10TaskBuffer[128];
osStaticThreadDef_t aht10TaskControlBlock;
osThreadId ldrTaskHandle;
uint32_t ldrTaskBuffer[128];
osStaticThreadDef_t ldrTaskControlBlock;
osThreadId esp8266TaskHandle;
uint32_t esp8266TaskBuffer[512];
osStaticThreadDef_t esp8266TaskControlBlock;
osThreadId buzzerTaskHandle;
uint32_t buzzerTaskBuffer[128];
osStaticThreadDef_t buzzerTaskControlBlock;
osThreadId blinkTaskHandle;
uint32_t blinkTaskBuffer[64];
osStaticThreadDef_t blinkTaskControlBlock;
osThreadId inputControllerHandle;
uint32_t inputControllerBuffer[128];
osStaticThreadDef_t inputControllerControlBlock;
osThreadId ecControllerHandle;
uint32_t ecControllerBuffer[64];
osStaticThreadDef_t ecControllerControlBlock;
osThreadId ttlControllerHandle;
uint32_t ttlControllerBuffer[128];
osStaticThreadDef_t ttlControllerControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void nixietubeController(void const *argument);
void VFDController(void const *argument);
void irtcController(void const *argument);
void AHT10Controller(void const *argument);
void LDRController(void const *argument);
void esp8266Controller(void const *argument);
void buzzerController(void const *argument);
void blinkController(void const *argument);
void inputControllerTask(void const *argument);
void ecControllerTask(void const *argument);
void ttlControllerTask(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */
  clockCurrentStatus.operatingMode = ClockMode_Normal;
  if (newSetupFlag == 1)
  { // if new setup, set to setting mode and setup DateTime
    clockCurrentStatus.operatingMode = ClockMode_Setting;
    settingModeFlag = 7;
  }
  IRTC_Init();
  IRTC_GetRTCDateTime(&localRTCDateTime);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of nixietubeTask */
  osThreadStaticDef(nixietubeTask, nixietubeController, osPriorityHigh, 0, 128, nixietubeControBuffer, &nixietubeControControlBlock);
  nixietubeTaskHandle = osThreadCreate(osThread(nixietubeTask), NULL);

  /* definition and creation of vfdTask */
  osThreadStaticDef(vfdTask, VFDController, osPriorityHigh, 0, 128, vfdTaskBuffer, &vfdTaskControlBlock);
  vfdTaskHandle = osThreadCreate(osThread(vfdTask), NULL);

  /* definition and creation of irtcTask */
  osThreadStaticDef(irtcTask, irtcController, osPriorityAboveNormal, 0, 128, irtcTaskBuffer, &irtcTaskControlBlock);
  irtcTaskHandle = osThreadCreate(osThread(irtcTask), NULL);

  /* definition and creation of aht10Task */
  osThreadStaticDef(aht10Task, AHT10Controller, osPriorityLow, 0, 128, aht10TaskBuffer, &aht10TaskControlBlock);
  aht10TaskHandle = osThreadCreate(osThread(aht10Task), NULL);

  /* definition and creation of ldrTask */
  osThreadStaticDef(ldrTask, LDRController, osPriorityBelowNormal, 0, 128, ldrTaskBuffer, &ldrTaskControlBlock);
  ldrTaskHandle = osThreadCreate(osThread(ldrTask), NULL);

  /* definition and creation of esp8266Task */
  osThreadStaticDef(esp8266Task, esp8266Controller, osPriorityNormal, 0, 512, esp8266TaskBuffer, &esp8266TaskControlBlock);
  esp8266TaskHandle = osThreadCreate(osThread(esp8266Task), NULL);

  /* definition and creation of buzzerTask */
  osThreadStaticDef(buzzerTask, buzzerController, osPriorityLow, 0, 128, buzzerTaskBuffer, &buzzerTaskControlBlock);
  buzzerTaskHandle = osThreadCreate(osThread(buzzerTask), NULL);

  /* definition and creation of blinkTask */
  osThreadStaticDef(blinkTask, blinkController, osPriorityIdle, 0, 64, blinkTaskBuffer, &blinkTaskControlBlock);
  blinkTaskHandle = osThreadCreate(osThread(blinkTask), NULL);

  /* definition and creation of inputController */
  osThreadStaticDef(inputController, inputControllerTask, osPriorityRealtime, 0, 128, inputControllerBuffer, &inputControllerControlBlock);
  inputControllerHandle = osThreadCreate(osThread(inputController), NULL);

  /* definition and creation of ecController */
  osThreadStaticDef(ecController, ecControllerTask, osPriorityHigh, 0, 64, ecControllerBuffer, &ecControllerControlBlock);
  ecControllerHandle = osThreadCreate(osThread(ecController), NULL);

  /* definition and creation of ttlController */
  osThreadStaticDef(ttlController, ttlControllerTask, osPriorityNormal, 0, 128, ttlControllerBuffer, &ttlControllerControlBlock);
  ttlControllerHandle = osThreadCreate(osThread(ttlController), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_nixietubeController */
/**
 * @brief  Function implementing the nixietubeTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_nixietubeController */
void nixietubeController(void const *argument)
{
  /* USER CODE BEGIN nixietubeController */
  NixieTube_Init();
  osDelay(1000);
  nixieTubeRefreshCounter = clockUserSetting.nixieTubeRefreshInterval * 60000; // min to ms
  /* Infinite loop */
  for (;;)
  {

    // Avoid NixieTube burnout
    nixieTubeRefreshCounter-=225;
    if (nixieTubeRefreshCounter <= 226)
    {
      for (int i = 0; i < 10; i++)
      {
        NixieTube_SetTube1(i);
        NixieTube_SetTube2(i);
        NixieTube_Update();
        NixieTube_SetTube1(i);
        NixieTube_SetTube2(i);
        NixieTube_Update2();
        NixieTube_SetColon(i % 4);
        osDelay(100);
      }
      nixieTubeRefreshCounter = clockUserSetting.nixieTubeRefreshInterval * 60000;
    }

    if (clockUserSetting.enableAutoBrightness == 1)
    {
      NixieTube_SetBrightness(LDR_brightness);
    }
    else
    {
      NixieTube_SetBrightness(clockUserSetting.brightness);
    }

    if (clockCurrentStatus.operatingMode == ClockMode_Normal) // Normal Mode
    {
      Tnum[0] = localRTCDateTime.Hours / 10;
      Tnum[1] = localRTCDateTime.Hours % 10;
      Tnum[2] = localRTCDateTime.Minutes / 10;
      Tnum[3] = localRTCDateTime.Minutes % 10;
      if (localRTCDateTime.Seconds == 30 || localRTCDateTime.Seconds == 0)
      {
        Tnum[4] = 1;
      }
      else
      {
        Tnum[4] = 0;
      }

      NixieTube_SetTube1(Tnum[0]);
      NixieTube_SetTube2(Tnum[1]);
      NixieTube_Update();
      NixieTube_SetTube1(Tnum[2]);
      NixieTube_SetTube2(Tnum[3]);
      NixieTube_Update2();
      NixieTube_SetColon(Tnum[4]);
    }
    else if (clockCurrentStatus.operatingMode == ClockMode_Countdown)
    { // Countdown Mode
      if (clockCurrentStatus.countdownModeStatus == Counter_Pause)
      {
        NixieTube_SetTube1(countdownSettingMins / 10);
        NixieTube_SetTube2(countdownSettingMins % 10);
        NixieTube_Update();
        NixieTube_SetTube1(countdownSettingSecs / 10);
        NixieTube_SetTube2(countdownSettingSecs % 10);
        NixieTube_Update2();
        if (countdownModeToggleFlag == 1)
        {
          NixieTube_SetColon(3);
        }
        else
        {
          NixieTube_SetColon(2);
        }
      }
      else
      {
        NixieTube_SetTube1(IRTC_GetCountdownRemainingTime() / 60 / 10);
        NixieTube_SetTube2(IRTC_GetCountdownRemainingTime() / 60 % 10);
        NixieTube_Update();
        NixieTube_SetTube1(IRTC_GetCountdownRemainingTime() % 60 / 10);
        NixieTube_SetTube2(IRTC_GetCountdownRemainingTime() % 60 % 10);
        NixieTube_Update2();
        if ((IRTC_GetCountdownRemainingTime() % 60 % 10) % 10 == 0)
        {
          NixieTube_SetColon(1);
        }
        else
        {
          NixieTube_SetColon(0);
        }
      }
    }
    else if (clockCurrentStatus.operatingMode == ClockMode_Alarm)
    { // Alarm Mode
      NixieTube_SetTube1(alarmSettingHours / 10);
      NixieTube_SetTube2(alarmSettingHours % 10);
      NixieTube_Update();
      NixieTube_SetTube1(alarmSettingMins / 10);
      NixieTube_SetTube2(alarmSettingMins % 10);
      NixieTube_Update2();
      if (alarmModeToggleFlag == 1)
      {
        NixieTube_SetColon(3);
      }
      else
      {
        NixieTube_SetColon(2);
      }
    }
    else if (clockCurrentStatus.operatingMode == ClockMode_Setting)
    { // Setting Mode
      if (settingModeFlag < 5)
      {
        NixieTube_SetTube1(10);
        NixieTube_SetTube2(10);
        NixieTube_Update2();
      }
      if (settingModeFlag == 0)
      {
        if (clockUserSetting.enableAutoBrightness == 0)
        {
          NixieTube_SetTube1(clockUserSetting.brightness / 10);
          NixieTube_SetTube2(clockUserSetting.brightness % 10);
          NixieTube_Update();
        }
        else
        {
          NixieTube_SetTube1(0);
          NixieTube_SetTube2(0);
          NixieTube_Update();
        }
      }
      else if (settingModeFlag == 1)
      {
        NixieTube_SetTube1(clockUserSetting.timeZone / 10);
        NixieTube_SetTube2(clockUserSetting.timeZone % 10);
        NixieTube_Update();
      }
      else if (settingModeFlag == 2)
      {
        NixieTube_SetTube1(clockUserSetting.networkTimeSyncInterval / 10);
        NixieTube_SetTube2(clockUserSetting.networkTimeSyncInterval % 10);
        NixieTube_Update();
      }
      else if (settingModeFlag == 3)
      {
        NixieTube_SetTube1(clockUserSetting.nixieTubeRefreshInterval / 10);
        NixieTube_SetTube2(clockUserSetting.nixieTubeRefreshInterval % 10);
        NixieTube_Update();
      }
      else if (settingModeFlag == 4)
      {
        NixieTube_SetTube1(clockUserSetting.aht10RefreshInterval / 10);
        NixieTube_SetTube2(clockUserSetting.aht10RefreshInterval % 10);
        NixieTube_Update();
      }
      else if (settingModeFlag == 5)
      {
        NixieTube_SetTube1(0);
        NixieTube_SetTube2(0);
        NixieTube_Update();
        NixieTube_SetTube1(0);
        NixieTube_SetTube2(0);
        NixieTube_Update2();
        NixieTube_SetColon(1);
      }
      else if (settingModeFlag == 6)
      {
        NixieTube_SetTube1(0);
        NixieTube_SetTube2(0);
        NixieTube_Update();
        NixieTube_SetTube1(0);
        NixieTube_SetTube2(0);
        NixieTube_Update2();
        NixieTube_SetColon(1);
      }
      else if (settingModeFlag == 7)
      {
        if (configDateTimeFlag == 0)
        {
          NixieTube_SetTube1(2);
          NixieTube_SetTube2(0);
          NixieTube_Update();
          NixieTube_SetTube1(new_iRTC_dateTime.Year / 10);
          NixieTube_SetTube2(new_iRTC_dateTime.Year % 10);
          NixieTube_Update2();
        }
        else if (configDateTimeFlag == 1)
        {
          NixieTube_SetTube1(new_iRTC_dateTime.Month / 10);
          NixieTube_SetTube2(new_iRTC_dateTime.Month % 10);
          NixieTube_Update();
        }
        else if (configDateTimeFlag == 2)
        {
          NixieTube_SetTube1(new_iRTC_dateTime.Date / 10);
          NixieTube_SetTube2(new_iRTC_dateTime.Date % 10);
          NixieTube_Update();
        }
        else if (configDateTimeFlag == 3)
        {
          NixieTube_SetTube1(new_iRTC_dateTime.WeekDay / 10);
          NixieTube_SetTube2(new_iRTC_dateTime.WeekDay % 10);
          NixieTube_Update();
        }
        else if (configDateTimeFlag == 4)
        {
          NixieTube_SetTube1(new_iRTC_dateTime.Hours / 10);
          NixieTube_SetTube2(new_iRTC_dateTime.Hours % 10);
          NixieTube_Update();
        }
        else if (configDateTimeFlag == 5)
        {
          NixieTube_SetTube1(new_iRTC_dateTime.Minutes / 10);
          NixieTube_SetTube2(new_iRTC_dateTime.Minutes % 10);
          NixieTube_Update();
        }
        else if (configDateTimeFlag == 6)
        {
          NixieTube_SetTube1(new_iRTC_dateTime.Seconds / 10);
          NixieTube_SetTube2(new_iRTC_dateTime.Seconds % 10);
          NixieTube_Update();
        }
      }
    }
    else if (clockCurrentStatus.operatingMode == ClockMode_Debug)
    { // Debug Mode
      if (nixieTubeColon == 0)
      {
        nixieTubeColon = 1;
      }
      else
      {
        nixieTubeColon = 0;
      }
      nixieTubeNumber = nixieTubeTestCounter % 10;
      nixieTubeDutyCycle = nixieTubeDutyCycle + 5;
      if (nixieTubeDutyCycle >= 95)
      {
        nixieTubeDutyCycle = 0;
      }

      NixieTube_SetTube1(nixieTubeNumber);
      NixieTube_SetTube2(nixieTubeNumber);
      NixieTube_Update();

      NixieTube_SetTube1(nixieTubeNumber);
      NixieTube_SetTube2(nixieTubeNumber);
      NixieTube_Update2();
      NixieTube_SetColon(nixieTubeColon);
      if (nixieTubeTestFlag == 0)
      {
        nixieTubeTestCounter++;
      }
    }
    osDelay(200);
  }
  /* USER CODE END nixietubeController */
}

/* USER CODE BEGIN Header_VFDController */
/**
 * @brief Function implementing the vfdTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_VFDController */
void VFDController(void const *argument)
{
  /* USER CODE BEGIN VFDController */

  uint8_t buffer[8] = {0};

  uint8_t custom_char[8][5] = {
      {0x01, 0x01, 0x01, 0x01, 0x01},
      {0x02, 0x02, 0x02, 0x02, 0x02},
      {0x04, 0x04, 0x04, 0x04, 0x04},
      {0x08, 0x08, 0x08, 0x08, 0x08},
      {0x10, 0x10, 0x10, 0x10, 0x10},
      {0x20, 0x20, 0x20, 0x20, 0x20},
      {0x40, 0x40, 0x40, 0x40, 0x40},
      {0x80, 0x80, 0x80, 0x80, 0x80},
  };
  VFD_Init();

  VFD_DisplayString(0, "SN Clock");
  osDelay(1000);
  VFD_DisplayString(0, " FW:1.0 ");
  osDelay(1000);
  /* Infinite loop */
  for (;;)
  {

    if (clockCurrentStatus.operatingMode == ClockMode_Normal) // Normal Mode
    {
      switch (localRTCDateTime.WeekDay)
      {
      case 0:
        sprintf(buffer, "SUN%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
        break;
      case 1:
        sprintf(buffer, "MON%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
        break;
      case 2:
        sprintf(buffer, "TUE%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
        break;
      case 3:
        sprintf(buffer, "WED%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
        break;
      case 4:
        sprintf(buffer, "THU%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
        break;
      case 5:
        sprintf(buffer, "FRI%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
        break;
      case 6:
        sprintf(buffer, "SAT%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
        break;
      default:
        sprintf(buffer, "ERR-C01");
        break;
      }
      VFD_ClearDisplay();
      VFD_DisplayString(0, buffer);
      osDelay(2000);
      VFD_ClearDisplay();
      sprintf(buffer, "Temp:%02dC", (int)aht10Data.temperature);
      VFD_DisplayString(0, buffer);
      osDelay(1000);
      VFD_ClearDisplay();
      sprintf(buffer, "Humi:%02d%%", (int)aht10Data.humidity);
      VFD_DisplayString(0, buffer);
      osDelay(1000);
      if (clockUserSetting.enableAida64Monitor == 1)
      {
        VFD_ClearDisplay();
        sprintf(buffer, "CPU:%03d%%", (int)USBTTL.aida64.cpuUsage);
        VFD_DisplayString(0, buffer);
        osDelay(1000);
        VFD_ClearDisplay();
        sprintf(buffer, "RAM:%03d%%", (int)USBTTL.aida64.ramUsage);
        VFD_DisplayString(0, buffer);
        osDelay(1000);
        // VFD_ClearDisplay();
        // sprintf(buffer, "GPU:%03d%%", (int)USBTTL.aida64.gpuUsage);
        // VFD_DisplayString(0, buffer);
        // osDelay(1000);
        // VFD_ClearDisplay();
        // sprintf(buffer, "VRAM:%02d%%", (int)USBTTL.aida64.vramUsage);
        // VFD_DisplayString(0, buffer);
        // osDelay(1000);
      }
    }
    else if (clockCurrentStatus.operatingMode == ClockMode_Countdown) // Countdown Mode
    {
      if (clockCurrentStatus.countdownModeStatus == Counter_Pause)
      {
        VFD_DisplayString(0, "CD:PAUSE");
      }
      else
      {
        progressCounter = ((float)IRTC_GetCountdownRemainingTime() / ((float)countdownSettingMins * 60 + (float)countdownSettingSecs)) * 100.0;
        if (progressCounter <= 15.0)
        {
          for (uint8_t j = 0; j < 1; j++)
          {
            VFD_DisplayChar(j, 127);
          }
          for (uint8_t j = 1; j < 8; j++)
          {
            VFD_DisplayChar(j, 0);
          }
        }
        else if (progressCounter <= 30.0)
        {
          for (uint8_t j = 0; j < 2; j++)
          {
            VFD_DisplayChar(j, 127);
          }
          for (uint8_t j = 2; j < 8; j++)
          {
            VFD_DisplayChar(j, 0);
          }
        }
        else if (progressCounter <= 45.0)
        {
          for (uint8_t j = 0; j < 4; j++)
          {
            VFD_DisplayChar(j, 127);
          }
          for (uint8_t j = 4; j < 8; j++)
          {
            VFD_DisplayChar(j, 0);
          }
        }
        else if (progressCounter <= 60.0)
        {
          for (uint8_t j = 0; j < 5; j++)
          {
            VFD_DisplayChar(j, 127);
          }
          for (uint8_t j = 5; j < 8; j++)
          {
            VFD_DisplayChar(j, 0);
          }
        }
        else if (progressCounter <= 75.0)
        {
          for (uint8_t j = 0; j < 6; j++)
          {
            VFD_DisplayChar(j, 127);
          }
          for (uint8_t j = 6; j < 8; j++)
          {
            VFD_DisplayChar(j, 0);
          }
        }
        else if (progressCounter <= 90.0)
        {
          for (uint8_t j = 0; j < 7; j++)
          {
            VFD_DisplayChar(j, 127);
          }
          for (uint8_t j = 7; j < 8; j++)
          {
            VFD_DisplayChar(j, 0);
          }
        }
        else
        {
          for (uint8_t j = 0; j < 8; j++)
          {
            VFD_DisplayChar(j, 127);
          }
        }
      }
      osDelay(100);
    }
    else if (clockCurrentStatus.operatingMode == ClockMode_Alarm) // Alarm Mode
    {
      if (clockUserSetting.enableAlarm == Alarm_ON)
      {
        VFD_DisplayString(0, "ALARM:ON ");
      }
      else
      {
        VFD_DisplayString(0, "ALARM:OFF");
      }
      osDelay(100);
    }
    else if (clockCurrentStatus.operatingMode == ClockMode_Setting) // Setting Mode
    {
      if (settingModeFlag == 0)
      {
        if (clockUserSetting.enableAutoBrightness == 0)
        {
          VFD_DisplayString(0, "ATBr:OFF");
        }
        else
        {
          VFD_DisplayString(0, "ATBr:ON ");
        }
      }
      else if (settingModeFlag == 1)
      {
        if (clockUserSetting.timeZonePosNeg == 0)
        {
          VFD_DisplayString(0, "TimeZ:+ ");
        }
        else
        {
          VFD_DisplayString(0, "TimeZ:- ");
        }
      }
      else if (settingModeFlag == 2)
      {
        if (clockUserSetting.enableNetworkSync == 0)
        {
          VFD_DisplayString(0, "NetS:OFF");
        }
        else
        {
          VFD_DisplayString(0, "NetS:ON ");
        }
      }
      else if (settingModeFlag == 3)
      {
        VFD_DisplayString(0, "NTRefres");
      }
      else if (settingModeFlag == 4)
      {
        VFD_DisplayString(0, "ATH10Ref");
      }
      else if (settingModeFlag == 5)
      {
        VFD_DisplayString(0, "ResetAll");
      }
      else if (settingModeFlag == 6)
      {
        VFD_DisplayString(0, "DebugMode");
      }
      else if (settingModeFlag == 7)
      {
        if (configDateTimeFlag == 0)
        {
          VFD_DisplayString(0, "SetYear ");
        }
        else if (configDateTimeFlag == 1)
        {
          VFD_DisplayString(0, "SetMonth");
        }
        else if (configDateTimeFlag == 2)
        {
          VFD_DisplayString(0, "SetDate ");
        }
        else if (configDateTimeFlag == 3)
        {
          VFD_DisplayString(0, "SetWeekD");
        }
        else if (configDateTimeFlag == 4)
        {
          VFD_DisplayString(0, "SetHour ");
        }
        else if (configDateTimeFlag == 5)
        {
          VFD_DisplayString(0, "SetMin  ");
        }
        else if (configDateTimeFlag == 6)
        {
          VFD_DisplayString(0, "SetSec  ");
        }
      }
      else if (settingModeFlag == 8)
      {
        if (clockUserSetting.enableAida64Monitor == 1)
        {
          VFD_DisplayString(0, "Aida:ON ");
        }
        else
        {
          VFD_DisplayString(0, "Aida:OFF");
        }
      }
      osDelay(100);
    }
    else if (clockCurrentStatus.operatingMode == ClockMode_Debug) // Debug Mode
    {
      if (vfdTestFlag == 0)
      {
        for (uint8_t i = 0; i < 8; i++)
        {
          for (uint8_t j = 0; j < 8; j++)
          {
            VFD_DisplayCustomChar(i, custom_char[i]);
            VFD_DisplayChar(j, i);
          }
          osDelay(50);
        }
        osDelay(200);
      }
      else if (vfdTestFlag == 1)
      {
        switch (localRTCDateTime.WeekDay)
        {
        case 0:
          sprintf(buffer, "SUN%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
          break;
        case 1:
          sprintf(buffer, "MON%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
          break;
        case 2:
          sprintf(buffer, "TUE%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
          break;
        case 3:
          sprintf(buffer, "WED%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
          break;
        case 4:
          sprintf(buffer, "THU%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
          break;
        case 5:
          sprintf(buffer, "FRI%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
          break;
        case 6:
          sprintf(buffer, "SAT%02d/%02d", localRTCDateTime.Date, localRTCDateTime.Month);
          break;
        default:
          sprintf(buffer, "ERR-C01");
          break;
        }
        VFD_ClearDisplay();
        VFD_DisplayString(0, buffer);
        osDelay(5000);
        VFD_ClearDisplay();
        sprintf(buffer, "Temp:%02dC", (int)aht10Data.temperature);
        VFD_DisplayString(0, buffer);
        osDelay(3000);
        VFD_ClearDisplay();
        sprintf(buffer, "Humi:%02d%%", (int)aht10Data.humidity);
        VFD_DisplayString(0, buffer);
        osDelay(3000);
      }
      else if (vfdTestFlag == 2)
      {
        VFD_DisplayString(0, "ELEC3300");
        osDelay(3000);
        VFD_DisplayString(0, "TESTMODE");
        osDelay(3000);
      }
    }
    osDelay(1);
  }
  /* USER CODE END VFDController */
}

/* USER CODE BEGIN Header_irtcController */
/**
 * @brief Function implementing the irtcTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_irtcController */
void irtcController(void const *argument)
{
  /* USER CODE BEGIN irtcController */

  /* Infinite loop */
  for (;;)
  {
    rtcCounter = IRTC_ReadTimeCounter_Custom(&hrtc);
    IRTC_GetRTCDateTime(&localRTCDateTime);

    if (clockUserSetting.enableAlarm == Alarm_ON)
    {
      if (IRTC_GetAlarm() == 1)
      {
        alarmFlag = 1;
      }
      else
      {
        alarmFlag = 0;
        alarmNoticationCounter = 2;
      }
    }
    if (clockCurrentStatus.countdownModeStatus == Counter_Running)
    {
      if (IRTC_GetCountdownRemainingTime() <= 0)
      {
        countdownFlag = 1;
        clockCurrentStatus.countdownModeStatus = Counter_Pause;
      }
      else
      {
        countdownFlag = 0;
        countdownNoticationCounter = 2;
      }
    }

    osDelay(500);
  }
  /* USER CODE END irtcController */
}

/* USER CODE BEGIN Header_AHT10Controller */
/**
 * @brief Function implementing the aht10Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_AHT10Controller */
void AHT10Controller(void const *argument)
{
  /* USER CODE BEGIN AHT10Controller */

  AHT10_Init();
  osDelay(3000);
  AHT10_Update();
  /* Infinite loop */
  for (;;)
  {
    AHT10_Update();
    osDelay(msMin * clockUserSetting.aht10RefreshInterval);
  }
  /* USER CODE END AHT10Controller */
}

/* USER CODE BEGIN Header_LDRController */
/**
 * @brief Function implementing the ldrTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_LDRController */
void LDRController(void const *argument)
{
  /* USER CODE BEGIN LDRController */
  LDR_Init();
  osDelay(3000);
  LDR_Update();
  /* Infinite loop */
  for (;;)
  {
    if (clockUserSetting.enableAutoBrightness == 1)
    {
      LDR_Update();
      osDelay(1000);
    }
    osDelay(1);
  }
  /* USER CODE END LDRController */
}

/* USER CODE BEGIN Header_esp8266Controller */
/**
 * @brief Function implementing the esp8266Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_esp8266Controller */
void esp8266Controller(void const *argument)
{
  /* USER CODE BEGIN esp8266Controller */
  osDelay(1000);
  Wifi_Init();

  osDelay(3000);
  Wifi_SendStringAndWait("AT\r\n", 1000);
  Wifi_SetRfPower(82);
  Wifi_TcpIp_GetMultiConnection();
  Wifi_TcpIp_Close(0);
  Wifi_TcpIp_Close(1);
  Wifi_TcpIp_Close(2);
  Wifi_TcpIp_Close(3);
  Wifi_TcpIp_Close(4);
  Wifi_TcpIp_SetMultiConnection(true);
  Wifi_GetMode();

  Wifi_SetMode(WifiMode_Station);
  while (Wifi_Station_ConnectToAp(_WIFI_SSID, _WIFI_PASSWORD, NULL) == false)
    ;
  osDelay(3000);
  /* Infinite loop */
  for (;;)
  {
    if (clockUserSetting.enableNetworkSync == 1)
    {
      Wifi_GetMyIp();
      Wifi_TcpIp_GetConnectionStatus();
      Wifi_RxClear();
      for (uint8_t i = 0; i < 100; i++)
      {
        if (Wifi.GotNewData == true)
        {
          Wifi.GotNewData = false;
          for (uint8_t ii = 0; ii < 5; ii++)
          {
            if ((strstr(Wifi.TcpIpConnections[ii].Type, "TCP") != NULL) && (Wifi.RxDataConnectionNumber == Wifi.TcpIpConnections[ii].LinkId))
              Wifi_TcpIp_SendDataTcp(Wifi.RxDataConnectionNumber, Wifi.RxDataLen, Wifi.RxBufferForData);
          }
        }
        osDelay(10);
      }

      Wifi_TcpIp_StartTcpConnection(0, "api.k780.com", 80, 100);
      osDelay(100);
      Wifi_TcpIp_SendDataTcp(0, 104, "GET http://api.k780.com/?app=life.time&appkey={appkey}&sign={Type in your own sign}&format=json\r\n");
      if (strstr(Wifi.RxBufferForData, "success") != NULL)
      {
        memset(networkTimeStamp, 0, 10);
        memcpy(networkTimeStamp, Wifi.RxBufferForData + 38, 10);
        IRTC_SetRTCDateTime(atoi(networkTimeStamp));
      }
      osDelay(msMin * clockUserSetting.networkTimeSyncInterval);
    }
    osDelay(1);
  }
  /* USER CODE END esp8266Controller */
}

/* USER CODE BEGIN Header_buzzerController */
/**
 * @brief Function implementing the buzzerTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_buzzerController */
void buzzerController(void const *argument)
{
  /* USER CODE BEGIN buzzerController */
  Buzzer_Init();
  osDelay(1000);
  /* Infinite loop */
  for (;;)
  {
    if (alarmFlag == 1 && alarmNoticationCounter > 0)
    {
      Buzzer_PlayMusic(5,1);
      alarmNoticationCounter--;
    }

    if (countdownFlag == 1 && countdownNoticationCounter > 0)
    {
      Buzzer_PlayMusic(5,1);
      countdownNoticationCounter--;
    }
    osDelay(1);
  }
  /* USER CODE END buzzerController */
}

/* USER CODE BEGIN Header_blinkController */
/**
 * @brief Function implementing the blinkTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_blinkController */
void blinkController(void const *argument)
{
  /* USER CODE BEGIN blinkController */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);


  //Put it here

  /* Infinite loop */
  for (;;)
  {
    if (clockCurrentStatus.operatingMode == ClockMode_Debug) // Debug Mode
    {
      if (HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin) == GPIO_PIN_SET)
      {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
      }
      else
      {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
      }
      osDelay(1000);
    }
    osDelay(1);
  }
  /* USER CODE END blinkController */
}

/* USER CODE BEGIN Header_inputControllerTask */
/**
 * @brief Function implementing the inputController thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_inputControllerTask */
void inputControllerTask(void const *argument)
{
  /* USER CODE BEGIN inputControllerTask */

  /* Infinite loop */
  for (;;)
  {
    BUTTON_Update();

    if (BUTTON_keyStatus[0] == Button_Released)
    {
      input1FreezerFlag = 0;
    }
    if (BUTTON_keyStatus[1] == Button_Released)
    {
      input2FreezerFlag = 0;
    }
    if (BUTTON_keyStatus[2] == Button_Released)
    {
      input3FreezerFlag = 0;
    }
    if (BUTTON_keyStatus[3] == Button_Released)
    {
      input4FreezerFlag = 0;
    }
    if (BUTTON_keyStatus[4] == Button_Released)
    {
      input5FreezerFlag = 0;
    }
    if (ENCODER_data.xValue == 0)
    {
      input6FreezerFlag = 0;
    }
    // ##########################################################################################
    if (clockCurrentStatus.operatingMode == ClockMode_Normal) // Normal Mode
    {
      if (BUTTON_keyStatus[0] == Button_Pressed && input1FreezerFlag == 0) // Countdown Shotcut
      {
        clockCurrentStatus.operatingMode = ClockMode_Countdown;
        countdownSettingMins = 0;
        countdownSettingSecs = 15;
        IRTC_SetCountdownCounter(0, 15);
        IRTC_StartCountdown();
      }
      else if (BUTTON_keyStatus[1] == Button_Pressed && input2FreezerFlag == 0) // Press K3 to enter CountdownMode
      {
        clockCurrentStatus.operatingMode = ClockMode_Countdown;
      }
      else if (BUTTON_keyStatus[2] == Button_Pressed && input3FreezerFlag == 0) // Press K4 to enter AlarmMode
      {
        clockCurrentStatus.operatingMode = ClockMode_Alarm;
      }
      else if (BUTTON_keyStatus[3] == Button_Pressed && input4FreezerFlag == 0) // Press Encoder Button to enter Setting Mode
      {
        clockCurrentStatus.operatingMode = ClockMode_Setting;
      }
    }
    // ##########################################################################################
    else if (clockCurrentStatus.operatingMode == ClockMode_Countdown) // Countdown Mode
    {
      if (clockCurrentStatus.countdownModeStatus == Counter_Pause) // Not start counting
      {

        if (input6FreezerFlag == 0) // Encoder
        {
          if (countdownModeToggleFlag == 1)
          {
            countdownSettingSecs += ENCODER_data.xValue;
            if (countdownSettingSecs >= 60 || countdownSettingSecs <= -1)
            {
              countdownSettingSecs = 0;
            }
          }
          else
          {
            countdownSettingMins += ENCODER_data.xValue;
            if (countdownSettingMins >= 100 || countdownSettingMins <= -1)
            {
              countdownSettingMins = 0;
            }
          }
          IRTC_SetCountdownCounter(countdownSettingMins, countdownSettingSecs);
        }

        if (BUTTON_keyStatus[0] == Button_Pressed && input1FreezerFlag == 0)
        {
          if (countdownModeToggleFlag == 1)
          {
            countdownModeToggleFlag = 0;
          }
          else
          {
            countdownModeToggleFlag = 1;
          }
        }
        else if (BUTTON_keyStatus[1] == Button_Pressed && input2FreezerFlag == 0)
        {
          if (countdownModeToggleFlag == 1)
          {
            if (countdownSettingSecs >= 60)
            {
              countdownSettingSecs = 0;
            }
            else
            {
              countdownSettingSecs++;
            }
          }
          else
          {
            if (countdownSettingMins >= 100)
            {
              countdownSettingMins = 0;
            }
            else
            {
              countdownSettingMins++;
            }
          }
          IRTC_SetCountdownCounter(countdownSettingMins, countdownSettingSecs);
        }
        else if (BUTTON_keyStatus[2] == Button_Pressed && input3FreezerFlag == 0)
        {
          IRTC_StartCountdown();
        }
        else if (BUTTON_keyStatus[3] == Button_Pressed && input4FreezerFlag == 0)
        {
          clockCurrentStatus.operatingMode = ClockMode_Normal;
        }
      }
      else // Start counting
      {
        if (BUTTON_keyStatus[0] == Button_Pressed && input1FreezerFlag == 0)
        {
          IRTC_StopCountdown();
          countdownSettingMins = IRTC_GetCountdownRemainingTime() / 60;
          countdownSettingSecs = IRTC_GetCountdownRemainingTime() % 60;
        }
        else if (BUTTON_keyStatus[1] == Button_Pressed)
        {
        }
        else if (BUTTON_keyStatus[2] == Button_Pressed)
        {
        }
        else if (BUTTON_keyStatus[3] == Button_Pressed)
        {
        }
      }
    }
    // ##########################################################################################
    else if (clockCurrentStatus.operatingMode == ClockMode_Alarm) // Alarm Mode
    {
      if (input6FreezerFlag == 0) // Encoder
      {
        if (alarmModeToggleFlag == 1)
        {
          alarmSettingMins += ENCODER_data.xValue;
          if (alarmSettingMins >= 60 || alarmSettingMins <= -1)
          {
            alarmSettingMins = 0;
          }
        }
        else
        {
          alarmSettingHours += ENCODER_data.xValue;
          if (alarmSettingHours >= 24 || alarmSettingHours <= -1)
          {
            alarmSettingHours = 0;
          }
        }
        IRTC_SetAlarm(0, alarmSettingHours, alarmSettingMins);
      }

      if (BUTTON_keyStatus[0] == Button_Pressed && input1FreezerFlag == 0)
      {
        if (alarmModeToggleFlag == 1)
        {
          alarmModeToggleFlag = 0;
        }
        else
        {
          alarmModeToggleFlag = 1;
        }
      }
      else if (BUTTON_keyStatus[1] == Button_Pressed && input2FreezerFlag == 0)
      {
        if (alarmModeToggleFlag == 1)
        {
          if (alarmSettingHours >= 24)
          {
            alarmSettingHours = 0;
          }
          else
          {
            alarmSettingHours++;
          }
        }
        else
        {
          if (alarmSettingMins >= 60)
          {
            alarmSettingMins = 0;
          }
          else
          {
            alarmSettingMins++;
          }
        }
        IRTC_SetAlarm(0, alarmSettingHours, alarmSettingMins);
      }
      else if (BUTTON_keyStatus[2] == Button_Pressed && input3FreezerFlag == 0)
      {
        if (clockUserSetting.enableAlarm == Alarm_ON)
        {
          clockUserSetting.enableAlarm = Alarm_OFF;
        }
        else
        {
          clockUserSetting.enableAlarm = Alarm_ON;
        }
      }
      else if (BUTTON_keyStatus[3] == Button_Pressed && input4FreezerFlag == 0)
      {
        IRTC_SaveUserSetting();
        clockCurrentStatus.operatingMode = ClockMode_Normal;
      }
    }
    // ##########################################################################################
    else if (clockCurrentStatus.operatingMode == ClockMode_Setting) // Setting Mode
    {
      if (BUTTON_keyStatus[0] == Button_Pressed && input1FreezerFlag == 0)
      {
        settingModeFlag++;
        if (settingModeFlag >= 9)
        {
          settingModeFlag = 0;
        }
      }
      else if (BUTTON_keyStatus[1] == Button_Pressed && input2FreezerFlag == 0)
      {
        if (settingModeFlag == 0)
        {
          if (clockUserSetting.enableAutoBrightness == 0)
          {
            clockUserSetting.brightness += 5;
            if (clockUserSetting.brightness >= 100)
            {
              clockUserSetting.brightness = 0;
            }
          }
        }
        else if (settingModeFlag == 1)
        {
          clockUserSetting.timeZone++;
          if (clockUserSetting.timeZone >= 14)
          {
            clockUserSetting.timeZone = 0;
          }
        }
        else if (settingModeFlag == 2)
        {
          if (clockUserSetting.enableNetworkSync == 0)
          {
          }
          else
          {
            clockUserSetting.networkTimeSyncInterval++;
            if (clockUserSetting.networkTimeSyncInterval >= 60)
            {
              clockUserSetting.networkTimeSyncInterval = 1;
            }
          }
        }
        else if (settingModeFlag == 3)
        {
          clockUserSetting.nixieTubeRefreshInterval++;
          if (clockUserSetting.nixieTubeRefreshInterval >= 60)
          {
            clockUserSetting.nixieTubeRefreshInterval = 1;
          }
        }
        else if (settingModeFlag == 4)
        {
          clockUserSetting.aht10RefreshInterval++;
          if (clockUserSetting.aht10RefreshInterval >= 60)
          {
            clockUserSetting.aht10RefreshInterval = 1;
          }
        }
        else if (settingModeFlag == 7)
        {

          if (configDateTimeFlag == 0)
          {
            new_iRTC_dateTime.Year++;
            if (new_iRTC_dateTime.Year >= 100)
            {
              new_iRTC_dateTime.Year = 0;
            }
          }
          else if (configDateTimeFlag == 1)
          {
            new_iRTC_dateTime.Month++;
            if (new_iRTC_dateTime.Month >= 13)
            {
              new_iRTC_dateTime.Month = 1;
            }
          }
          else if (configDateTimeFlag == 2)
          {
            new_iRTC_dateTime.Date++;
            if (new_iRTC_dateTime.Date >= 32)
            {
              new_iRTC_dateTime.Date = 1;
            }
          }
          else if (configDateTimeFlag == 3)
          {
            new_iRTC_dateTime.WeekDay++;
            if (new_iRTC_dateTime.WeekDay >= 6)
            {
              new_iRTC_dateTime.WeekDay = 0;
            }
          }
          else if (configDateTimeFlag == 4)
          {
            new_iRTC_dateTime.Hours++;
            if (new_iRTC_dateTime.Hours >= 24)
            {
              new_iRTC_dateTime.Hours = 0;
            }
          }
          else if (configDateTimeFlag == 5)
          {
            new_iRTC_dateTime.Minutes++;
            if (new_iRTC_dateTime.Minutes >= 60)
            {
              new_iRTC_dateTime.Minutes = 0;
            }
          }
          else if (configDateTimeFlag == 6)
          {
            new_iRTC_dateTime.Seconds++;
            if (new_iRTC_dateTime.Seconds >= 60)
            {
              new_iRTC_dateTime.Seconds = 0;
            }
          }
        }
        else if (settingModeFlag == 8)
        {
          if (clockUserSetting.enableAida64Monitor == 0)
          {
            clockUserSetting.enableAida64Monitor = 1;
          }
          else
          {
            clockUserSetting.enableAida64Monitor = 0;
          }
        }
      }
      else if (BUTTON_keyStatus[2] == Button_Pressed && input3FreezerFlag == 0)
      {
        if (settingModeFlag == 0)
        {
          if (clockUserSetting.enableAutoBrightness == 0)
          {
            clockUserSetting.enableAutoBrightness = 1;
          }
          else
          {
            clockUserSetting.enableAutoBrightness = 0;
          }
        }
        else if (settingModeFlag == 1)
        {
          if (clockUserSetting.timeZonePosNeg == 0)
          {
            clockUserSetting.timeZonePosNeg = 1;
          }
          else
          {
            clockUserSetting.timeZonePosNeg = 0;
          }
        }
        else if (settingModeFlag == 2)
        {
          if (clockUserSetting.enableNetworkSync == 0)
          {
            clockUserSetting.enableNetworkSync = 1;
          }
          else
          {
            clockUserSetting.enableNetworkSync = 0;
          }
        }
        else if (settingModeFlag == 3)
        {
        }
        else if (settingModeFlag == 4)
        {
        }
        else if (settingModeFlag == 7)
        {
          configDateTimeFlag++;
          if (configDateTimeFlag >= 8)
          {
            configDateTimeFlag = 0;
          }
        }
      }
      else if (BUTTON_keyStatus[3] == Button_Pressed && input4FreezerFlag == 0)
      {
        if (settingModeFlag == 5)
        {
          IRTC_ResetUserSetting();
          HAL_PWR_EnableBkUpAccess();
          uint32_t defaultDateTime = 1704038370; // Sun Dec 31 2023 23:59:30 GMT+0800 (Hong Kong Standard Time)
          IRTC_WriteTimeCounter_Custom(&hrtc, defaultDateTime);
          HAL_PWR_DisableBkUpAccess();
          clockCurrentStatus.operatingMode = ClockMode_Normal;
        }
        else if (settingModeFlag == 6)
        {
          clockCurrentStatus.operatingMode = ClockMode_Debug;
        }
        else if (settingModeFlag == 7)
        {
          HAL_PWR_EnableBkUpAccess();
          struct tm stmT;
          stmT.tm_year = new_iRTC_dateTime.Year + 100;
          stmT.tm_mon = new_iRTC_dateTime.Month - 1;
          stmT.tm_mday = new_iRTC_dateTime.Date;
          if (clockUserSetting.timeZonePosNeg == 0)
          {
            stmT.tm_hour = new_iRTC_dateTime.Hours - clockUserSetting.timeZone;
          }
          else
          {
            stmT.tm_hour = new_iRTC_dateTime.Hours + clockUserSetting.timeZone;
          }
          stmT.tm_min = new_iRTC_dateTime.Minutes;
          stmT.tm_sec = new_iRTC_dateTime.Seconds;

          uint32_t newDateTime = mktime(&stmT);
          IRTC_WriteTimeCounter_Custom(&hrtc, newDateTime);
          HAL_PWR_DisableBkUpAccess();
          clockCurrentStatus.operatingMode = ClockMode_Normal;
          nixieTubeRefreshCounter = clockUserSetting.nixieTubeRefreshInterval * 60000;
        }
        else
        {
          nixieTubeRefreshCounter = clockUserSetting.nixieTubeRefreshInterval * 60000;
          IRTC_SaveUserSetting();
          clockCurrentStatus.operatingMode = ClockMode_Normal;
        }
      }
    }
    // ##########################################################################################
    else if (clockCurrentStatus.operatingMode == ClockMode_Debug) // Debug Mode
    {
      if (BUTTON_keyStatus[0] == Button_Pressed && input1FreezerFlag == 0) // Test Buzzer
      {
        alarmNoticationCounter = 1;
        alarmFlag = 1;
      }
      else if (BUTTON_keyStatus[1] == Button_Pressed && input2FreezerFlag == 0) // Test VFD
      {
        vfdTestFlag++;
        if (vfdTestFlag >= 3)
        {
          vfdTestFlag = 0;
        }
      }
      else if (BUTTON_keyStatus[2] == Button_Pressed && input3FreezerFlag == 0) // Test NixieTube
      {
        nixieTubeTestFlag++;
        if (nixieTubeTestFlag >= 2)
        {
          nixieTubeTestFlag = 0;
        }
      }
      else if (BUTTON_keyStatus[3] == Button_Pressed && input4FreezerFlag == 0) // Test Write BK reg
      {
        // clockUserSetting.otherSetting += 5;
        // IRTC_SaveUserSetting();
        // saveSettingFlag++;
        IRTC_ReadUserSetting();
      }
      else if (BUTTON_keyStatus[4] == Button_Pressed) // Test reset DateTime
      {
        IRTC_ResetUserSetting();
        HAL_PWR_EnableBkUpAccess();
        uint32_t defaultDateTime = 1704038370; // Sun Dec 31 2023 23:59:30 GMT+0800 (Hong Kong Standard Time)
        IRTC_WriteTimeCounter_Custom(&hrtc, defaultDateTime);
        HAL_PWR_DisableBkUpAccess();
      }
    }
    // ##########################################################################################
    if (BUTTON_keyStatus[0] == Button_Pressed)
    {
      input1FreezerFlag = 1;
    }
    if (BUTTON_keyStatus[1] == Button_Pressed)
    {
      input2FreezerFlag = 1;
    }
    if (BUTTON_keyStatus[2] == Button_Pressed)
    {
      input3FreezerFlag = 1;
    }
    if (BUTTON_keyStatus[3] == Button_Pressed)
    {
      input4FreezerFlag = 1;
    }
    if (BUTTON_keyStatus[4] == Button_Pressed)
    {
      input5FreezerFlag = 1;
    }

    if (ENCODER_data.xValue != 0)
    {
      input6FreezerFlag = 1;
    }
    osDelay(1);
  }
  /* USER CODE END inputControllerTask */
}

/* USER CODE BEGIN Header_ecControllerTask */
/**
 * @brief Function implementing the ecController thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_ecControllerTask */
void ecControllerTask(void const *argument)
{
  /* USER CODE BEGIN ecControllerTask */
  ENCODER_Init();
  /* Infinite loop */
  for (;;)
  {
    ENCODER_Update();
    osDelay(1);
  }
  /* USER CODE END ecControllerTask */
}

/* USER CODE BEGIN Header_ttlControllerTask */
/**
 * @brief Function implementing the ttlController thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_ttlControllerTask */
void ttlControllerTask(void const *argument)
{
  /* USER CODE BEGIN ttlControllerTask */
  /* Infinite loop */
  for (;;)
  {
    if (clockUserSetting.enableAida64Monitor == 1)
    {
      USBTTL_GetAida64Data();
      osDelay(1000);
    }
    osDelay(1);
  }
  /* USER CODE END ttlControllerTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
