#include "FEB_CAN_Heartbeat.h"

FEB_CAN_DEV FEB_CAN_NETWORK[FEB_NUM_CAN_DEV]={0};

uint8_t FEB_CAN_Heartbeat_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	uint16_t ids[] = {FEB_CAN_PCU_HEARTBEAT_FRAME_ID, FEB_CAN_DASH_HEARTBEAT_FRAME_ID, FEB_CAN_LVPDB_HEARTBEAT_FRAME_ID, 
						FEB_CAN_DCU_HEARTBEAT_FRAME_ID, FEB_CAN_FRONT_SENSOR_HEARTBEAT_MESSAGE_FRAME_ID, 
						FEB_CAN_REAR_SENSOR_HEARTBEAT_MESSAGE_FRAME_ID
					};

	for (uint8_t i = 0; i < 6; i++) {
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
		}
	}
	return filter_bank;
}

void FEB_CAN_Heartbeat_Store_Msg(CAN_RxHeaderTypeDef* rx_header, uint8_t RxData[]) {
	switch(rx_header->StdId) {
		uint8_t device;
		case 0:
	    	device = 0;
			FEB_CAN_NETWORK[device].last_received = HAL_GetTick();
			FEB_CAN_NETWORK[device].FAck = 0;
			FEB_CAN_NETWORK[device].LaOn = 0;
			break;
		case FEB_CAN_DASH_HEARTBEAT_FRAME_ID:
			device = FEB_HB_DASH;
			FEB_CAN_NETWORK[device].last_received = HAL_GetTick();
			FEB_CAN_NETWORK[device].FAck = 0;
			FEB_CAN_NETWORK[device].LaOn = 0;
			FEB_CAN_NETWORK[device].initialized = 0;
			break;
		case FEB_CAN_PCU_HEARTBEAT_FRAME_ID: 
			device = FEB_HB_PCU;
			FEB_CAN_NETWORK[device].last_received = HAL_GetTick();
			FEB_CAN_NETWORK[device].FAck = 0;
			FEB_CAN_NETWORK[device].LaOn = 0;
			FEB_CAN_NETWORK[device].initialized = 0;
			break;
		case FEB_CAN_LVPDB_HEARTBEAT_FRAME_ID:
			device = FEB_HB_LVPDB;
			FEB_CAN_NETWORK[device].last_received = HAL_GetTick();
			FEB_CAN_NETWORK[device].FAck = 0;
			FEB_CAN_NETWORK[device].LaOn = 0;
			FEB_CAN_NETWORK[device].initialized = 0;
			break;
		case FEB_CAN_DCU_HEARTBEAT_FRAME_ID:
			device = FEB_HB_DCU;
			FEB_CAN_NETWORK[device].last_received = HAL_GetTick();
			FEB_CAN_NETWORK[device].FAck = 0;
			FEB_CAN_NETWORK[device].LaOn = 0;
			FEB_CAN_NETWORK[device].initialized = 0;
			break;
		case FEB_CAN_FRONT_SENSOR_HEARTBEAT_MESSAGE_FRAME_ID:
			device = FEB_HB_FSN;
			FEB_CAN_NETWORK[device].last_received = HAL_GetTick();
			FEB_CAN_NETWORK[device].FAck = 0;
			FEB_CAN_NETWORK[device].LaOn = 0;
			FEB_CAN_NETWORK[device].initialized = 0;
			break;
		case FEB_CAN_REAR_SENSOR_HEARTBEAT_MESSAGE_FRAME_ID:
			device = FEB_HB_RSN;
			FEB_CAN_NETWORK[device].last_received = HAL_GetTick();
			FEB_CAN_NETWORK[device].FAck = 0;
			FEB_CAN_NETWORK[device].LaOn = 0;
			FEB_CAN_NETWORK[device].initialized = 0;
			break;
	}
}

FEB_DEV_STATUS FEB_GetStatus(uint8_t device_index) {
	if (FEB_CAN_NETWORK[device_index].initialized) {
		return INITIALIZED;
	} else if (FEB_CAN_NETWORK[device_index].FAck < FEB_FACK_THRESHOLD) {
		return CONNECTED;
	} else {
		return DISCONNECTED;
	}
}

FEB_DEV_STATUS FEB_COMBINED_STATUS() {
	FEB_DEV_STATUS dash = FEB_GetStatus(DEV_IND_DASH);
	if (dash == INITIALIZED) return INITIALIZED;

	FEB_DEV_STATUS pcu = FEB_GetStatus(DEV_IND_PCU);
	if (pcu == INITIALIZED) return INITIALIZED;

	if (dash == DISCONNECTED || pcu == DISCONNECTED) return DISCONNECTED;

	return CONNECTED;
}

// FEB_DEV_STATUS CheckAccumulatorStatus(void) {
//     bool any_initialized = false;
//     bool all_fAck_low = true;

//     for (int i = 0; i < FEB_NUM_CAN_DEV; ++i) {
//         if (FEB_CAN_NETWORK[i].initialized) {
//             any_initialized = true;
//             break;
//         }
//         if (FEB_CAN_NETWORK[i].FAck >= 3) {
//             all_fAck_low = false;
//         }
//     }

//     if (any_initialized) {
//         return INITIALIZED;
//     } else if (all_fAck_low) {
//         return CONNECTED;
//     } else {
//         return DISCONNECTED;
//     }
// }