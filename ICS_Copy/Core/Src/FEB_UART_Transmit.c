// **************************************** Includes ****************************************

#include "FEB_UART_Transmit.h"

extern UART_HandleTypeDef huart3;

// **************************************** Global Variables ********************************

static uint8_t counter = 0;

// **************************************** Functions ****************************************

void FEB_UART_Transmit_Int(uint8_t data) {
	char str[1024];

	sprintf(str, "Data: %d\n", data);

	HAL_UART_Transmit(&huart3, (uint8_t *) str, strlen(str), 100);
}

void FEB_UART_Transmit_String(char* data_str) {
	char str[1024];

	sprintf(str, "%s\n", data_str);

	HAL_UART_Transmit(&huart3, (uint8_t *) str, strlen(str), 100);
}

void FEB_UART_Transmit_Process(void) {
	char str[1024];

	sprintf(str, "Counter: %u\n", counter);

	HAL_UART_Transmit(&huart3, (uint8_t *) str, strlen(str), 100);

	counter++;
}

void FEB_UART_Transmit_Speed(void) {
	char str[1024];

	sprintf(str, "Speed: %u\n", FEB_CAN_ICS_Get_Speed());

	HAL_UART_Transmit(&huart3, (uint8_t *) str, strlen(str), 100);
}
