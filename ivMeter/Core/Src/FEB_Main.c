/*
 * FEB_Main.c
 *
 *  Created on: Dec 7, 2024
 *      Author: rahilpasha
 */
#include "FEB_Main.h"

#define ADC_RESOLUTION 4096
#define ADC_REFERENCE_VOLTAGE 3.3
#define HECS_DEFAULT_VOLTAGE 2.5
#define HECS_CALCULATION_CONSTANT 16 * 1000 * 5 / 3
#define LOW_RANGE_SENSITIVITY 100
#define HIGH_RANGE_SENSITIVITY 250

extern hadc;
extern hi2c1;
extern hcan;

uint16_t ivData[3];

enum HECS_Sensor
{
	LOW_RANGE,
	HIGH_RANGE
};

void CAN_Transmit_ivMeter(void)
{

}

void UART_Transmit(const char *string)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)string, strlen(string), 1000);
}

void Process_HECS(HECS_Sensor sensor, uint16_t reading)
{

}

void Voltage_I2C_Init(void)
{

}

void Process_Voltage_I2C(void)
{

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
