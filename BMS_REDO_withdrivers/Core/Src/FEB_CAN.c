
// **************************************** Includes & External ****************************************
#include "FEB_HW.h"
#include "FEB_CAN.h"
#include "FEB_IVT.h"
#include "FEB_DASH_CAN.h"
#include "FEB_UART_Transmit.h"

extern CAN_HandleTypeDef hcan1;
extern accumulator_t FEB_ACC;
extern uint8_t ERROR_TYPE;
// **************************************** CAN Configuration ****************************************

CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
static CAN_RxHeaderTypeDef FEB_CAN_Rx_Header;
extern FEB_CAN_DEV FEB_CAN_NETWORK[FEB_NUM_CAN_DEV];
static FEB_DEV_INDEX ping_alive;
uint8_t FEB_CAN_Tx_Data[8];
uint8_t FEB_CAN_Rx_Data[8];
uint32_t FEB_CAN_Tx_Mailbox;
//uint8_t FEB_CAN_PONGED;


// **************************************** Functions ****************************************

void FEB_CAN_Init(void) {
	FEB_CAN_Filter_Config();

	if (HAL_CAN_Start(&hcan1) != HAL_OK) {
        // Code Error - Shutdown
	}

	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	//TODO: Create directory
	for ( size_t i = 0; i < FEB_NUM_CAN_DEV; ++i ) {
		FEB_CAN_NETWORK[i].FAck = FEB_FACK_THRESHOLD;
		// FEB_CAN_NETWORK[i].LaOn=0;
		// FEB_CAN_NETWORK[i].last_received=0;
		FEB_CAN_NETWORK[i].initialized = INITIALIZED_THRESHOLD;
	}

	ping_alive = 0;
}

void FEB_CAN_Filter_Config(void) {
	uint8_t filter_bank = 0;
	filter_bank = FEB_CAN_IVT_Filter_Config(&hcan1, CAN_RX_FIFO0, filter_bank);
	filter_bank = FEB_CAN_DASH_Filter_Config(&hcan1,CAN_RX_FIFO0, filter_bank);
	filter_bank = FEB_CAN_Charger_Filter_Config(&hcan1,CAN_RX_FIFO0, filter_bank);
	filter_bank = FEB_CAN_Heartbeat_Filter_Config(&hcan1,CAN_RX_FIFO0, filter_bank);
//	filter_bank = FEB_CAN_Filter(&hcan1, CAN_RX_FIFO0, filter_bank);

	// Assign Filter
    //filter_bank = Function(&hcan1, CAN_RX_FIFO0, filter_bank);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &FEB_CAN_Rx_Header, FEB_CAN_Rx_Data) == HAL_OK) {

		FEB_CAN_IVT_Store_Msg(&FEB_CAN_Rx_Header, FEB_CAN_Rx_Data);
		FEB_CAN_DASH_Store_Msg(&FEB_CAN_Rx_Header,FEB_CAN_Rx_Data);
		FEB_CAN_Charger_Store_Msg(&FEB_CAN_Rx_Header,FEB_CAN_Rx_Data);

		FEB_CAN_Heartbeat_Store_Msg(&FEB_CAN_Rx_Header,FEB_CAN_Rx_Data);
	}
}

