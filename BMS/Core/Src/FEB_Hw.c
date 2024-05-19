// ******************************** Includes ********************************

#include "FEB_Hw.h"

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

}

void FEB_delay_m(uint16_t milli) {

}

void FEB_cs_low() {

}

void FEB_cs_high() {

}

void FEB_spi_write_array(uint8_t len, uint8_t data[]) {

}

void FEB_spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len) {

}

uint8_t FEB_spi_read_byte(uint8_t tx_data) {

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


