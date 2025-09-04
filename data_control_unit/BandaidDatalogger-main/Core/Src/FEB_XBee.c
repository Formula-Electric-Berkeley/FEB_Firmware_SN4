/*
 * FEB_XBee.c
 *
 *  Created on: Feb 1, 2025
 *      Author: mihirtakalkar
 */

#include "FEB_XBee.h"
#include <string.h>
#include <stdio.h>

/* Forward declarations */
static HAL_StatusTypeDef xbee_transmit_minimal(const char *data, uint8_t data_len);

extern SPI_HandleTypeDef hspi3;
extern UART_HandleTypeDef huart2;

// CHIP SELECT LINES
#define XBEE_CS_PORT GPIOA
#define XBEE_CS_PIN  GPIO_PIN_15

#define XBEE_ATTN_PORT GPIOC
#define XBEE_ATTN_PIN  GPIO_PIN_12


// CS Low (Start SPI Message)
static void xbee_select(void)
{
    HAL_GPIO_WritePin(XBEE_CS_PORT, XBEE_CS_PIN, GPIO_PIN_RESET);
}

// CS High (End SPI Message)
static void xbee_deselect(void)
{
    HAL_GPIO_WritePin(XBEE_CS_PORT, XBEE_CS_PIN, GPIO_PIN_SET);
}


//----TESTING LOOP----//
void FEB_xbee_loop()
{
    static uint32_t counter = 0;
    char message[50];
    snprintf(message, sizeof(message), "hello %lu", (unsigned long)counter);
    
    HAL_StatusTypeDef status = xbee_transmit_minimal(message, (uint8_t)strlen(message));
    if (status == HAL_OK) {
        XBEE_VERBOSE_PRINT("XBEE frame sent OK\r\n", 20);
    } else {
        XBEE_VERBOSE_PRINT("XBEE frame FAILED\r\n", 19);
    }

    counter++;
    HAL_Delay(500); /* Adjust delay for testing */
}

/* XBEE destination address configuration */
static uint8_t xbee_dest_addr[8] = {
    0x00, 0x13, 0xA2, 0x00, /* SH bytes */
    0x41, 0xC2, 0x69, 0x0F  /* SL bytes */
};

static HAL_StatusTypeDef xbee_transmit_minimal(const char *data, uint8_t data_len) {
    if (data == NULL || data_len == 0 || data_len > 100) {
        return HAL_ERROR;
    }
    
    uint8_t frame[150];
    /* Fixed frame structure: 3 bytes header + API frame + checksum */
    /* API frame: 1 type + 1 ID + 8 addr + 2 16bit + 1 radius + 1 options + data */
    uint16_t api_frame_len = 14 + data_len;  /* API frame payload length */
    uint16_t total_frame_len = 3 + api_frame_len + 1;  /* Header + payload + checksum */
    
    /* Validate SPI peripheral state */
    if (hspi3.State != HAL_SPI_STATE_READY) {
        return HAL_BUSY;
    }

    /* XBee API frame structure */
    frame[0] = 0x7E;                           /* Start delimiter */
    frame[1] = (api_frame_len >> 8) & 0xFF;    /* Length MSB (payload only) */
    frame[2] = api_frame_len & 0xFF;           /* Length LSB (payload only) */

    /* API frame payload starts here */
    frame[3] = 0x10;    /* Frame type: TX Request 64-bit */
    frame[4] = 0x01;    /* Frame ID (non-zero to get TX status) */

    /* Destination 64-bit address - use broadcast for testing */
    frame[5] = 0x00;    /* SH byte 0 */
    frame[6] = 0x00;    /* SH byte 1 */
    frame[7] = 0x00;    /* SH byte 2 */
    frame[8] = 0x00;    /* SH byte 3 */
    frame[9] = 0x00;    /* SL byte 0 */
    frame[10] = 0x00;   /* SL byte 1 */
    frame[11] = 0x00;   /* SL byte 2 */
    frame[12] = 0xFF;   /* SL byte 3 - broadcast (0x000000000000FFFF) */

    /* 16-bit network address - broadcast */
    frame[13] = 0xFF;   /* 16-bit addr MSB */
    frame[14] = 0xFE;   /* 16-bit addr LSB */

    frame[15] = 0x00;   /* Broadcast radius (0 = max hops) */
    frame[16] = 0x00;   /* Options (0x00 = enable ACK, 0x01 = disable ACK) */

    /* Copy user data */
    memcpy(&frame[17], data, data_len);

    /* Calculate checksum - sum all bytes from frame type to end of data */
    uint8_t sum = 0;
    for (uint16_t i = 3; i < 17 + data_len; i++) {
        sum += frame[i];
    }
    frame[17 + data_len] = 0xFF - sum;  /* Checksum at correct position */

    /* Transmit complete frame */
    xbee_select();
    HAL_Delay(1);  /* Brief delay after CS assert for XBee */
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi3, frame, total_frame_len, 2000);
    HAL_Delay(1);  /* Brief delay before CS deassert */
    xbee_deselect();
    
    /* Enhanced debug output */
    if (status == HAL_OK) {
        XBEE_DEBUG_PRINT("XBEE TX OK", 10);
        #if XBEE_VERBOSE_ENABLED
        char frame_info[150];
        snprintf(frame_info, sizeof(frame_info), 
                " [%d bytes total, API len=%d, data_len=%d, checksum=0x%02X]\r\n", 
                total_frame_len, api_frame_len, data_len, frame[17 + data_len]);
        HAL_UART_Transmit(&huart2, (uint8_t*)frame_info, strlen(frame_info), 100);
        #endif
    } else {
        XBEE_DEBUG_PRINT("XBEE TX FAIL\r\n", 14);
        #if XBEE_DEBUG_ENABLED
        char error_info[50];
        snprintf(error_info, sizeof(error_info), " [SPI Error: %d]\r\n", status);
        HAL_UART_Transmit(&huart2, (uint8_t*)error_info, strlen(error_info), 100);
        #endif
    }
    
    return status;
}


