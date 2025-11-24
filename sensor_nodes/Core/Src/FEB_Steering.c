/*
 * FEB_Steering.c
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 *
 *  AS5600 Magnetic Rotary Position Sensor Implementation
 *  Professional API for steering wheel angle measurement
 */

// **************************************** Includes & External ****************************************

#include "FEB_Steering.h"

extern CAN_HandleTypeDef hcan1;
extern I2C_HandleTypeDef hi2c3;

// **************************************** Global Variables ****************************************

// Global handle for backward compatibility
AS5600_Handle_t g_steering_encoder = {0};

// Legacy compatibility data structures
typedef struct {
    uint32_t can_counter;
    uint16_t flags;
    uint16_t angle;
} CAN_Data;

static CAN_Data can_data;

#if SEND_CAN && STEER
static uint8_t canTx[8];
#endif

// **************************************** Utility Functions ****************************************

/**
 * @brief Convert raw angle value to radians
 * @param raw_angle Raw angle value (0-4095)
 * @return Angle in radians (0 to 2π)
 */
float AS5600_RawToRadians(uint16_t raw_angle)
{
    return (float)raw_angle * AS5600_RAW_TO_RADIANS;
}

/**
 * @brief Convert raw angle value to degrees
 * @param raw_angle Raw angle value (0-4095)
 * @return Angle in degrees (0 to 360)
 */
float AS5600_RawToDegrees(uint16_t raw_angle)
{
    return (float)raw_angle * AS5600_RAW_TO_DEGREES;
}

/**
 * @brief Convert degrees to raw angle value
 * @param degrees Angle in degrees
 * @return Raw angle value (0-4095)
 */
uint16_t AS5600_DegreesToRaw(float degrees)
{
    // Normalize to 0-360 range
    while (degrees < 0.0f) degrees += 360.0f;
    while (degrees >= 360.0f) degrees -= 360.0f;
    
    uint16_t raw = (uint16_t)(degrees / AS5600_RAW_TO_DEGREES);
    return raw < AS5600_RESOLUTION ? raw : AS5600_RESOLUTION - 1;
}

/**
 * @brief Convert radians to raw angle value
 * @param radians Angle in radians
 * @return Raw angle value (0-4095)
 */
uint16_t AS5600_RadiansToRaw(float radians)
{
    // Normalize to 0-2π range
    while (radians < 0.0f) radians += 2.0f * M_PI;
    while (radians >= 2.0f * M_PI) radians -= 2.0f * M_PI;
    
    uint16_t raw = (uint16_t)(radians / AS5600_RAW_TO_RADIANS);
    return raw < AS5600_RESOLUTION ? raw : AS5600_RESOLUTION - 1;
}

/**
 * @brief Get error string for result code
 * @param result AS5600 result code
 * @return Human-readable error string
 */
const char* AS5600_GetErrorString(AS5600_Result_t result)
{
    switch (result) {
        case AS5600_OK:                return "OK";
        case AS5600_ERROR_I2C:         return "I2C Error";
        case AS5600_ERROR_TIMEOUT:     return "Timeout";
        case AS5600_ERROR_INVALID_PARAM: return "Invalid Parameter";
        case AS5600_ERROR_CONFIG:      return "Configuration Error";
        case AS5600_ERROR_MAGNET:      return "Magnet Error";
        case AS5600_ERROR_BURN_LIMIT:  return "Burn Limit Exceeded";
        case AS5600_ERROR_NOT_CONNECTED: return "Not Connected";
        default:                       return "Unknown Error";
    }
}

// **************************************** Low-Level I2C Functions ****************************************

