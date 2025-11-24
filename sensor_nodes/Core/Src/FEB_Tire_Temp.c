/*
 * FEB_Tire_Temp.c
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

// **************************************** Includes & External ****************************************

#include "FEB_Tire_Temp.h"
#include "FEB_Comms.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim6;  // For non-blocking delays

// ******************************************** Constants **********************************************

// Number of configuration attempts
#define CONFIG_MAX_ATTEMPTS     3
#define CONFIG_SEND_INTERVAL_MS 1000  // 1 second between config messages
#define CONFIG_TOTAL_TIME_MS    10000 // Total configuration time

// ******************************************** Variables **********************************************

// Sensor state management
static IRTS_State_t sensor_state = IRTS_STATE_IDLE;
static uint32_t config_start_time = 0;
static uint32_t last_config_send_time = 0;

// Temperature data storage
static IRTS_SensorData_t tire_temp_data[4];  // FL, FR, RL, RR

// Sensor position mapping
static const uint16_t sensor_can_ids[4] = {
    IRTS_ID_FRONT_LEFT,
    IRTS_ID_FRONT_RIGHT,
    IRTS_ID_REAR_LEFT,
    IRTS_ID_REAR_RIGHT
};

// ******************************************** Functions **********************************************

/**
 * @brief Initialize tire temperature sensor system
 * @note Sets up CAN2 for IRTS-V3 communication and configures filters
 */
void Tire_Temp_Init(void)
{
#if TIRETEMP
    CAN_FilterTypeDef filter_config;
    
    // Configure CAN2 filter to receive IRTS messages (0x4B0-0x4BF)
    filter_config.FilterBank = 14;  // Use filter bank 14 for CAN2 (banks 14-27 for CAN2)
    filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
    filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
    filter_config.FilterIdHigh = (IRTS_BASE_CAN_ID << 5);  // Shift for 32-bit scale
    filter_config.FilterIdLow = 0x0000;
    filter_config.FilterMaskIdHigh = (IRTS_CAN_ID_MASK << 5);  // Accept 0x4B0-0x4BF
    filter_config.FilterMaskIdLow = 0x0000;
    filter_config.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter_config.FilterActivation = CAN_FILTER_ENABLE;
    filter_config.SlaveStartFilterBank = 14;  // CAN2 starts at bank 14
    
    if (HAL_CAN_ConfigFilter(&hcan2, &filter_config) != HAL_OK) {
        #ifdef DEBUG_TIRE_TEMP_CONFIG
        printf("IRTS: Failed to configure CAN filter\r\n");
        #endif
        sensor_state = IRTS_STATE_ERROR;
        return;
    }
    
    // Start CAN2
    if (HAL_CAN_Start(&hcan2) != HAL_OK) {
        #ifdef DEBUG_TIRE_TEMP_CONFIG
        printf("IRTS: Failed to start CAN2\r\n");
        #endif
        sensor_state = IRTS_STATE_ERROR;
        return;
    }
    
    // Enable CAN2 RX interrupt
    if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        #ifdef DEBUG_TIRE_TEMP_CONFIG
        printf("IRTS: Failed to activate CAN2 notifications\r\n");
        #endif
        sensor_state = IRTS_STATE_ERROR;
        return;
    }
    
    // Initialize data structures
    memset(tire_temp_data, 0, sizeof(tire_temp_data));
    
    // Start configuration process for each sensor
    sensor_state = IRTS_STATE_CONFIGURING;
    config_start_time = HAL_GetTick();
    last_config_send_time = 0;
    
    #ifdef DEBUG_TIRE_TEMP_CONFIG
    printf("IRTS: Initialization complete, starting configuration\r\n");
    #endif
#endif
}

/**
 * @brief Send configuration message to IRTS sensor
 * @param sensor_id Index of sensor to configure (0-3)
 */
