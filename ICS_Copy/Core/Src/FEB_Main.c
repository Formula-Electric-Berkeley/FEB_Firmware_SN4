// **************************************** Includes ****************************************

#include "FEB_Main.h"

uint64_t bms_errors;
extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;

// ********************************** Variables **********************************
char buf[128];
uint8_t buf_len; //stolen from Main_Setup (SN2)

// **************************************** Functions ****************************************

void FEB_Main_Setup(void) {

	FEB_IO_ICS_Init();

	FEB_UI_Init();

	FEB_CAN_Init();
	// FEB_TPS2482_Setup();
	// memset(&bms_errors, 0xFF, sizeof(uint64_t));
}

void FEB_Main_Loop(void) {
	FEB_UI_Update();

	FEB_IO_ICS_Loop();

	FEB_Radio_Transmit();

	HAL_Delay(10);
	// FEB_CAN_HEARTBEAT_Transmit(&hcan1, &bms_errors);
}


