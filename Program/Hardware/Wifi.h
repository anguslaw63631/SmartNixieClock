#ifndef _WIFI_H
#define _WIFI_H

#include "HardwareConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "usart.h"
#include "gpio.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef enum
{
  WifiMode_Error = 0,
  WifiMode_Station = 1,
  WifiMode_SoftAp = 2,
  WifiMode_StationAndSoftAp = 3,
} WifiMode_t;
typedef enum
{
  WifiConnectionStatus_Error = 0,
  WifiConnectionStatus_GotIp = 2,
  WifiConnectionStatus_Connected = 3,
  WifiConnectionStatus_Disconnected = 4,
  WifiConnectionStatus_ConnectionFail = 5,
} WifiConnectionStatus_t;

typedef struct
{
  WifiConnectionStatus_t status;
  uint8_t LinkId;
  char Type[4];
  char RemoteIp[17];
  uint16_t RemotePort;
  uint16_t LocalPort;
  bool RunAsServer;
} WifiConnection_t;

typedef struct
{
  //----------------Usart	Paremeter
  uint8_t usartBuff;
  uint8_t RxBuffer[_WIFI_RX_SIZE];
  uint8_t TxBuffer[_WIFI_TX_SIZE];
  uint16_t RxIndex;
  uint8_t RxBufferForData[_WIFI_RX_FOR_DATA_SIZE];
  uint8_t RxBufferForDataTmp[8];
  uint8_t RxIndexForDataTmp;
  uint16_t RxIndexForData;
  uint16_t RxDataLen;
  uint8_t RxDataConnectionNumber;
  uint32_t RxDataLastTime;
  bool RxIsData;
  bool GotNewData;
  //----------------General	Parameter
  WifiMode_t Mode;
  char MyIP[16];
  char MyGateWay[16];
  //----------------TcpIp Parameter
  bool TcpIpMultiConnection;
  WifiConnection_t TcpIpConnections[5];
} Wifi_t;

extern Wifi_t Wifi;
extern uint8_t networkTimeStamp[10];

void Wifi_RxCallBack(void);
void Wifi_Init(void);
bool Wifi_SetRfPower(uint8_t Power_0_to_82);
bool Wifi_SetMode(WifiMode_t WifiMode_);
bool Wifi_GetMode(void);
bool Wifi_GetMyIp(void);
bool Wifi_Station_ConnectToAp(char *SSID, char *Pass, char *MAC);
bool Wifi_Station_Disconnect(void);
bool Wifi_TcpIp_GetConnectionStatus(void);
bool Wifi_TcpIp_SetMultiConnection(bool EnableMultiConnections);
bool Wifi_TcpIp_GetMultiConnection(void);
bool Wifi_TcpIp_StartTcpConnection(uint8_t LinkId, char *RemoteIp, uint16_t RemotePort, uint16_t TimeOut_S);
bool Wifi_TcpIp_Close(uint8_t LinkId);
bool Wifi_TcpIp_SendDataTcp(uint8_t LinkId, uint16_t dataLen, uint8_t *data);

#endif
