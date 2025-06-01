// ******************************** Includes ********************************

#include "FEB_Main.h"
#include "FEB_UART_Transmit.h"

// ******************************** Functions ********************************

void FEB_Main_Setup() {
	//SM setup
	FEB_ADBMS_Init();

	// Uncomment to Balance
	// FEB_Cell_Balance_Start();

	FEB_CAN_Init();
	FEB_SM_Init(); //this occurs last to transition out of boot
	FEB_CAN_Charger_Init();
	HAL_Delay(5);


	//IVT Setup
}

void FEB_Task_ADBMS() {
	FEB_ADBMS_Voltage_Process();
	FEB_ADBMS_Temperature_Process();
	HAL_Delay(5);
}

void FEB_Task_SM() {
	FEB_SM_Process();
}

void FEB_Task_Charge() {
	FEB_CAN_Charger_Process();
}
static int cyc=0;
bool balancing = true;
void FEB_Task_Balance() {
	if ( ++cyc < 4 ) {
		return;
	}
	cyc = 0;
	FEB_Cell_Balance_Process();
	HAL_Delay(500);
}

void FEB_Task_IVT() {
	FEB_CAN_IVT_Process();
//	FEB_IVT_V1_Transmit();

}

void FEB_Task_CAN() {
	FEB_SM_CAN_Transmit();
	FEB_ACC_VOLT_CAN_Transmit();
	FEB_ACC_TEMP_CAN_Transmit();
	FEB_ACC_FAULTS_CAN_Transmit();
}

static int cyc2=0;
void FEB_Task_UART() {
	if(++cyc2<15) {
		return;
	} else {
		cyc2=0;
		FEB_SM_UART_Transmit();
	//	FEB_Transmit_AIR_Status();
	//	FEB_MONITOR_UART_Transmit();
		FEB_IVT_V1_Transmit();
		FEB_ADBMS_UART_Transmit();
		FEB_CAN_Charger_UART_Transmit();
	}
}
