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

uint16_t ADC1_Readings[7];
uint16_t ADC2_Readings[4]; // 1st and 2nd are linear potentiometer, 3rd and 4th are coolant pressure

uint8_t Strain_Gauge_Data[8];
uint8_t Thermocouple_Data[8];
uint8_t Lin_Pot_Data[8];
uint8_t Coolant_Pressure_Data[8];

// ******************************************** Functions **********************************************

uint16_t StrainGaugeConversion(uint16_t adc_value) {
	return  adc_value & 0xFFFF;
}

uint16_t ThermocoupleConversion(uint16_t adc_value) {
	return adc_value & 0xFFFF;
}

uint16_t LinearPotentiometerConversion(uint16_t adc_value) {
	return (uint16_t) adc_value / ADC_RESOLUTION * LIN_POT_LENGTH;
}

uint16_t CoolantPressureConversion(uint16_t adc_value) {
	float voltage = (float) adc_value * 3.3 / ADC_RESOLUTION;
	return (uint16_t) 1000 * ((voltage - 0.5) * 30) / (4.5 - 0.5);
}


void UART_Transmit_ADC1_Readings(void) {

	for (int i = 0; i < 4; i++) {
		#ifdef DEBUG_ADC_UART_TRANSMIT_ADC1
		printf("Strain Gauge %d: %u\r\n", i, (unsigned) ADC1_Readings[i]);
		#endif
	}

	for (int i = 4; i < 7; i++) {
		#ifdef DEBUG_ADC_UART_TRANSMIT_ADC1
		printf("Thermocouple %d: %u\r\n", i, (unsigned) ADC1_Readings[i]);
		#endif
	}

}

void UART_Transmit_ADC2_Readings(void) {

	for (int i = 0; i < 2; i++) {
		#ifdef DEBUG_ADC_UART_TRANSMIT_ADC2
		printf("Linear Potentiometer %d: %u\r\n", i, (unsigned) ADC2_Readings[i]);
		#endif
	}

	for (int i = 2; i < 4; i++) {
		#ifdef DEBUG_ADC_UART_TRANSMIT_ADC2
		printf("Coolant Pressure %d: %u\r\n", i, (unsigned) ADC2_Readings[i]);
		#endif
	}

}

void Fill_Strain_Gauge_Data(void) {

	uint16_t StGa1 = StrainGaugeConversion(ADC1_Readings[0]);
	uint16_t StGa2 = StrainGaugeConversion(ADC1_Readings[1]);
	uint16_t StGa3 = StrainGaugeConversion(ADC1_Readings[2]);
	uint16_t StGa4 = StrainGaugeConversion(ADC1_Readings[3]);

	// Fill the data
	Strain_Gauge_Data[0] = (StGa1 >> 8) & 0xFF;
	Strain_Gauge_Data[1] = StGa1 & 0xFF;
	Strain_Gauge_Data[2] = (StGa2 >> 8) & 0xFF;
	Strain_Gauge_Data[3] = StGa2 & 0xFF;
	Strain_Gauge_Data[4] = (StGa3 >> 8) & 0xFF;
	Strain_Gauge_Data[5] = StGa3 & 0xFF;
	Strain_Gauge_Data[6] = (StGa4 >> 8) & 0xFF;
	Strain_Gauge_Data[7] = StGa4 & 0xFF;

}

void Fill_Thermocouple_Data(void) {

	uint16_t Thermo1 = ThermocoupleConversion(ADC1_Readings[4]);
	uint16_t Thermo2 = ThermocoupleConversion(ADC1_Readings[5]);
	uint16_t Thermo3 = ThermocoupleConversion(ADC1_Readings[6]);

	// Fill the data
	Thermocouple_Data[0] = (Thermo1 >> 8) & 0xFF;
	Thermocouple_Data[1] = Thermo1 & 0xFF;
	Thermocouple_Data[2] = (Thermo2 >> 8) & 0xFF;
	Thermocouple_Data[3] = Thermo2 & 0xFF;
	Thermocouple_Data[4] = (Thermo3 >> 8) & 0xFF;
	Thermocouple_Data[5] = Thermo3 & 0xFF;

}

void Fill_Lin_Pot_Data(void) {

	uint16_t LinPot1 = LinearPotentiometerConversion(ADC2_Readings[0]);
	uint16_t LinPot2 = LinearPotentiometerConversion(ADC2_Readings[1]);

	// Fill the data
	Lin_Pot_Data[0] = (LinPot1 >> 8) & 0xFF;
	Lin_Pot_Data[1] = LinPot1 & 0xFF;
	Lin_Pot_Data[2] = (LinPot2 >> 8) & 0xFF;
	Lin_Pot_Data[3] = LinPot2 & 0xFF;

}

void Fill_Coolant_Pressure_Data(void) {

	uint16_t CoPr1 = CoolantPressureConversion(ADC2_Readings[2]);
	uint16_t CoPr2 = CoolantPressureConversion(ADC2_Readings[3]);

	// Fill the data
	Coolant_Pressure_Data[0] = (CoPr1 >> 8) & 0xFF;
	Coolant_Pressure_Data[1] = CoPr1 & 0xFF;
	Coolant_Pressure_Data[2] = (CoPr2 >> 8) & 0xFF;
	Coolant_Pressure_Data[3] = CoPr2 & 0xFF;

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {

	if (hadc->Instance == ADC1) {
		UART_Transmit_ADC1_Readings();

		Fill_Strain_Gauge_Data();

#ifndef IS_FRONT_NODE
			Fill_Thermocouple_Data();
#endif
	}

	if (hadc->Instance == ADC2) {
		UART_Transmit_ADC2_Readings();

		Fill_Lin_Pot_Data();

#ifndef IS_FRONT_NODE
			Fill_Coolant_Pressure_Data();
#endif
	}

}

void ADC_Init(void) {
//	HAL_ADC_Start_DMA(&hadc1, ADC1_Readings, 7);
//	HAL_ADC_Start_DMA(&hadc2, ADC2_Readings, 4);
}

void ADC_Main(void) {


	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);

	HAL_ADC_PollForConversion(&hadc1, 10);
	HAL_ADC_PollForConversion(&hadc2, 10);

	for (int i = 0; i < 7; i++) {
		ADC1_Readings[i] = HAL_ADC_GetValue(&hadc1);
	}

	for (int i = 0; i < 4; i++) {
		ADC2_Readings[i] = HAL_ADC_GetValue(&hadc2);
	}

	HAL_ADC_Stop(&hadc1);
	HAL_ADC_Stop(&hadc2);


#ifdef IS_FRONT_NODE
		CAN_Transmit(CAN_ID_LIN_POT_FRONT, Lin_Pot_Data);
		CAN_Transmit(CAN_ID_STRAIN_GAUGE_FRONT, Strain_Gauge_Data);
#else
		CAN_Transmit(CAN_ID_LIN_POT_REAR, Lin_Pot_Data);
		CAN_Transmit(CAN_ID_STRAIN_GAUGE_REAR, Strain_Gauge_Data);

		CAN_Transmit(CAN_ID_COOLANT_PRESSURE, Coolant_Pressure_Data);
		CAN_Transmit(CAN_ID_THERMOCOUPLE, Thermocouple_Data);
#endif

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
