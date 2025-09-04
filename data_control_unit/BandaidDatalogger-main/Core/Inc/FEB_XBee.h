#ifndef INC_FEB_XBEE_H_
#define INC_FEB_XBEE_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "FEB_CircularBuffer.h"

/* XBee Mode Control */
#define XBEE_STARTUP_UART_MODE 0  /* Set to 1 to switch to UART mode on startup */

/* Debug print control macros */
#define XBEE_DEBUG_ENABLED 1      /* Set to 1 to enable debug prints */
#define XBEE_VERBOSE_ENABLED 1    /* Set to 1 for extra verbose output - ENABLED for troubleshooting */

#if XBEE_DEBUG_ENABLED
    #define XBEE_DEBUG_PRINT(msg, len) HAL_UART_Transmit(&huart2, (uint8_t*)(msg), (len), 100)
#else
    #define XBEE_DEBUG_PRINT(msg, len) do { } while(0)
#endif

#if XBEE_VERBOSE_ENABLED
    #define XBEE_VERBOSE_PRINT(msg, len) HAL_UART_Transmit(&huart2, (uint8_t*)(msg), (len), 100)
#else
    #define XBEE_VERBOSE_PRINT(msg, len) do { } while(0)
#endif

// SPI Handle for XBee
extern SPI_HandleTypeDef hspi3;

// Debug UART Handle
extern UART_HandleTypeDef huart2;

HAL_StatusTypeDef FEB_xbee_transmit(const char *message);
HAL_StatusTypeDef FEB_xbee_transmit_raw(const char *data, uint8_t data_len);
void FEB_xbee_loop(void);
HAL_StatusTypeDef FEB_xbee_transmit_can_data(circBuffer *cb);
void FEB_xbee_transmit_sd_data(void);
uint8_t FEB_xbee_receive_status(void);
HAL_StatusTypeDef FEB_xbee_transmit_hello(void);
void FEB_xbee_set_destination(const uint8_t *addr_64bit);
HAL_StatusTypeDef FEB_xbee_spi_test(void);
void FEB_xbee_uart_fallback_test(void);
HAL_StatusTypeDef FEB_xbee_simple_test(void);
HAL_StatusTypeDef FEB_xbee_switch_to_uart_mode(void);
#endif /* INC_FEB_XBEE_H_ */
