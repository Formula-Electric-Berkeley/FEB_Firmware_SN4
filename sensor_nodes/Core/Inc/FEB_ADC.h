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

uint32_t get_linear_potentiometer_reading(void);

#endif /* INC_FEB_ADC_H_ */
