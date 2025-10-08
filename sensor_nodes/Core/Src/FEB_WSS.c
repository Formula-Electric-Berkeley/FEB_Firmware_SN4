/*
 * FEB_WSS.c
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

// **************************************** Includes & External ****************************************

#include "FEB_WSS.h"

#include <math.h>

extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
extern UART_HandleTypeDef huart2;

// ******************************************** Constants **********************************************

// Timer configuration
#define TIMER_FREQUENCY_HZ 10        // TIM6 triggers at 10Hz (100ms period)

// Encoder configuration  
#define ENCODER_PULSES_PER_REV 40     // Number of encoder pulses per wheel revolution
#define ENCODER_QUADRATURE_MODE 4     // Quadrature encoding multiplier (4x mode)
#define TICKS_PER_ROTATION (ENCODER_PULSES_PER_REV * ENCODER_QUADRATURE_MODE)

// Wheel specifications
#define WHEEL_DIAMETER_INCHES 12      // Wheel diameter in inches
#define INCHES_TO_MILES 63360         // Conversion factor: inches to miles
#define RPM_TO_MPH_FACTOR (WHEEL_DIAMETER_INCHES * M_PI * 60 / INCHES_TO_MILES)

// ******************************************** Variables **********************************************

uint8_t WSS_Data[8];

int32_t ticks_right = 0;
int32_t ticks_left = 0;

uint16_t wss_counter_right = 0;
uint16_t wss_counter_left = 0;

uint8_t wss_right = 0;
uint8_t wss_left = 0;

char direction_right = '+';
char direction_left = '+';

// ******************************************** Functions **********************************************

/**
 * @brief Initialize wheel speed sensor encoders
 * @note Starts TIM3 and TIM5 in encoder mode for quadrature decoding
 */
void WSS_Init(void)
{
	// Start Encoder Timers for the Wheel Speed Sensors
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);  // Left wheel encoder
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);  // Right wheel encoder
}

/**
 * @brief Pack wheel speed data into CAN transmission buffer
 * @note Data format: [0]=right wheel RPM, [1]=left wheel RPM
 */
void Fill_WSS_Data(void)
{
	WSS_Data[0] = wss_right;  // Right wheel speed in RPM
	WSS_Data[1] = wss_left;   // Left wheel speed in RPM
	// Bytes 2-7 reserved for future use (e.g., direction, status)
}

/**
 * @brief Calculate encoder ticks since last reading
 * @param ticks Pointer to store calculated tick difference
 * @param prev_counter Pointer to previous counter value (updated after calculation)
 * @param htim Timer handle for the encoder interface
 * @note Handles timer overflow and calculates signed difference for bidirectional counting
 */
void update_WSS_ticks(int32_t *ticks, uint16_t *prev_counter, TIM_HandleTypeDef *htim)
{
    uint16_t current_counter = (uint16_t)__HAL_TIM_GET_COUNTER(htim);
    uint16_t max_count = (uint16_t)__HAL_TIM_GET_AUTORELOAD(htim);
    
    // Calculate signed difference accounting for overflow
    // In encoder mode, the counter can go up or down
    int32_t diff = (int32_t)current_counter - (int32_t)*prev_counter;
    
    // Handle overflow/underflow for 16-bit counter
    if (diff > (int32_t)(max_count / 2)) {
        // Counter underflowed (went from low to high value)
        diff -= (max_count + 1);
    } else if (diff < -(int32_t)(max_count / 2)) {
        // Counter overflowed (went from high to low value)
        diff += (max_count + 1);
    }
    
    *ticks = diff;
    *prev_counter = current_counter;
}

/**
 * @brief Handle negative tick values and determine direction
 * @param ticks Pointer to tick value (converted to absolute value)
 * @param direction Pointer to direction character ('+' for forward, '-' for reverse)
 */
void handle_reverse_ticks(int32_t *ticks, char *direction)
{
    if (*ticks < 0) {
        *ticks = -(*ticks);  // Convert to positive
        *direction = '-';    // Negative ticks mean reverse direction
    } else {
        *direction = '+';    // Positive ticks mean forward direction
    }
}

/**
 * @brief Main wheel speed sensor processing routine
 * @note Called periodically by TIM6 interrupt at 10Hz
 *       Reads encoder values, calculates wheel speeds, and transmits via CAN
 */
void WSS_Main(void)
{

	// Update the tick values
	update_WSS_ticks(&ticks_right, &wss_counter_right, &htim5);
	update_WSS_ticks(&ticks_left, &wss_counter_left, &htim3);

	handle_reverse_ticks(&ticks_right, &direction_right);
	handle_reverse_ticks(&ticks_left, &direction_left);

	// Calculate wheel speed in RPM
	// RPM = (ticks_per_period / ticks_per_rotation) * (60 seconds / period_in_seconds)
	float period_seconds = 1.0f / TIMER_FREQUENCY_HZ;
	wss_right = (uint8_t)((float)ticks_right / TICKS_PER_ROTATION * 60.0f / period_seconds);
	wss_left = (uint8_t)((float)ticks_left / TICKS_PER_ROTATION * 60.0f / period_seconds);

	// Debug output for wheel speed data
#if WSS && defined(DEBUG_WSS_PRINTF)
	printf("WSS Right: %c%d RPM (%d mph) | Left: %c%d RPM (%d mph)\r\n", 
		direction_right, wss_right, (int)(wss_right * RPM_TO_MPH_FACTOR),
		direction_left, wss_left, (int)(wss_left * RPM_TO_MPH_FACTOR));
#endif

#if SEND_CAN && WSS
#if IS_FRONT_NODE
	CAN_Transmit(FEB_CAN_WSS_FRONT_DATA_FRAME_ID, WSS_Data);
#else
	CAN_Transmit(FEB_CAN_WSS_REAR_DATA_FRAME_ID, WSS_Data);
#endif
#endif

	Fill_WSS_Data();



}

