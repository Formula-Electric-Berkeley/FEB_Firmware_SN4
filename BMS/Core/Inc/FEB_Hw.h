#ifndef INC_FEB_HW_H_
#define INC_FEB_HW_H_

// ******************************** Includes ********************************

#include <stdint.h>

// ******************************** Relay States ********************************

typedef enum {
	FEB_RELAY_STATE_CLOSE,
	FEB_RELAY_STATE_OPEN
} FEB_Relay_State_t;

// ******************************** SPI ********************************

void FEB_delay_u(uint16_t micro);
void FEB_delay_m(uint16_t milli);
void FEB_cs_low(void);
void FEB_cs_high(void);
void FEB_spi_write_array(uint8_t len, uint8_t data[]);
void FEB_spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len);
uint8_t FEB_spi_read_byte(uint8_t tx_data);

// ******************************** Relay Control ********************************

void FEB_Hw_Shutdown_Close(void);
void FEB_Hw_Shutdown_Open(void);
void FEB_Hw_AIR_Plus_Close(void);
void FEB_Hw_AIR_Plus_Open(void);
void FEB_Hw_Precharge_Close(void);
void FEB_Hw_Precharge_Open(void);

// ******************************** Relay State ********************************

FEB_Relay_State_t FEB_Relay_Shutdown_State(void);
FEB_Relay_State_t FEB_Relay_AIR_Plus_State(void);
FEB_Relay_State_t FEB_Relay_Precharge_State(void);


#endif /* INC_FEB_HW_H_ */
