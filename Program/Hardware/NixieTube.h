/*
 * NixieTube.h
 *
 *  Created on: Oct 22, 2023
 *      Author: Angus
 */
#ifndef __NixieTube_H__
#define __NixieTube_H__
#include "HardwareConfig.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "spi.h"

#define USE_13001 1

void NixieTube_Init(void);

void NixieTube_SetTube1(uint8_t number);
void NixieTube_SetTube2(uint8_t number);

void NixieTube_SetColon(uint8_t number);

void NixieTube_Update(void);
void NixieTube_Update2(void);

void HC595_SendData(uint8_t *data);
void HC595_SendData2(uint8_t *data);

void NixieTube_SetBrightness(uint8_t dutyCycle);

#endif