/**
 * @brief Write single byte to AS5600 register
 * @param handle AS5600 device handle
 * @param reg Register address
 * @param value Value to write
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_WriteRegister(AS5600_Handle_t *handle, uint8_t reg, uint8_t value)
{
    if (!handle || !handle->hi2c) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    uint8_t data[2] = {reg, value};
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(handle->hi2c, 
                                                      AS5600_I2C_ADDRESS_SHIFTED, 
                                                      data, 2, 
                                                      AS5600_I2C_TIMEOUT_MS);
    
    if (status == HAL_TIMEOUT) {
        return AS5600_ERROR_TIMEOUT;
    } else if (status != HAL_OK) {
        handle->connected = false;
        return AS5600_ERROR_I2C;
    }
    
    return AS5600_OK;
}

/**
 * @brief Write 16-bit value to AS5600 register pair
 * @param handle AS5600 device handle
 * @param reg_h High byte register address
 * @param value 16-bit value to write
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_WriteRegister16(AS5600_Handle_t *handle, uint8_t reg_h, uint16_t value)
{
    if (!handle || !handle->hi2c) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    uint8_t data[3] = {
        reg_h,
        (value >> 8) & 0xFF,    // High byte
        value & 0xFF            // Low byte
    };
    
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(handle->hi2c, 
                                                      AS5600_I2C_ADDRESS_SHIFTED, 
                                                      data, 3, 
                                                      AS5600_I2C_TIMEOUT_MS);
    
    if (status == HAL_TIMEOUT) {
        return AS5600_ERROR_TIMEOUT;
    } else if (status != HAL_OK) {
        handle->connected = false;
        return AS5600_ERROR_I2C;
    }
    
    return AS5600_OK;
}

/**
 * @brief Read single byte from AS5600 register
 * @param handle AS5600 device handle
 * @param reg Register address
 * @param value Pointer to store read value
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_ReadRegister(AS5600_Handle_t *handle, uint8_t reg, uint8_t *value)
{
    if (!handle || !handle->hi2c || !value) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    // Write register address
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(handle->hi2c, 
                                                      AS5600_I2C_ADDRESS_SHIFTED, 
                                                      &reg, 1, 
                                                      AS5600_I2C_TIMEOUT_MS);
    
    if (status == HAL_TIMEOUT) {
        return AS5600_ERROR_TIMEOUT;
    } else if (status != HAL_OK) {
        handle->connected = false;
        return AS5600_ERROR_I2C;
    }
    
    // Read register value
    status = HAL_I2C_Master_Receive(handle->hi2c, 
                                   AS5600_I2C_ADDRESS_SHIFTED, 
                                   value, 1, 
                                   AS5600_I2C_TIMEOUT_MS);
    
    if (status == HAL_TIMEOUT) {
        return AS5600_ERROR_TIMEOUT;
    } else if (status != HAL_OK) {
        handle->connected = false;
        return AS5600_ERROR_I2C;
    }
    
    handle->connected = true;
    return AS5600_OK;
}

/**
 * @brief Read 16-bit value from AS5600 register pair
 * @param handle AS5600 device handle
 * @param reg_h High byte register address
 * @param value Pointer to store 16-bit value
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_ReadRegister16(AS5600_Handle_t *handle, uint8_t reg_h, uint16_t *value)
{
    if (!handle || !handle->hi2c || !value) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    // Write register address
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(handle->hi2c, 
                                                      AS5600_I2C_ADDRESS_SHIFTED, 
                                                      &reg_h, 1, 
                                                      AS5600_I2C_TIMEOUT_MS);
    
    if (status == HAL_TIMEOUT) {
        return AS5600_ERROR_TIMEOUT;
    } else if (status != HAL_OK) {
        handle->connected = false;
        return AS5600_ERROR_I2C;
    }
    
    // Read 2 bytes
    uint8_t data[2];
    status = HAL_I2C_Master_Receive(handle->hi2c, 
                                   AS5600_I2C_ADDRESS_SHIFTED, 
                                   data, 2, 
                                   AS5600_I2C_TIMEOUT_MS);
    
    if (status == HAL_TIMEOUT) {
        return AS5600_ERROR_TIMEOUT;
    } else if (status != HAL_OK) {
        handle->connected = false;
        return AS5600_ERROR_I2C;
    }
    
    // Combine bytes (AS5600 uses 12-bit values, so mask upper bits)
    *value = ((uint16_t)(data[0] & 0x0F) << 8) | data[1];
    
    handle->connected = true;
    return AS5600_OK;
}

// **************************************** Core API Functions ****************************************

/**
 * @brief Initialize AS5600 sensor
 * @param handle AS5600 device handle
 * @param hi2c I2C handle
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_Init(AS5600_Handle_t *handle, I2C_HandleTypeDef *hi2c)
{
    if (!handle || !hi2c) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    // Initialize handle
    memset(handle, 0, sizeof(AS5600_Handle_t));
    handle->hi2c = hi2c;
    handle->initialized = false;
    handle->connected = false;
    
    // Test connection
    uint8_t test_value;
    AS5600_Result_t result = AS5600_ReadRegister(handle, AS5600_REG_STATUS, &test_value);
    if (result != AS5600_OK) {
        return AS5600_ERROR_NOT_CONNECTED;
    }
    
    // Read current configuration
    result = AS5600_GetConfig(handle, &handle->config);
    if (result != AS5600_OK) {
        return result;
    }
    
    // Set default configuration if needed
    if (handle->config.config == 0 || handle->config.config == 0xFFFF) {
        result = AS5600_SetConfig(handle, AS5600_DEFAULT_CONFIG);
        if (result != AS5600_OK) {
            return result;
        }
    }
    
    handle->initialized = true;
    handle->connected = true;
    
    return AS5600_OK;
}

/**
 * @brief Reset AS5600 sensor to default configuration
 * @param handle AS5600 device handle
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_Reset(AS5600_Handle_t *handle)
{
    if (!handle || !handle->initialized) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    // Set default configuration
    AS5600_Result_t result = AS5600_SetConfig(handle, AS5600_DEFAULT_CONFIG);
    if (result != AS5600_OK) {
        return result;
    }
    
    // Clear zero and max positions
    result = AS5600_SetZeroPosition(handle, 0);
    if (result != AS5600_OK) {
        return result;
    }
    
    result = AS5600_SetMaxPosition(handle, AS5600_RESOLUTION - 1);
    if (result != AS5600_OK) {
        return result;
    }
    
    return AS5600_OK;
}

/**
 * @brief Set AS5600 configuration
 * @param handle AS5600 device handle
 * @param config Configuration value
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_SetConfig(AS5600_Handle_t *handle, uint16_t config)
{
    if (!handle || !handle->initialized) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    AS5600_Result_t result = AS5600_WriteRegister16(handle, AS5600_REG_CONF_H, config);
    if (result == AS5600_OK) {
        handle->config.config = config;
    }
    
    return result;
}

/**
 * @brief Get AS5600 configuration
 * @param handle AS5600 device handle
 * @param config Pointer to store configuration
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_GetConfig(AS5600_Handle_t *handle, AS5600_Config_t *config)
{
    if (!handle || !config) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    AS5600_Result_t result;
    
    // Read burn count
    uint8_t zmco;
    result = AS5600_ReadRegister(handle, AS5600_REG_ZMCO, &zmco);
    if (result != AS5600_OK) return result;
    config->burn_count = zmco & 0x03;
    
    // Read zero position
    result = AS5600_ReadRegister16(handle, AS5600_REG_ZPOS_H, &config->zero_position);
    if (result != AS5600_OK) return result;
    
    // Read max position
    result = AS5600_ReadRegister16(handle, AS5600_REG_MPOS_H, &config->max_position);
    if (result != AS5600_OK) return result;
    
    // Read max angle
    result = AS5600_ReadRegister16(handle, AS5600_REG_MANG_H, &config->max_angle);
    if (result != AS5600_OK) return result;
    
    // Read configuration
    result = AS5600_ReadRegister16(handle, AS5600_REG_CONF_H, &config->config);
    if (result != AS5600_OK) return result;
    
    // Update handle configuration
    handle->config = *config;
    
    return AS5600_OK;
}

// **************************************** Angle Reading Functions ****************************************

/**
 * @brief Read comprehensive angle data from AS5600
 * @param handle AS5600 device handle
 * @param data Pointer to store angle data
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_ReadAngle(AS5600_Handle_t *handle, AS5600_Data_t *data)
{
    if (!handle || !handle->initialized || !data) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    AS5600_Result_t result;
    
    // Read raw angle
    result = AS5600_ReadRegister16(handle, AS5600_REG_RAW_ANGLE_H, &data->raw_angle);
    if (result != AS5600_OK) return result;
    
    // Read processed angle
    result = AS5600_ReadRegister16(handle, AS5600_REG_ANGLE_H, &data->angle);
    if (result != AS5600_OK) return result;
    
    // Read status
    uint8_t status;
    result = AS5600_ReadRegister(handle, AS5600_REG_STATUS, &status);
    if (result != AS5600_OK) return result;
    
    // Determine magnet status
    if (!(status & AS5600_STATUS_MD_MASK)) {
        data->magnet_status = AS5600_MAGNET_NOT_DETECTED;
    } else if (status & AS5600_STATUS_MH_MASK) {
        data->magnet_status = AS5600_MAGNET_TOO_STRONG;
    } else if (status & AS5600_STATUS_ML_MASK) {
        data->magnet_status = AS5600_MAGNET_TOO_WEAK;
    } else {
        data->magnet_status = AS5600_MAGNET_OK;
    }
    
    // Read AGC
    result = AS5600_ReadRegister(handle, AS5600_REG_AGC, &data->agc);
    if (result != AS5600_OK) return result;
    
    // Read magnitude
    result = AS5600_ReadRegister16(handle, AS5600_REG_MAGNITUDE_H, &data->magnitude);
    if (result != AS5600_OK) return result;
    
    // Convert to degrees and radians
    data->angle_degrees = AS5600_RawToDegrees(data->angle);
    data->angle_radians = AS5600_RawToRadians(data->angle);
    
    // Set timestamp
    data->timestamp = HAL_GetTick();
    
    // Update last reading
    handle->last_reading = *data;
    
    return AS5600_OK;
}

/**
 * @brief Read raw angle value only
 * @param handle AS5600 device handle
 * @param angle Pointer to store raw angle
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_ReadRawAngle(AS5600_Handle_t *handle, uint16_t *angle)
{
    if (!handle || !handle->initialized || !angle) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    return AS5600_ReadRegister16(handle, AS5600_REG_RAW_ANGLE_H, angle);
}

/**
 * @brief Read processed angle value only
 * @param handle AS5600 device handle
 * @param angle Pointer to store processed angle
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_ReadProcessedAngle(AS5600_Handle_t *handle, uint16_t *angle)
{
    if (!handle || !handle->initialized || !angle) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    return AS5600_ReadRegister16(handle, AS5600_REG_ANGLE_H, angle);
}

// **************************************** Status Functions ****************************************

/**
 * @brief Get AS5600 status register
 * @param handle AS5600 device handle
 * @param status Pointer to store status
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_GetStatus(AS5600_Handle_t *handle, uint8_t *status)
{
    if (!handle || !handle->initialized || !status) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    return AS5600_ReadRegister(handle, AS5600_REG_STATUS, status);
}

/**
 * @brief Get magnet detection status
 * @param handle AS5600 device handle
 * @param status Pointer to store magnet status
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_GetMagnetStatus(AS5600_Handle_t *handle, AS5600_MagnetStatus_t *status)
{
    if (!handle || !handle->initialized || !status) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    uint8_t reg_status;
    AS5600_Result_t result = AS5600_ReadRegister(handle, AS5600_REG_STATUS, &reg_status);
    if (result != AS5600_OK) {
        return result;
    }
    
    if (!(reg_status & AS5600_STATUS_MD_MASK)) {
        *status = AS5600_MAGNET_NOT_DETECTED;
    } else if (reg_status & AS5600_STATUS_MH_MASK) {
        *status = AS5600_MAGNET_TOO_STRONG;
    } else if (reg_status & AS5600_STATUS_ML_MASK) {
        *status = AS5600_MAGNET_TOO_WEAK;
    } else {
        *status = AS5600_MAGNET_OK;
    }
    
    return AS5600_OK;
}

/**
 * @brief Get magnetic field magnitude
 * @param handle AS5600 device handle
 * @param magnitude Pointer to store magnitude
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_GetMagnitude(AS5600_Handle_t *handle, uint16_t *magnitude)
{
    if (!handle || !handle->initialized || !magnitude) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    return AS5600_ReadRegister16(handle, AS5600_REG_MAGNITUDE_H, magnitude);
}

/**
 * @brief Get automatic gain control value
 * @param handle AS5600 device handle
 * @param agc Pointer to store AGC value
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_GetAGC(AS5600_Handle_t *handle, uint8_t *agc)
{
    if (!handle || !handle->initialized || !agc) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    return AS5600_ReadRegister(handle, AS5600_REG_AGC, agc);
}

/**
 * @brief Check if sensor is connected and responding
 * @param handle AS5600 device handle
 * @return true if connected, false otherwise
 */
