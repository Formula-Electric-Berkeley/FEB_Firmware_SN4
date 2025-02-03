/*
 * FEB_Main.h
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_MAIN_H_
#define INC_FEB_MAIN_H_

// ******************************** Includes ********************************

#include <FEB_ADC.h>
#include <stm32f4xx_hal.h>
#include <stdint.h>

#include "FEB_WSS.h"
#include "FEB_Steering.h"
#include "FEB_Tire_Temp.h"

// ******************************** Functions ********************************

void FEB_Main_Setup(void);
void FEB_Main_While(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* INC_FEB_MAIN_H_ */