uint8_t FEB_xbee_receive_status(void) {
    uint8_t rx_buf[32] = {0};

    xbee_select();

    for (int i = 0; i < sizeof(rx_buf); i++) {
        uint8_t dummy = 0x00;
        HAL_SPI_TransmitReceive(&hspi3, &dummy, &rx_buf[i], 1, HAL_MAX_DELAY);
    }

    xbee_deselect();

    for (int i = 0; i < sizeof(rx_buf) - 5; i++) {
        if (rx_buf[i] == 0x7E && rx_buf[i + 3] == 0x8B) {
            uint8_t delivery_status = rx_buf[i + 8];  
            /* 0x00 = success, 0x01 = no ACK (but transmitted) */
            return (delivery_status == 0x00 || delivery_status == 0x01) ? 1 : 0;
        }
    }

    return 0;
}

HAL_StatusTypeDef FEB_xbee_transmit_can_data(circBuffer *cb)
{
    if (cb == NULL || cb->count == 0) {
        return HAL_ERROR;
    }

    /* Build binary CAN frame (timestamp + ID + 8 bytes data + checksum = 17 bytes) */
    uint8_t payload[17];
    size_t pos = 0;

    /* 1. Timestamp (4 bytes, big-endian) */
    uint32_t ts = cb->buffer[cb->read].timestamp;
    payload[pos++] = (ts >> 24) & 0xFF;
    payload[pos++] = (ts >> 16) & 0xFF;
    payload[pos++] = (ts >> 8)  & 0xFF;
    payload[pos++] = (ts)       & 0xFF;

    /* 2. CAN ID (4 bytes, big-endian) */
    uint32_t id = cb->buffer[cb->read].id;
    payload[pos++] = (id >> 24) & 0xFF;
    payload[pos++] = (id >> 16) & 0xFF;
    payload[pos++] = (id >> 8)  & 0xFF;
    payload[pos++] = (id)       & 0xFF;

    /* 3. Data (8 bytes) */
    memcpy(&payload[pos], cb->buffer[cb->read].data, 8);
    pos += 8;

    /* 4. Simple XOR checksum */
    uint8_t checksum = 0;
    for (size_t i = 0; i < pos; i++) {
        checksum ^= payload[i];
    }
    payload[pos++] = checksum;

    /* Advance circular buffer */
    cb->read = (cb->read + 1) % cb->capacity;
    cb->count--;

    #if XBEE_DEBUG_ENABLED
    /* Print CAN data being transmitted */
    char debug_msg[200];
    snprintf(debug_msg, sizeof(debug_msg), 
             "TX CAN: ID=0x%lX, T=%lu, Data=%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
             (unsigned long)id, (unsigned long)ts, 
             cb->buffer[(cb->read + cb->capacity - 1) % cb->capacity].data[0],
             cb->buffer[(cb->read + cb->capacity - 1) % cb->capacity].data[1], 
             cb->buffer[(cb->read + cb->capacity - 1) % cb->capacity].data[2],
             cb->buffer[(cb->read + cb->capacity - 1) % cb->capacity].data[3],
             cb->buffer[(cb->read + cb->capacity - 1) % cb->capacity].data[4],
             cb->buffer[(cb->read + cb->capacity - 1) % cb->capacity].data[5],
             cb->buffer[(cb->read + cb->capacity - 1) % cb->capacity].data[6],
             cb->buffer[(cb->read + cb->capacity - 1) % cb->capacity].data[7]);
    HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), 100);
    #endif

    /* Send using XBEE SPI transmit function with retry logic */
    HAL_StatusTypeDef status = xbee_transmit_minimal((const char *)payload, pos);
    
    /* Retry once on failure */
    if (status != HAL_OK) {
        HAL_Delay(50); /* Longer delay before retry */
        status = xbee_transmit_minimal((const char *)payload, pos);
    }
    
    return status;
}

