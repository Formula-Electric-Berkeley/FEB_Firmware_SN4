#include "FEB_Coolant_ReedSW.h"

extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;

void Coolant_ReedSW_Main(void) {

	GPIO_PinState SWstate = HAL_GPIO_ReadPin(Coolant_ReedSW_GPIO_Port, Coolant_ReedSW_Pin); // 0 for off, 1 for on

	char msg[50];
	sprintf(msg, "Coolant Reed Switch State: %d", SWstate);
	HAL_UART_Transmit(&huart2, (uint8_t *) msg, sizeof((uint8_t *) msg), HAL_MAX_DELAY);

	CAN_TxHeaderTypeDef TxHeader;
	uint8_t TxData[8];
	uint32_t TxMailbox;

	TxHeader.DLC = 8; // Data length
	TxHeader.IDE = CAN_ID_STD; // Standard ID
	TxHeader.RTR = CAN_RTR_DATA; // Data frame
	TxHeader.StdId = 0x545; // CAN ID
	TxHeader.ExtId = 0; // Not used with standard ID

	// Fill the data
	TxData[0] = SWstate;

	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		// Transmission request error
		char msg[50];
		sprintf(msg, "CAN transmit error");
		HAL_UART_Transmit(&huart2, (uint8_t *) msg, sizeof((uint8_t *) msg), HAL_MAX_DELAY);
//		Error_Handler();
	}
}
