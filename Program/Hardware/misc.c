/*
 * misc.c
 *
 *  Created on: Nov 1, 2023
 *      Author: Angus
 */
#include "misc.h"
double clamp_double(double d, double min, double max)
{
  const double t = d < min ? min : d;
  return t > max ? max : t;
}

float clamp_float(float d, float min, float max)
{
  const float t = d < min ? min : d;
  return t > max ? max : t;
}

int clamp_int(int d, int min, int max)
{
  const int t = d < min ? min : d;
  return t > max ? max : t;
}

short clamp_short(short d, short min, short max)
{
  const short t = d < min ? min : d;
  return t > max ? max : t;
}

uint16_t u8_to_u16(uint8_t data1, uint8_t data2)
{
  return (data2 << 8)+data1;
}

void u16_to_u8(uint16_t u, uint8_t* bytes) {
  bytes[0] = u;
  bytes[1] = u >> 8;
}
