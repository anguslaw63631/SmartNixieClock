/*
 * LDR.h
 *
 *  Created on: Oct 24, 2023
 *      Author: Angus
 */
#ifndef __LDR_H__
#define __LDR_H__
#include "HardwareConfig.h"
#include "stm32f1xx_hal.h"
#include "adc.h"

extern uint32_t LDR_value;
extern uint8_t LDR_brightness;
void LDR_Init(void);

uint32_t LDR_GetValue(void);

void LDR_Update(void);

#endif