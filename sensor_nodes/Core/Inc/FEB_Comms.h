/*
 * FEB_Comms.h
 *
 *  Created on: Apr 13, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_COMMS_H_
#define INC_FEB_COMMS_H_

// **************************************** Includes ****************************************

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// **************************************** Debug Macros ****************************************

// Granular debug macros for Comms functions
#define DEBUG_COMMS_PRINTF
#define DEBUG_COMMS_CAN_TRANSMIT

// ************************************** CAN Constants ***************************************

#define IS_FRONT_NODE 0

// ADCs
#define CAN_ID_LIN_POT_FRONT 0x500
#define CAN_ID_LIN_POT_REAR 0x501
#define CAN_ID_STRAIN_GAUGE_FRONT 0x502
#define CAN_ID_STRAIN_GAUGE_REAR 0x503

#define CAN_ID_COOLANT_PRESSURE 0x504
#define CAN_ID_THERMOCOUPLE 0x505

// Coolant Reed Switch
#define CAN_ID_COOLANT_REED_SW 0x506

// Tire Temp
#define FEB_CAN_ID_FRONT_LEFT_TIRE_TEMP 0x507
#define FEB_CAN_ID_FRONT_RIGHT_TIRE_TEMP 0x508
#define FEB_CAN_ID_REAR_LEFT_TIRE_TEMP 0x509
#define FEB_CAN_ID_REAR_RIGHT_TIRE_TEMP 0x510

// Wheel Speed Sensor (WSS)
#define CAN_ID_WSS_FRONT 0x511
#define CAN_ID_WSS_REAR 0x512

// Inertial Measuring Unit
#define CAN_ID_IMU 0x513

// GPS
#define FEB_CAN_ID_GPS 0x514

// Steering Encoder
#define CAN_ID_STEER_ENC 0x515


// **************************************** Functions ****************************************

void UART_Console(const char *string);
void CAN_Transmit(uint16_t CAN_ID, uint8_t TxData[8]);

#endif /* INC_FEB_COMMS_H_ */
