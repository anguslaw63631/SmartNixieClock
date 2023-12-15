/*
 * AHT10.h
 *
 *  Created on: Oct 22, 2023
 *      Author: Angus
 */

#ifndef __AHT10_H__
#define __AHT10_H__

#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "HardwareConfig.h"

#define AHT10_ADDRESS 0x70

#define Initialize_Command 0xE1
#define Measure_Trig_Command 0xAC
#define Soft_Reset_Command 0xBA

typedef struct
{
    float temperature;
    float humidity;
} AHT10_Data;

extern AHT10_Data aht10Data;

void AHT10_Init(void);
void AHT10_Reset(void);

uint8_t AHT10_Measure(float *pTemperature, float *pHumidity);
void AHT10_Trig_Measure(void);
uint8_t AHT10_Read_Data(uint8_t *pOriginData);

float AHT10_GetHumidity(void);
float AHT10_GetTemperature(void);

void AHT10_Update(void);

#endif
