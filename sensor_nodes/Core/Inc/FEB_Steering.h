/*
 * FEB_Steering.h
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 *  
 *  AS5600 Magnetic Rotary Position Sensor API
 *  12-bit contactless angle measurement sensor
 */

#ifndef INC_FEB_STEERING_H_
#define INC_FEB_STEERING_H_

// **************************************** Includes ****************************************

#include "FEB_Comms.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// **************************************** AS5600 Constants ****************************************

// I2C Configuration
#define AS5600_I2C_ADDRESS          0x36    // 7-bit address (0x6C when shifted left)
#define AS5600_I2C_ADDRESS_SHIFTED   (AS5600_I2C_ADDRESS << 1)
#define AS5600_I2C_TIMEOUT_MS        100     // I2C transaction timeout

// Register Map (per AS5600 datasheet)
// Configuration Registers
#define AS5600_REG_ZMCO              0x00    // ZMCO (8-bit) - Burn count
#define AS5600_REG_ZPOS_H            0x01    // ZPOS[11:8] - Zero position high byte
#define AS5600_REG_ZPOS_L            0x02    // ZPOS[7:0] - Zero position low byte
#define AS5600_REG_MPOS_H            0x03    // MPOS[11:8] - Maximum position high byte
#define AS5600_REG_MPOS_L            0x04    // MPOS[7:0] - Maximum position low byte
#define AS5600_REG_MANG_H            0x05    // MANG[11:8] - Maximum angle high byte
#define AS5600_REG_MANG_L            0x06    // MANG[7:0] - Maximum angle low byte
#define AS5600_REG_CONF_H            0x07    // CONF[15:8] - Configuration high byte
#define AS5600_REG_CONF_L            0x08    // CONF[7:0] - Configuration low byte

// Output Registers
#define AS5600_REG_RAW_ANGLE_H       0x0C    // RAW ANGLE[11:8] - Raw angle high byte
#define AS5600_REG_RAW_ANGLE_L       0x0D    // RAW ANGLE[7:0] - Raw angle low byte
#define AS5600_REG_ANGLE_H           0x0E    // ANGLE[11:8] - Processed angle high byte
#define AS5600_REG_ANGLE_L           0x0F    // ANGLE[7:0] - Processed angle low byte

// Status Registers
#define AS5600_REG_STATUS            0x0B    // STATUS - Device status
#define AS5600_REG_AGC               0x1A    // AGC - Automatic gain control
#define AS5600_REG_MAGNITUDE_H       0x1B    // MAGNITUDE[11:8] - Magnetic field strength high
#define AS5600_REG_MAGNITUDE_L       0x1C    // MAGNITUDE[7:0] - Magnetic field strength low

// Special Registers
#define AS5600_REG_BURN              0xFF    // BURN - Burn command register

// Configuration Register Bit Definitions
// CONF Register (0x07-0x08)
#define AS5600_CONF_PM_MASK          0x0003  // Power Mode [1:0]
#define AS5600_CONF_PM_NOM           0x0000  // Normal mode
#define AS5600_CONF_PM_LPM1          0x0001  // Low Power Mode 1
#define AS5600_CONF_PM_LPM2          0x0002  // Low Power Mode 2
#define AS5600_CONF_PM_LPM3          0x0003  // Low Power Mode 3

#define AS5600_CONF_HYST_MASK        0x000C  // Hysteresis [3:2]
#define AS5600_CONF_HYST_OFF         0x0000  // Hysteresis OFF
#define AS5600_CONF_HYST_1LSB        0x0004  // 1 LSB hysteresis
#define AS5600_CONF_HYST_2LSB        0x0008  // 2 LSB hysteresis
#define AS5600_CONF_HYST_3LSB        0x000C  // 3 LSB hysteresis

#define AS5600_CONF_OUTS_MASK        0x0030  // Output Stage [5:4]
#define AS5600_CONF_OUTS_ANALOG      0x0000  // Analog (full range from 0% to 100% VDD)
#define AS5600_CONF_OUTS_ANALOG_90   0x0010  // Analog (reduced range from 10% to 90% VDD)
#define AS5600_CONF_OUTS_PWM         0x0020  // Digital PWM

#define AS5600_CONF_PWMF_MASK        0x00C0  // PWM Frequency [7:6]
#define AS5600_CONF_PWMF_115HZ       0x0000  // 115 Hz
#define AS5600_CONF_PWMF_230HZ       0x0040  // 230 Hz
#define AS5600_CONF_PWMF_460HZ       0x0080  // 460 Hz
#define AS5600_CONF_PWMF_920HZ       0x00C0  // 920 Hz