bool AS5600_IsConnected(AS5600_Handle_t *handle)
{
    if (!handle || !handle->hi2c) {
        return false;
    }
    
    uint8_t status;
    AS5600_Result_t result = AS5600_ReadRegister(handle, AS5600_REG_STATUS, &status);
    
    handle->connected = (result == AS5600_OK);
    return handle->connected;
}

// **************************************** Calibration Functions ****************************************

/**
 * @brief Set zero position
 * @param handle AS5600 device handle
 * @param position Zero position value (0-4095)
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_SetZeroPosition(AS5600_Handle_t *handle, uint16_t position)
{
    if (!handle || !handle->initialized || position >= AS5600_RESOLUTION) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    AS5600_Result_t result = AS5600_WriteRegister16(handle, AS5600_REG_ZPOS_H, position);
    if (result == AS5600_OK) {
        handle->config.zero_position = position;
    }
    
    return result;
}

/**
 * @brief Set maximum position
 * @param handle AS5600 device handle
 * @param position Maximum position value (0-4095)
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_SetMaxPosition(AS5600_Handle_t *handle, uint16_t position)
{
    if (!handle || !handle->initialized || position >= AS5600_RESOLUTION) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    AS5600_Result_t result = AS5600_WriteRegister16(handle, AS5600_REG_MPOS_H, position);
    if (result == AS5600_OK) {
        handle->config.max_position = position;
    }
    
    return result;
}

/**
 * @brief Set maximum angle
 * @param handle AS5600 device handle
 * @param angle Maximum angle value (0-4095)
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_SetMaxAngle(AS5600_Handle_t *handle, uint16_t angle)
{
    if (!handle || !handle->initialized || angle >= AS5600_RESOLUTION) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    AS5600_Result_t result = AS5600_WriteRegister16(handle, AS5600_REG_MANG_H, angle);
    if (result == AS5600_OK) {
        handle->config.max_angle = angle;
    }
    
    return result;
}

/**
 * @brief Calibrate zero position using current angle
 * @param handle AS5600 device handle
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_CalibrateZeroPosition(AS5600_Handle_t *handle)
{
    if (!handle || !handle->initialized) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    uint16_t current_angle;
    AS5600_Result_t result = AS5600_ReadRawAngle(handle, &current_angle);
    if (result != AS5600_OK) {
        return result;
    }
    
    return AS5600_SetZeroPosition(handle, current_angle);
}

/**
 * @brief Calibrate angle range
 * @param handle AS5600 device handle
 * @param start_pos Start position (will be set as zero)
 * @param end_pos End position (will be set as max)
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_CalibrateRange(AS5600_Handle_t *handle, uint16_t start_pos, uint16_t end_pos)
{
    if (!handle || !handle->initialized || 
        start_pos >= AS5600_RESOLUTION || end_pos >= AS5600_RESOLUTION) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    AS5600_Result_t result;
    
    result = AS5600_SetZeroPosition(handle, start_pos);
    if (result != AS5600_OK) return result;
    
    result = AS5600_SetMaxPosition(handle, end_pos);
    if (result != AS5600_OK) return result;
    
    // Calculate and set max angle
    uint16_t max_angle = (end_pos > start_pos) ? 
                        (end_pos - start_pos) : 
                        (AS5600_RESOLUTION - start_pos + end_pos);
    
    result = AS5600_SetMaxAngle(handle, max_angle);
    
    return result;
}

// **************************************** Programming Functions ****************************************

/**
 * @brief Get burn count
 * @param handle AS5600 device handle
 * @param count Pointer to store burn count
 * @return AS5600_Result_t
 */
