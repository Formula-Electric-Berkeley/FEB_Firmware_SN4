// ******************************** Includes & External ********************************

#include "FEB_IVT.h"

//IDs for the IVT
#define FEB_CAN_ID_IVT_COMMAND 0x411
#define FEB_CAN_ID_IVT_DEBUG 0x510
#define FEB_CAN_ID_IVT_RESPONSE 0x511
#define FEB_CAN_ID_IVT_CURRENT 0x521
#define FEB_CAN_ID_IVT_VOLTAGE_1 0x522
#define FEB_CAN_ID_IVT_VOLTAGE_2 0x523
#define FEB_CAN_ID_IVT_VOLTAGE_3 0x524
#define FEB_CAN_ID_IVT_TEMPERATURE 0x525
#define FEB_CAN_ID_IVT_POWER 0x526
#define FEB_CAN_ID_IVT_CURRENT_COUNTER 0x527
#define FEB_CAN_ID_IVT_ENERGY_COUNTER 0x528


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
extern UART_HandleTypeDef huart2;
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
	    	int32_t raw_current = (int32_t) ((rx_data[2] << 24) | (rx_data[3] << 16) | (rx_data[4] << 8) + rx_data[5]);
	    	FEB_CAN_IVT_Message.current_mA = ((float) raw_current) * (-0.001f) * 1000.0f;; // correct IVT for reversed direction
			break;
	    case FEB_CAN_ID_IVT_VOLTAGE_1:
	    	IVT_CAN_flag.voltage_1 = true;
	    	uint32_t raw_voltage_1 = ((uint32_t)rx_data[2] << 24) | ((uint32_t)rx_data[3] << 16) | ((uint32_t)rx_data[4] << 8) | (uint32_t)rx_data[5];
	    	FEB_CAN_IVT_Message.voltage_1_mV = (float) raw_voltage_1;
	    	break;
	    case FEB_CAN_ID_IVT_VOLTAGE_2:
	    	IVT_CAN_flag.voltage_2 = true;
	    	uint32_t raw_voltage_2 = ((uint32_t)rx_data[2] << 24) | ((uint32_t)rx_data[3] << 16) | ((uint32_t)rx_data[4] << 8) | (uint32_t)rx_data[5];
	    	FEB_CAN_IVT_Message.voltage_2_mV = (float) raw_voltage_2;
	    	break;
	    case FEB_CAN_ID_IVT_VOLTAGE_3:
	    	IVT_CAN_flag.voltage_3 = true;
	    	uint32_t raw_voltage_3 = ((uint32_t)rx_data[2] << 24) | ((uint32_t)rx_data[3] << 16) | ((uint32_t)rx_data[4] << 8) | (uint32_t)rx_data[5];
	    	FEB_CAN_IVT_Message.voltage_3_mV = (float) raw_voltage_3;
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

void FEB_IVT_Serial(void){
	static char str[128];
	sprintf(str, "ivt %f %f %f %f\n",
	FEB_CAN_IVT_Message.current_mA, FEB_CAN_IVT_Message.voltage_1_mV, FEB_CAN_IVT_Message.voltage_2_mV, FEB_CAN_IVT_Message.voltage_3_mV
	);

	//while (osMutexAcquire(FEB_UART_LockHandle, UINT32_MAX) != osOK);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
}

int32_t FEB_IVT_V1_Voltage(){
	return FEB_CAN_IVT_Message.voltage_1_mV;

}

int32_t FEB_IVT_V1_Current(){
	return FEB_CAN_IVT_Message.current_mA;

}