#define AS5600_CONF_SF_MASK          0x0300  // Slow Filter [9:8]
#define AS5600_CONF_SF_16X           0x0000  // 16x (1)
#define AS5600_CONF_SF_8X            0x0100  // 8x (2)
#define AS5600_CONF_SF_4X            0x0200  // 4x (3)
#define AS5600_CONF_SF_2X            0x0300  // 2x (4)

#define AS5600_CONF_FTH_MASK         0x1C00  // Fast Filter Threshold [12:10]
#define AS5600_CONF_FTH_SLOW_ONLY    0x0000  // Slow filter only
#define AS5600_CONF_FTH_6LSB         0x0400  // 6 LSBs
#define AS5600_CONF_FTH_7LSB         0x0800  // 7 LSBs
#define AS5600_CONF_FTH_9LSB         0x0C00  // 9 LSBs
#define AS5600_CONF_FTH_18LSB        0x1000  // 18 LSBs
#define AS5600_CONF_FTH_21LSB        0x1400  // 21 LSBs
#define AS5600_CONF_FTH_24LSB        0x1800  // 24 LSBs
#define AS5600_CONF_FTH_10LSB        0x1C00  // 10 LSBs

#define AS5600_CONF_WD_MASK          0x2000  // Watchdog [13]
#define AS5600_CONF_WD_OFF           0x0000  // Watchdog OFF
#define AS5600_CONF_WD_ON            0x2000  // Watchdog ON

// Status Register Bit Definitions
#define AS5600_STATUS_MH_MASK        0x08    // Magnet too strong
#define AS5600_STATUS_ML_MASK        0x10    // Magnet too weak
#define AS5600_STATUS_MD_MASK        0x20    // Magnet detected

// Constants
#define AS5600_RESOLUTION            4096    // 12-bit resolution (2^12)
#define AS5600_MAX_ANGLE_DEG         360.0f  // Maximum angle in degrees
#define AS5600_RAW_TO_DEGREES        (AS5600_MAX_ANGLE_DEG / AS5600_RESOLUTION)
#define AS5600_RAW_TO_RADIANS        (2.0f * M_PI / AS5600_RESOLUTION)

// Burn limits
#define AS5600_MAX_BURN_COUNT        3       // Maximum number of burn operations

// Default configuration for steering wheel application
#define AS5600_DEFAULT_CONFIG        (AS5600_CONF_PM_NOM | \
                                     AS5600_CONF_HYST_OFF | \
                                     AS5600_CONF_OUTS_ANALOG | \
                                     AS5600_CONF_PWMF_115HZ | \
                                     AS5600_CONF_SF_16X | \
                                     AS5600_CONF_FTH_SLOW_ONLY | \
                                     AS5600_CONF_WD_OFF)

// **************************************** Data Structures ****************************************

/**
 * @brief AS5600 error codes
 */
typedef enum {
    AS5600_OK = 0,              // Success
    AS5600_ERROR_I2C,           // I2C communication error
    AS5600_ERROR_TIMEOUT,       // Communication timeout
    AS5600_ERROR_INVALID_PARAM, // Invalid parameter
    AS5600_ERROR_CONFIG,        // Configuration error
    AS5600_ERROR_MAGNET,        // Magnet detection error
    AS5600_ERROR_BURN_LIMIT,    // Burn limit exceeded
    AS5600_ERROR_NOT_CONNECTED  // Sensor not responding
} AS5600_Result_t;

/**
 * @brief AS5600 magnet status
 */
typedef enum {
    AS5600_MAGNET_OK = 0,       // Magnet detected and in good range
    AS5600_MAGNET_TOO_WEAK,     // Magnet too weak
    AS5600_MAGNET_TOO_STRONG,   // Magnet too strong
    AS5600_MAGNET_NOT_DETECTED  // No magnet detected
} AS5600_MagnetStatus_t;

/**
 * @brief AS5600 device configuration
 */
typedef struct {
    uint16_t config;            // Configuration register value
    uint16_t zero_position;     // Zero position (ZPOS)
    uint16_t max_position;      // Maximum position (MPOS)
    uint16_t max_angle;         // Maximum angle (MANG)
    uint8_t burn_count;         // Number of burns performed
} AS5600_Config_t;

/**
 * @brief AS5600 angle measurement data
 */
