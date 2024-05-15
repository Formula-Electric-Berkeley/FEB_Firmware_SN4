// ********************************** Includes & External **********************************

#include "bms_hardware.h"

extern SPI_HandleTypeDef hspi1;

// ********************************** Functions **********************************

// ******************** Delay ********************

void delay_u(uint16_t micro) {
	HAL_Delay(1);
}

void delay_m(uint16_t milli) {
	HAL_Delay(milli);
}

// ******************** SPI ********************

void cs_low() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
}

void cs_high() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
}

void spi_write_array(uint8_t len, uint8_t data[]) {
	HAL_SPI_Transmit(&hspi1, data, len, 100);
}

void spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len) {
	for (uint8_t i = 0; i < tx_len; i++) {
		HAL_SPI_Transmit(&hspi1, &tx_Data[i], 1, 100);
	}

	for (uint8_t i = 0; i < rx_len; i++) {
		HAL_SPI_Receive(&hspi1, &rx_data[i], 1, 100);
	}
}

uint8_t spi_read_byte(uint8_t tx_dat) {
	uint8_t data;
	HAL_SPI_Receive(&hspi1, &data, 1, 100);
	return data;
}