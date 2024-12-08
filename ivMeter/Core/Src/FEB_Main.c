/*
 * FEB_Main.c
 *
 *  Created on: Dec 7, 2024
 *      Author: rahilpasha
 */
#include "FEB_Main.h"
#include "main.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FEB_CAN_ID_IV_METER 0x21

#define ADC_RESOLUTION 4096
#define ADC_REFERENCE_VOLTAGE 3.3
#define HECS_VOLTAGE_DIV 5 / 3

#define HECS_DEFAULT_VOLTAGE 2.5
#define HECS_CALCULATION_CONSTANT 16 * 1000

#define LOW_RANGE_SENSITIVITY 100
#define HIGH_RANGE_SENSITIVITY 250

#define I2C_ID 0b0110100
#define HV_DIV 1006200 / 6200

extern ADC_HandleTypeDef hadc;
extern I2C_HandleTypeDef hi2c1;
extern CAN_HandleTypeDef hcan;
extern UART_HandleTypeDef huart2;

uint16_t ivData[3];
uint8_t i2c_buf[2];

enum HECS_Sensor
{
	LOW_RANGE,
	HIGH_RANGE
};
typedef enum HECS_Sensor HECS_Sensor;

void UART_Transmit(const char *string)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)string, strlen(string), 1000);
}

void CAN_Transmit_ivMeter(void)
{
	CAN_TxHeaderTypeDef TxHeader;
	uint8_t TxData[8];
	uint32_t TxMailbox;

	TxHeader.DLC = 8; // Data length
	TxHeader.IDE = CAN_ID_STD; // Standard ID
	TxHeader.RTR = CAN_RTR_DATA; // Data frame
	TxHeader.StdId = FEB_CAN_ID_IV_METER; // CAN ID
	TxHeader.ExtId = 0; // Not used with standard ID

	// Fill the data
	TxData[0] = (ivData[0] >> 8) & 0xFF;
	TxData[1] = ivData[0] & 0xFF;
	TxData[2] = (ivData[1] >> 8) & 0xFF;
	TxData[3] = ivData[1] & 0xFF;
	TxData[4] = (ivData[2] >> 8) & 0xFF;
	TxData[5] = ivData[2] & 0xFF;

	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0) {}

	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		// Transmission request error
		char msg[50];
		sprintf(msg, "CAN transmit error");
		UART_Transmit(msg);
		Error_Handler();
	}
}

void Process_HECS(HECS_Sensor sensor, uint16_t reading)
{
	uint32_t output_voltage = HECS_VOLTAGE_DIV * reading * ADC_REFERENCE_VOLTAGE / ADC_RESOLUTION;
	uint32_t current = (output_voltage - HECS_DEFAULT_VOLTAGE) * HECS_CALCULATION_CONSTANT;

	if (sensor == LOW_RANGE)
	{
		current /= LOW_RANGE_SENSITIVITY;
		ivData[0] = (uint16_t) current;
	}
	else
	{
		current /= HIGH_RANGE_SENSITIVITY;
		ivData[1] = (uint16_t) current;
	}

}

void Voltage_I2C_Init(void)
{
	uint8_t i2c_init[2];
	i2c_init[0] = 0b11010010;
	i2c_init[1] = 0b00000011;

	if (HAL_I2C_Master_Transmit(&hi2c1, I2C_ID, i2c_init, 2, HAL_MAX_DELAY) != HAL_OK)
	{
		// Transmission request error
		char msg[50];
		sprintf(msg, "CAN transmit error");
		UART_Transmit(msg);
		Error_Handler();
	}
}

void Process_Voltage_I2C(void)
{
	if (HAL_I2C_Master_Receive(&hi2c1, I2C_ID, i2c_buf, 2, HAL_MAX_DELAY) != HAL_OK)
	{
		// Transmission request error
		char msg[50];
		sprintf(msg, "CAN transmit error");
		UART_Transmit(msg);
		Error_Handler();
	}

	uint16_t HV_reading = (i2c_buf[0] & 0xFFFF) << 8 | i2c_buf[1];

	ivData[3] = HV_reading * HV_DIV / 100;

}

void FEB_Main_Setup(void)
{

	HAL_ADC_Start(&hadc);
	Voltage_I2C_Init();

}

void FEB_Main_Loop(void)
{

	HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);

	uint16_t low_range_reading = (uint16_t) HAL_ADC_GetValue(&hadc);
	Process_HECS(LOW_RANGE, low_range_reading);

	HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY); // Not sure if this is necessary

	uint16_t high_range_reading = (uint16_t) HAL_ADC_GetValue(&hadc);
	Process_HECS(HIGH_RANGE, high_range_reading);

	Process_Voltage_I2C();

	CAN_Transmit_ivMeter();

}
