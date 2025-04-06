/*
 * FEB_LinPot.c
 *
 *  Created on: Feb 2, 2025
 *      Author: rahilpasha
 */

// **************************************** Includes & External ****************************************

#include <FEB_ADC.h>

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;

// ******************************************** Variables **********************************************

#define ADC_RESOLUTION 4095
#define LIN_POT_LENGTH 75000 // micrometers

uint32_t ADC1_Readings[7];
uint32_t ADC2_Readings[4]; // 1st and 2nd are linear potentiometer, 3rd and 4th are coolant pressure
char buf[164];
uint8_t TxData[8];

// ******************************************** Functions **********************************************

uint16_t LinearPotentiometerConversion(uint32_t adc_value) {
	return (uint16_t) adc_value / ADC_RESOLUTION * LIN_POT_LENGTH;
}

uint16_t CoolantPressureConversion(uint32_t adc_value) {
	float voltage = (float) adc_value * 3.3 / ADC_RESOLUTION;
	return (uint16_t) 1000 * ((voltage - 0.5) * 30) / (4.5 - 0.5);
}


void UART_Transmit_ADC1_Readings(void) {
	sprintf(buf, "Strain Gauge 1: %u\r\n", (unsigned) ADC1_Readings[0]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Strain Gauge 2: %u\r\n", (unsigned) ADC1_Readings[1]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Strain Gauge 3: %u\r\n", (unsigned) ADC1_Readings[2]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Strain Gauge 4: %u\r\n", (unsigned) ADC1_Readings[3]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Thermocouple 1: %u\r\n", (unsigned) ADC1_Readings[4]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Thermocouple 2: %u\r\n", (unsigned) ADC1_Readings[5]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Thermocouple 3: %u\r\n", (unsigned) ADC1_Readings[6]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);
}

void UART_Transmit_ADC2_Readings(void) {
	sprintf(buf, "LIN_POT 1: %u\r\n", (unsigned) ADC2_Readings[0]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "LIN_POT 2: %u\r\n", (unsigned) ADC2_Readings[1]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Coolant Pressure 1: %u\r\n", (unsigned) ADC2_Readings[2]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Coolant Pressure 2: %u\r\n", (unsigned) ADC2_Readings[3]);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);
}

void Fill_CAN_Data_ADC(void) {

	uint16_t LinPot1 = LinearPotentiometerConversion(ADC2_Readings[0]);
	uint16_t LinPot2 = LinearPotentiometerConversion(ADC2_Readings[1]);
	uint16_t CoPr1 = CoolantPressureConversion(ADC2_Readings[2]);
	uint16_t CoPr2 = CoolantPressureConversion(ADC2_Readings[3]);

	// Fill the data
	TxData[0] = (LinPot1 >> 8) & 0xFF;
	TxData[1] = LinPot1 & 0xFF;
	TxData[2] = (LinPot2 >> 8) & 0xFF;
	TxData[3] = LinPot2 & 0xFF;
	TxData[4] = (CoPr1 >> 8) & 0xFF;
	TxData[5] = CoPr1 & 0xFF;
	TxData[6] = (CoPr2 >> 8) & 0xFF;
	TxData[7] = CoPr2 & 0xFF;


}

void CAN_ADC_Transmit(void)
{
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;

	TxHeader.DLC = 8; // Data length
	TxHeader.IDE = CAN_ID_STD; // Standard ID
	TxHeader.RTR = CAN_RTR_DATA; // Data frame
	TxHeader.StdId = 0x545; // CAN ID
	TxHeader.ExtId = 0; // Not used with standard ID


	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		// Transmission request error
		char msg[50];
		sprintf(msg, "CAN transmit error");
		HAL_UART_Transmit(&huart2, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
//		Error_Handler();
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {

	if (hadc->Instance == ADC1) {

		UART_Transmit_ADC1_Readings();

	}

	if (hadc->Instance == ADC2) {
		UART_Transmit_ADC2_Readings();

		Fill_CAN_Data_ADC();

		CAN_ADC_Transmit();
	}

}

void ADC2_Main(void) {

	HAL_ADC_Start_DMA(&hadc1, ADC1_Readings, 7);
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
