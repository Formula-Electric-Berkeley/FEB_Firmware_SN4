// **************************************** Includes ****************************************

#include "FEB_CAN_ICS.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint8_t FEB_CAN_Tx_Data[8];
extern uint32_t FEB_CAN_Tx_Mailbox;

// ******************************** Variables ********************************
//bool READY_TO_DRIVE = 0;
// static bool previous_button_state = false;  // Currently unused, commented out to avoid warning
static bool READY_TO_DRIVE = false;

// **************************************** Functions ****************************************

uint8_t FEB_CAN_ICS_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
    // For multiple filters, create array of filter IDs and loop over IDs.

	uint16_t ids[] = {FEB_CAN_DASH_IO_FRAME_ID};

	for (uint8_t i = 0; i < 1; i++) {
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
			// Shutdown
		}
	}

	return filter_bank;
}

void FEB_CAN_ICS_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]) {
	switch(rx_header->StdId) {
		case FEB_CAN_DASH_IO_FRAME_ID:

			bool current_button_state = ((rx_data[0] | 0b11111101) == 0b11111111);

			READY_TO_DRIVE = current_button_state;

			break;
	}
}

bool FEB_Ready_To_Drive() {
	return READY_TO_DRIVE;
}
