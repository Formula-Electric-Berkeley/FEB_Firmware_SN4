// ********************************** Includes & External **********************************

#include "FEB_Main.h"

extern ADC_HandleTypeDef hadc1;
u_int64_t bms_errors;
extern UART_HandleTypeDef huart2;

// ********************************** Variables **********************************
char buf[128];
uint8_t buf_len; //stolen from Main_Setup (SN2)
const bool IS_TEST = true;
// ********************************** Functions **********************************

void FEB_Main_Setup(void){
	HAL_ADCEx_InjectedStart(&hadc1); //@lovehate - where does this go
//	FEB_Timer_Init();
//	FEB_TPS2482_Setup();
	FEB_CAN_Init(); //FEB_CAN_Init() // The transceiver must be connected otherwise you get sent into an infinite loop
	FEB_CAN_RMS_Setup();

	memset(&bms_errors, 0xFF, sizeof(uint64_t));
}

void FEB_Main_While(void){
//	FEB_CAN_ICS_Transmit();
	FEB_SM_ST_t bms_state = FEB_CAN_BMS_getState();

	if (IS_TEST == true) {
		FEB_Normalized_updateAcc();
		FEB_CAN_RMS_Process();
	}

	else { // not testing
		if (FEB_Ready_To_Drive() && (bms_state == FEB_SM_ST_DRIVE || bms_state == FEB_SM_ST_DRIVE_REGEN)) {
			FEB_Normalized_updateAcc();
			FEB_CAN_RMS_Process();

		} else {
			FEB_Normalized_setAcc0();
			FEB_CAN_RMS_Disable();
		}
	}

	FEB_HECS_update();

	FEB_CAN_RMS_Torque();
	FEB_Normalized_CAN_sendBrake();

	FEB_CAN_HEARTBEAT_Transmit(&hcan1, &bms_errors);
	HAL_Delay(10);
}
