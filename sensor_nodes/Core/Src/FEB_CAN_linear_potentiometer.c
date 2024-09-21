// ******************************** Includes & External ********************************

#include "FEB_CAN_linear_potentiometer.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint8_t FEB_CAN_Tx_Data[8];
extern uint32_t FEB_CAN_Tx_Mailbox;

#define FEB_CAN_ID_LINEAR_POTENTIOMETER 0x00;
uint16_t measurement;

// ******************************** Functions ********************************

void FEB_CAN_Linear_Potentiometer_Transmit(CAN_HandleTypeDef* hcan) {
	// Initialize Transmission Header
	FEB_CAN_Tx_Header.DLC = 2;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_ID_LINEAR_POTENTIOMETER;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Configure FEB_CAN_Tx_Data
	measurement = get_linear_potentiometer_reading();
    FEB_CAN_Tx_Data[0] = (uint8_t)(measurement & 0xFF);
	FEB_CAN_Tx_Data[1] = (uint8_t)((measurement >> 8) & 0xFF);

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
	}
}
