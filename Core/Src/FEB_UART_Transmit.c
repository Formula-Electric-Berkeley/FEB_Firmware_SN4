// **************************************** Includes ****************************************
#include "FEB_CAN.h"
#include "FEB_UART_Transmit.h"

extern UART_HandleTypeDef huart2;

// **************************************** Global Variables ********************************

static uint8_t counter = 0;

// **************************************** Functions ****************************************

void FEB_UART_Transmit_Process(void) {
	char str[2048];

	sprintf(str, "Counter: %u\n\r  ", counter);

	counter++;

	HAL_UART_Transmit(&huart2, (uint8_t *) str, strlen(str), 100);
}

void FEB_UART_Transmit_PingPong_Members(void) {
	char str[2048];

	sprintf(str, "Members: %u\n\r", FEB_CAN_GET_PONGED());

	HAL_UART_Transmit(&huart2, (uint8_t *) str, strlen(str), 100);
}

void FEB_UART_Transmit_CAN_Error(int ErrorCode) {
	char str[2048];

	switch (ErrorCode){
	case 0:
		break;
	case FEB_CAN_EXCEPTION_TIMEOUT:
		sprintf(str, "CAN Timeout while waiting for empty mailboxes!\n");
		break;
	case FEB_CAN_EXCEPTION_TxFAIL:
		sprintf(str, "Transmission failed to send!\n");
		break;
	}
	HAL_UART_Transmit(&huart2, (uint8_t *) str, strlen(str), 100);
}