AS5600_Result_t AS5600_GetBurnCount(AS5600_Handle_t *handle, uint8_t *count)
{
    if (!handle || !handle->initialized || !count) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    uint8_t zmco;
    AS5600_Result_t result = AS5600_ReadRegister(handle, AS5600_REG_ZMCO, &zmco);
    if (result == AS5600_OK) {
        *count = zmco & 0x03;
        handle->config.burn_count = *count;
    }
    
    return result;
}

/**
 * @brief Burn angle settings to non-volatile memory
 * @param handle AS5600 device handle
 * @return AS5600_Result_t
 * @warning This permanently programs the device! Use with caution.
 */
AS5600_Result_t AS5600_BurnAngle(AS5600_Handle_t *handle)
{
    if (!handle || !handle->initialized) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    // Check burn count
    uint8_t burn_count;
    AS5600_Result_t result = AS5600_GetBurnCount(handle, &burn_count);
    if (result != AS5600_OK) {
        return result;
    }
    
    if (burn_count >= AS5600_MAX_BURN_COUNT) {
        return AS5600_ERROR_BURN_LIMIT;
    }
    
    // Send burn command
    return AS5600_WriteRegister(handle, AS5600_REG_BURN, 0x80);
}

/**
 * @brief Burn configuration settings to non-volatile memory
 * @param handle AS5600 device handle
 * @return AS5600_Result_t
 * @warning This permanently programs the device! Use with caution.
 */
