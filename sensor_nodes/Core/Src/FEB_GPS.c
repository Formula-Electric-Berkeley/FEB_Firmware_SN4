/*
 * FEB_GPS.c
 *
 *  Created on: Feb 9, 2025
 *      Author: rahilpasha
 */

#include "FEB_GPS.h"

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;

uint8_t GPS_Buffer[100];

void Read_GPS_Data(void)
{

	if (HAL_UART_Receive(&huart4, GPS_Buffer, sizeof(GPS_Buffer), HAL_MAX_DELAY) != HAL_OK)
	{
		char msg[50];
		sprintf(msg, "UART GPS receiving error");
		HAL_UART_Transmit(&huart2, (uint8_t *) msg, sizeof((uint8_t *) msg), HAL_MAX_DELAY);
//		Error_Handler();
	}

}

void Parse_NMEA_Message(void)
{
	// for now just send the entire message over UART to see the format
	if (HAL_UART_Transmit(&huart2, GPS_Buffer, sizeof(GPS_Buffer), HAL_MAX_DELAY) != HAL_OK)
	{
		char msg[50];
		sprintf(msg, "UART GPS transmitting error");
		HAL_UART_Transmit(&huart2, (uint8_t *) msg, sizeof((uint8_t *) msg), HAL_MAX_DELAY);
//		Error_Handler();
	}
}

void GPS_Main(void)
{
	Read_GPS_Data();
	Parse_NMEA_Message();
}
