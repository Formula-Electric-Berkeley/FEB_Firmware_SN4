// **************************************** Includes ****************************************

#include "FEB_Main.h"

#include "main.h"

// **************************************** Functions ****************************************

void FEB_Main_Setup(void) {
	FEB_CAN_Init();
}

void FEB_Main_Loop(void) {

	FEB_UART_Transmit_Process(FEB_CAN_Dummy_Transmit());

	FEB_UART_Transmit_Speed();

	HAL_GPIO_TogglePin(GPIOA, LD2_Pin);

	HAL_Delay(1000);
}
