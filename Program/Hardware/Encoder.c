/*
 * Encoder.c
 *
 *  Created on: Oct 22, 2023
 *      Author: Angus
 */
#include "Encoder.h"

ENCODER_Data ENCODER_data = {0};

void ENCODER_Init(void)
{
  HAL_TIM_Encoder_Start(&ENCODER_TIM, ENCODER_TIM_CH1 | ENCODER_TIM_CH2);
  __HAL_TIM_SET_COUNTER(&ENCODER_TIM, 0);
}

int16_t Encoder_GetValue(void)
{
  int16_t temp = __HAL_TIM_GET_COUNTER(&ENCODER_TIM);

  if (temp < 0)
  {
    temp = -temp;
  }
  osDelay(75);
  int16_t temp2 = __HAL_TIM_GET_COUNTER(&ENCODER_TIM);
  if (temp2 < 0)
  {
    temp2 = -temp2;
  }

  int16_t temp3 = temp2 - temp;
  if (temp3 < 0)
  {
    temp3 = -temp3;
  }

  return temp3;
}

int8_t Encoder_GetDirection(void)
{
  return __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM);
}

void ENCODER_GetData(int16_t *value, int8_t *direction)
{
  *direction = Encoder_GetDirection();
  *value = Encoder_GetValue();
}

void ENCODER_Update(void)
{
  if(ENCODER_data.value == 0){
    ENCODER_data.xValue = 0;
  }
  ENCODER_GetData(&ENCODER_data.value, &ENCODER_data.direction);
  if (ENCODER_data.value != 0)
  {
    if (ENCODER_data.direction == 0)
    {
      ENCODER_data.xValue = -ENCODER_data.value/2;
    }
    else
    {
      ENCODER_data.xValue = ENCODER_data.value/2;
    }
  }
}