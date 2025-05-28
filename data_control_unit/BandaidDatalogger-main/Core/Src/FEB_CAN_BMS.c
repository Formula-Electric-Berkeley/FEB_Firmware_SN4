// ********************************** Includes & External **********************************

#include "FEB_CAN_BMS.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"


// *********************************** Struct ************************************

typedef enum {
	FEB_HB_NULL,
	FEB_HB_DASH,
	FEB_HB_PCU,
	FEB_HB_LVPDB,
	FEB_HB_DCU,
	FEB_HB_FSN,
	FEB_HB_RSN
} FEB_HB_t;

typedef struct BMS_MESSAGE_TYPE {
    FEB_SM_ST_t state;
    FEB_HB_t ping_ack; // ping message

} BMS_MESSAGE_TYPE;
BMS_MESSAGE_TYPE bms_message;

// **************************************** Functions ****************************************

uint8_t FEB_CAN_BMS_getDeviceSelect() {
	return bms_message.ping_ack;
}

FEB_SM_ST_t FEB_CAN_BMS_getState(){
	return bms_message.state;
}

// ***** CAN FUNCTIONS ****

uint8_t FEB_CAN_BMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	uint16_t ids[] = {FEB_CAN_BMS_STATE_FRAME_ID,FEB_CAN_BMS_ACCUMULATOR_TEMPERATURE_FRAME_ID};

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
			//Code Error - shutdown
		}
	}

	return filter_bank;
}

void FEB_CAN_BMS_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData) {
    switch (pHeader -> StdId){
        case FEB_CAN_BMS_STATE_FRAME_ID:
        	bms_message.state = RxData[0] & 0x1F;
        	bms_message.ping_ack = RxData[0] & 0xE0;

        	if ( bms_message.state == FEB_SM_ST_HEALTH_CHECK || bms_message.ping_ack == FEB_HB_DCU ) {
        		FEB_CAN_HEARTBEAT_Transmit();
        	}

            break;
    }
}





//// ********************************** Includes & External **********************************
//
//#include "FEB_CAN_BMS.h"
//
//// *********************************** Struct ************************************
//
//
//typedef struct BMS_MESSAGE_TYPE {
//     FEB_SM_ST_t state;
//     uint32_t last_message_time;
//} BMS_MESSAGE_TYPE;
//BMS_MESSAGE_TYPE BMS_MESSAGE;
//
//#define BMS_TIMEOUT 1200
//
//// **************************************** Functions ****************************************
//
//
//
//FEB_SM_ST_t FEB_CAN_BMS_Get_State(){
//	return BMS_MESSAGE.state;
//}
//
//
//// ***** CAN FUNCTIONS ****
//
//uint8_t FEB_CAN_BMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
//	uint16_t ids[] = {FEB_CAN_ID_BMS_STATE};
//
//	for (uint8_t i = 0; i < 1; i++) {
//		CAN_FilterTypeDef filter_config;
//
//	    // Standard CAN - 2.0A - 11 bit
//	    filter_config.FilterActivation = CAN_FILTER_ENABLE;
//		filter_config.FilterBank = filter_bank;
//		filter_config.FilterFIFOAssignment = FIFO_assignment;
//		filter_config.FilterIdHigh = ids[i] << 5;
//		filter_config.FilterIdLow = 0;
//		filter_config.FilterMaskIdHigh = 0xFFE0;
//		filter_config.FilterMaskIdLow = 0;
//		filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
//		filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
//		filter_config.SlaveStartFilterBank = 27;
//	    filter_bank++;
//
//		if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
//			//Code Error - shutdown
//		}
//	}
//
//	return filter_bank;
//}
//
//void FEB_CAN_BMS_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData) {
//    switch (pHeader -> StdId){
//        case FEB_CAN_ID_BMS_STATE:
//        	BMS_MESSAGE.state = RxData[0];
//        	if(is_r2d() && !(BMS_MESSAGE.state == FEB_SM_ST_DRIVE || BMS_MESSAGE.state == FEB_SM_ST_DRIVE_REGEN)){
//        		disable_r2d();
//        	}
//        	HAL_GetTick();
//        	break;
//    }
//}
//
//bool FEB_CAN_BMS_is_stale(){
//	if(HAL_GetTick() - BMS_MESSAGE.last_message_time >= BMS_TIMEOUT ){
//		return true;
//	}
//	return false;
//}
//
//
//
//
//
//