void FEB_SM_CAN_Transmit(void) {

	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 2;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_BMS_STATE_FRAME_ID;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	uint8_t relay_state = ((FEB_PIN_RD(PN_PC_REL) & 1) << 2) | ((FEB_PIN_RD(PN_AIRP_SENSE) & 1) << 1) | ( FEB_PIN_RD(PN_SHS_IN) &0b1 );
	uint8_t gpio_sense = ((FEB_PIN_RD(PN_PC_REL) & 1) << 2) | ((FEB_PIN_RD(PN_AIRP_SENSE) & 1) << 1) | ( FEB_PIN_RD(PN_SHS_IN) &0b1 );

	FEB_CAN_NETWORK[ping_alive].last_received = 0;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = FEB_SM_Get_Current_State();
	FEB_CAN_Tx_Data[0] |= (ping_alive & 0x0F) << 5;
	FEB_CAN_Tx_Data[1] = ((gpio_sense & 0x3F) << 3) | (relay_state & 0x03);

	// Delay until mailbox available
	int timeout =0;
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {if(++timeout==1000)break;}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// FEB_SM_Set_Current_State(FEB_SM_ST_SHUTDOWN);
	}

	if (FEB_CAN_NETWORK[ping_alive].initialized > 0) {
		FEB_CAN_NETWORK[ping_alive].initialized--;
	}
	ping_alive = (ping_alive + 1) % FEB_NUM_CAN_DEV;

	for ( int i = 0; i < FEB_NUM_CAN_DEV; i++ ) {
		FEB_CAN_NETWORK[i].LaOn += 1;
	}

	int prev_device_index;
	if (ping_alive == 0) {
		prev_device_index = FEB_NUM_CAN_DEV - 1;
	} else {
		prev_device_index = (ping_alive - 1) % FEB_NUM_CAN_DEV;
	}
	
	if (FEB_CAN_NETWORK[prev_device_index].last_received == 0) {
		FEB_CAN_NETWORK[prev_device_index].FAck++;
	}

	// for(int i =0;i<FEB_NUM_CAN_DEV;i++){
	// 	if(FEB_CAN_NETWORK[i].FAck<2)return;
	// }
	//FEB_SM_Transition(FEB_SM_ST_FREE);
}

void FEB_CELL_CAN_Transmit(void) {

	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 6;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_BMS_CELL_DATA_FRAME_ID;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	uint8_t cell_flags = 0;
	uint8_t cell_num = 0;
	uint8_t bank_num = 0;
	uint16_t cell_volt_mV = 0;
	int16_t cell_temp_C = 0;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = cell_flags & 0x0F;
	FEB_CAN_Tx_Data[1] = ((bank_num & 0x0F) << 4) | (cell_num & 0x0F);
	FEB_CAN_Tx_Data[2] = cell_volt_mV;
	FEB_CAN_Tx_Data[4] = cell_temp_C;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// FEB_SM_Set_Current_State(FEB_SM_ST_SHUTDOWN);
	}
}

void FEB_ACC_VOLT_CAN_Transmit(void) {

	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 6;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_BMS_ACCUMULATOR_VOLTAGE_FRAME_ID;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	float pack_volt_mV = FEB_ACC.total_voltage_V;
	float min_cell_volt_mV = FEB_ACC.banks[0].cells[0].voltage_V;
	float max_cell_volt_mV = FEB_ACC.banks[0].cells[0].voltage_V;

	for ( size_t i = 0; i < FEB_NBANKS; ++i) {
		for ( size_t j = 0; j < FEB_NUM_CELL_PER_BANK; ++j) {
			min_cell_volt_mV = fmin(min_cell_volt_mV, FEB_ACC.banks[i].cells[j].voltage_V);
			max_cell_volt_mV = fmax(max_cell_volt_mV, FEB_ACC.banks[i].cells[j].voltage_V);
		}
	}

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = ((uint16_t) (pack_volt_mV * 100.0f) & 0xFF);
	FEB_CAN_Tx_Data[1] = (((uint16_t) (pack_volt_mV * 100.0f) >> 8) & 0xFF);
	FEB_CAN_Tx_Data[2] = ((uint16_t) (min_cell_volt_mV * 1000) & 0xFF);
	FEB_CAN_Tx_Data[3] = (((uint16_t) (min_cell_volt_mV * 1000) >> 8) & 0xFF);
	FEB_CAN_Tx_Data[4] = ((uint16_t) (max_cell_volt_mV * 1000) & 0xFF);
	FEB_CAN_Tx_Data[5] = (((uint16_t)( max_cell_volt_mV * 1000) >> 8) & 0xFF);

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// FEB_SM_Set_Current_State(FEB_SM_ST_SHUTDOWN);
	}
}

