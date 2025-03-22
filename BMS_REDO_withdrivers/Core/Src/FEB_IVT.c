// ******************************** Includes & External ********************************

#include "FEB_IVT.h"


// ******************************** Struct ********************************

typedef struct {
	volatile bool current;
	volatile bool voltage_1;
	volatile bool voltage_2;
	volatile bool voltage_3;
} IVT_CAN_flag_t;

// ******************************** Variables ********************************

static IVT_CAN_flag_t IVT_CAN_flag;
FEB_CAN_IVT_Message_t FEB_CAN_IVT_Message;

// ******************************** Functions ********************************

uint8_t FEB_CAN_IVT_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	uint16_t ids[] = {FEB_CAN_ID_IVT_CURRENT, FEB_CAN_ID_IVT_VOLTAGE_1, FEB_CAN_ID_IVT_VOLTAGE_2, FEB_CAN_ID_IVT_VOLTAGE_3};

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
//			FEB_SM_Set_Current_State(FEB_SM_ST_SHUTDOWN);
		}
	}

	return filter_bank;
}

void FEB_CAN_IVT_Store_Msg(CAN_RxHeaderTypeDef* rx_header, uint8_t rx_data[]) {
	switch(rx_header->StdId) {
	    case FEB_CAN_ID_IVT_CURRENT:
	    	IVT_CAN_flag.current = true;
	    	FEB_CAN_IVT_Message.current_mA = (rx_data[2] << 24) + (rx_data[3] << 16) + (rx_data[4] << 8) + rx_data[5];
			break;
	    case FEB_CAN_ID_IVT_VOLTAGE_1:
	    	IVT_CAN_flag.voltage_1 = true;
	    	FEB_CAN_IVT_Message.voltage_1_mV = (rx_data[2] << 24) + (rx_data[3] << 16) + (rx_data[4] << 8) + rx_data[5];
	    	break;
	    case FEB_CAN_ID_IVT_VOLTAGE_2:
	    	IVT_CAN_flag.voltage_2 = true;
	    	FEB_CAN_IVT_Message.voltage_2_mV = (rx_data[2] << 24) + (rx_data[3] << 16) + (rx_data[4] << 8) + rx_data[5];
	    	break;
	    case FEB_CAN_ID_IVT_VOLTAGE_3:
	    	IVT_CAN_flag.voltage_3 = true;
	    	FEB_CAN_IVT_Message.voltage_3_mV = (rx_data[2] << 24) + (rx_data[3] << 16) + (rx_data[4] << 8) + rx_data[5];
	    	break;
	}
}

void FEB_CAN_IVT_Process(void) {
	if (IVT_CAN_flag.current) {
		IVT_CAN_flag.current = false;
		// TODO: Check current flowing through battery
		// float current_A = FEB_CAN_IVT_Message.current_mA * 0.001;
	}
	if (IVT_CAN_flag.voltage_1) {
		IVT_CAN_flag.voltage_1 = false;
		if (FEB_SM_Get_Current_State() == FEB_SM_ST_PRECHARGE) {
			// TODO: Check precharge complete
			float voltage_V = (float) FEB_CAN_IVT_Message.voltage_1_mV * 0.001;
			float target_voltage_V = FEB_ADBMS_Get_Total_Voltage() * FEB_CONST_PRECHARGE_PCT;

			//Make sure to change this to target_voltage
			if (voltage_V >= 0.9*30) {
				FEB_SM_Transition(FEB_SM_ST_ENERGIZED);			}
		}
	}
	if (IVT_CAN_flag.voltage_2) {
		IVT_CAN_flag.voltage_2 = false;
		// ...
	}
	if (IVT_CAN_flag.voltage_3) {
		IVT_CAN_flag.voltage_3 = false;
		// ...
	}
}

int32_t FEB_IVT_V1_Voltage(){
	return FEB_CAN_IVT_Message.voltage_1_mV;

}
