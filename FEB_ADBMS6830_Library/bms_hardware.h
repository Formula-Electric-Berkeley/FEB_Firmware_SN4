#ifndef BMSHARDWARE_H
#define BMSHARDWARE_H

// ********************************** Includes **********************************

//#include "FEB_Timer.h"

#include "stdint.h"
//#include "stm32f4xx_hal.h"

// ********************************** Functions **********************************

// Delay
void delay_u(uint16_t micro);
void delay_m(uint16_t milli);

// SPI
void cs_low(void);
void cs_high(void);
void set_spi_freq(void);
void spi_write_array(uint8_t len, uint8_t data[]);
void spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len);
uint8_t spi_read_byte(uint8_t tx_dat);

#endif