typedef struct {
    uint16_t raw_angle;         // Raw angle value (0-4095)
    uint16_t angle;             // Processed angle value (0-4095)
    float angle_degrees;        // Angle in degrees (0-360)
    float angle_radians;        // Angle in radians (0-2π)
    AS5600_MagnetStatus_t magnet_status; // Magnet detection status
    uint8_t agc;                // Automatic gain control value
    uint16_t magnitude;         // Magnetic field strength
    uint32_t timestamp;         // Measurement timestamp (HAL_GetTick())
} AS5600_Data_t;

/**
 * @brief AS5600 device handle
 */
typedef struct {
    I2C_HandleTypeDef *hi2c;    // I2C handle
    AS5600_Config_t config;     // Device configuration
    AS5600_Data_t last_reading; // Last measurement
    bool initialized;           // Initialization status
    bool connected;             // Connection status
} AS5600_Handle_t;

// **************************************** Function Prototypes ****************************************

// Initialization and Configuration
AS5600_Result_t AS5600_Init(AS5600_Handle_t *handle, I2C_HandleTypeDef *hi2c);
AS5600_Result_t AS5600_Reset(AS5600_Handle_t *handle);
AS5600_Result_t AS5600_SetConfig(AS5600_Handle_t *handle, uint16_t config);
AS5600_Result_t AS5600_GetConfig(AS5600_Handle_t *handle, AS5600_Config_t *config);

// Basic I2C Communication
AS5600_Result_t AS5600_WriteRegister(AS5600_Handle_t *handle, uint8_t reg, uint8_t value);
AS5600_Result_t AS5600_WriteRegister16(AS5600_Handle_t *handle, uint8_t reg_h, uint16_t value);
AS5600_Result_t AS5600_ReadRegister(AS5600_Handle_t *handle, uint8_t reg, uint8_t *value);
AS5600_Result_t AS5600_ReadRegister16(AS5600_Handle_t *handle, uint8_t reg_h, uint16_t *value);

// Angle Measurement
AS5600_Result_t AS5600_ReadAngle(AS5600_Handle_t *handle, AS5600_Data_t *data);
AS5600_Result_t AS5600_ReadRawAngle(AS5600_Handle_t *handle, uint16_t *angle);
AS5600_Result_t AS5600_ReadProcessedAngle(AS5600_Handle_t *handle, uint16_t *angle);

// Status and Diagnostics
AS5600_Result_t AS5600_GetStatus(AS5600_Handle_t *handle, uint8_t *status);
AS5600_Result_t AS5600_GetMagnetStatus(AS5600_Handle_t *handle, AS5600_MagnetStatus_t *status);
AS5600_Result_t AS5600_GetMagnitude(AS5600_Handle_t *handle, uint16_t *magnitude);
AS5600_Result_t AS5600_GetAGC(AS5600_Handle_t *handle, uint8_t *agc);
bool AS5600_IsConnected(AS5600_Handle_t *handle);

// Calibration Functions
AS5600_Result_t AS5600_SetZeroPosition(AS5600_Handle_t *handle, uint16_t position);
AS5600_Result_t AS5600_SetMaxPosition(AS5600_Handle_t *handle, uint16_t position);
AS5600_Result_t AS5600_SetMaxAngle(AS5600_Handle_t *handle, uint16_t angle);
AS5600_Result_t AS5600_CalibrateZeroPosition(AS5600_Handle_t *handle);
AS5600_Result_t AS5600_CalibrateRange(AS5600_Handle_t *handle, uint16_t start_pos, uint16_t end_pos);

// Programming (Burn) Functions
AS5600_Result_t AS5600_BurnAngle(AS5600_Handle_t *handle);
AS5600_Result_t AS5600_BurnSetting(AS5600_Handle_t *handle);
AS5600_Result_t AS5600_GetBurnCount(AS5600_Handle_t *handle, uint8_t *count);

// Utility Functions
float AS5600_RawToRadians(uint16_t raw_angle);
float AS5600_RawToDegrees(uint16_t raw_angle);
uint16_t AS5600_DegreesToRaw(float degrees);
uint16_t AS5600_RadiansToRaw(float radians);
const char* AS5600_GetErrorString(AS5600_Result_t result);

// High-Level Interface (for integration with existing code)
bool Steer_ENC_I2C_Init(void);
void Steer_ENC_I2C_Read(void);
void Steer_ENC_I2C_Full_Read(void);
void Steer_ENC_CAN_Message(uint8_t *canTx);
void Steer_ENC_Main(void);

// Global handle for backward compatibility
extern AS5600_Handle_t g_steering_encoder;

#endif /* INC_FEB_STEERING_H_ */