void FEB_ACC_TEMP_CAN_Transmit(void) {

	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 6;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_BMS_ACCUMULATOR_TEMPERATURE_FRAME_ID;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	double pack_temp_C = 0;
	float min_cell_temp_C = fmax(FEB_ACC.banks[0].temp_sensor_readings_V[0], 0);
	float max_cell_temp_C = FEB_ACC.banks[0].temp_sensor_readings_V[0];

	for ( size_t i = 0; i < FEB_NBANKS; ++i) {
		for ( size_t j = 0; j < FEB_NUM_CELL_PER_BANK; ++j) {
			min_cell_temp_C = fmin(min_cell_temp_C, fmax(FEB_ACC.banks[i].temp_sensor_readings_V[j], 0));
			max_cell_temp_C = fmax(max_cell_temp_C, FEB_ACC.banks[i].temp_sensor_readings_V[j]);
			pack_temp_C += FEB_ACC.banks[i].temp_sensor_readings_V[j] / (1.0 * FEB_NBANKS * FEB_NUM_CELL_PER_BANK);
		}
	}

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = ((int16_t) (pack_temp_C * 100) & 0xFF);
	FEB_CAN_Tx_Data[1] = (((int16_t) (pack_temp_C * 100) >> 8) & 0xFF);
	FEB_CAN_Tx_Data[2] = ((int16_t) (min_cell_temp_C * 100) & 0xFF);
	FEB_CAN_Tx_Data[3] = (((int16_t) (min_cell_temp_C * 100) >> 8) & 0xFF);
	FEB_CAN_Tx_Data[4] = ((int16_t) (max_cell_temp_C * 100) & 0xFF);
	FEB_CAN_Tx_Data[5] = (((int16_t) (max_cell_temp_C * 100) >> 8) & 0xFF);

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// FEB_SM_Set_Current_State(FEB_SM_ST_SHUTDOWN);
	}
}

void FEB_ACC_FAULTS_CAN_Transmit(void) {

	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 1;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_ACCUMULATOR_FAULTS_FRAME_ID;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	FEB_Relay_State bms_fault_sense = FEB_PIN_RD(PN_INDICATOR); // closed is fault
	FEB_Relay_State imd_fault_sense = FEB_PIN_RD(PN_SHS_IMD); //  open is fault

	uint8_t fault = 0x00;

	if (bms_fault_sense == FEB_RELAY_STATE_CLOSE) {
		fault = fault | 0x01;
	} else {
		fault = fault & 0xFE;
	}

	if (imd_fault_sense == FEB_RELAY_STATE_OPEN) {
			fault = fault | 0x02;
	} else {
			fault = fault & 0xFD;
	}

	fault |= ERROR_TYPE;
	//ERROR_TYPE
	// 0x1X : temp range exception
	// 0x2X : temp under read
	// 0x3X : voltage exception
	// 0x4X : relay trip

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = fault;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// FEB_SM_Set_Current_State(FEB_SM_ST_SHUTDOWN);
	}
}

