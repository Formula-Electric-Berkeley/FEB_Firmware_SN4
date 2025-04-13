// ******************************** Includes ********************************

#include "FEB_Main.h"
#include "FEB_UART_Transmit.h"

// ******************************** Functions ********************************

void FEB_Main_Setup() {
	//SM setup
	FEB_ADBMS_Init();
	//FEB_Cell_Balance_Start();
	//FEB_CAN_Init();
	//FEB_SM_Init(); //this occurs last to transition out of boot
	HAL_Delay(5);


	//IVT Setup
}

void FEB_Task_ADBMS() {
	FEB_ADBMS_Voltage_Process();
	//FEB_Siren_Activate();
	FEB_ADBMS_Temperature_Process();
	HAL_Delay(5);
}

void FEB_Task_SM() {
	FEB_SM_Process();
	FEB_SM_State_Transmit();
}

void FEB_Task_Charge() {
	FEB_CAN_Charger_Process();
}

void FEB_Task_Balance() {
	FEB_Cell_Balance_Process();
	HAL_Delay(1500);
}

void FEB_Task_IVT() {
	FEB_CAN_IVT_Process();
	FEB_IVT_V1_Transmit();
	FEB_Transmit_AIR_Status();
}

void FEB_Task_CAN() {

}

void FEB_Task_UART() {
	if(FEB_SM_ST_DEBUG){
		//FEB_ADBMS_UART_Transmit();
//		FEB_SM_UART_Transmit();
	}
}
