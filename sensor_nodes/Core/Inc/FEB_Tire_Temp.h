/*
 * FEB_Tire_Temp.h
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_TIRE_TEMP_H_
#define INC_FEB_TIRE_TEMP_H_

// ******************************** Includes ********************************

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "FEB_Main.h"

// ******************************** Configuration ********************************

// Debug macros - comment out to disable
#define DEBUG_TIRE_TEMP_PRINTF
#define DEBUG_TIRE_TEMP_CONFIG
#define DEBUG_TIRE_TEMP_DATA
#define DEBUG_TIRE_TEMP_CAN_RX

// ******************************** IRTS-V3 Constants ********************************

// IRTS-V3 Sensor Configuration
#define IRTS_BASE_CAN_ID        0x4B0  // Base CAN ID for IRTS sensors
#define IRTS_CAN_ID_MASK        0xFF0  // Mask to identify IRTS messages
#define IRTS_CHANNELS_PER_MSG   4      // 4 temperature channels per CAN message
#define IRTS_BYTES_PER_CHANNEL  2      // 16-bit temperature values

// Configuration message constants
#define IRTS_CONFIG_MAGIC_MSB   0x75   // Programming constant MSB
#define IRTS_CONFIG_MAGIC_LSB   0x30   // Programming constant LSB

// Sensor parameters
#define IRTS_EMISSIVITY         85     // Emissivity for rubber tires (0.85 * 100)
#define IRTS_SAMPLE_FREQ        8      // 100Hz sampling rate
#define IRTS_NUM_CHANNELS       160    // 16 channels enabled
#define IRTS_CAN_BITRATE        2      // 500kbps

// Temperature conversion
#define IRTS_TEMP_SCALE         0.01f  // Temperature scale factor
#define IRTS_TEMP_OFFSET        100.0f // Temperature offset in Celsius

// Sensor IDs for different positions
#define IRTS_ID_FRONT_LEFT      0x4B0
#define IRTS_ID_FRONT_RIGHT     0x4B4
#define IRTS_ID_REAR_LEFT       0x4B8
#define IRTS_ID_REAR_RIGHT      0x4BC

// Configuration state machine
typedef enum {
    IRTS_STATE_IDLE,
    IRTS_STATE_CONFIGURING,
    IRTS_STATE_READY,
    IRTS_STATE_ERROR
} IRTS_State_t;

// Temperature data structure
typedef struct {
    uint16_t channels[16];     // 16 temperature channels
    uint8_t valid_channels;    // Bitmask of valid channels
    uint32_t last_update;      // Timestamp of last update
} IRTS_SensorData_t;

// ******************************** Functions ********************************

void Tire_Temp_Init(void);
void Tire_Temp_Configure_Sensor(uint16_t sensor_id);
void Tire_Temp_Process_Config(void);
void Tire_Temp_Process_Message(CAN_RxHeaderTypeDef *RxHeader, uint8_t *RxData);
void Tire_Temp_Main(void);
IRTS_State_t Tire_Temp_Get_State(void);
float Tire_Temp_Convert_Temperature(uint16_t raw_value);
uint16_t Tire_Temp_Get_Average(uint16_t sensor_id);

// CAN callback handler (called from stm32f4xx_it.c)
void Tire_Temp_CAN_Callback(CAN_HandleTypeDef *hcan);

#endif /* INC_FEB_TIRE_TEMP_H_ */
