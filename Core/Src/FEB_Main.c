/*
 * FEB_Main.c
 *
 *  Author: Dorukhan User
 */
#include "FEB_Main.h"
#include <stdio.h>
#include <string.h>

extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
extern volatile uint32_t pulse_count;

void FEB_Setup(void) {
	HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_1); // Start TIM2 in counter mode
	HAL_ADC_Start(&hadc1); // Start ADC for temperature sensor
}

void FEB_Loop(void) {
    float temperature = getTemperature();
    float flow_rate = getFlowRate();

    char buffer[60];
    snprintf(buffer, sizeof(buffer), "Temp: %.2f°C, Flow: %.2f LPM\n", temperature, flow_rate);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

    HAL_Delay(1000);
}


