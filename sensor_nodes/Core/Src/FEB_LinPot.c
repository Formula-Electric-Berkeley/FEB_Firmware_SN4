/*
 * FEB_LinPot.c
 *
 *  Created on: Feb 2, 2025
 *      Author: rahilpasha
 */

#include "FEB_LinPot.h"

extern ADC_HandleTypeDef hadc2;
extern UART_HandleTypeDef huart2;

uint32_t linear_potentiometer_reading;
float linear_potentiometer_value;
char buf[164];
float adc_resolution = 4095;
float linear_potentiometer_length = 75; //mm


uint32_t get_linear_potentiometer_reading(void) {

	if (HAL_ADC_Start(&hadc2) != HAL_OK) {
	  Error_Handler();
	}

	if (HAL_ADC_PollForConversion(&hadc2, 100) == HAL_OK) {
		linear_potentiometer_reading = (uint32_t)HAL_ADC_GetValue(&hadc2);
	}

	linear_potentiometer_value = linear_potentiometer_reading / adc_resolution * linear_potentiometer_length;


	sprintf(buf, "Potentiometer Measurement: %d\r\n", (int) linear_potentiometer_value);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);
	return linear_potentiometer_reading;
}