//void FEB_CAN_PING(void) {
//	//Reset known members
//	FEB_CAN_PONGED=0;
//
//	FEB_CAN_Tx_Header.DLC = 8;
//	FEB_CAN_Tx_Header.StdId = FEB_CAN_FEB_PING_PONG_COUNTER1_FRAME_ID;
//	//FEB_CAN_Tx_Header.StdId = FEB_CAN_PINGER_B;
//	//FEB_CAN_Tx_Header.StdId = FEB_CAN_PINGER_C;
//	//FEB_CAN_Tx_Header.StdId = FEB_CAN_PINGER_D;
//	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
//	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
//	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;
//
//	// Copy data to Tx buffer
//	FEB_CAN_Tx_Data[0] = (uint8_t) 0xA1;
//	FEB_CAN_Tx_Data[1] = (uint8_t) 0xA2;
//	FEB_CAN_Tx_Data[2] = (uint8_t) 0xA3;
//	FEB_CAN_Tx_Data[3] = (uint8_t) 0xA4;
//	FEB_CAN_Tx_Data[4] = (uint8_t) 0xA5;
//	FEB_CAN_Tx_Data[5] = (uint8_t) 0xA6;
//	FEB_CAN_Tx_Data[6] = (uint8_t) 0xA7;
//	FEB_CAN_Tx_Data[7] = (uint8_t) 0xA8;
//
//	// Delay until mailbox available
//	uint32_t timeout=HAL_GetTick()+FEB_CAN_TIMEOUT_MS;
//	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {if(HAL_GetTick()>timeout) FEB_UART_Transmit_CAN_Error(FEB_CAN_EXCEPTION_TIMEOUT);}
//
//	// Add Tx data to mailbox
//	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {FEB_UART_Transmit_CAN_Error(FEB_CAN_EXCEPTION_TxFAIL);}
//}
//
//void FEB_CAN_PONG(CAN_RxHeaderTypeDef *FEB_CAN_Rx_Header, uint8_t FEB_CAN_Rx_Data[]){
//	FEB_CAN_PONGED+=FEB_CAN_Rx_Header->StdId;
//}
//
//uint8_t FEB_CAN_GET_PONGED(void){
//	return FEB_CAN_PONGED;
//}
//
//uint8_t FEB_CAN_PINGPONG_Filter(CAN_HandleTypeDef *hcan, uint8_t FIFO_assignment, uint8_t filter_bank){
//    // For multiple filters, create array of filter IDs and loop over IDs.
//
//	CAN_FilterTypeDef filter_config;
//
//    // Standard CAN - 2.0A - 11 bit
//    filter_config.FilterActivation = CAN_FILTER_ENABLE;
//	filter_config.FilterBank = filter_bank;
//	filter_config.FilterFIFOAssignment = FIFO_assignment;
//	filter_config.FilterIdHigh = 0xA0001;
//	filter_config.FilterIdLow = 0xD1;
//	filter_config.FilterMaskIdHigh = 0xFFE0;
//	filter_config.FilterMaskIdLow = 0;
//	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
//	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
//	filter_config.SlaveStartFilterBank = 27;
//    filter_bank++;
//
//    // Extended CAN - 2.0B - 29 bit
//	filter_config.FilterActivation = CAN_FILTER_ENABLE;
//	filter_config.FilterBank = filter_bank;
//	filter_config.FilterFIFOAssignment = FIFO_assignment;
//	filter_config.FilterIdHigh = /* ID */ 0 >> 13;
//	filter_config.FilterIdLow = (/* ID */ 0 & 0x1FFF) << 3;
//	filter_config.FilterMaskIdHigh = 0xFFFF;
//	filter_config.FilterMaskIdLow = 0xFFF8;
//	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
//	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
//	filter_config.SlaveStartFilterBank = 27;
//    filter_bank++;
//
//	if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
//        // Code Error - Shutdown
//	}
//
//	return filter_bank;
//}
// **************************************** Template Code [Other Files] ****************************************
//
//uint8_t FEB_CAN_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
//    // For multiple filters, create array of filter IDs and loop over IDs.
//
//	CAN_FilterTypeDef filter_config;
//
//    // Standard CAN - 2.0A - 11 bit
//    filter_config.FilterActivation = CAN_FILTER_ENABLE;
//	filter_config.FilterBank = filter_bank;
//	filter_config.FilterFIFOAssignment = FIFO_assignment;
//	filter_config.FilterIdHigh = /* ID */ 0 << 5;
//	filter_config.FilterIdLow = 0;
//	filter_config.FilterMaskIdHigh = 0;//0xFFE0;
//	filter_config.FilterMaskIdLow = 0;
//	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
//	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
//	filter_config.SlaveStartFilterBank = 27;
//    filter_bank++;
//
//    // Extended CAN - 2.0B - 29 bit
//	filter_config.FilterActivation = CAN_FILTER_ENABLE;
//	filter_config.FilterBank = filter_bank;
//	filter_config.FilterFIFOAssignment = FIFO_assignment;
//	filter_config.FilterIdHigh = /* ID */ 0 >> 13;
//	filter_config.FilterIdLow = (/* ID */ 0 & 0x1FFF) << 3;
//	filter_config.FilterMaskIdHigh = 0xFFFF;
//	filter_config.FilterMaskIdLow = 0xFFF8;
//	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
//	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
//	filter_config.SlaveStartFilterBank = 27;
//    filter_bank++;
//
//	if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
//        // Code Error - Shutdown
//	}
//
//	return filter_bank;
//}
//
//void FEB_CAN_Transmit(CAN_HandleTypeDef* hcan) {
//	// Initialize Transmission Header
//    // Write Code Here
//
//	// Configure FEB_CAN_Tx_Data
//    // Write Code Here
//
//	// Delay until mailbox available
//	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {}
//
//	// Add Tx data to mailbox
//	if (HAL_CAN_AddTxMessage(hcan, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
//		// Code Error - Shutdown
//	}
//}
