/*
 * AHT10.c
 *
 *  Created on: Oct 22, 2023
 *      Author: Angus
 */

#include "AHT10.h"

uint8_t AHT10_Original_Data[6] = {0};
AHT10_Data aht10Data = {0};

/// @brief Init AHT10
/// @param  None
void AHT10_Init(void)
{
    uint8_t Init_Commands[3] = {0};
    Init_Commands[0] = Initialize_Command;
    Init_Commands[1] = 0x08;
    Init_Commands[2] = 0x00;
    HAL_I2C_Master_Transmit(&AHT10_I2C, AHT10_ADDRESS, Init_Commands, 3, 100);
}

/// @brief Reset AHT10
/// @param  None
void AHT10_Reset(void)
{
    uint8_t Reset_Command = Soft_Reset_Command;
    HAL_I2C_Master_Transmit(&AHT10_I2C, AHT10_ADDRESS, &Reset_Command, 1, 100);
    osDelay(20);
}

/// @brief Trigger AHT10 to measure temperature and humidity
/// @param  None
void AHT10_Trig_Measure(void)
{
    uint8_t Trig_Commands[3] = {0};
    Trig_Commands[0] = Measure_Trig_Command;
    Trig_Commands[1] = 0x33;
    Trig_Commands[2] = 0x00;
    HAL_I2C_Master_Transmit(&AHT10_I2C, AHT10_ADDRESS, Trig_Commands, 3, 100);
}

/// @brief Obtain original data from AHT10(6 bytes)
/// @param pOriginData
/// @return 0 success 1 fail
uint8_t AHT10_Read_Data(uint8_t *pOriginData)
{
    HAL_I2C_Master_Receive(&AHT10_I2C, AHT10_ADDRESS, pOriginData, 6, 100);
    uint8_t checkTemp = pOriginData[0];
    checkTemp &= 0x80; // using bit7 to check if data is valid
    if (checkTemp == 0x00)
    { // if data is valid return 0
        return 0;
    }
    else
    {
        return 1;
    }
}

/// @brief Obtain temperature and humidity from original data
/// @param pTemperature
/// @param pHumidity
/// @return 0 success 1 fail
uint8_t AHT10_Measure(float *pTemperature, float *pHumidity)
{
    uint8_t readFlag = 0;
    AHT10_Trig_Measure();
    osDelay(100);
    readFlag = AHT10_Read_Data(AHT10_Original_Data);
    if (readFlag == 0)
    {
        *pHumidity = AHT10_GetHumidity();
        *pTemperature = AHT10_GetTemperature();
    }
    return readFlag;
}

/// @brief Obtain humidity from original data
/// @param  None
/// @return humidity (float)
float AHT10_GetHumidity(void)
{
    uint8_t humiTemp[3] = {0};
    uint32_t humi = 0;
    float tempHumidity = 0;

    humiTemp[0] = AHT10_Original_Data[1];
    humiTemp[1] = AHT10_Original_Data[2];
    humiTemp[2] = AHT10_Original_Data[3] & 0xF0; // Filter out temperature data

    for (int i = 0; i < 3; i++)
    {
        humi += humiTemp[i];
        humi <<= 8;
    }
    humi >>= 12;                                 // only 20bit have valid data
    tempHumidity = (humi / 1048576.0f) * 100.0f; // calculate humidity

    for (int i = 0; i < 3; i++)
    { // clear temp data
        humiTemp[i] = 0;
    }
    humi = 0;

    return tempHumidity;
}

/// @brief obtain temperature from original data
/// @param  None
/// @return temperature (float)
float AHT10_GetTemperature(void)
{
    uint8_t tempTemp[3] = {0};
    uint32_t temp = 0;
    float tempTemperature = 0;

    tempTemp[0] = (AHT10_Original_Data[3] & 0x0F) << 4; // Only obtain temperature data
    tempTemp[0] += (AHT10_Original_Data[4] & 0xF0) >> 4;
    tempTemp[1] = (AHT10_Original_Data[4] & 0x0F) << 4;
    tempTemp[1] += (AHT10_Original_Data[5] & 0xF0) >> 4;
    tempTemp[2] = (AHT10_Original_Data[5] & 0x0F) << 4;
    tempTemp[2] += (AHT10_Original_Data[6] & 0xF0) >> 4;

    for (int i = 0; i < 3; i++)
    {
        temp += tempTemp[i];
        temp <<= 8;
    }
    temp >>= 12;                                              // only 20bit have valid data
    tempTemperature = ((temp / 1048576.0f) * 200.0f) - 50.0f; // calculate temperature

    for (int i = 0; i < 3; i++)
    { // clear temp data
        tempTemp[i] = 0;
    }
    temp = 0;

    return tempTemperature;
}

/// @brief update temperature and humidity in AHT10_Data
/// @param  None
void AHT10_Update(void)
{
    AHT10_Measure(&aht10Data.temperature, &aht10Data.humidity);
}
