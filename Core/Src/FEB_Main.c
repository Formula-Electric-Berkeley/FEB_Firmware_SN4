// ******************************** Includes ********************************

#include "FEB_Main.h"


// ******************************** Functions ********************************

void FEB_Main_Setup() {
	//SM setup
	FEB_ADBMS_Init();
	//FEB_SM_Init();
	//FEB_CAN_Init();
	//IVT Setup
}

void FEB_Task_ADBMS() {
	FEB_ADBMS_Voltage_Process();
	//FEB_ADBMS_Temperature_Process();
	HAL_Delay(250);
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
		//FEB_ADBMS_UART_Transmit();
		//FEB_SM_UART_Transmit();
	}
}
