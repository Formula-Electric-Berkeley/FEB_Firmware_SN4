// **************************************** Includes & External ****************************************

#include "FEB_CAN.h"
#include "FEB_CircularBuffer.h"
extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;

// **************************************** CAN Configuration ****************************************

CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
static CAN_RxHeaderTypeDef FEB_CAN_Rx_Header;

extern circBuffer FEBBuffer;
extern char* buffer;

uint8_t FEB_CAN_Tx_Data[8];
uint8_t FEB_CAN_Rx_Data[8];

uint32_t FEB_CAN_Tx_Mailbox;

// **************************************** Functions ****************************************


void FEB_CAN_Init(void) {
	FEB_CAN_Filter(&hcan1, 0, 0);
	if (HAL_CAN_Start(&hcan1) != HAL_OK) {
        // Code Error - Shutdown
		printf("Could not initialize CAN Filter");
	}
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/*void FEB_CAN_Filter_Config(void) {
	uint8_t filter_bank = 0;
	// Assign Filter
    // filter_bank = Function(&hcan1, CAN_RX_FIFO0, filter_bank);
} */

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &FEB_CAN_Rx_Header, FEB_CAN_Rx_Data) != HAL_OK) {
		// Store Message
        // Function(&FEB_CAN_Rx_Header, FEB_CAN_Rx_Data);
	}

	//Case 1: Std Id
	if(FEB_CAN_Rx_Header.IDE == CAN_ID_STD){ 
		FEB_circBuf_write(&FEBBuffer, FEB_CAN_Rx_Header.StdId, FEB_CAN_Rx_Header.Timestamp, FEB_CAN_Rx_Data);

	//Case 2: Ext Id
	}else if(FEB_CAN_Rx_Header.IDE == CAN_ID_EXT){
		FEB_circBuf_write(&FEBBuffer, FEB_CAN_Rx_Header.ExtId, FEB_CAN_Rx_Header.Timestamp, FEB_CAN_Rx_Data);


	}





}

// **************************************** Template Code [Other Files] ****************************************

uint8_t FEB_CAN_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
    // For multiple filters, create array of filter IDs and loop over IDs.

	CAN_FilterTypeDef filter_config;

    // Standard CAN - 2.0A - 11 bit
    filter_config.FilterActivation = CAN_FILTER_ENABLE;
	filter_config.FilterBank = filter_bank;
	filter_config.FilterFIFOAssignment = FIFO_assignment;
	filter_config.FilterIdHigh = 0;
	filter_config.FilterIdLow = 0;
	filter_config.FilterMaskIdHigh = 0;
	filter_config.FilterMaskIdLow = 0;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.SlaveStartFilterBank = 27;



	if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
        // Code Error - Shutdown
	}

	return filter_bank;
}

void FEB_CAN_Transmit_Test_Data(CAN_HandleTypeDef* hcan) {
	// Initialize Transmission Header
    // Write Code Here
	FEB_CAN_Tx_Header.DLC = 8;
	FEB_CAN_Tx_Header.StdId = 0x1;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
//	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;
	FEB_CAN_Tx_Header.TransmitGlobalTime = ENABLE;

	FEB_CAN_Tx_Data[0] = 0x11;
	FEB_CAN_Tx_Data[1] = 0x22;
	FEB_CAN_Tx_Data[2] = 0x33;
	FEB_CAN_Tx_Data[3] = 0x44;
	FEB_CAN_Tx_Data[4] = 0x55;
	FEB_CAN_Tx_Data[5] = 0x66;
	FEB_CAN_Tx_Data[6] = 0x77;
	FEB_CAN_Tx_Data[7] = 0x88;





	// Configure FEB_CAN_Tx_Data
    // Write Code Here

//	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(hcan, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
		printf("Unable to send CAN message");
	}
} 

void FEB_CAN_Transmit_Test_Data_ExtId(CAN_HandleTypeDef* hcan) {
	// Initialize Transmission Header
    // Write Code Here
	FEB_CAN_Tx_Header.DLC =8;
	FEB_CAN_Tx_Header.ExtId = 0x2;
	FEB_CAN_Tx_Header.IDE = CAN_ID_EXT;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
//	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;
	FEB_CAN_Tx_Header.TransmitGlobalTime = ENABLE;
	FEB_CAN_Tx_Data[0] = 0x01;
	FEB_CAN_Tx_Data[1] = 0x02;
	FEB_CAN_Tx_Data[2] = 0x03;
	FEB_CAN_Tx_Data[3] = 0x04;
	FEB_CAN_Tx_Data[4] = 0x05;
	FEB_CAN_Tx_Data[5] = 0x06;
	FEB_CAN_Tx_Data[6] = 0x07;
	FEB_CAN_Tx_Data[7] = 0x08;



	// Configure FEB_CAN_Tx_Data
    // Write Code Here

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(hcan, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
		printf("Unable to send CAN message");
	}
}
