/*
 * NixieTube.c
 *
 *  Created on: Oct 22, 2023
 *      Author: Angus
 */
#include "NixieTube.h"

// 2 for each tube , 1 for colon
uint8_t NixieTube_Raw_Data[4];
uint8_t NixieTube_Data[3];

uint8_t Tube_OFF = 0b00000000;
//===============START OF TUBE 1===========================
// A = IC1 B = IC2
uint8_t Tube1_Number_0_A = 0b00000001;
uint8_t Tube1_Number_1_A = 0b00000010;
uint8_t Tube1_Number_2_A = 0b00000100;
uint8_t Tube1_Number_3_A = 0b00001000;
uint8_t Tube1_Number_4_A = 0b00010000;
uint8_t Tube1_Number_5_A = 0b00100000;
uint8_t Tube1_Number_6_A = 0b01000000;
uint8_t Tube1_Number_7_A = 0b10000000;
uint8_t Tube1_Number_8_A = 0b00000000;
uint8_t Tube1_Number_9_A = 0b00000000;
uint8_t Tube1_Number_DP_A = 0b00000000;

uint8_t Tube1_Number_0_B = 0b00000000;
uint8_t Tube1_Number_1_B = 0b00000000;
uint8_t Tube1_Number_2_B = 0b00000000;
uint8_t Tube1_Number_3_B = 0b00000000;
uint8_t Tube1_Number_4_B = 0b00000000;
uint8_t Tube1_Number_5_B = 0b00000000;
uint8_t Tube1_Number_6_B = 0b00000000;
uint8_t Tube1_Number_7_B = 0b00000000;
uint8_t Tube1_Number_8_B = 0b00000001;
uint8_t Tube1_Number_9_B = 0b00000010;
uint8_t Tube1_Number_DP_B = 0b00000100;

//===============START OF TUBE 2===========================
// A = IC2 B = IC3
uint8_t Tube2_Number_0_A = 0b00001000;
uint8_t Tube2_Number_1_A = 0b00010000;
uint8_t Tube2_Number_2_A = 0b00100000;
uint8_t Tube2_Number_3_A = 0b01000000;
uint8_t Tube2_Number_4_A = 0b10000000;
uint8_t Tube2_Number_5_A = 0b00000000;
uint8_t Tube2_Number_6_A = 0b00000000;
uint8_t Tube2_Number_7_A = 0b00000000;
uint8_t Tube2_Number_8_A = 0b00000000;
uint8_t Tube2_Number_9_A = 0b00000000;
uint8_t Tube2_Number_DP_A = 0b00000000;

uint8_t Tube2_Number_0_B = 0b00000000;
uint8_t Tube2_Number_1_B = 0b00000000;
uint8_t Tube2_Number_2_B = 0b00000000;
uint8_t Tube2_Number_3_B = 0b00000000;
uint8_t Tube2_Number_4_B = 0b00000000;
uint8_t Tube2_Number_5_B = 0b00000001;
uint8_t Tube2_Number_6_B = 0b00000010;
uint8_t Tube2_Number_7_B = 0b00000100;
uint8_t Tube2_Number_8_B = 0b00001000;
uint8_t Tube2_Number_9_B = 0b00010000;
uint8_t Tube2_Number_DP_B = 0b00100000;

void HC595_SendData(uint8_t *data)
{
  HAL_GPIO_WritePin(NIXIETUBE_CS_PORT, NIXIETUBE_CS_PIN, GPIO_PIN_RESET);

  osDelay(10);
  // Transmit data via SPI
  HAL_SPI_Transmit(&NIXIETUBE_SPI, data, 3, 300);

  // Pull CS low to start data transmission

  // Pull CS high to latch data
  HAL_GPIO_WritePin(NIXIETUBE_CS_PORT, NIXIETUBE_CS_PIN, GPIO_PIN_SET);

  osDelay(10);
}

void HC595_SendData2(uint8_t *data)
{
  HAL_GPIO_WritePin(NIXIETUBE_CS2_PORT, NIXIETUBE_CS2_PIN, GPIO_PIN_RESET);

  osDelay(10);

  // Transmit data via SPI
  HAL_SPI_Transmit(&NIXIETUBE_SPI, data, 3, 300);

  // Pull CS low to start data transmission

  // Pull CS high to latch data
  HAL_GPIO_WritePin(NIXIETUBE_CS2_PORT, NIXIETUBE_CS2_PIN, GPIO_PIN_SET);

  osDelay(10);
}

