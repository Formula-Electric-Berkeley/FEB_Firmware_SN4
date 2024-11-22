// **************************************** Includes ****************************************

#include "FEB_Main.h"

// **************************************** Functions ****************************************

void FEB_Main_Setup(void) {
	FEB_CAN_Init();
}

void FEB_Main_Loop(void) {
	FEB_UART_Transmit_Process();

	FEB_UART_Transmit_Speed();

	FEB_CAN_ICS_Transmit();

	HAL_Delay(1000);
}
