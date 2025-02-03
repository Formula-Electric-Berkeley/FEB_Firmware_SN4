/*
 * FEB_LinPot.c
 *
 *  Created on: Feb 2, 2025
 *      Author: rahilpasha
 */

// **************************************** Includes & External ****************************************

#include <FEB_ADC.h>

extern ADC_HandleTypeDef hadc2;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_adc2;

// ******************************************** Variables **********************************************

#define ADC_RESOLUTION 4095
#define LIN_POT_LENGTH 75 // mm

uint32_t ADC2_Readings[4]; // 1st and 2nd are linear potentiometer, 3rd and 4th are coolant pressure
char buf[164];

// ******************************************** Functions **********************************************

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {

	if (hadc->Instance == ADC2) {

		sprintf(buf, "LIN_POT 1: %u\r\n", (unsigned) ADC2_Readings[0] / ADC_RESOLUTION * LIN_POT_LENGTH);
		HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

		sprintf(buf, "LIN_POT 2: %u\r\n", (unsigned) ADC2_Readings[1] / ADC_RESOLUTION * LIN_POT_LENGTH);
		HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

		sprintf(buf, "Coolant Pressure 1: %u\r\n", (unsigned) ADC2_Readings[2]);
		HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

		sprintf(buf, "Coolant Pressure 2: %u\r\n", (unsigned) ADC2_Readings[3]);
		HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	}

}

void ADC2_Main(void) {

	HAL_ADC_Start_DMA(&hadc2, ADC2_Readings, 4);

}

//
//uint32_t get_linear_potentiometer_reading(void) {
//
//	if (HAL_ADC_Start(&hadc2) != HAL_OK) {
//	  Error_Handler();
//	}
//
//	if (HAL_ADC_PollForConversion(&hadc2, 100) == HAL_OK) {
//		linear_potentiometer_reading = (uint32_t)HAL_ADC_GetValue(&hadc2);
//	}
//
//	linear_potentiometer_value = linear_potentiometer_reading / adc_resolution * linear_potentiometer_length;
//
//
//	sprintf(buf, "Potentiometer Measurement: %d\r\n", (int) linear_potentiometer_value);
//	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);
//	return linear_potentiometer_reading;
//}