/// @brief Init NixieTube
void NixieTube_Init(void)
{
  HAL_TIM_PWM_Start(&NIXIETUBE_TIM, NIXIETUBE_TIM_CH);
  __HAL_TIM_SET_COMPARE(&NIXIETUBE_TIM, NIXIETUBE_TIM_CH, 50 - 1);
}

/// @brief Set brightness of Nixie Tube(0-100)
/// @param dutyCycle 0-100
void NixieTube_SetBrightness(uint8_t dutyCycle)
{
  HAL_TIM_PWM_Stop(&NIXIETUBE_TIM, NIXIETUBE_TIM_CH);
  __HAL_TIM_SET_COMPARE(&NIXIETUBE_TIM, NIXIETUBE_TIM_CH, dutyCycle);
  __HAL_TIM_SET_COUNTER(&NIXIETUBE_TIM, 0);
  HAL_TIM_PWM_Start(&NIXIETUBE_TIM, NIXIETUBE_TIM_CH);
}

void NixieTube_SetTube1(uint8_t number)
{
  switch (number)
  {
  case 0:
    NixieTube_Raw_Data[0] = Tube1_Number_0_A;
    NixieTube_Raw_Data[1] = Tube1_Number_0_B;
    break;
  case 1:
    NixieTube_Raw_Data[0] = Tube1_Number_1_A;
    NixieTube_Raw_Data[1] = Tube1_Number_1_B;
    break;
  case 2:
    NixieTube_Raw_Data[0] = Tube1_Number_2_A;
    NixieTube_Raw_Data[1] = Tube1_Number_2_B;
    break;
  case 3:
    NixieTube_Raw_Data[0] = Tube1_Number_3_A;
    NixieTube_Raw_Data[1] = Tube1_Number_3_B;
    break;
  case 4:
    NixieTube_Raw_Data[0] = Tube1_Number_4_A;
    NixieTube_Raw_Data[1] = Tube1_Number_4_B;
    break;
  case 5:
    NixieTube_Raw_Data[0] = Tube1_Number_5_A;
    NixieTube_Raw_Data[1] = Tube1_Number_5_B;
    break;
  case 6:
    NixieTube_Raw_Data[0] = Tube1_Number_6_A;
    NixieTube_Raw_Data[1] = Tube1_Number_6_B;
    break;
  case 7:
    NixieTube_Raw_Data[0] = Tube1_Number_7_A;
    NixieTube_Raw_Data[1] = Tube1_Number_7_B;
    break;
  case 8:
    NixieTube_Raw_Data[0] = Tube1_Number_8_A;
    NixieTube_Raw_Data[1] = Tube1_Number_8_B;
    break;
  case 9:
    NixieTube_Raw_Data[0] = Tube1_Number_9_A;
    NixieTube_Raw_Data[1] = Tube1_Number_9_B;
    break;
  case 10:
    NixieTube_Raw_Data[0] = Tube1_Number_DP_A;
    NixieTube_Raw_Data[1] = Tube1_Number_DP_B;
    break;
  case 11:
    NixieTube_Raw_Data[0] = Tube_OFF;
    NixieTube_Raw_Data[1] = Tube_OFF;
    break;

  default:
    NixieTube_Raw_Data[0] = Tube1_Number_0_A;
    NixieTube_Raw_Data[1] = Tube1_Number_0_B;
    break;
  }
}

