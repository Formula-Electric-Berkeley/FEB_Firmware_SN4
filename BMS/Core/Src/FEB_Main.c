// ******************************** Includes ********************************

#include "FEB_Main.h"

// ******************************** Functions ********************************

void FEB_Main_Setup() {
	FEB_ADBMS_Init();
	//FEB_Balance_Start();
	//FEB_SM_Init();
	//FEB_CAN_Init();
	//IVT Setup
}

void FEB_Task_ADBMS() {
	FEB_ADBMS_Voltage_Process();
	FEB_ADBMS_Temperature_Process();
	//FEB_ADBMS_Temperature_Process();
	//FEB_Cell_Balance_Process();
	HAL_Delay(500);
}

void FEB_Task_SM() {
	//FEB_SM_Process();
}

void FEB_Task_Charge() {

}

void FEB_Task_Balance() {

}

void FEB_Task_IVT() {

}

void FEB_Task_CAN() {

}

void FEB_Task_UART() {
	if(FEB_SM_ST_DEBUG){

		//FEB_SM_UART_Transmit();
	}
}
