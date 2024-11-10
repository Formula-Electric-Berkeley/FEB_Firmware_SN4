/*
 * FEB_XBee.c
 *
 *  Created on: Jun 1, 2024
 *      Author: sshashi
 */

#include "FEB_XBee.h"

extern SPI_HandleTypeDef hspi2;

void cs_low(uint8_t pin) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
}

void cs_high(uint8_t pin) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
}

void spi_write_array(uint8_t len, uint8_t data[]) {
	HAL_SPI_Transmit(&hspi2, data, len, 100);
}

void spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len) {
	for (uint8_t i = 0; i < tx_len; i++) {
		HAL_SPI_Transmit(&hspi2, &tx_Data[i], 1, 100);
	}

	for (uint8_t i = 0; i < rx_len; i++) {
		HAL_SPI_Receive(&hspi2, &rx_data[i], 1, 100);
	}
}

uint8_t spi_read_byte(uint8_t tx_dat) {
	uint8_t data;
	HAL_SPI_Receive(&hspi2, &data, 1, 100);
	return data;
}
