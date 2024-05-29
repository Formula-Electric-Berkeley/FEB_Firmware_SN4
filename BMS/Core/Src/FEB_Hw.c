// ******************************** Includes & External ********************************

#include "FEB_Hw.h"
#include "stm32f4xx_hal.h"

extern SPI_HandleTypeDef hspi1;

// ******************************** Static ********************************

typedef struct {
	FEB_Relay_State_t shutdown;
	FEB_Relay_State_t AIR_plus;
	FEB_Relay_State_t precharge;
} relay_status_t;

static relay_status_t relay_status;
static uint8_t shutdown_close_guard = 0; //guard for shutdown relay

// ******************************** SPI ********************************

void FEB_delay_u(uint16_t micro) {
	HAL_Delay(1);
}

void FEB_delay_m(uint16_t milli) {
	HAL_Delay(milli);
}

void FEB_cs_low() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
}

void FEB_cs_high() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
}

void FEB_spi_write_array(uint8_t len, uint8_t data[]) {
	HAL_SPI_Transmit(&hspi1, data, len, 100);
}

void FEB_spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len) {
	for (uint8_t i = 0; i < tx_len; i++) {
		HAL_SPI_Transmit(&hspi1, &tx_Data[i], 1, 100);
	}

	for (uint8_t i = 0; i < rx_len; i++) {
		HAL_SPI_Receive(&hspi1, &rx_data[i], 1, 100);
	}
}

uint8_t FEB_spi_read_byte(uint8_t tx_data) {
	uint8_t data;
	HAL_SPI_Receive(&hspi1, &data, 1, 100);
	return data;
}

// ******************************** Relay Control ********************************

void FEB_Hw_Shutdown_Close() {

}

void FEB_Hw_Shutdown_Open() {

}

void FEB_Hw_AIR_Plus_Close() {

}

void FEB_Hw_AIR_Plus_Open() {

}

void FEB_Hw_Precharge_Close() {

}

void FEB_Hw_Precharge_Open() {

}

FEB_Relay_State_t FEB_Relay_Shutdown_State() {
	return relay_status.shutdown;
}

FEB_Relay_State_t FEB_Relay_AIR_Plus_State() {
	return relay_status.AIR_plus;
}

FEB_Relay_State_t FEB_Relay_Precharge_State() {
	return relay_status.precharge;
}


