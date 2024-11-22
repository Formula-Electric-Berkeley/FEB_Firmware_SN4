// **************************************** Includes ****************************************

#include "FEB_CAN_ICS.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint8_t FEB_CAN_Tx_Data[8];
extern uint32_t FEB_CAN_Tx_Mailbox;

// ******************************** Variables ********************************

typedef struct {
	volatile uint8_t speed;
} FEB_CAN_ICS_Message_t;
FEB_CAN_ICS_Message_t FEB_CAN_ICS_Message;

uint8_t speed = 0xA0;

// **************************************** Functions ****************************************

uint8_t FEB_CAN_ICS_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
    // For multiple filters, create array of filter IDs and loop over IDs.

	CAN_FilterTypeDef filter_config;

    // Standard CAN - 2.0A - 11 bit
    filter_config.FilterActivation = CAN_FILTER_ENABLE;
	filter_config.FilterBank = filter_bank;
	filter_config.FilterFIFOAssignment = FIFO_assignment;
	filter_config.FilterIdHigh = 0;
	filter_config.FilterIdLow = 0;
	filter_config.FilterMaskIdHigh = 0;
	filter_config.FilterMaskIdLow = 0;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.SlaveStartFilterBank = 27;
    filter_bank++;

	if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
        // Code Error - Shutdown
	}

	return filter_bank;
}

void FEB_CAN_ICS_Rx_Handler(CAN_RxHeaderTypeDef *FEB_CAN_Rx_Header, uint8_t FEB_CAN_Rx_Data[]) {
	speed = FEB_CAN_Rx_Data[0];
}

void FEB_CAN_ICS_Transmit(void) {
	FEB_CAN_Tx_Header.DLC = 8;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_ID_ICS_HIL;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = (uint8_t) 0xA1;
	FEB_CAN_Tx_Data[1] = (uint8_t) 0xA2;
	FEB_CAN_Tx_Data[2] = (uint8_t) 0xA3;
	FEB_CAN_Tx_Data[3] = (uint8_t) 0xA4;
	FEB_CAN_Tx_Data[4] = (uint8_t) 0xA5;
	FEB_CAN_Tx_Data[5] = (uint8_t) 0xA6;
	FEB_CAN_Tx_Data[6] = (uint8_t) 0xA7;
	FEB_CAN_Tx_Data[7] = (uint8_t) 0xA8;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		//
	}
}

uint8_t FEB_CAN_ICS_Get_Speed(void) {
	return speed;
}
