/*
 * Button.c
 *
 *  Created on: Oct 24, 2023
 *      Author: Angus
 */
#include "Button.h"

static KEY_TypeDef Key[5] =
    {{KEY_OFF, KEY_OFF, 0, 0},
     {KEY_OFF, KEY_OFF, 0, 0},
     {KEY_OFF, KEY_OFF, 0, 0},
     {KEY_OFF, KEY_OFF, 0, 0},
     {KEY_OFF, KEY_OFF, 0, 0}};

int8_t BUTTON_keyStatus[5] = {0};

void BUTTON_Update()
{
    switch (Key_Scan(SW1_GPIO_Port, SW1_PIN))
    {
    case KEY_ON:
        BUTTON_keyStatus[0] = 1;
        break;
    case KEY_OFF:
        BUTTON_keyStatus[0] = 0;
        break;
    case KEY_ERROR:
        BUTTON_keyStatus[0] = -1;
        break;
    default:
        break;
    }

    switch (Key_Scan(SW2_GPIO_Port, SW2_PIN))
    {
    case KEY_ON:
        BUTTON_keyStatus[1] = 1;
        break;
    case KEY_OFF:
        BUTTON_keyStatus[1] = 0;
        break;
    case KEY_ERROR:
        BUTTON_keyStatus[1] = -1;
        break;
    default:
        break;
    }
    switch (Key_Scan(SW3_GPIO_Port, SW3_PIN))
    {
    case KEY_ON:
        BUTTON_keyStatus[2] = 1;
        break;
    case KEY_OFF:
        BUTTON_keyStatus[2] = 0;
        break;
    case KEY_ERROR:
        BUTTON_keyStatus[2] = -1;
        break;
    default:
        break;
    }
    switch (Key_Scan(SW4_GPIO_Port, SW4_PIN))
    {
    case KEY_ON:
        BUTTON_keyStatus[3] = 1;
        break;
    case KEY_OFF:
        BUTTON_keyStatus[3] = 0;
        break;
    case KEY_ERROR:
        BUTTON_keyStatus[3] = -1;
        break;
    default:
        break;
    }
    switch (Key_Scan(ENCODER_SW_GPIO_Port, ENCODER_SW_PIN))
    {
    case KEY_ON:
        BUTTON_keyStatus[4] = 1;
        break;
    case KEY_OFF:
        BUTTON_keyStatus[4] = 0;
        break;
    case KEY_ERROR:
        BUTTON_keyStatus[4] = -1;
        break;
    default:
        break;
    }
}

uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    KEY_TypeDef *KeyTemp;
    switch ((uint32_t)GPIOx)
    {
    case (uint32_t)SW1_GPIO_Port: // SW1 and SW2 are on the same port
        switch (GPIO_Pin)
        {
        case SW1_PIN:
            KeyTemp = &Key[0];
            break;
        case SW2_PIN:
            KeyTemp = &Key[1];
            break;
        default:
            return KEY_IDLE;
        }
        break;
    case (uint32_t)SW3_GPIO_Port: // SW3, SW4, Encoder Key are on the same port
        switch (GPIO_Pin)
        {
        case SW3_PIN:
            KeyTemp = &Key[2];
            break;
        case SW4_PIN:
            KeyTemp = &Key[3];
            break;
        case ENCODER_SW_PIN:
            KeyTemp = &Key[4];
            break;
        default:
            return KEY_IDLE;
        }
        break;
    }

    KeyTemp->KeyPhysic = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

    switch (KeyTemp->KeyLogic)
    {

    case KEY_ON:
        switch (KeyTemp->KeyPhysic)
        {

        case KEY_ON:
            KeyTemp->KeyOFFCounts = 0;
            return KEY_IDLE;

        case KEY_OFF:
            KeyTemp->KeyOFFCounts++;
            if (KeyTemp->KeyOFFCounts >= SHAKES_COUNTS)
            {
                KeyTemp->KeyLogic = KEY_OFF;
                KeyTemp->KeyOFFCounts = 0;
                return KEY_OFF;
            }
            return KEY_IDLE;

        default:
            break;
        }

    case KEY_OFF:
        switch (KeyTemp->KeyPhysic)
        {

        case KEY_ON:
            (KeyTemp->KeyONCounts)++;
            if (KeyTemp->KeyONCounts >= SHAKES_COUNTS)
            {
                KeyTemp->KeyLogic = KEY_ON;
                KeyTemp->KeyONCounts = 0;

                return KEY_ON;
            }
            return KEY_IDLE;

        case KEY_OFF:
            (KeyTemp->KeyONCounts) = 0;
            return KEY_IDLE;
        default:
            break;
        }
    default:
        break;
    }
    return KEY_ERROR;
}