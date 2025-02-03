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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void ADC2_Main(void);

#endif /* INC_FEB_ADC_H_ */
