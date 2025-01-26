/*
 * FEB_Main.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Dorukhan User
 */

#include "FEB_Main.h"
#include "IMUCode.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;

BNO08x_RVC_Data sensor_data;
//char buffer[256];  // Buffer to hold formatted strings

void FEB_Main_Setup (void) {
	/*if (BNO08x_RVC_Init(&huart4) != 0) {
	        sprintf(buffer, "Failed to initialize BNO08x sensor.\r\n");
	        HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
	        Error_Handler();
	    }
	    sprintf(buffer, "BNO08x sensor initialized successfully.\r\n");
	    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY); */
}

void FEB_Main_Loop(void) {
	// Read sensor data
	BNO08x_RVC_Read(&huart4);
	/*if (BNO08x_RVC_Read(&huart4, &sensor_data) == 0) {
	// Data successfully read and debug-printed inside BNO08x_RVC_Read
	} else {
	// Handle reading error
	sprintf(buffer, "Failed to read sensor data.\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
	} */

	HAL_Delay(500); // Delay for 1 second

}
