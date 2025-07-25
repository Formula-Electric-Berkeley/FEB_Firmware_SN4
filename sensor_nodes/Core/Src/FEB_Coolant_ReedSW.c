#include "FEB_Coolant_ReedSW.h"

extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;

uint8_t Coolant_Reed_SW_Data[8];

void Coolant_ReedSW_Main(void) {

	GPIO_PinState SWstate = HAL_GPIO_ReadPin(Coolant_ReedSW_GPIO_Port, Coolant_ReedSW_Pin); // 0 for off, 1 for on

	Coolant_Reed_SW_Data[0] = SWstate;

	#ifdef DEBUG_COOLANT_REEDSW_MAIN
	printf("Coolant Reed Switch: %u\r\n", SWstate);
	#endif

	CAN_Transmit(CAN_ID_COOLANT_REED_SW, Coolant_Reed_SW_Data);
}
