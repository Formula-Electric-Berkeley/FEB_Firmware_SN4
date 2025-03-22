
// **************************************** Includes & External ****************************************
#include "FEB_HW.h"
#include "FEB_CAN.h"

extern CAN_HandleTypeDef hcan1;

// **************************************** CAN Configuration ****************************************

CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
static CAN_RxHeaderTypeDef FEB_CAN_Rx_Header;
FEB_CAN_DEV FEB_CAN_NETWORK[FEB_NUM_CAN_DEV]={0};
uint8_t FEB_CAN_Tx_Data[8];
uint8_t FEB_CAN_Rx_Data[8];
uint32_t FEB_CAN_Tx_Mailbox;
uint8_t FEB_CAN_PONGED;


// **************************************** Functions ****************************************

void FEB_CAN_Init(void) {
	FEB_CAN_Filter_Config();
	if (HAL_CAN_Start(&hcan1) != HAL_OK) {
        // Code Error - Shutdown
	}
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	//TODO: Create directory
}

void FEB_CAN_Filter_Config(void) {
	uint8_t filter_bank = 0;
	filter_bank = FEB_CAN_IVT_Filter_Config(&hcan1, CAN_RX_FIFO0, filter_bank);
//	filter_bank = FEB_CAN_Filter(&hcan1, CAN_RX_FIFO0, filter_bank);

//    if(FEB_CAN_PINGPONG_MODE) filter_bank =FEB_CAN_PINGPONG_Filter(&hcan1, CAN_RX_FIFO0, filter_bank);

	// Assign Filter
    //filter_bank = Function(&hcan1, CAN_RX_FIFO0, filter_bank);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &FEB_CAN_Rx_Header, FEB_CAN_Rx_Data) == HAL_OK) {
//		if(FEB_CAN_PINGPONG_MODE){
//			FEB_CAN_PONG(&FEB_CAN_Rx_Header, FEB_CAN_Rx_Data);
//		} else {
//			//YOUR HANDLER HERE!!!
//		}

		FEB_CAN_IVT_Store_Msg(&FEB_CAN_Rx_Header, FEB_CAN_Rx_Data);

	}
}


void FEB_SM_CAN_Transmit(void) {

	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 3;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_ID_BMS_STATE;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Get data
	uint16_t cell_min_voltage=0;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = FEB_SM_Get_Current_State();
	FEB_CAN_Tx_Data[1] = cell_min_voltage && 0xFF;
	FEB_CAN_Tx_Data[2] = cell_min_voltage >> 8;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// FEB_SM_Set_Current_State(FEB_SM_ST_SHUTDOWN);
	}
}

void FEB_CAN_PING(void) {
	//Reset known members
	FEB_CAN_PONGED=0;

	FEB_CAN_Tx_Header.DLC = 8;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_PINGER_A;
	//FEB_CAN_Tx_Header.StdId = FEB_CAN_PINGER_B;
	//FEB_CAN_Tx_Header.StdId = FEB_CAN_PINGER_C;
	//FEB_CAN_Tx_Header.StdId = FEB_CAN_PINGER_D;
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
	uint32_t timeout=HAL_GetTick()+FEB_CAN_TIMEOUT_MS;
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {if(HAL_GetTick()>timeout) FEB_UART_Transmit_CAN_Error(FEB_CAN_EXCEPTION_TIMEOUT);}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {FEB_UART_Transmit_CAN_Error(FEB_CAN_EXCEPTION_TxFAIL);}
}

void FEB_CAN_PONG(CAN_RxHeaderTypeDef *FEB_CAN_Rx_Header, uint8_t FEB_CAN_Rx_Data[]){
	FEB_CAN_PONGED+=FEB_CAN_Rx_Header->StdId;
}

uint8_t FEB_CAN_GET_PONGED(void){
	return FEB_CAN_PONGED;
}

uint8_t FEB_CAN_PINGPONG_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank){
    // For multiple filters, create array of filter IDs and loop over IDs.

	CAN_FilterTypeDef filter_config;

    // Standard CAN - 2.0A - 11 bit
    filter_config.FilterActivation = CAN_FILTER_ENABLE;
	filter_config.FilterBank = filter_bank;
	filter_config.FilterFIFOAssignment = FIFO_assignment;
	filter_config.FilterIdHigh = 0xA0001;
	filter_config.FilterIdLow = 0xD1;
	filter_config.FilterMaskIdHigh = 0xFFE0;
	filter_config.FilterMaskIdLow = 0;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.SlaveStartFilterBank = 27;
    filter_bank++;

    // Extended CAN - 2.0B - 29 bit
	filter_config.FilterActivation = CAN_FILTER_ENABLE;
	filter_config.FilterBank = filter_bank;
	filter_config.FilterFIFOAssignment = FIFO_assignment;
	filter_config.FilterIdHigh = /* ID */ 0 >> 13;
	filter_config.FilterIdLow = (/* ID */ 0 & 0x1FFF) << 3;
	filter_config.FilterMaskIdHigh = 0xFFFF;
	filter_config.FilterMaskIdLow = 0xFFF8;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.SlaveStartFilterBank = 27;
    filter_bank++;

	if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
        // Code Error - Shutdown
	}

	return filter_bank;
}
// **************************************** Template Code [Other Files] ****************************************

uint8_t FEB_CAN_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
    // For multiple filters, create array of filter IDs and loop over IDs.

	CAN_FilterTypeDef filter_config;

    // Standard CAN - 2.0A - 11 bit
    filter_config.FilterActivation = CAN_FILTER_ENABLE;
	filter_config.FilterBank = filter_bank;
	filter_config.FilterFIFOAssignment = FIFO_assignment;
	filter_config.FilterIdHigh = /* ID */ 0 << 5;
	filter_config.FilterIdLow = 0;
	filter_config.FilterMaskIdHigh = 0;//0xFFE0;
	filter_config.FilterMaskIdLow = 0;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.SlaveStartFilterBank = 27;
    filter_bank++;

    // Extended CAN - 2.0B - 29 bit
	filter_config.FilterActivation = CAN_FILTER_ENABLE;
	filter_config.FilterBank = filter_bank;
	filter_config.FilterFIFOAssignment = FIFO_assignment;
	filter_config.FilterIdHigh = /* ID */ 0 >> 13;
	filter_config.FilterIdLow = (/* ID */ 0 & 0x1FFF) << 3;
	filter_config.FilterMaskIdHigh = 0xFFFF;
	filter_config.FilterMaskIdLow = 0xFFF8;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.SlaveStartFilterBank = 27;
    filter_bank++;

	if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
        // Code Error - Shutdown
	}

	return filter_bank;
}

void FEB_CAN_Transmit(CAN_HandleTypeDef* hcan) {
	// Initialize Transmission Header
    // Write Code Here

	// Configure FEB_CAN_Tx_Data
    // Write Code Here

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(hcan, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
	}
}
