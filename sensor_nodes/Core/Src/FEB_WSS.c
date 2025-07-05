/*
 * FEB_WSS.c
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

// **************************************** Includes & External ****************************************

#include "FEB_WSS.h"

extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
extern UART_HandleTypeDef huart2;

// ******************************************** Variables **********************************************

uint8_t WSS_Data[8];

#define TIMER_ELAPSED_HZ 1000 // # of times TIM6 elapses per second
#define TICKS_PER_ROTATION 84 * 4
#define RPM_to_MPH 0.06098555871

int32_t ticks_right = 0;
int32_t ticks_left = 0;

uint16_t wss_counter_right = 0;
uint16_t wss_counter_left = 0;

uint8_t wss_right = 0;
uint8_t wss_left = 0;

char direction_right = '+';
char direction_left = '+';

// ******************************************** Functions **********************************************

void WSS_Init(void)
{
	// Start Encoder Timers for the Wheel Speed Sensors
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
}

void Fill_WSS_Data(void)
{

	WSS_Data[0] = wss_right;
	WSS_Data[1] = wss_left;

}

void update_WSS_ticks(int32_t *ticks, uint16_t *prev_counter, TIM_HandleTypeDef *htim)
{
	uint16_t counter = (uint16_t) __HAL_TIM_GET_COUNTER(htim);

	if (!(__HAL_TIM_IS_TIM_COUNTING_DOWN(htim))) // Check if the wheel is moving forward
	{
		if (counter > *prev_counter)
		{
			*ticks = counter - *prev_counter;
		}
		else // Check if the timer has reloaded
		{
			*ticks = counter + (__HAL_TIM_GET_AUTORELOAD(htim) - *prev_counter);
		}
	}
	else // Reverse direction
	{
		if (counter < *prev_counter)
		{
			*ticks = counter - *prev_counter;
		}
		else
		{
			*ticks = counter - (__HAL_TIM_GET_AUTORELOAD(htim) + *prev_counter);
		}
	}

	*prev_counter = counter;
}

void handle_reverse_ticks(int32_t *ticks, char *direction)
{
	if (*ticks < 0)
	{
		*ticks = 0 - *ticks; // Make the ticks positive
		*direction = '-';
	}
	else
	{
		*direction = '+';
	}
}

void WSS_Main(void)
{
	// Update the tick values
	update_WSS_ticks(&ticks_right, &wss_counter_right, &htim3);
	update_WSS_ticks(&ticks_left, &wss_counter_left, &htim5);

	handle_reverse_ticks(&ticks_right, &direction_right);
	handle_reverse_ticks(&ticks_left, &direction_left);

	// Set the current wheel speed
	wss_right = (uint8_t) ((float) ticks_right * TIMER_ELAPSED_HZ / TICKS_PER_ROTATION * 60);
	wss_left = (uint8_t) ((float) ticks_left * TIMER_ELAPSED_HZ / TICKS_PER_ROTATION * 60);

	// Send the wheel speed data
#ifdef DEBUG_READ_WSS
	printf("Right: %c%d rpm  %d mph\tLeft: %c%d  %d mph\r\n", direction_right, wss_right, direction_left, wss_left, (int) (wss_right * RPM_to_MPH), (int) (wss_left * RPM_to_MPH));
#endif

	Fill_WSS_Data();

#ifdef IS_FRONT_NODE
		CAN_Transmit(CAN_ID_WSS_FRONT, WSS_Data);
#else
		CAN_Transmit(CAN_ID_WSS_REAR, WSS_Data);
#endif

}