HAL_StatusTypeDef FEB_xbee_transmit_hello(void) {
    const char *msg = "hello";
    return xbee_transmit_minimal(msg, (uint8_t)strlen(msg));
}

/* Set XBEE destination address */
void FEB_xbee_set_destination(const uint8_t *addr_64bit) {
    if (addr_64bit != NULL) {
        memcpy(xbee_dest_addr, addr_64bit, 8);
    }
}

/* Enhanced XBee diagnostics */
HAL_StatusTypeDef FEB_xbee_spi_test(void) {
    HAL_UART_Transmit(&huart2, (uint8_t*)"=== XBEE SPI DIAGNOSTICS ===\r\n", 31, 100);
    
    /* Check XBee network settings first */
    HAL_UART_Transmit(&huart2, (uint8_t*)"0. Checking XBee network settings...\r\n", 39, 100);
    
    /* Send AT CH command to get channel */
    uint8_t at_ch_frame[] = {0x7E, 0x00, 0x04, 0x08, 0x02, 'C', 'H', 0x6D};
    xbee_select();
    HAL_SPI_Transmit(&hspi3, at_ch_frame, sizeof(at_ch_frame), 1000);
    xbee_deselect();
    HAL_Delay(50);
    
    /* Send AT ID command to get PAN ID */
    uint8_t at_id_frame[] = {0x7E, 0x00, 0x04, 0x08, 0x03, 'I', 'D', 0x6A};
    xbee_select();
    HAL_SPI_Transmit(&hspi3, at_id_frame, sizeof(at_id_frame), 1000);
    xbee_deselect();
    HAL_Delay(50);
    
    /* Read responses */
    uint8_t config_buf[64] = {0};
    xbee_select();
    for (int i = 0; i < sizeof(config_buf); i++) {
        uint8_t dummy = 0x00;
        HAL_SPI_TransmitReceive(&hspi3, &dummy, &config_buf[i], 1, 10);
    }
    xbee_deselect();
    
    /* Parse and display network config */
    HAL_UART_Transmit(&huart2, (uint8_t*)"   Network config: ", 19, 100);
    for (int i = 0; i < 32; i++) {
        char hex_str[8];
        snprintf(hex_str, sizeof(hex_str), "%02X ", config_buf[i]);
        HAL_UART_Transmit(&huart2, (uint8_t*)hex_str, 3, 100);
    }
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
    
    /* Test 1: Send AT VR (firmware version) command */
    HAL_UART_Transmit(&huart2, (uint8_t*)"1. Testing AT VR command...\r\n", 29, 100);
    uint8_t at_vr_frame[] = {0x7E, 0x00, 0x04, 0x08, 0x01, 'V', 'R', 0x70};
    
    xbee_select();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi3, at_vr_frame, sizeof(at_vr_frame), 1000);
    xbee_deselect();
    
    HAL_Delay(50);
    
    /* Read response */
    uint8_t rx_buf[64] = {0};
    xbee_select();
    for (int i = 0; i < sizeof(rx_buf); i++) {
        uint8_t dummy = 0x00;
        HAL_SPI_TransmitReceive(&hspi3, &dummy, &rx_buf[i], 1, 10);
    }
    xbee_deselect();
    
    /* Check for valid response (should start with 0x7E) */
    int response_found = 0;
    for (int i = 0; i < 32; i++) {
        if (rx_buf[i] == 0x7E && rx_buf[i+3] == 0x88) {  /* AT Response frame */
            response_found = 1;
            char msg[100];
            snprintf(msg, sizeof(msg), "   AT Response found at byte %d: Status=0x%02X\r\n", i, rx_buf[i+7]);
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
            break;
        }
    }
    
    if (!response_found) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"   ERROR: No AT response - XBee may not be in SPI mode\r\n", 57, 100);
    }
    
    /* Test 2: Send simple RF transmission */
    HAL_UART_Transmit(&huart2, (uint8_t*)"2. Testing RF transmission...\r\n", 31, 100);
    const char* test_msg = "TEST";
    (void)xbee_transmit_minimal(test_msg, 4); /* Suppress unused variable warning */
    
    /* Test 3: Check ATTn pin behavior */
    HAL_UART_Transmit(&huart2, (uint8_t*)"3. Checking ATTn pin...\r\n", 25, 100);
    int attn_readings[10];
    for (int i = 0; i < 10; i++) {
        attn_readings[i] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12);
        HAL_Delay(10);
    }
    
    char attn_msg[100];
    snprintf(attn_msg, sizeof(attn_msg), "   ATTn readings: %d %d %d %d %d %d %d %d %d %d\r\n", 
             attn_readings[0], attn_readings[1], attn_readings[2], attn_readings[3], attn_readings[4],
             attn_readings[5], attn_readings[6], attn_readings[7], attn_readings[8], attn_readings[9]);
    HAL_UART_Transmit(&huart2, (uint8_t*)attn_msg, strlen(attn_msg), 100);
    
    /* Test 4: Print full RX buffer for debugging */
    HAL_UART_Transmit(&huart2, (uint8_t*)"4. Full RX buffer dump:\r\n", 25, 100);
    for (int line = 0; line < 4; line++) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"   ", 3, 100);
        for (int col = 0; col < 16; col++) {
            char hex_str[8];
            snprintf(hex_str, sizeof(hex_str), "%02X ", rx_buf[line*16 + col]);
            HAL_UART_Transmit(&huart2, (uint8_t*)hex_str, 3, 100);
        }
        HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
    }
    
    HAL_UART_Transmit(&huart2, (uint8_t*)"=== DIAGNOSTICS COMPLETE ===\r\n", 31, 100);
    return status;
}

