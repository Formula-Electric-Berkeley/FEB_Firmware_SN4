/*
 * FEB_Comms.h
 *
 *  Created on: Apr 13, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_COMMS_H_
#define INC_FEB_COMMS_H_

// **************************************** Includes ****************************************

#include "FEB_CAN_Library_SN4/gen/feb_can.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// **************************************** Debug Macros ****************************************

// Granular debug macros for Comms functions
#define DEBUG_COMMS_PRINTF
#define DEBUG_COMMS_CAN_TRANSMIT

// ************************************** CAN Constants ***************************************

// Node Configuration
#define IS_FRONT_NODE 1        // 1 for front node, 0 for rear node

// Communication Configuration
#define SEND_CAN 1             // Enable CAN bus transmission

// Sensor Enable/Disable Flags
// Set to 1 to enable, 0 to disable each sensor subsystem
#define WSS 1                  // Wheel Speed Sensors (Encoders on TIM3/TIM5)
#define TIRETEMP 1             // Tire Temperature Sensors (IRTS-V3 on CAN2)
#define ADCS 0                 // ADC Sensors (Linear Pots, Strain Gauges, etc.)
#define STEER 0                // Steering Angle Encoder
#define IMU 0                  // Inertial Measurement Unit
#define GPS 0                  // GPS Module
#define SWITCH 0               // Coolant Reed Switch

// **************************************** Functions ****************************************

void UART_Console(const char *string);
void CAN_Transmit(uint16_t CAN_ID, uint8_t TxData[8]);

#endif /* INC_FEB_COMMS_H_ */
