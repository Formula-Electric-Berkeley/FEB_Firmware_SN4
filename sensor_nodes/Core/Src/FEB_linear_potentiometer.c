/*
 * linearPotentiometer.c
 *
 *  Created on: Sep 20, 2024
 *      Author: dorukhanuser
 */

#include <FEB_linear_potentiometer.h>

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
uint16_t linear_potentiometer_reading;
char buf[28];

uint16_t get_linear_potentiometer_reading(void){
	if (HAL_ADC_Start(&hadc1) != HAL_OK) {
				  // Start Error

	}
			// do displacement reading
	if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
		linear_potentiometer_reading = HAL_ADC_GetValue(&hadc1);
		sprintf(buf, "Potentiometer Measurement: %hu \r\n", linear_potentiometer_reading);
		HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	}

	HAL_ADC_Stop(&hadc1);
	return linear_potentiometer_reading;
}
