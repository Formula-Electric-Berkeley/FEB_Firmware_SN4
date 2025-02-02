#ifndef INC_FEB_XBEE_H_
#define INC_FEB_XBEE_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

// UART for XBee
void xbee_transmit(const char *message);
void xbee_loop(void);
void xbee_transmit_can_data(circBuffer *cb);
void xbee_transmit_sd_data(void);

#endif /* INC_FEB_XBEE_H_ */
