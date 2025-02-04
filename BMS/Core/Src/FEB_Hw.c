// ******************************** Includes & External ********************************

#include <FEB_HW.h>
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
	int a=0;
	while(micro--)a=micro;
}

void FEB_delay_m(uint16_t milli) {
	HAL_Delay(milli);
}

void FEB_cs_low() {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
}

void FEB_cs_high() {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}

void FEB_spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len) {
	if(HAL_SPI_Transmit(&hspi1,tx_Data,tx_len,HAL_MAX_DELAY) != HAL_OK){
		//catch error
	}
	if(HAL_SPI_Receive(&hspi1,rx_data,rx_len,HAL_MAX_DELAY)!= HAL_OK){
		//catch error
	}
	return;
}

// ******************************** Relay Control ********************************
void FEB_PIN_RST(FEB_GPIO PinOut){
	HAL_GPIO_WritePin(PinOut.group, PinOut.pin, GPIO_PIN_RESET);
}
void FEB_PIN_SET(FEB_GPIO PinOut){
	HAL_GPIO_WritePin(PinOut.group, PinOut.pin, GPIO_PIN_SET);
}
void FEB_PIN_TGL(FEB_GPIO PinOut){
	HAL_GPIO_TogglePin(PinOut.group, PinOut.pin);
}
GPIO_PinState FEB_PIN_RD(FEB_GPIO PinOut){
	return HAL_GPIO_ReadPin(PinOut.group, PinOut.pin);
}
