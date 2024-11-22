// **************************************** Includes ****************************************

#include "FEB_UART_Transmit.h"

extern UART_HandleTypeDef huart2;

// **************************************** Global Variables ********************************

static uint8_t counter = 0;

// **************************************** Functions ****************************************

void FEB_UART_Transmit_Process(void) {
	char str[1024];

	sprintf(str, "Counter: %u\n\r", counter);

	HAL_UART_Transmit(&huart2, (uint8_t *) str, strlen(str), 100);

	counter++;
}

void FEB_UART_Transmit_Speed(void) {
	char str[1024];

	sprintf(str, "Speed: %u\n\r", FEB_CAN_ICS_Get_Speed());

	HAL_UART_Transmit(&huart2, (uint8_t *) str, strlen(str), 100);
}
