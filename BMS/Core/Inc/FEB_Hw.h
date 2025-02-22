#ifndef INC_FEB_HW_H_
#define INC_FEB_HW_H_

// ******************************** Includes ***********************************

#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"
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
void FEB_spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len);
void FEB_spi_write_array(uint8_t, uint8_t[]);
uint8_t FEB_spi_read_byte(uint8_t tx_data);

// ******************************** Relay Control ********************************
void FEB_PIN_RST(FEB_GPIO);
void FEB_PIN_SET(FEB_GPIO);
void FEB_PIN_TGL(FEB_GPIO);
FEB_Relay_State FEB_PIN_RD(FEB_GPIO);
// ******************************** Struct ********************************

typedef struct {
	float voltage_V;
	float voltage_S;
	uint8_t dischargeAmount;
} cell_t;

typedef struct {
	float total_voltage_V;
	float temp_sensor_readings_V[FEB_NUM_TEMP_SENSE_PER_BANK];
	cell_t cells[FEB_NUM_CELLS_PER_BANK];
} bank_t;
/*! Cell Voltage data structure. */
typedef struct
{
  uint16_t c_codes[18]; //!< Cell Voltage Codes
  uint16_t s_codes[18]; //!< Cell Voltage Codes
  uint8_t pec_match[6]; //!< If a PEC error was detected during most recent read cmd
} cv;

/*! AUX Reg Voltage Data structure */
typedef struct
{
  uint16_t a_codes[10]; //!< Aux Voltage Codes
  uint8_t pec_match[4]; //!< If a PEC error was detected during most recent read cmd
} ax;

/*! Status Reg data structure. */
typedef struct
{
  uint16_t stat_codes[4]; //!< Status codes.
  uint8_t flags[3]; //!< Byte array that contains the uv/ov flag data
  uint8_t mux_fail[1]; //!< Mux self test status flag
  uint8_t thsd[1]; //!< Thermal shutdown status
  uint8_t pec_match[2]; //!< If a PEC error was detected during most recent read cmd
} st;

/*! IC register structure. */
typedef struct
{
  uint8_t tx_data[6];  //!< Stores data to be transmitted
  uint8_t rx_data[8];  //!< Stores received data
  uint8_t rx_pec_match; //!< If a PEC error was detected during most recent read cmd
} ic_register;

/*! PEC error counter structure. */
typedef struct
{
  uint16_t pec_count; //!< Overall PEC error count
  uint16_t cfgr_pec;  //!< Configuration register data PEC error count
  uint16_t cell_pec[6]; //!< Cell voltage register data PEC error count
  uint16_t aux_pec[4];  //!< Aux register data PEC error count
  uint16_t stat_pec[2]; //!< Status register data PEC error count
} pec_counter;

/*! Register configuration structure */
typedef struct
{
  uint8_t cell_channels; //!< Number of Cell channels
  uint8_t stat_channels; //!< Number of Stat channels
  uint8_t aux_channels;  //!< Number of Aux channels
  uint8_t num_cv_reg;    //!< Number of Cell voltage register
  uint8_t num_gpio_reg;  //!< Number of Aux register
  uint8_t num_stat_reg;  //!< Number of  Status register
} register_cfg;

/*! Cell variable structure */
typedef struct
{
  ic_register configa;
  ic_register configb;
  cv  cells;
  ax  aux;
  st  stat;
  ic_register com;
  ic_register pwm;
  ic_register pwmb;
  ic_register sctrl;
  ic_register sctrlb;
  uint8_t sid[6];
  bool isospi_reverse;
  pec_counter crc_count;
  register_cfg ic_reg;
  long system_open_wire;
} cell_asic;

typedef struct {
	float total_voltage_V;
	bank_t banks[FEB_NUM_BANKS];
	cell_asic IC_Config[FEB_NUM_IC];
} accumulator_t;

//ACC STATE
static accumulator_t FEB_ACC = {0};
#endif /* INC_FEB_HW_H_ */
