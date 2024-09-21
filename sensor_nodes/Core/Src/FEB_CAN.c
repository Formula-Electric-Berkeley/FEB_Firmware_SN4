// **************************************** Includes & External ****************************************

#include "FEB_CAN.h"

#include <string.h>
#include <stdio.h>

extern CAN_HandleTypeDef hcan1;

// **************************************** CAN Configuration ****************************************

CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
static CAN_RxHeaderTypeDef FEB_CAN_Rx_Header;

uint8_t FEB_CAN_Tx_Data[8];
uint8_t FEB_CAN_Rx_Data[8];

uint32_t FEB_CAN_Tx_Mailbox;

// **************************************** Functions ****************************************

void FEB_CAN_Init(void) {
	FEB_CAN_Filter_Config();
	if (HAL_CAN_Start(&hcan1) != HAL_OK) {
		// Error handle
	}
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
		// Error handle
	}
}

void FEB_CAN_Filter_Config(void) {
	// uint8_t filter_bank = 0;
	// filter_bank = FEB_CAN_Charger_Filter_Config(&hcan1, CAN_RX_FIFO0, filter_bank);
	// do nothing - not interested in reading data for now
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &FEB_CAN_Rx_Header, FEB_CAN_Rx_Data) == HAL_OK) {
		// do nothing - not interested in reading data for now
	}
}
