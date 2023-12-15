/*
 * HardwareConfig.h
 *
 *  Created on: Oct 22, 2023
 *      Author: Angus
 */

#ifndef __HARDWARECONFIG_H__
#define __HARDWARECONFIG_H__

#include "stm32f1xx_HAL.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "misc.h"

// AHT10
#define AHT10_I2C hi2c1

// NixieTube
#define NIXIETUBE_SPI hspi2

#define NIXIETUBE_CS_PORT GPIOB
#define NIXIETUBE_CS_PIN NIXIETUBE_CS_Pin

#define NIXIETUBE_CS2_PORT GPIOB
#define NIXIETUBE_CS2_PIN NIXIETUBE_CS2_Pin

#define NIXIETUBE_TIM htim1
#define NIXIETUBE_TIM_CH TIM_CHANNEL_4

#define NIXIETUBE_C1_PORT NIXIETUBE_C1_GPIO_Port
#define NIXIETUBE_C1_PIN NIXIETUBE_C1_Pin

#define NIXIETUBE_C2_PORT NIXIETUBE_C2_GPIO_Port
#define NIXIETUBE_C2_PIN NIXIETUBE_C2_Pin

// VFD
#define VFD_SPI hspi1

#define VFD_CS_PORT GPIOA
#define VFD_CS_PIN VFD_CS_Pin

#define VFD_RST_PORT GPIOB
#define VFD_RST_PIN VFD_RST_Pin

#define VFD_EN_PORT GPIOA
#define VFD_EN_PIN VFD_EN_Pin

// Encoder
#define ENCODER_TIM htim4
#define ENCODER_TIM_CH1 TIM_CHANNEL_1
#define ENCODER_TIM_CH2 TIM_CHANNEL_2
#define ENCODER_SW_PORT GPIOB
#define ENCODER_SW_PIN ENCODER_SW_Pin

// ESP8266
#define _WIFI_USART huart2

#define _WIFI_RX_SIZE 128
#define _WIFI_RX_FOR_DATA_SIZE 450
#define _WIFI_TX_SIZE 128

#define _WIFI_WAIT_TIME_LOW 1000
#define _WIFI_WAIT_TIME_MED 10000
#define _WIFI_WAIT_TIME_HIGH 25000
#define _WIFI_WAIT_TIME_VERYHIGH 60000

#define _WIFI_SSID "SmartNixieClock"
#define _WIFI_PASSWORD "12345678"

// TTL
#define _TTL_USART huart1
#define _TTL_RX_SIZE 18

// LDR
#define LDR_ADC hadc1

// Button
#define SW1_PORT GPIOA
#define SW1_PIN SW1_Pin

#define SW2_PORT GPIOA
#define SW2_PIN SW2_Pin

#define SW3_PORT GPIOB
#define SW3_PIN SW3_Pin

#define SW4_PORT GPIOB
#define SW4_PIN SW4_Pin

// Buzzer
#define Buzzer_TIM htim2
#define Buzzer_TIM_CH TIM_CHANNEL_1

#endif