void NixieTube_SetTube2(uint8_t number)
{
  switch (number)
  {
  case 0:
    NixieTube_Raw_Data[2] = Tube2_Number_0_A;
    NixieTube_Raw_Data[3] = Tube2_Number_0_B;
    break;
  case 1:
    NixieTube_Raw_Data[2] = Tube2_Number_1_A;
    NixieTube_Raw_Data[3] = Tube2_Number_1_B;
    break;
  case 2:
    NixieTube_Raw_Data[2] = Tube2_Number_2_A;
    NixieTube_Raw_Data[3] = Tube2_Number_2_B;
    break;
  case 3:
    NixieTube_Raw_Data[2] = Tube2_Number_3_A;
    NixieTube_Raw_Data[3] = Tube2_Number_3_B;
    break;
  case 4:
    NixieTube_Raw_Data[2] = Tube2_Number_4_A;
    NixieTube_Raw_Data[3] = Tube2_Number_4_B;
    break;
  case 5:
    NixieTube_Raw_Data[2] = Tube2_Number_5_A;
    NixieTube_Raw_Data[3] = Tube2_Number_5_B;
    break;
  case 6:
    NixieTube_Raw_Data[2] = Tube2_Number_6_A;
    NixieTube_Raw_Data[3] = Tube2_Number_6_B;
    break;
  case 7:
    NixieTube_Raw_Data[2] = Tube2_Number_7_A;
    NixieTube_Raw_Data[3] = Tube2_Number_7_B;
    break;
  case 8:
    NixieTube_Raw_Data[2] = Tube2_Number_8_A;
    NixieTube_Raw_Data[3] = Tube2_Number_8_B;
    break;
  case 9:
    NixieTube_Raw_Data[2] = Tube2_Number_9_A;
    NixieTube_Raw_Data[3] = Tube2_Number_9_B;
    break;
  case 10:
    NixieTube_Raw_Data[2] = Tube2_Number_DP_A;
    NixieTube_Raw_Data[3] = Tube2_Number_DP_B;
    break;
  case 11:
    NixieTube_Raw_Data[2] = Tube_OFF;
    NixieTube_Raw_Data[3] = Tube_OFF;
    break;

  default:
    NixieTube_Raw_Data[2] = Tube2_Number_0_A;
    NixieTube_Raw_Data[3] = Tube2_Number_0_B;
    break;
  }
}

void NixieTube_SetColon(uint8_t number)
{
  switch (number)
  {
  case 0:
    HAL_GPIO_WritePin(NIXIETUBE_C1_PORT, NIXIETUBE_C1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(NIXIETUBE_C2_PORT, NIXIETUBE_C2_PIN, GPIO_PIN_RESET);
    break;
  case 1:
    HAL_GPIO_WritePin(NIXIETUBE_C1_PORT, NIXIETUBE_C1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(NIXIETUBE_C2_PORT, NIXIETUBE_C2_PIN, GPIO_PIN_SET);
    break;
  case 2:
    HAL_GPIO_WritePin(NIXIETUBE_C1_PORT, NIXIETUBE_C1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(NIXIETUBE_C2_PORT, NIXIETUBE_C2_PIN, GPIO_PIN_RESET);
    break;
  case 3:
    HAL_GPIO_WritePin(NIXIETUBE_C1_PORT, NIXIETUBE_C1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(NIXIETUBE_C2_PORT, NIXIETUBE_C2_PIN, GPIO_PIN_SET);
    break;

  default:
    HAL_GPIO_WritePin(NIXIETUBE_C1_PORT, NIXIETUBE_C1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(NIXIETUBE_C2_PORT, NIXIETUBE_C2_PIN, GPIO_PIN_RESET);
    break;
  }
}

void NixieTube_Update(void)
{
  if (USE_13001 == 1)
  {
    NixieTube_Data[0] = NixieTube_Raw_Data[3];
    NixieTube_Data[1] = (NixieTube_Raw_Data[1] | NixieTube_Raw_Data[2]);
    NixieTube_Data[2] = NixieTube_Raw_Data[0];
  }
  else
  {
    NixieTube_Data[0] = ~NixieTube_Raw_Data[3];
    NixieTube_Data[1] = ~(NixieTube_Raw_Data[1] | NixieTube_Raw_Data[2]);
    NixieTube_Data[2] = ~NixieTube_Raw_Data[0];
  }

  HC595_SendData(NixieTube_Data);
}

void NixieTube_Update2(void)
{
  if (USE_13001 == 1)
  {
    NixieTube_Data[0] = NixieTube_Raw_Data[3];
    NixieTube_Data[1] = (NixieTube_Raw_Data[1] | NixieTube_Raw_Data[2]);
    NixieTube_Data[2] = NixieTube_Raw_Data[0];
  }
  else
  {
    NixieTube_Data[0] = ~NixieTube_Raw_Data[3];
    NixieTube_Data[1] = ~(NixieTube_Raw_Data[1] | NixieTube_Raw_Data[2]);
    NixieTube_Data[2] = ~NixieTube_Raw_Data[0];
  }

  HC595_SendData2(NixieTube_Data);
}