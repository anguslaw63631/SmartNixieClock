/*
 * USBTTL.c
 *
 *  Created on: Nov 21, 2023
 *      Author: Angus
 */
#include "USBTTL.h"

USBTTL_t USBTTL;

int obtainAida64Num()
{
    char str[_TTL_RX_SIZE];
    char *endptr;
    memcpy(str, USBTTL.aida64.aida64Buffer, _TTL_RX_SIZE);
    for (int i = 0; i < _TTL_RX_SIZE; i++)
    {
        if (isdigit(str[i]) == 0)
        {
            str[i] = ' ';
        }
    }
    return strtol(str, &endptr, 10);
}

void USBTTL_RXCallBack()
{
    if (USBTTL.RxIndex >= _TTL_RX_SIZE)
    {
        memcpy(USBTTL.aida64.aida64Buffer, USBTTL.RxBuffer, _TTL_RX_SIZE);
        USBTTL.RxIndex = 0;
        memset(USBTTL.RxBuffer, 0, _TTL_RX_SIZE);
    }

    USBTTL.RxBuffer[USBTTL.RxIndex] = USBTTL.usartBuff;
    if (USBTTL.RxIndex < _TTL_RX_SIZE)
        USBTTL.RxIndex++;
    HAL_UART_Receive_IT(&_TTL_USART, &USBTTL.usartBuff, 1);
}

void USBTTL_GetAida64Data()
{
    HAL_UART_Receive_IT(&_TTL_USART, &USBTTL.usartBuff, 1);
    osDelay(10);
    int temp = NULL;
    int num = 0;
    int count = 0;
    int i = 0;
    char *str = USBTTL.aida64.aida64Buffer;

    if (strchr(USBTTL.aida64.aida64Buffer, 'A') != NULL)
    {
        USBTTL.aida64.cpuUsage = obtainAida64Num();
    }
    else if (strchr(USBTTL.aida64.aida64Buffer, 'B') != NULL)
    {
        USBTTL.aida64.ramUsage = obtainAida64Num();
    }
    else if (strchr(USBTTL.aida64.aida64Buffer, 'C') != NULL)
    {
        USBTTL.aida64.gpuUsage = obtainAida64Num();
    }
    else if (strchr(USBTTL.aida64.aida64Buffer, 'D') != NULL)
    {
        USBTTL.aida64.vramUsage = obtainAida64Num();
    }
}