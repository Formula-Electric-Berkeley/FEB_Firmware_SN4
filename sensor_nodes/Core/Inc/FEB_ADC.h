/*
 * FEB_LinPot.h
 *
 *  Created on: Feb 2, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_ADC_H_
#define INC_FEB_ADC_H_

// **************************************** Includes ****************************************

#include "FEB_Comms.h"

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// **************************************** Functions ****************************************

uint16_t StrainGaugeConversion(uint16_t adc_value);
uint16_t ThermocoupleConversion(uint16_t adc_value);
uint16_t LinearPotentiometerConversion(uint16_t adc_value);
uint16_t CoolantPressureConversion(uint16_t adc_value);
void UART_Transmit_ADC1_Readings(void);
void UART_Transmit_ADC2_Readings(void);
void Fill_Strain_Gauge_Data(void);
void Fill_Thermocouple_Data(void);
void Fill_Lin_Pot_Data(void);
void Fill_Coolant_Pressure_Data(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void ADC_Init(void);
void ADC_Main(void);

#endif /* INC_FEB_ADC_H_ */
