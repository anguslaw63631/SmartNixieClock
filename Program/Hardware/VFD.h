/*
 * VFD.h
 *
 *  Created on: Oct 22, 2023
 *      Author: Angus
 */
#ifndef __VFD_H__
#define __VFD_H__

#define SET_CS HAL_GPIO_WritePin(VFD_CS_PORT, VFD_CS_PIN, GPIO_PIN_SET)
#define CLR_CS HAL_GPIO_WritePin(VFD_CS_PORT, VFD_CS_PIN, GPIO_PIN_RESET)

#define SET_RST HAL_GPIO_WritePin(VFD_RST_PORT, VFD_RST_PIN, GPIO_PIN_SET)
#define CLR_RST HAL_GPIO_WritePin(VFD_RST_PORT, VFD_RST_PIN, GPIO_PIN_RESET)

#define SET_EN HAL_GPIO_WritePin(VFD_EN_PORT, VFD_EN_PIN, GPIO_PIN_SET)
#define CLR_EN HAL_GPIO_WritePin(VFD_EN_PORT, VFD_EN_PIN, GPIO_PIN_RESET)

/** VFD 8-MD-06INKMÃüÁî */
#define Write_DCRAM_CMD 0x20  // Write Data Control RAM Command
#define Write_CGRAM_CMD 0x40  // Write Character Generator RAM Command
#define Write_ADRAM_CMD 0x60  // Write Additional Data RAM Command
#define Write_URAM_CMD 0x80   // Write Univeral Data RAM Command
#define Set_Timing_CMD 0xE0   // Set Display Timming Command
#define Set_Dimming_CMD 0xE4  // Write Brightness Control Data Command
#define Light_Normal_CMD 0xE8 // Display Light Normal Operation
#define Light_Off_CMD 0xEA    // Set All Display Light Off
#define Light_On_CMD 0xE9     // Set All Display Light On
#define Standby_Off_CMD 0xEC  // Standby Mode Off, Normal Operation Mode
#define Standby_On_CMD 0xED   // Standby Mode On, Save Power

/** Brightness(0~240) */
#define Brightness 100

void VFD_SendData(uint8_t data);

void VFD_DisplayChar(uint8_t position, uint8_t data);

void VFD_DisplayCustomChar(uint8_t address, uint8_t *buffer);

void VFD_DisplayString(uint8_t position, uint8_t *data);

void VFD_ClearDisplay(void);

void VFD_Init(void);

#endif