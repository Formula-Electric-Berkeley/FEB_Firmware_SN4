// ******************************** Includes ********************************

#include "FEB_Main.h"


// ******************************** Functions ********************************

void FEB_Main_Setup() {
	//SM setup
	FEB_ADBMS_Init();
	//CAN Filter Config Setup
	//IVT Setup
}

void FEB_Main_Task1_VT() {
	FEB_ADBMS_AcquireData();
	FEB_ADBMS_UART_Transmit();
	HAL_Delay(100);
}

void FEB_Main_Task2_SM() {

}

void FEB_Main_Task3_Charge() {

}

void FEB_Main_Task4_Balance() {

}

void FEB_Main_Task5_IVT() {

}

void FEB_Main_Task6_CAN() {

}
