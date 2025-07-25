/*
 * FEB_WSS.h
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_WSS_H_
#define INC_FEB_WSS_H_

// **************************************** Includes ****************************************

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FEB_Main.h"

// **************************************** Configuration ****************************************

#define DEBUG_READ_WSS

// **************************************** Functions ****************************************

void WSS_Init(void);
void Fill_WSS_Data(void);
void update_WSS_ticks(int32_t *ticks, uint16_t *prev_counter, TIM_HandleTypeDef *htim);
void handle_reverse_ticks(int32_t *ticks, char *direction);
void WSS_Main(void);


#endif /* INC_FEB_WSS_H_ */