/* Public wrapper functions */
HAL_StatusTypeDef FEB_xbee_transmit(const char *message) {
    if (message == NULL) {
        return HAL_ERROR;
    }
    return xbee_transmit_minimal(message, (uint8_t)strlen(message));
}

HAL_StatusTypeDef FEB_xbee_transmit_raw(const char *data, uint8_t data_len) {
    return xbee_transmit_minimal(data, data_len);
}

/* Simple test with known good data */
HAL_StatusTypeDef FEB_xbee_simple_test(void) {
    HAL_UART_Transmit(&huart2, (uint8_t*)"=== SIMPLE XBEE TEST ===\r\n", 27, 100);
    
    /* Send a very simple known message repeatedly */
    for (int i = 0; i < 5; i++) {
        char test_msg[20];
        snprintf(test_msg, sizeof(test_msg), "TEST_%d", i);
        
        HAL_StatusTypeDef status = xbee_transmit_minimal(test_msg, strlen(test_msg));
        
        if (status == HAL_OK) {
            char debug_msg[50];
            snprintf(debug_msg, sizeof(debug_msg), "Test %d: SENT '%s'\r\n", i, test_msg);
            HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), 100);
        } else {
            HAL_UART_Transmit(&huart2, (uint8_t*)"Test FAILED\r\n", 13, 100);
        }
        
        HAL_Delay(1000); /* 1 second between tests */
    }
    
    HAL_UART_Transmit(&huart2, (uint8_t*)"=== TEST COMPLETE ===\r\n", 24, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"Check XCTU for 'TEST_0', 'TEST_1', etc.\r\n", 42, 100);
    
    return HAL_OK;
}

