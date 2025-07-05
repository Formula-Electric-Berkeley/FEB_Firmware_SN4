/*
 * FEB_Tire_Temp.h
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_TIRE_TEMP_H_
#define INC_FEB_TIRE_TEMP_H_

// ******************************** Includes ********************************

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

#include "FEB_Main.h"

// ******************************** Configuration ********************************

// Granular debug macros for Tire Temperature functions
#define DEBUG_READ_TIRE_TEMP
#define DEBUG_TIRE_TEMP_CONFIGURE
#define DEBUG_TIRE_TEMP_READ_DATA
#define DEBUG_TIRE_TEMP_CAN_RX

// ******************************** Functions ********************************

void Tire_Temp_Init(void);
void UART_Transmit(const char *string);
void Configure_Tire_Temp_Sensor(uint16_t currentCAN_ID, uint16_t newCAN_ID, float emissivity,
		uint8_t sampleFrequency, uint8_t channels, uint8_t bitrate);
void Read_Tire_Temp_Data(CAN_RxHeaderTypeDef RxHeader, uint8_t *RxData);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void Tire_Temp_Main(void);

#endif /* INC_FEB_TIRE_TEMP_H_ */
