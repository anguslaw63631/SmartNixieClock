/*
 * Button.h
 *
 *  Created on: Oct 24, 2023
 *      Author: Angus
 */
#ifndef __Button_H__
#define __Button_H__
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "HardwareConfig.h"

typedef struct
{
    uint8_t KeyLogic;
    uint8_t KeyPhysic;
    uint8_t KeyONCounts;
    uint8_t KeyOFFCounts;
} KEY_TypeDef;

extern int8_t BUTTON_keyStatus[5];

#define KEY_OFF 1
#define KEY_ON 0
#define KEY_IDLE 2
#define KEY_ERROR 10

#define SHAKES_COUNTS 5

uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void BUTTON_Update();

enum{
    Button_Released,
    Button_Pressed
};
#endif