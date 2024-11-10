/*
 * FEB_XBee.h
 *
 *  Created on: Jun 1, 2024
 *      Author: sshashi
 */

#ifndef INC_FEB_XBEE_H_
#define INC_FEB_XBEE_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"


// SPI
void cs_low(uint8_t pin);
void cs_high(uint8_t pin);
void set_spi_freq(void);
void spi_write_array(uint8_t len, uint8_t data[]);
void spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len);
uint8_t spi_read_byte(uint8_t tx_dat);

#endif /* INC_FEB_XBEE_H_ */
