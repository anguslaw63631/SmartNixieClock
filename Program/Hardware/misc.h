/*
 * misc.h
 *
 *  Created on: Nov 1, 2023
 *      Author: Angus
 */

#ifndef __misc_H__
#define __misc_H__
#include "time.h"

#define msMin 60000
double clamp_double(double d, double min, double max);
float clamp_float(float d, float min, float max);
int clamp_int(int d, int min, int max);
short clamp_short(short d, short min, short max);

uint16_t u8_to_u16(uint8_t data1, uint8_t data2);
void u16_to_u8(uint16_t u, uint8_t* bytes);

#endif