/*
 * linearPotentiometer.c
 *
 *  Created on: Sep 20, 2024
 *      Author: dorukhanuser
 */


#include "linearPotentiometer.h"

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

uint16_t measurement;
char buf[28];

void linearPotentiometerRun(){
	if (HAL_ADC_Start(&hadc1) != HAL_OK) {
				  // Start Error


	}
			// do displacement reading
	if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
		measurement = HAL_ADC_GetValue(&hadc1);
		sprintf(buf, "Potentiometer Measurement: %hu \r\n", measurement);
		HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	}

	HAL_ADC_Stop(&hadc1);
}
