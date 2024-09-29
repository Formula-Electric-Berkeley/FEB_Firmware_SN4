#include "FEB_Main.h"

extern CAN_HandleTypeDef hcan1;

void FEB_Main_Setup(void){
	FEB_CAN_Init();
}

void FEB_Main_Loop(void){

	get_linear_potentiometer_reading();
//	FEB_CAN_Linear_Potentiometer_Transmit(&hcan1);
	HAL_Delay(1000);
}
