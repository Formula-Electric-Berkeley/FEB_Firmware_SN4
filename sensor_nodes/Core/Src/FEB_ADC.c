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

// ******************************************** Constants **********************************************

// ADC Configuration
#define ADC_RESOLUTION 4095           // 12-bit ADC resolution (2^12 - 1)
#define ADC_VREF 3.3f                 // Reference voltage in volts

// Linear Potentiometer Calibration Values
// Min/Max values are ADC readings at fully retracted/extended positions
// Length is in units of 10 micrometers (0.01mm)
#define LIN_POT_MIN_1 1120            // ADC value at minimum position (sensor 1)
#define LIN_POT_MAX_1 4095            // ADC value at maximum position (sensor 1)  
#define LIN_POT_LENGTH_1_uM_x_10 7500 // Total travel: 75mm = 7500 * 10μm

#define LIN_POT_MIN_2 1110            // ADC value at minimum position (sensor 2)
#define LIN_POT_MAX_2 4095            // ADC value at maximum position (sensor 2)
#define LIN_POT_LENGTH_2_uM_x_10 7500 // Total travel: 75mm = 7500 * 10μm

// ******************************************** Variables **********************************************

uint32_t ADC1_Readings[7];
uint16_t ADC2_Readings[4]; // 1st and 2nd are linear potentiometer, 3rd and 4th are coolant pressure

uint8_t Strain_Gauge_Data[8];
uint8_t Thermocouple_Data[8];
uint8_t Lin_Pot_Data[8];
uint8_t Coolant_Pressure_Data[8];

// ******************************************** Functions **********************************************

/**
 * @brief Convert linear potentiometer ADC reading to position in 10μm units
 * @param x Current ADC reading
 * @param length Total travel distance in 10μm units
 * @param min ADC reading at minimum position
 * @param max ADC reading at maximum position
 * @return Position in 10μm units (0 = fully extended, length = fully retracted)
 * @note Formula: position = length * (max - x) / (max - min)
 *       See: https://www.desmos.com/calculator/ieqxqbpwlz
 */
#define LINEAR_POTENTIOMETER_CONVERSION_10uM(x, length, min, max) \
    ((uint16_t)(((int32_t)(length) * ((int32_t)(max) - (int32_t)(x))) / ((int32_t)(max) - (int32_t)(min))))


/**
 * @brief Convert strain gauge ADC reading to output value
 * @param adc_value Raw ADC reading
 * @return Strain gauge value (currently raw ADC value, TODO: calibrate)
 */
uint16_t StrainGaugeConversion(uint32_t adc_value) {
	return adc_value & 0xFFFF;
}

/**
 * @brief Convert thermocouple ADC reading to temperature
 * @param adc_value Raw ADC reading
 * @return Temperature value (currently raw ADC value, TODO: calibrate)
 */
uint16_t ThermocoupleConversion(uint32_t adc_value) {
	return adc_value & 0xFFFF;
}

/**
 * @brief Convert coolant pressure sensor ADC reading to pressure
 * @param adc_value Raw ADC reading  
 * @return Pressure in millibar (mbar)
 * @note Sensor outputs 0.5-4.5V for 0-30 PSI range
 *       Formula: pressure_psi = (voltage - 0.5) * 30 / 4.0
 */
uint16_t CoolantPressureConversion(uint16_t adc_value) {
	float voltage = (float)adc_value * ADC_VREF / ADC_RESOLUTION;
	// Convert to PSI then to millibar (1 PSI = 68.9476 mbar)
	float pressure_psi = ((voltage - 0.5f) * 30.0f) / 4.0f;
	return (uint16_t)(pressure_psi * 68.9476f);
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

/**
 * @brief Pack linear potentiometer data into CAN transmission buffer
 * @note Data format: [0:1]=LinPot1 position, [2:3]=LinPot2 position (big-endian)
 *       Position values are in 10μm units
 */
void Fill_Lin_Pot_Data(void) {
	// Convert ADC readings to position in 10μm units
	uint16_t LinPot1 = LINEAR_POTENTIOMETER_CONVERSION_10uM(
		ADC2_Readings[0], LIN_POT_LENGTH_1_uM_x_10, LIN_POT_MIN_1, LIN_POT_MAX_1);
	uint16_t LinPot2 = LINEAR_POTENTIOMETER_CONVERSION_10uM(
		ADC2_Readings[1], LIN_POT_LENGTH_2_uM_x_10, LIN_POT_MIN_2, LIN_POT_MAX_2);
	
	// Pack data in big-endian format
	Lin_Pot_Data[0] = (LinPot1 >> 8) & 0xFF;  // LinPot1 high byte
	Lin_Pot_Data[1] = LinPot1 & 0xFF;         // LinPot1 low byte
	Lin_Pot_Data[2] = (LinPot2 >> 8) & 0xFF;  // LinPot2 high byte
	Lin_Pot_Data[3] = LinPot2 & 0xFF;         // LinPot2 low byte
	// Bytes 4-7 reserved for future use
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

/**
 * @brief Initialize ADC peripherals
 * @note Currently empty - ADC initialization handled by HAL in main.c
 */
void ADC_Init(void) {
	// ADC initialization is handled by CubeMX generated code
}

/**
 * @brief Main ADC processing routine
 * @note Called periodically by TIM6 interrupt at 10Hz
 *       Reads ADC channels, processes data, and transmits via CAN
 */
void ADC_Main(void) {
	HAL_StatusTypeDef status;
	
	// Start ADC2 conversion sequence
	status = HAL_ADC_Start(&hadc2);
	if (status != HAL_OK) {
		// Handle ADC start error
		return;
	}


//	for (int i = 0; i < 7; i++) {
// 		if (HAL_ADC_PollForConversion(&hadc1, 100) != HAL_OK) {
// 			continue;
// 		}
//		ADC1_Readings[i] = HAL_ADC_GetValue(&hadc1);
//	}

	// Read 4 channels from ADC2 (2x Linear Pots, 2x Coolant Pressure)
	for (int i = 0; i < 4; i++) {
		status = HAL_ADC_PollForConversion(&hadc2, 100);
		if (status == HAL_OK) {
			ADC2_Readings[i] = HAL_ADC_GetValue(&hadc2);
		} else {
			// Use last value on conversion error
			// ADC2_Readings[i] retains previous value
		}
	}

	// Stop ADC2
	HAL_ADC_Stop(&hadc2);
	
	// Process linear potentiometer data
	Fill_Lin_Pot_Data();

	// Transmit data via CAN if enabled
#if ADCS && SEND_CAN
	#if IS_FRONT_NODE
		CAN_Transmit(FEB_CAN_LINEAR_POTENTIOMETER_FRONT_FRAME_ID, Lin_Pot_Data);
		// TODO: Enable when strain gauge is connected
		// CAN_Transmit(CAN_ID_STRAIN_GAUGE_FRONT, Strain_Gauge_Data);
	#else
		CAN_Transmit(FEB_CAN_LINEAR_POTENTIOMETER_REAR_FRAME_ID, Lin_Pot_Data);
		// TODO: Enable when sensors are connected
		// CAN_Transmit(CAN_ID_STRAIN_GAUGE_REAR, Strain_Gauge_Data);
		// CAN_Transmit(CAN_ID_COOLANT_PRESSURE, Coolant_Pressure_Data);
		// CAN_Transmit(CAN_ID_THERMOCOUPLE, Thermocouple_Data);
	#endif
#endif

}