/* Switch XBee back to UART mode for XCTU access (preserves network settings) */
HAL_StatusTypeDef FEB_xbee_switch_to_uart_mode(void) {
    HAL_UART_Transmit(&huart2, (uint8_t*)"=== SWITCHING XBEE TO UART MODE ===\r\n", 38, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"NOTE: Network settings (CH, ID, etc.) will be preserved\r\n", 57, 100);
    
    /* Send AT commands to switch DIO pins back to default UART mode */
    /* This only changes pin functions, NOT network settings */
    
    /* P0 (DIO0) = 0 (disabled/default UART) - corrected checksum */
    uint8_t at_p0_frame[] = {0x7E, 0x00, 0x05, 0x08, 0x10, 'P', '0', 0x00, 0x67};
    xbee_select();
    HAL_Delay(1);
    HAL_StatusTypeDef status1 = HAL_SPI_Transmit(&hspi3, at_p0_frame, sizeof(at_p0_frame), 1000);
    HAL_Delay(1);
    xbee_deselect();
    HAL_Delay(100);
    
    /* P1 (DIO1) = 0 (disabled/default UART) - corrected checksum */
    uint8_t at_p1_frame[] = {0x7E, 0x00, 0x05, 0x08, 0x11, 'P', '1', 0x00, 0x65};
    xbee_select();
    HAL_Delay(1);
    HAL_StatusTypeDef status2 = HAL_SPI_Transmit(&hspi3, at_p1_frame, sizeof(at_p1_frame), 1000);
    HAL_Delay(1);
    xbee_deselect();
    HAL_Delay(100);
    
    /* P2 (DIO2) = 0 (disabled/default UART) - corrected checksum */
    uint8_t at_p2_frame[] = {0x7E, 0x00, 0x05, 0x08, 0x12, 'P', '2', 0x00, 0x63};
    xbee_select();
    HAL_Delay(1);
    HAL_StatusTypeDef status3 = HAL_SPI_Transmit(&hspi3, at_p2_frame, sizeof(at_p2_frame), 1000);
    HAL_Delay(1);
    xbee_deselect();
    HAL_Delay(100);
    
    /* P3 (DIO3) = 0 (disabled/default UART) - corrected checksum */
    uint8_t at_p3_frame[] = {0x7E, 0x00, 0x05, 0x08, 0x13, 'P', '3', 0x00, 0x61};
    xbee_select();
    HAL_Delay(1);
    HAL_StatusTypeDef status4 = HAL_SPI_Transmit(&hspi3, at_p3_frame, sizeof(at_p3_frame), 1000);
    HAL_Delay(1);
    xbee_deselect();
    HAL_Delay(100);
    
    /* Write settings to flash with WR command - corrected checksum */
    uint8_t at_wr_frame[] = {0x7E, 0x00, 0x04, 0x08, 0x14, 'W', 'R', 0x5E};
    xbee_select();
    HAL_StatusTypeDef status5 = HAL_SPI_Transmit(&hspi3, at_wr_frame, sizeof(at_wr_frame), 1000);
    xbee_deselect();
    HAL_Delay(500);
    
    if (status1 == HAL_OK && status2 == HAL_OK && status3 == HAL_OK && status4 == HAL_OK && status5 == HAL_OK) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"XBee switch to UART commands sent successfully\r\n", 49, 100);
        HAL_UART_Transmit(&huart2, (uint8_t*)"Network settings (CH, ID, AP) are PRESERVED\r\n", 46, 100);
        HAL_UART_Transmit(&huart2, (uint8_t*)"Reset XBee to apply changes, then:\r\n", 36, 100);
        HAL_UART_Transmit(&huart2, (uint8_t*)"  1. Connect XBee DIN  -> PB0 (MOSI pin)\r\n", 42, 100);
        HAL_UART_Transmit(&huart2, (uint8_t*)"  2. Connect XBee DOUT -> PC11 (MISO pin)\r\n", 43, 100);
        HAL_UART_Transmit(&huart2, (uint8_t*)"  3. Use 9600 baud in XCTU\r\n", 28, 100);
        HAL_UART_Transmit(&huart2, (uint8_t*)"  4. Same CH and ID values should be visible\r\n", 46, 100);
    } else {
        HAL_UART_Transmit(&huart2, (uint8_t*)"ERROR: Failed to send UART switch commands\r\n", 45, 100);
        HAL_UART_Transmit(&huart2, (uint8_t*)"May not work if AT commands over SPI are failing\r\n", 51, 100);
    }
    
    HAL_UART_Transmit(&huart2, (uint8_t*)"===================================\r\n", 38, 100);
    
    return (status1 == HAL_OK && status2 == HAL_OK && status3 == HAL_OK && status4 == HAL_OK && status5 == HAL_OK) ? HAL_OK : HAL_ERROR;
}

/* UART fallback test - if XBee is in UART mode */
void FEB_xbee_uart_fallback_test(void) {
    HAL_UART_Transmit(&huart2, (uint8_t*)"=== UART FALLBACK TEST ===\r\n", 29, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"If XBee is in UART mode, try connecting:\r\n", 42, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"  XBee DIN  -> PB0 (current MOSI)\r\n", 35, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"  XBee DOUT -> PC11 (current MISO)\r\n", 36, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"  Baud rate: 9600 (default)\r\n", 29, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"Call FEB_xbee_switch_to_uart_mode() to switch back\r\n", 53, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"OR configure XBee for SPI mode using XCTU:\r\n", 45, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"  P0 (DIO0) = 1 (SPI_MISO)\r\n", 28, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"  P1 (DIO1) = 4 (SPI_nSS)\r\n", 27, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"  P2 (DIO2) = 5 (SPI_MOSI)\r\n", 28, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"  P3 (DIO3) = 6 (SPI_CLK)\r\n", 27, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"==============================\r\n", 33, 100);
}
