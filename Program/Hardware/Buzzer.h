/*
 * Buzzer.h
 *
 *  Created on: Oct 24, 2023
 *      Author: Angus
 */
#ifndef __Buzzer_H__
#define __Buzzer_H__
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "HardwareConfig.h"

#define proport 72000 // Tclk/(arr+1)=72000000/(1000)

// Tout(psc)= ((arr+1)*(psc+1))/Tclk

// Tone List
#define L1 ((proport / 262) - 1) // do
#define L2 ((proport / 296) - 1) // re
#define L3 ((proport / 330) - 1) // mi
#define L4 ((proport / 349) - 1) // fa
#define L5 ((proport / 392) - 1) // sol
#define L6 ((proport / 440) - 1) // la
#define L7 ((proport / 494) - 1) // si

#define M1 ((proport / 523) - 1)
#define M2 ((proport / 587) - 1)
#define M3 ((proport / 659) - 1)
#define M4 ((proport / 699) - 1)
#define M5 ((proport / 784) - 1)
#define M6 ((proport / 880) - 1)
#define M7 ((proport / 988) - 1)

#define H1 ((proport / 1048) - 1)
#define H2 ((proport / 1176) - 1)
#define H3 ((proport / 1320) - 1)
#define H4 ((proport / 1480) - 1)
#define H5 ((proport / 1640) - 1)
#define H6 ((proport / 1760) - 1)
#define H7 ((proport / 1976) - 1)

// small tune for music
#define X1 ((proport / 415) - 1)
#define X2 ((proport / 622) - 1)
#define X3 ((proport / 740) - 1)
#define X4 ((proport / 831) - 1)

#define X5 ((proport / 370) - 1)
#define X6 ((proport / 554) - 1)
#define X7 ((proport / 277) - 1)

#define Z0 0 // OFF

void Buzzer_Init(void);

void Buzzer_PlayMusic(uint16_t volume,uint8_t musicTrack);

extern void Buzzer_On(uint16_t psc, uint16_t pwm);
extern void Buzzer_Off(void);

#endif