// ******************************** Includes & External ********************************

#include "FEB_HW.h"
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
extern SPI_HandleTypeDef hspi1;

// ******************************** SPI ********************************

void FEB_delay_u(uint16_t micro) {
	int a=0;
	while(micro--)a=micro;
	a++;
}

void FEB_delay_m(uint16_t milli) {
	HAL_Delay(milli);
}
void FEB_Siren_Activate(){
	int period = 900;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		while(1){
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
			int nops=2200+period;
			while(nops-->0);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
			nops=1100+period;
			while(nops-->0);
			period-=1;
			if(period==10)period=1000;
		}

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
}
void FEB_cs_low() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
}


void FEB_cs_high() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
}
void FEB_spi_write_array(
		uint16_t size,                     /*Option: Number of bytes to be written on the SPI port*/
		uint8_t *tx_Data                       /*Array of bytes to be written on the SPI port*/
		)
		{
			HAL_SPI_Transmit(&hspi1, tx_Data, size, HAL_MAX_DELAY); /* SPI1 , data, size, timeout */
		}
uint8_t FEB_spi_read_byte(uint8_t tx_data) {
	uint8_t data;
	if(HAL_SPI_Receive(&hspi1, &data, 1, 100)!=HAL_OK){
		//catch error
	}
	return data;
}

void FEB_spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, size_t rx_len) {
	HAL_SPI_Transmit(&hspi1,tx_Data,tx_len,HAL_MAX_DELAY);
		//catch error
	if(HAL_SPI_Receive(&hspi1,rx_data,rx_len,HAL_MAX_DELAY) != HAL_OK){
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
FEB_Relay_State FEB_PIN_RD(FEB_GPIO PinOut){
	return (FEB_Relay_State)HAL_GPIO_ReadPin(PinOut.group, PinOut.pin);
}
