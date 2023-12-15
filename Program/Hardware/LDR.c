/*
 * LDR.c
 *
 *  Created on: Oct 24, 2023
 *      Author: Angus
 */
#include "LDR.h"

uint32_t LDR_value = 0;
uint8_t LDR_brightness = 0;

void LDR_Init(void)
{
    HAL_ADCEx_Calibration_Start(&LDR_ADC);
    HAL_ADC_Start(&LDR_ADC);
}

uint32_t LDR_GetValue(void)
{
    if (HAL_ADC_PollForConversion(&LDR_ADC, 100) == HAL_OK)
    {
        return HAL_ADC_GetValue(&LDR_ADC);
    }
    else
        return 0;
}

/// @brief Update LDR brightness
void LDR_Update(void)
{
    LDR_value = LDR_GetValue();
    short s = (short)(LDR_value & 0xFFFF);
    float f = s / 4096.0 * 100.0;
    LDR_brightness = clamp_short(100 - (short)f, 0, 100);
}