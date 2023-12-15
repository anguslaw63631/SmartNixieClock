/*
 * Encoder.h
 *
 *  Created on: Oct 22, 2023
 *      Author: Angus
 */
#ifndef __ENCODER_H__
#define __ENCODER_H__
#include "HardwareConfig.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

typedef struct
{
    int16_t value;
    int8_t direction;
    int16_t xValue;
} ENCODER_Data;

extern ENCODER_Data ENCODER_data;

void ENCODER_Init(void);

int8_t ENCODER_GetValue(void);

int8_t ENCODER_GetDirection(void);
void ENCODER_GetData(int16_t *value, int8_t *direction);
void ENCODER_Update(void);
#endif