void Tire_Temp_Configure_Sensor(uint16_t sensor_id)
{
    if (sensor_id >= 4) return;
    
    CAN_TxHeaderTypeDef tx_header;
    uint8_t tx_data[8];
    uint32_t tx_mailbox;
    
    uint16_t current_can_id = sensor_can_ids[sensor_id];
    uint16_t new_can_id = current_can_id;  // Keep same ID
    
    // Prepare configuration message
    tx_header.StdId = current_can_id;
    tx_header.ExtId = 0;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;
    tx_header.TransmitGlobalTime = DISABLE;
    
    // Configuration data per IRTS-V3 protocol
    tx_data[0] = IRTS_CONFIG_MAGIC_MSB;        // Programming constant MSB
    tx_data[1] = IRTS_CONFIG_MAGIC_LSB;        // Programming constant LSB
    tx_data[2] = (new_can_id >> 8) & 0xFF;     // New CAN ID MSB
    tx_data[3] = new_can_id & 0xFF;            // New CAN ID LSB
    tx_data[4] = IRTS_EMISSIVITY;              // Emissivity (0.85 * 100)
    tx_data[5] = IRTS_SAMPLE_FREQ;             // Sample frequency (100Hz)
    tx_data[6] = IRTS_NUM_CHANNELS;            // Number of channels (16)
    tx_data[7] = IRTS_CAN_BITRATE;             // CAN bitrate (500kbps)
    
    // Send configuration message
    if (HAL_CAN_AddTxMessage(&hcan2, &tx_header, tx_data, &tx_mailbox) == HAL_OK) {
        #ifdef DEBUG_TIRE_TEMP_CONFIG
        printf("IRTS: Config sent to 0x%03X\r\n", current_can_id);
        #endif
    } else {
        #ifdef DEBUG_TIRE_TEMP_CONFIG
        printf("IRTS: Config send failed for 0x%03X\r\n", current_can_id);
        #endif
    }
}

/**
 * @brief Process configuration state machine
 * @note Called periodically from main loop to handle non-blocking configuration
 */
void Tire_Temp_Process_Config(void)
{
    if (sensor_state != IRTS_STATE_CONFIGURING) {
        return;
    }
    
    uint32_t current_time = HAL_GetTick();
    
    // Check for timeout
    if (current_time - config_start_time > CONFIG_TOTAL_TIME_MS) {
        sensor_state = IRTS_STATE_READY;  // Configuration complete
        #ifdef DEBUG_TIRE_TEMP_CONFIG
        printf("IRTS: Configuration complete\r\n");
        #endif
        return;
    }
    
    // Send configuration messages at intervals
    if (current_time - last_config_send_time >= CONFIG_SEND_INTERVAL_MS) {
        last_config_send_time = current_time;
        
        #if IS_FRONT_NODE
        // Configure front sensors
        Tire_Temp_Configure_Sensor(0);  // Front left
        Tire_Temp_Configure_Sensor(1);  // Front right
        #else
        // Configure rear sensors
        Tire_Temp_Configure_Sensor(2);  // Rear left
        Tire_Temp_Configure_Sensor(3);  // Rear right
        #endif
    }
}

/**
 * @brief Convert raw temperature value to Celsius
 * @param raw_value Raw 16-bit temperature value from sensor
 * @return Temperature in degrees Celsius
 */
float Tire_Temp_Convert_Temperature(uint16_t raw_value)
{
    // Temperature = (raw_value * 0.01) - 100
    return ((float)raw_value * IRTS_TEMP_SCALE) - IRTS_TEMP_OFFSET;
}

/**
 * @brief Process received CAN message from IRTS sensor
 * @param RxHeader CAN message header
 * @param RxData CAN message data (8 bytes)
 */
void Tire_Temp_Process_Message(CAN_RxHeaderTypeDef *RxHeader, uint8_t *RxData)
{
    // Check if message is from IRTS sensor
    if ((RxHeader->StdId & IRTS_CAN_ID_MASK) != IRTS_BASE_CAN_ID) {
        return;
    }
    
    // Determine which sensor sent the message
    uint8_t sensor_index = 0xFF;
    for (int i = 0; i < 4; i++) {
        if ((RxHeader->StdId & 0xFFC) == sensor_can_ids[i]) {
            sensor_index = i;
            break;
        }
    }
    
    if (sensor_index == 0xFF) {
        return;  // Unknown sensor
    }
    
    // Determine which channels are in this message
    uint8_t channel_offset = (RxHeader->StdId & 0x3) * 4;  // 0, 4, 8, or 12
    
    // Extract 4 temperature values (2 bytes each)
    for (int i = 0; i < IRTS_CHANNELS_PER_MSG; i++) {
        uint16_t raw_temp = (RxData[i * 2] << 8) | RxData[i * 2 + 1];
        uint8_t channel_num = channel_offset + i;
        
        if (channel_num < 16) {
            tire_temp_data[sensor_index].channels[channel_num] = raw_temp;
            tire_temp_data[sensor_index].valid_channels |= (1 << channel_num);
        }
        
        #ifdef DEBUG_TIRE_TEMP_DATA
        float temp_c = Tire_Temp_Convert_Temperature(raw_temp);
        printf("IRTS[%d] Ch%02d: %.2f°C\r\n", sensor_index, channel_num, temp_c);
        #endif
    }
    
    tire_temp_data[sensor_index].last_update = HAL_GetTick();
}

/**
 * @brief Calculate average temperature for a sensor
 * @param sensor_id Sensor index (0-3)
 * @return Average temperature in 0.01°C units with offset
 */
