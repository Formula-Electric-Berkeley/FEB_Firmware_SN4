/*
 * CoolantSensor.c
 *
 * Author: Dorukhan User
 */
#include "CoolantSensor.h"

extern ADC_HandleTypeDef hadc1;
extern volatile uint32_t pulse_count;
extern TIM_HandleTypeDef htim2;

float getTemperature(void) {
    uint32_t adcValue;
    float resistance, temperature;

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    adcValue = HAL_ADC_GetValue(&hadc1);

    resistance = SERIES_RESISTOR * ((float)adcValue / (4095.0 - (float)adcValue + 0.1));
    temperature = 1.0 / ((1.0 / NOMINAL_TEMPERATURE) + (log(resistance / NOMINAL_RESISTANCE) / BETA_COEFFICIENT));

    return temperature - 273.15; // Convert Kelvin to Celsius
}

float getFlowRate(void) {
    float pulses_per_liter = 7.5;  // Adjust based on doing validations
    pulse_count = __HAL_TIM_GET_COUNTER(&htim2); // Read TIM2 counter

    __HAL_TIM_SET_COUNTER(&htim2, 0);  // Reset counter after reading

    return pulse_count / pulses_per_liter;  // Convert pulses to flow rate (LPM)
}