AS5600_Result_t AS5600_BurnSetting(AS5600_Handle_t *handle)
{
    if (!handle || !handle->initialized) {
        return AS5600_ERROR_INVALID_PARAM;
    }
    
    // Check burn count
    uint8_t burn_count;
    AS5600_Result_t result = AS5600_GetBurnCount(handle, &burn_count);
    if (result != AS5600_OK) {
        return result;
    }
    
    if (burn_count >= AS5600_MAX_BURN_COUNT) {
        return AS5600_ERROR_BURN_LIMIT;
    }
    
    // Send burn command
    return AS5600_WriteRegister(handle, AS5600_REG_BURN, 0x40);
}

// **************************************** Legacy Compatibility Functions ****************************************

/**
 * @brief Legacy initialization function
 * @return true if successful, false otherwise
 */
bool Steer_ENC_I2C_Init(void)
{
    AS5600_Result_t result = AS5600_Init(&g_steering_encoder, &hi2c3);
    return (result == AS5600_OK);
}

/**
 * @brief Legacy read function (reads processed angle only)
 */
void Steer_ENC_I2C_Read(void)
{
    AS5600_ReadProcessedAngle(&g_steering_encoder, &g_steering_encoder.last_reading.angle);
}

/**
 * @brief Legacy full read function
 */
