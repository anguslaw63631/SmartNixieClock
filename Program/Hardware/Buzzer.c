/*
 * Buzzer.c
 *
 *  Created on: Oct 24, 2023
 *      Author: Angus
 */
#include "Buzzer.h"

void Buzzer_Init(void)
{
    HAL_TIM_PWM_Start(&Buzzer_TIM, Buzzer_TIM_CH);
    Buzzer_Off();
}

void Buzzer_On(uint16_t psc, uint16_t pwm)
{
    __HAL_TIM_PRESCALER(&Buzzer_TIM, psc);
    __HAL_TIM_SetCompare(&Buzzer_TIM, Buzzer_TIM_CH, pwm);
}
void Buzzer_Off(void)
{
    // __HAL_TIM_SetCompare(&Buzzer_TIM, Buzzer_TIM_CH, 0);

    __HAL_TIM_PRESCALER(&Buzzer_TIM, Z0);
    __HAL_TIM_SetCompare(&Buzzer_TIM, Buzzer_TIM_CH, 1);
}

void Buzzer_PlayMusic(uint16_t volume, uint8_t musicTrack)
{
    // Max 500
    clamp_short(volume, 0, 500);
    uint16_t kawaii[] =
        {
            M1, 185, Z0, 300, Z0, 83, M1, 167, Z0, 23, M2, 167, Z0, 23, M2, 167, Z0, 23, M1, 167, Z0, 23, M2, 167,
            Z0, 23, M3, 185, Z0, 100, Z0, 94, M7, 185, Z0, 100, Z0, 94, H1, 185, Z0, 100, Z0, 94, M5, 167, Z0, 23,
            M3, 167, Z0, 23, M1, 531, Z0, 31, M1, 167, Z0, 23, M2, 167, Z0, 23, M2, 167, Z0, 23, M1, 167, Z0, 23,
            M2, 167, Z0, 23, M3, 185, Z0, 100, Z0, 94, M4, 185, Z0, 100, Z0, 94, M5, 185};
    uint16_t nokia[] = {
        M3, 125, M2, 125, X5, 250, X1, 250, X6, 125, L7, 125, L2, 250, L3, 250, L7, 125, L6, 125, X7, 250, L3, 250, L6, 500};

    if (musicTrack == 0)
    {
        uint16_t length = sizeof(kawaii) / sizeof(kawaii[0]);
        for (uint16_t i = 0; i < (length / 2); i++)
        {
            Buzzer_On(kawaii[2 * i], volume);
            osDelay(1 * kawaii[2 * i + 1]); // playback speed 1 = 100us, 5 = 500us
        }
    }
    else if (musicTrack == 1)
    {
        uint16_t length = sizeof(nokia) / sizeof(nokia[0]);
        for (uint16_t i = 0; i < (length / 2); i++)
        {
            Buzzer_On(nokia[2 * i], volume);
            osDelay(1 * nokia[2 * i + 1]); // playback speed 1 = 100us, 5 = 500us
        }
    }

    Buzzer_Off();
}