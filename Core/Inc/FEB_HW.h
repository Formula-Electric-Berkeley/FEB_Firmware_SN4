#ifndef INC_FEB_HW_H_
#define INC_FEB_HW_H_

// ******************************** Includes ***********************************

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "FEB_Const.h"

// ******************************** GPIO MAP ***********************************
typedef struct {
	GPIO_TypeDef*group;
	uint16_t pin;
}FEB_GPIO;

//Pin codes
#define P_PA0 (FEB_GPIO){GPIOA,GPIO_PIN_0}
#define P_PB0 (FEB_GPIO){GPIOB,GPIO_PIN_0}
#define P_PB1 (FEB_GPIO){GPIOB,GPIO_PIN_1}
#define P_PB5 (FEB_GPIO){GPIOB,GPIO_PIN_5}
#define P_PB6 (FEB_GPIO){GPIOB,GPIO_PIN_6}
#define P_PB7 (FEB_GPIO){GPIOB,GPIO_PIN_7}
#define P_PC0 (FEB_GPIO){GPIOC,GPIO_PIN_0}
#define P_PC1 (FEB_GPIO){GPIOC,GPIO_PIN_1}
#define P_PC2 (FEB_GPIO){GPIOC,GPIO_PIN_2}
#define P_PC3 (FEB_GPIO){GPIOC,GPIO_PIN_3}
#define P_PC4 (FEB_GPIO){GPIOC,GPIO_PIN_4}
#define P_PC5 (FEB_GPIO){GPIOC,GPIO_PIN_5}
#define P_PC10 (FEB_GPIO){GPIOC,GPIO_PIN_10}
#define P_PC11 (FEB_GPIO){GPIOC,GPIO_PIN_11}
#define P_PC12 (FEB_GPIO){GPIOC,GPIO_PIN_12}
#define P_PC13 (FEB_GPIO){GPIOC,GPIO_PIN_13}
#define P_PD2 (FEB_GPIO){GPIOD,GPIO_PIN_0}

//Pin names
#define PN_BUZZER P_PA0
#define PN_WAKE P_PB0
#define PN_INTR P_PB1
#define PN_RST P_PB5
#define PN_PG P_PB6
#define PN_ALERT P_PB7
#define PN_BMS_IND P_PC0
#define PN_BMS_A P_PC1
#define PN_PC_AIR P_PC2
#define PN_CS P_PC3
#define PN_AIRM_SENSE P_PC4
#define PN_AIRP_SENSE P_PC5
#define PN_SHS_IMD P_PC10
#define PN_SHS_TSMS P_PC11
#define PN_SHS_IN P_PC12
#define PN_INDICATOR P_PC13
#define PN_PC_REL P_PD2

// ******************************** SPI ****************************************

void FEB_delay_u(uint16_t micro);
void FEB_delay_m(uint16_t milli);
void FEB_cs_low(void);
void FEB_cs_high(void);
void FEB_spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, size_t rx_len);
void FEB_spi_write_array(uint16_t, uint8_t[]);
uint8_t FEB_spi_read_byte(uint8_t tx_data);
void FEB_Siren_Activate();

// ******************************** Relay Control ********************************
void FEB_PIN_RST(FEB_GPIO);
void FEB_PIN_SET(FEB_GPIO);
void FEB_PIN_TGL(FEB_GPIO);
FEB_Relay_State FEB_PIN_RD(FEB_GPIO);

#endif /* INC_FEB_HW_H_ */
