/*
 * FEB_LinPot.h
 *
 *  Created on: Feb 2, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_ADC_H_
#define INC_FEB_ADC_H_

// **************************************** Includes ****************************************

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// **************************************** Functions ****************************************

uint16_t LinearPotentiometerConversion(uint32_t adc_value);
uint16_t CoolantPressureConversion(uint32_t adc_value);
void UART_Transmit_ADC1_Readings(void);
void UART_Transmit_ADC2_Readings(void);
void Fill_CAN_Data_ADC(void);
void CAN_ADC_Transmit(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void ADC2_Main(void);

#endif /* INC_FEB_ADC_H_ */