uint16_t Tire_Temp_Get_Average(uint16_t sensor_id)
{
    if (sensor_id >= 4) return 0;
    
    uint32_t sum = 0;
    uint8_t count = 0;
    
    // Calculate average of valid channels
    for (int i = 0; i < 16; i++) {
        if (tire_temp_data[sensor_id].valid_channels & (1 << i)) {
            sum += tire_temp_data[sensor_id].channels[i];
            count++;
        }
    }
    
    if (count > 0) {
        return (uint16_t)(sum / count);
    }
    
    return 10000;  // Return 0°C (with 100°C offset) if no valid data
}

/**
 * @brief CAN receive callback handler
 * @param hcan CAN handle
 * @note This is called from HAL_CAN_IRQHandler via weak callback
 */
void Tire_Temp_CAN_Callback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance == CAN2) {
        CAN_RxHeaderTypeDef rx_header;
        uint8_t rx_data[8];
        
        // Get message from FIFO0
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) {
            // Process IRTS message
            Tire_Temp_Process_Message(&rx_header, rx_data);
        }
    }
}

/**
 * @brief Main tire temperature processing routine
 * @note Called periodically from TIM6 interrupt
 */
void Tire_Temp_Main(void)
{
#if TIRETEMP
    // Handle configuration state machine
    if (sensor_state == IRTS_STATE_CONFIGURING) {
        Tire_Temp_Process_Config();
        return;  // Don't transmit data while configuring
    }
    
    if (sensor_state != IRTS_STATE_READY) {
        return;  // Not ready to transmit
    }
    
    // Prepare and send temperature data
    #if IS_FRONT_NODE
    uint16_t avg_fl = Tire_Temp_Get_Average(0);
    uint16_t avg_fr = Tire_Temp_Get_Average(1);
    
    // Pack and send front left temperature
    uint8_t fl_data[8] = {(avg_fl >> 8) & 0xFF, avg_fl & 0xFF, 0, 0, 0, 0, 0, 0};
    uint8_t fr_data[8] = {(avg_fr >> 8) & 0xFF, avg_fr & 0xFF, 0, 0, 0, 0, 0, 0};
    
#if SEND_CAN
    CAN_Transmit(FEB_CAN_FRONT_LEFT_TIRE_TEMP_FRAME_ID, fl_data);
    CAN_Transmit(FEB_CAN_FRONT_RIGHT_TIRE_TEMP_FRAME_ID, fr_data);
#endif
    
#else  // Rear node
    uint16_t avg_rl = Tire_Temp_Get_Average(2);
    uint16_t avg_rr = Tire_Temp_Get_Average(3);
    
    // Pack and send rear temperatures
    uint8_t rl_data[8] = {(avg_rl >> 8) & 0xFF, avg_rl & 0xFF, 0, 0, 0, 0, 0, 0};
    uint8_t rr_data[8] = {(avg_rr >> 8) & 0xFF, avg_rr & 0xFF, 0, 0, 0, 0, 0, 0};
    
#if SEND_CAN
    CAN_Transmit(FEB_CAN_REAR_LEFT_TIRE_TEMP_FRAME_ID, rl_data);
    CAN_Transmit(FEB_CAN_REAR_RIGHT_TIRE_TEMP_FRAME_ID, rr_data);
#endif
#endif
    
    #ifdef DEBUG_TIRE_TEMP_PRINTF
    // Debug output every second
    static uint32_t last_debug_time = 0;
    if (HAL_GetTick() - last_debug_time > 1000) {
        last_debug_time = HAL_GetTick();
        #if IS_FRONT_NODE
        printf("Tire Temps - FL: %.1f°C, FR: %.1f°C\r\n",
            Tire_Temp_Convert_Temperature(avg_fl),
            Tire_Temp_Convert_Temperature(avg_fr));
        #else
        printf("Tire Temps - RL: %.1f°C, RR: %.1f°C\r\n",
            Tire_Temp_Convert_Temperature(avg_rl),
            Tire_Temp_Convert_Temperature(avg_rr));
        #endif
    }
    #endif
#endif
}

/**
 * @brief Get current sensor state
 * @return Current IRTS state
 */
IRTS_State_t Tire_Temp_Get_State(void)
{
    return sensor_state;
}

/**
 * @brief HAL CAN receive callback (weak override)
 * @param hcan CAN handle
 * @note This overrides the weak callback in HAL driver
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    // Route to appropriate handler
    if (hcan->Instance == CAN2) {
        #if TIRETEMP
        Tire_Temp_CAN_Callback(hcan);
        #endif
    }
    // Add other CAN handlers here if needed
}
