#include "Wifi.h"

osSemaphoreId WifiSemHandle;

Wifi_t Wifi;
uint8_t networkTimeStamp[10] = {0};

bool Wifi_SendRaw(uint8_t *data, uint16_t len)
{
	if (len <= _WIFI_TX_SIZE)
	{
		memcpy(Wifi.TxBuffer, data, len);
		if (HAL_UART_Transmit(&_WIFI_USART, data, len, 100) == HAL_OK)
			return true;
		else
			return false;
	}
	else
		return false;
}
bool Wifi_SendString(char *data)
{
	return Wifi_SendRaw((uint8_t *)data, strlen(data));
}
bool Wifi_SendStringAndWait(char *data, uint16_t DelayMs)
{
	if (Wifi_SendRaw((uint8_t *)data, strlen(data)) == false)
		return false;
	osDelay(DelayMs);
	return true;
}
bool Wifi_WaitForString(uint32_t TimeOut_ms, uint8_t *result, uint8_t CountOfParameter, ...)
{

	if (result == NULL)
		return false;
	if (CountOfParameter == 0)
		return false;

	*result = 0;

	va_list tag;
	va_start(tag, CountOfParameter);
	char *arg[CountOfParameter];
	for (uint8_t i = 0; i < CountOfParameter; i++)
		arg[i] = va_arg(tag, char *);
	va_end(tag);
	for (uint32_t t = 0; t < TimeOut_ms; t += 50)
	{
		osDelay(50);
		for (uint8_t mx = 0; mx < CountOfParameter; mx++)
		{
			if (strstr((char *)Wifi.RxBuffer, arg[mx]) != NULL)
			{
				*result = mx + 1;
				return true;
			}
		}
	}
	return false;
}
bool Wifi_ReturnString(char *result, uint8_t WantWhichOne, char *SplitterChars)
{
	if (result == NULL)
		return false;
	if (WantWhichOne == 0)
		return false;

	char *str = (char *)Wifi.RxBuffer;

	str = strtok(str, SplitterChars);
	if (str == NULL)
	{
		strcpy(result, "");
		return false;
	}
	while (str != NULL)
	{
		str = strtok(NULL, SplitterChars);
		if (str != NULL)
			WantWhichOne--;
		if (WantWhichOne == 0)
		{
			strcpy(result, str);
			return true;
		}
	}
	strcpy(result, "");
	return false;
}
bool Wifi_ReturnStrings(char *InputString, char *SplitterChars, uint8_t CountOfParameter, ...)
{
	if (CountOfParameter == 0)
		return false;
	va_list tag;
	va_start(tag, CountOfParameter);
	char *arg[CountOfParameter];
	for (uint8_t i = 0; i < CountOfParameter; i++)
		arg[i] = va_arg(tag, char *);
	va_end(tag);

	char *str;
	str = strtok(InputString, SplitterChars);
	if (str == NULL)
		return false;
	uint8_t i = 0;
	while (str != NULL)
	{
		str = strtok(NULL, SplitterChars);
		if (str != NULL)
			CountOfParameter--;
		strcpy(arg[i], str);
		i++;
		if (CountOfParameter == 0)
		{
			return true;
		}
	}
	return false;
}
bool Wifi_ReturnInteger(int32_t *result, uint8_t WantWhichOne, char *SplitterChars)
{
	if ((char *)Wifi.RxBuffer == NULL)
		return false;
	if (Wifi_ReturnString((char *)Wifi.RxBuffer, WantWhichOne, SplitterChars) == false)
		return false;
	*result = atoi((char *)Wifi.RxBuffer);
	return true;
}
bool Wifi_ReturnFloat(float *result, uint8_t WantWhichOne, char *SplitterChars)
{
	if ((char *)Wifi.RxBuffer == NULL)
		return false;
	if (Wifi_ReturnString((char *)Wifi.RxBuffer, WantWhichOne, SplitterChars) == false)
		return false;
	*result = atof((char *)Wifi.RxBuffer);
	return true;
}
void Wifi_RemoveChar(char *str, char garbage)
{
	char *src, *dst;
	for (src = dst = str; *src != '\0'; src++)
	{
		*dst = *src;
		if (*dst != garbage)
			dst++;
	}
	*dst = '\0';
}
void Wifi_RxClear(void)
{
	memset(Wifi.RxBuffer, 0, _WIFI_RX_SIZE);
	Wifi.RxIndex = 0;
	HAL_UART_Receive_IT(&_WIFI_USART, &Wifi.usartBuff, 1);
}
void Wifi_TxClear(void)
{
	memset(Wifi.TxBuffer, 0, _WIFI_TX_SIZE);
}
void Wifi_RxCallBack(void)
{
	//at command buffer
	if (Wifi.RxIsData == false)
	{
		Wifi.RxBuffer[Wifi.RxIndex] = Wifi.usartBuff;
		if (Wifi.RxIndex < _WIFI_RX_SIZE)
			Wifi.RxIndex++;
	}
	//data buffer
	else
	{
		if (HAL_GetTick() - Wifi.RxDataLastTime > 50)
			Wifi.RxIsData = false;
		if (Wifi.RxDataLen == 0)
		{
			if (Wifi.TcpIpMultiConnection == false)
			{
				Wifi.RxBufferForDataTmp[Wifi.RxIndexForDataTmp] = Wifi.usartBuff;
				Wifi.RxIndexForDataTmp++;
				if (Wifi.RxBufferForDataTmp[Wifi.RxIndexForDataTmp - 1] == ':')
				{
					Wifi.RxDataConnectionNumber = 0;
					Wifi.RxDataLen = atoi((char *)&Wifi.RxBufferForDataTmp[1]);
				}
			}
			else
			{
				Wifi.RxBufferForDataTmp[Wifi.RxIndexForDataTmp] = Wifi.usartBuff;
				Wifi.RxIndexForDataTmp++;
				if (Wifi.RxBufferForDataTmp[2] == ',')
				{
					Wifi.RxDataConnectionNumber = Wifi.RxBufferForDataTmp[1] - 48;
				}
				if ((Wifi.RxIndexForDataTmp > 3) && (Wifi.RxBufferForDataTmp[Wifi.RxIndexForDataTmp - 1] == ':'))
					Wifi.RxDataLen = atoi((char *)&Wifi.RxBufferForDataTmp[3]);
			}
		}
		else
		{
			Wifi.RxBufferForData[Wifi.RxIndexForData] = Wifi.usartBuff;
			if (Wifi.RxIndexForData < _WIFI_RX_FOR_DATA_SIZE)
				Wifi.RxIndexForData++;
			if (Wifi.RxIndexForData >= Wifi.RxDataLen)
			{
				Wifi.RxIsData = false;
				Wifi.GotNewData = true;
			}
		}
	}
	HAL_UART_Receive_IT(&_WIFI_USART, &Wifi.usartBuff, 1);
	if (Wifi.RxIndex > 4)
	{
		if ((Wifi.RxBuffer[Wifi.RxIndex - 4] == '+') && (Wifi.RxBuffer[Wifi.RxIndex - 3] == 'I') && (Wifi.RxBuffer[Wifi.RxIndex - 2] == 'P') && (Wifi.RxBuffer[Wifi.RxIndex - 1] == 'D'))
		{
			memset(Wifi.RxBufferForDataTmp, 0, sizeof(Wifi.RxBufferForDataTmp));
			Wifi.RxBuffer[Wifi.RxIndex - 4] = 0;
			Wifi.RxBuffer[Wifi.RxIndex - 3] = 0;
			Wifi.RxBuffer[Wifi.RxIndex - 2] = 0;
			Wifi.RxBuffer[Wifi.RxIndex - 1] = 0;
			Wifi.RxIndex -= 4;
			Wifi.RxIndexForData = 0;
			Wifi.RxIndexForDataTmp = 0;
			Wifi.RxIsData = true;
			Wifi.RxDataLen = 0;
			Wifi.RxDataLastTime = HAL_GetTick();
		}
	}
}
void Wifi_Init(void)
{
	HAL_UART_Receive_IT(&_WIFI_USART, &Wifi.usartBuff, 1);
	Wifi_RxClear();
	Wifi_TxClear();
	osSemaphoreDef(WifiSemHandle);
	WifiSemHandle = osSemaphoreCreate(osSemaphore(WifiSemHandle), 1);
}
bool Wifi_SetRfPower(uint8_t Power_0_to_82)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		sprintf((char *)Wifi.TxBuffer, "AT+RFPOWER=%d\r\n", Power_0_to_82);
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_SetMode(WifiMode_t WifiMode_)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		sprintf((char *)Wifi.TxBuffer, "AT+CWMODE_CUR=%d\r\n", WifiMode_);
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;
		Wifi.Mode = WifiMode_;
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_GetMode(void)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		sprintf((char *)Wifi.TxBuffer, "AT+CWMODE_CUR?\r\n");
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;
		if (Wifi_ReturnInteger((int32_t *)&result, 1, ":"))
			Wifi.Mode = (WifiMode_t)result;
		else
			Wifi.Mode = WifiMode_Error;
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_GetMyIp(void)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		sprintf((char *)Wifi.TxBuffer, "AT+CIFSR\r\n");
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;
		sscanf((char *)Wifi.RxBuffer, "AT+CIFSR\r\r\n+CIFSR:APIP,\"%[^\"]", Wifi.MyIP);
		sscanf((char *)Wifi.RxBuffer, "AT+CIFSR\r\r\n+CIFSR:STAIP,\"%[^\"]", Wifi.MyIP);

		Wifi_RxClear();
		sprintf((char *)Wifi.TxBuffer, "AT+CIPSTA?\r\n");
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;

		char *str = strstr((char *)Wifi.RxBuffer, "gateway:");
		if (str == NULL)
			break;
		if (Wifi_ReturnStrings(str, "\"", 1, Wifi.MyGateWay) == false)
			break;

		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_Station_ConnectToAp(char *SSID, char *Pass, char *MAC)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		if (MAC == NULL)
			sprintf((char *)Wifi.TxBuffer, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", SSID, Pass);
		else
			sprintf((char *)Wifi.TxBuffer, "AT+CWJAP_CUR=\"%s\",\"%s\",\"%s\"\r\n", SSID, Pass, MAC);
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_VERYHIGH, &result, 3, "\r\nOK\r\n", "\r\nERROR\r\n", "\r\nFAIL\r\n") == false)
			break;
		if (result > 1)
			break;
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_Station_Disconnect(void)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		sprintf((char *)Wifi.TxBuffer, "AT+CWQAP\r\n");
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_TcpIp_GetConnectionStatus(void)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		sprintf((char *)Wifi.TxBuffer, "AT+CIPSTATUS\r\n");
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;

		char *str = strstr((char *)Wifi.RxBuffer, "\nSTATUS:");
		if (str == NULL)
			break;
		str = strchr(str, ':');
		str++;
		for (uint8_t i = 0; i < 5; i++)
			Wifi.TcpIpConnections[i].status = (WifiConnectionStatus_t)atoi(str);
		str = strstr((char *)Wifi.RxBuffer, "+CIPSTATUS:");
		for (uint8_t i = 0; i < 5; i++)
		{
			sscanf(str, "+CIPSTATUS:%d,\"%3s\",\"%[^\"]\",%d,%d,%d", (int *)&Wifi.TcpIpConnections[i].LinkId, Wifi.TcpIpConnections[i].Type, Wifi.TcpIpConnections[i].RemoteIp, (int *)&Wifi.TcpIpConnections[i].RemotePort, (int *)&Wifi.TcpIpConnections[i].LocalPort, (int *)&Wifi.TcpIpConnections[i].RunAsServer);
			str++;
			str = strstr(str, "+CIPSTATUS:");
			if (str == NULL)
				break;
		}
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_TcpIp_SetMultiConnection(bool EnableMultiConnections)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		sprintf((char *)Wifi.TxBuffer, "AT+CIPMUX=%d\r\n", EnableMultiConnections);
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;
		Wifi.TcpIpMultiConnection = EnableMultiConnections;
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_TcpIp_GetMultiConnection(void)
{

	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		sprintf((char *)Wifi.TxBuffer, "AT+CIPMUX?\r\n");
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;
		if (Wifi_ReturnInteger((int32_t *)&result, 1, ":") == false)
			break;
		Wifi.TcpIpMultiConnection = (bool)result;
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_TcpIp_StartTcpConnection(uint8_t LinkId, char *RemoteIp, uint16_t RemotePort, uint16_t TimeOut)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		sprintf((char *)Wifi.TxBuffer, "AT+CIPSERVER=1,%d\r\n", RemotePort);
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;
		Wifi_RxClear();
		if (Wifi.TcpIpMultiConnection == false)
			sprintf((char *)Wifi.TxBuffer, "AT+CIPSTART=\"TCP\",\"%s\",%d,%d\r\n", RemoteIp, RemotePort, TimeOut);
		else
			sprintf((char *)Wifi.TxBuffer, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d,%d\r\n", LinkId, RemoteIp, RemotePort, TimeOut);
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_HIGH, &result, 3, "OK", "CONNECT", "ERROR") == false)
			break;
		if (result == 3)
			break;
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_TcpIp_Close(uint8_t LinkId)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		if (Wifi.TcpIpMultiConnection == false)
			sprintf((char *)Wifi.TxBuffer, "AT+CIPCLOSE\r\n");
		else
			sprintf((char *)Wifi.TxBuffer, "AT+CIPCLOSE=%d\r\n", LinkId);
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (result == 2)
			break;
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
bool Wifi_TcpIp_SendDataTcp(uint8_t LinkId, uint16_t dataLen, uint8_t *data)
{
	osSemaphoreWait(WifiSemHandle, osWaitForever);
	uint8_t result;
	bool returnVal = false;
	do
	{
		Wifi_RxClear();
		if (Wifi.TcpIpMultiConnection == false)
			sprintf((char *)Wifi.TxBuffer, "AT+CIPSENDBUF=%d\r\n", dataLen);
		else
			sprintf((char *)Wifi.TxBuffer, "AT+CIPSENDBUF=%d,%d\r\n", LinkId, dataLen);
		if (Wifi_SendString((char *)Wifi.TxBuffer) == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 3, ">", "ERROR", "busy") == false)
			break;
		if (result > 1)
			break;
		Wifi_RxClear();
		Wifi_SendRaw(data, dataLen);
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
			break;
		returnVal = true;
	} while (0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
