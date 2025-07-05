/*
 * FEB_Comms.c
 *
 *  Created on: Apr 13, 2025
 *      Author: rahilpasha
 */

#include "FEB_Comms.h"

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;

void CAN_Transmit(uint16_t CAN_ID, uint8_t TxData[8])
{
	CAN_TxHeaderTypeDef TxHeader;

	uint32_t TxMailbox;

	TxHeader.DLC = 8; // Data length
	TxHeader.IDE = CAN_ID_STD; // Standard ID
	TxHeader.RTR = CAN_RTR_DATA; // Data frame
	TxHeader.StdId = CAN_ID; // CAN ID
	TxHeader.ExtId = 0; // Not used with standard ID

	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		// Transmission request error
		#ifdef DEBUG_COMMS_CAN_TRANSMIT
		printf("CAN transmit error\r\n");
		#endif
//		Error_Handler();
	}
}
