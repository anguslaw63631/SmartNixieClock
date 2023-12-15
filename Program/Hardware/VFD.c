/*
 * VFD.c
 *
 *  Created on: Oct 22, 2023
 *      Author: Angus
 */

#include "HardwareConfig.h"
#include "VFD.h"
void VFD_SendData(uint8_t data)
{
	HAL_SPI_Transmit(&hspi1, &data, 1, 10);
}

void VFD_DisplayChar(uint8_t position, uint8_t data)
{
	CLR_CS;

	VFD_SendData(Write_DCRAM_CMD | position);
	VFD_SendData(data);

	SET_CS;
}

void VFD_DisplayCustomChar(uint8_t address, uint8_t *buffer)
{
	CLR_CS;

	VFD_SendData(Write_CGRAM_CMD | address);

	for (uint8_t i = 0; i < 5; i++)
	{
		VFD_SendData(*buffer);
		buffer++;
	}

	SET_CS;
}

void VFD_DisplayString(uint8_t position, uint8_t *data)
{
	uint8_t i = 0;
	while (data[i] != '\0')
	{
		VFD_DisplayChar(position + i, data[i]);
		i++;
		if (i >= 8)
		{
			return;
		}
	}
}

void VFD_ClearDisplay(void)
{
	VFD_DisplayString(0, "        ");
}

void VFD_Init(void)
{

	SET_CS;
	osDelay(100);
	CLR_RST;
	osDelay(5);
	SET_RST;

	CLR_CS;
	VFD_SendData(Set_Timing_CMD);
	osDelay(1);
	VFD_SendData(0x07);
	SET_CS;

	osDelay(1);

	CLR_CS;
	VFD_SendData(Set_Dimming_CMD);
	osDelay(1);
	VFD_SendData(Brightness);
	SET_CS;

	osDelay(1);

	CLR_CS;
	VFD_SendData(Light_Normal_CMD);
	SET_CS;
}