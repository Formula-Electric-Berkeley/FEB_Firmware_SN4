#include "FEB_DASH_CAN.h"

FEB_CAN_DASH_Message_t FEB_CAN_DASH_Message;

uint8_t FEB_CAN_DASH_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
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
//			FEB_SM_Set_Current_State(FEB_SM_ST_SHUTDOWN);
		}
	}

	//Initialize r2r to false on startup
	FEB_CAN_DASH_Message.r2r = false;

	return filter_bank;
}

void FEB_CAN_DASH_Store_Msg(CAN_RxHeaderTypeDef* rx_header, uint8_t rx_data[]) {
	switch(rx_header->StdId) {
		case FEB_CAN_DASH_IO_FRAME_ID:
				FEB_CAN_DASH_Message.r2r = ((rx_data[0] | 0b11111101) == 0b11111111);
				break;
	}
}


bool FEB_CAN_DASH_Get_R2R(){
	return FEB_CAN_DASH_Message.r2r;
}
