/*
 * USBTTL.h
 *
 *  Created on: Nov 21, 2023
 *      Author: Angus
 */
#ifndef __USBTTL_H__
#define __USBTTL_H__

#include "HardwareConfig.h"
#include "stm32f1xx_hal.h"
#include "usart.h"

typedef struct
{
  uint8_t aida64Buffer[_TTL_RX_SIZE];
  uint8_t cpuUsage;
  uint8_t ramUsage;
  uint8_t gpuUsage;
  uint8_t vramUsage;
} AIDA64_t;
typedef struct
{
  //----------------Usart	Paremeter
  uint8_t usartBuff;
  uint8_t RxBuffer[_TTL_RX_SIZE];
  uint16_t RxIndex;
  //----------------Aida64 Data
  AIDA64_t aida64;
} USBTTL_t;



extern USBTTL_t USBTTL;

void USBTTL_RXCallBack();

void USBTTL_GetAida64Data();


#endif