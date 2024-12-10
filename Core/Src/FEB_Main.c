// **************************************** Includes ****************************************

#include "FEB_Main.h"

#include "main.h"

// **************************************** Functions ****************************************

void FEB_Main_Setup(void) {
	FEB_CAN_Init();
}

void FEB_Main_Loop(void) {
	if(FEB_CAN_PINGPONG_MODE){
		FEB_CAN_PING();

		FEB_UART_Transmit_Process();

		FEB_UART_Transmit_PingPong_Members();

		HAL_GPIO_TogglePin(GPIOA, LD2_Pin);

		HAL_Delay(1000);
	}
}