void Steer_ENC_I2C_Full_Read(void)
{
    AS5600_ReadAngle(&g_steering_encoder, &g_steering_encoder.last_reading);
}

/**
 * @brief Legacy CAN message formatting
 * @param canTx CAN transmit buffer
 */
void Steer_ENC_CAN_Message(uint8_t *canTx)
{
    if (!canTx) return;
    
    memset(canTx, 0, 8);
    
    // Get magnet status flags
    uint8_t flags = 0;
    if (g_steering_encoder.last_reading.magnet_status == AS5600_MAGNET_OK) {
        flags |= 0x01;  // Magnet detected
    }
    if (g_steering_encoder.last_reading.magnet_status == AS5600_MAGNET_TOO_STRONG) {
        flags |= 0x02;  // Magnet too strong
    }
    if (g_steering_encoder.last_reading.magnet_status == AS5600_MAGNET_TOO_WEAK) {
        flags |= 0x04;  // Magnet too weak
    }
    
    can_data.flags = flags;
    can_data.angle = g_steering_encoder.last_reading.angle;
    can_data.can_counter++;
    
    memcpy(canTx, &can_data, sizeof(can_data));
}

/**
 * @brief Legacy main function
 */
void Steer_ENC_Main(void)
{
#if STEER
    if (!g_steering_encoder.initialized) {
        return;
    }
    
    // Read angle data
    Steer_ENC_I2C_Read();
    
    // Prepare CAN message
    Steer_ENC_CAN_Message(canTx);
    
    // Send via CAN
#if SEND_CAN
    CAN_Transmit(0x19, canTx); // Todo: add the ID
#endif
#endif
}
