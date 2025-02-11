/*
 * AirTemp.c
 *
 * Author: Dorukhan User
 */
#include "AirTemp.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;

void AirTemp_Init() {
 //Fill this if needed in the future
}

float Read_Temperature() {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
    return Convert_To_Temperature(adc_value);
}

float Convert_To_Temperature(uint32_t adc_value) {
    float voltage = (adc_value * 3.3) / 4095.0;
    float resistance = (10000 * voltage) / (3.3 - voltage);
    return (1.0 / ((1.0 / 298.15) + (log(resistance / 10000) / 3950))) - 273.15;
}



