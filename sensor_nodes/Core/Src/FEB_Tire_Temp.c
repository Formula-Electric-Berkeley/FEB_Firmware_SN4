/*
 * FEB_Tire_Temp.c
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

// **************************************** Includes & External ****************************************

#include "FEB_Tire_Temp.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern UART_HandleTypeDef huart2;

// ******************************************** Variables **********************************************

#define TIRE_TEMP_BASE_CAN_ID 0x4B0

uint16_t tire_temp_left[4];
uint16_t tire_temp_right[4];

// ******************************************** Functions **********************************************

void Tire_Temp_Init(void)
{
	// Start CAN
	HAL_CAN_Start(&hcan2);
	if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		Error_Handler();
	}

	// Configure Tire Temp Sensors
	if (IS_FRONT_NODE)
	{
		Configure_Tire_Temp_Sensor(0x4B0, 0x4B0, 0.85, 8, 160, 2);
		Configure_Tire_Temp_Sensor(0x4B4, 0x4B4, 0.85, 8, 160, 2);
	}
	else
	{
		Configure_Tire_Temp_Sensor(0x4B8, 0x4B8, 0.85, 8, 160, 2);
		Configure_Tire_Temp_Sensor(0x4BC, 0x4BC, 0.85, 8, 160, 2);
	}
}

void UART_Transmit(const char *string)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)string, strlen(string), 1000);
}

void Configure_Tire_Temp_Sensor(uint16_t currentCAN_ID, uint16_t newCAN_ID, float emissivity,
		uint8_t sampleFrequency, uint8_t channels, uint8_t bitrate)
{
	// Set up the CAN message
	CAN_TxHeaderTypeDef TxHeader;
	uint8_t TxData[8];
	uint32_t TxMailbox;

	TxHeader.DLC = 8; // Data length
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA; // Data frame
	TxHeader.StdId = currentCAN_ID; // Current CAN ID of the sensor
	TxHeader.ExtId = 0; // Not used with standard ID

	// Add the configuration settings
	TxData[0] = 0x75; // Programming Constant MSB
	TxData[1] = 0x30; // Programming Constant LSB
	TxData[2] = (newCAN_ID >> 8) & 0xFF; // New CAN Base ID MSB
	TxData[3] = newCAN_ID & 0xFF; // New CAN Base ID LSB
	TxData[4] = (uint8_t) (emissivity * 100); // Emissivity (0.01–1.00 scaled to 1–100)
	TxData[5] = sampleFrequency; // Sampling frequency (8=100Hz)
	TxData[6] = channels; // Channels (40=4, 80=8, 160=16)
	TxData[7] = bitrate; // Bit rate (1=1Mbit/s, 2=500kbit/s, 3=250kbit/s, or 4=100kbit/s)

	// Transmit the message at 1Hz for 10 seconds
	for (int i = 0; i < 10; i++)
	{
		if (HAL_CAN_AddTxMessage(&hcan2, &TxHeader, TxData, &TxMailbox) != HAL_OK)
		{
			// Transmission request error
			#ifdef DEBUG_TIRE_TEMP_CONFIGURE
			printf("CAN transmit error\r\n");
			#endif
			Error_Handler();
		}

		HAL_Delay(1000);
	}
}

void Read_Tire_Temp_Data(CAN_RxHeaderTypeDef RxHeader, uint8_t *RxData)
{
	// Calculate the first channel in the message
	uint8_t channelStart = ((RxHeader.StdId - TIRE_TEMP_BASE_CAN_ID) % 4) * 4 + 1;
	uint16_t average_temp = 0;

	// Calculate the temperature from all four channels and transmit them
	for (int i = 0; i < 4; i++)
	{
		// Get data from one channel and add it to average temp (subtract 1000 to account for the 100 degree celsius offset
		int16_t temp_raw = (RxData[2 * i] << 8) | RxData[2 * i + 1];
		average_temp += temp_raw - 1000;

		// Send data through UART
		float temperature = temp_raw * 0.01 - 100;
		
#ifdef DEBUG_READ_TIRE_TEMP
		printf("Channel %d Temperature: %d C\r\n", channelStart + i, (int) temperature);
#endif
	}

	average_temp /= 4;
	if (RxHeader.StdId % 16 < 4 || (RxHeader.StdId % 16 >= 8 && RxHeader.StdId % 16 < 12)) // Check if it is the right tire
	{
		tire_temp_right[RxHeader.StdId % 4] = average_temp >> 8 & 0xFF;
		tire_temp_right[RxHeader.StdId % 4 + 1] = average_temp & 0xFF;
	}
	else // Check case if it is the left tire
	{
		tire_temp_left[RxHeader.StdId % 4] = average_temp >> 8 & 0xFF;
		tire_temp_left[RxHeader.StdId % 4 + 1] = average_temp & 0xFF;
	}

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {

	if (hcan->Instance == CAN2)
	{
		CAN_RxHeaderTypeDef RxHeader;
	    uint8_t RxData[8];

	    // Retrieve the message
	    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
			#ifdef DEBUG_TIRE_TEMP_CAN_RX
			printf("CAN receive error\r\n");
			#endif
			Error_Handler();
	    }

	    if ((RxHeader.StdId >> 4) == (TIRE_TEMP_BASE_CAN_ID >> 4)) // Check if the message is from one of the tire temp sensors
	    {
	    	Read_Tire_Temp_Data(RxHeader, RxData);
	    }
	}

}

void Tire_Temp_Main(void)
{
	if (IS_FRONT_NODE)
	{
		CAN_Transmit(FEB_CAN_ID_FRONT_LEFT_TIRE_TEMP, (uint8_t*) tire_temp_right);
		CAN_Transmit(FEB_CAN_ID_FRONT_RIGHT_TIRE_TEMP, (uint8_t*) tire_temp_left);
	}
	else
	{
		CAN_Transmit(FEB_CAN_ID_REAR_LEFT_TIRE_TEMP, (uint8_t*) tire_temp_right);
		CAN_Transmit(FEB_CAN_ID_REAR_RIGHT_TIRE_TEMP, (uint8_t*) tire_temp_left);
	}
}
