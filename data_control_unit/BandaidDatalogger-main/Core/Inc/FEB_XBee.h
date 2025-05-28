#ifndef INC_FEB_XBEE_H_
#define INC_FEB_XBEE_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "FEB_CircularBuffer.h"

// SPI Handle for XBee
extern SPI_HandleTypeDef hspi3;

// Debug UART Handle
extern UART_HandleTypeDef huart2;

void FEB_xbee_transmit(const char *message);
void FEB_xbee_loop(void);
void FEB_xbee_transmit_can_data(circBuffer *cb);
void FEB_xbee_transmit_sd_data(void);
uint8_t FEB_xbee_receive_status();
void FEB_xbee_transmit_hello();
#endif /* INC_FEB_XBEE_H_ */
