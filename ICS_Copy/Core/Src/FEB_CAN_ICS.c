// **************************************** Includes ****************************************

#include "FEB_CAN_ICS.h"

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart3;
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint8_t FEB_CAN_Tx_Data[8];
extern uint32_t FEB_CAN_Tx_Mailbox;

// ******************************** Variables ********************************

ICS_UI_Values_t ICS_UI_Values;

// **************************************** Functions ****************************************

uint8_t FEB_CAN_ICS_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
    // For multiple filters, create array of filter IDs and loop over IDs.

//	CAN_FilterTypeDef filter_config;
//
//	// Standard CAN - 2.0A - 11 bit
//	filter_config.FilterActivation = CAN_FILTER_ENABLE;
//	filter_config.FilterBank = filter_bank;
//	filter_config.FilterFIFOAssignment = FIFO_assignment;
//	filter_config.FilterIdHigh = 0;
//	filter_config.FilterIdLow = 0;
//	filter_config.FilterMaskIdHigh = 0;
//	filter_config.FilterMaskIdLow = 0;
//	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
//	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
//	filter_config.SlaveStartFilterBank = 27;
//	filter_bank++;
//
//	if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
//		// Code Error - Shutdown
//	}

	uint16_t ids[] = {
	    FEB_CAN_ID_BMS_STATE,
	    FEB_CAN_ID_BMS_VOLTAGE,
	    FEB_CAN_ID_BMS_TEMPERATURE,
	    FEB_CAN_ID_RMS_MOTOR
	};

	for (uint8_t i = 0; i < 4; i++) {
		CAN_FilterTypeDef filter_config;

		// Standard CAN - 2.0A - 11 bit
		filter_config.FilterActivation = CAN_FILTER_ENABLE;
		filter_config.FilterBank = filter_bank;
		filter_config.FilterFIFOAssignment = FIFO_assignment;
		filter_config.FilterIdHigh = ids[i] << 5;
		filter_config.FilterIdLow = 0;
		filter_config.FilterMaskIdHigh = 0xFFE0;
		filter_config.FilterMaskIdLow = 0;
		filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
		filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
		filter_config.SlaveStartFilterBank = 27;
		filter_bank++;

		if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
			// Code Error - Shutdown
		}
	}

	return filter_bank++;
}

void FEB_CAN_ICS_Rx_Handler(CAN_RxHeaderTypeDef *FEB_CAN_Rx_Header, uint8_t FEB_CAN_Rx_Data[]) {
	switch(FEB_CAN_Rx_Header->StdId) {
		case FEB_CAN_ID_BMS_STATE:
			ICS_UI_Values.bms_state = FEB_CAN_Rx_Data[0];
			break;
		case FEB_CAN_ID_BMS_VOLTAGE:
			ICS_UI_Values.pack_voltage = (FEB_CAN_Rx_Data[0] << 8) + FEB_CAN_Rx_Data[1];
			ICS_UI_Values.min_voltage = (FEB_CAN_Rx_Data[2] << 8) + FEB_CAN_Rx_Data[3]; //2.4-4.3
			uint8_t x1 = FEB_CAN_Rx_Data[1];
			uint8_t x2 = FEB_CAN_Rx_Data[2];
			break;
		case FEB_CAN_ID_BMS_TEMPERATURE:
			ICS_UI_Values.acc_temp = (FEB_CAN_Rx_Data[0] << 8) + FEB_CAN_Rx_Data[1];
			break;
		case FEB_CAN_ID_RMS_MOTOR:
			if (FEB_CAN_Rx_Data[3] == 255) {
				ICS_UI_Values.motor_speed = 0;
			} else {
				ICS_UI_Values.motor_speed = (FEB_CAN_Rx_Data[2] << 8) + FEB_CAN_Rx_Data[3];
				memcpy(&(ICS_UI_Values.motor_speed), FEB_CAN_Rx_Data+2, 2);
			}
			uint8_t x3 = FEB_CAN_Rx_Data[2];
			uint8_t x4 = FEB_CAN_Rx_Data[3];
			uint16_t x5 = ICS_UI_Values.motor_speed;
			break;
//		case FEB_CAN_ID_IVT_VOLTAGE_1:
//			ICS_UI_Values.ivt_voltage = ((FEB_CAN_Rx_Data[2] << 24) + (FEB_CAN_Rx_Data[3] << 16) + (FEB_CAN_Rx_Data[4] << 8) + FEB_CAN_Rx_Data[5]) * 0.001;
//			break;
	}
}

void FEB_CAN_ICS_Transmit_Button_State(uint8_t transmit_button_state) {
	FEB_CAN_Tx_Header.DLC = 1;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_ID_ICS_BUTTON_STATE;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = transmit_button_state;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		//
	}
}
