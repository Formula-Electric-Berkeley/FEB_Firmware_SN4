#ifndef INC_FEB_ADBMS6830B_H_
#define INC_FEB_ADBMS6830B_H_

// ******************************** Includes ********************************

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "FEB_ADBMS6830B_Driver.h"
#include "FEB_Const.h"

// ********************************** ADBMS6830B Configuration **********************************

typedef enum {
	RD_OFF = 0x00,
	RD_ON
} RD;

typedef enum {
	DCP_OFF = 0x00,
	DCP_ON
} DCP;

typedef enum {
	SINGLE = 0x00,
	CONTINUOUS
} CONT;


typedef enum {
	RSTF_OFF = 0x00,
	RSTF_ON
} RSTF;

typedef enum {
	OW_OFF_ALL_CH = 0x00,
	OW_ON_EVEN_CH,
	OW_ON_ODD_CH,
	OW_ON_ALL_CH,
} OW;

typedef enum {
	AUX_OW_OFF = 0x00,
	AUX_OW_ON
} AUX_OW;

typedef enum {
	PUP_DOWN = 0x00,
	PUP_UP
} PUP;

typedef enum {
	AUX_ALL = 0x00,
	GPIO1,
	GPIO2,
	GPIO3,
	GPIO4,
	GPIO5,
	GPIO6,
	GPIO7,
	GPIO8,
	GPIO9,
	GPIO10,
	VREF2,
	VD,
	VA,
	ITEMP,
	VPV,
	VMV,
	VRES
} AUX_CH;

// ******************************** Read Config ***************************
#define CONTVR CONTINUOUS
#define DCPVR DCP_OFF
#define RSTFVR RSTF_OFF
#define OWVR OW_OFF_ALL_CH
#define RDVR RD_OFF
// ******************************** Struct ********************************

typedef struct {
	float voltage_V;
} cell_t;

typedef struct {
	float total_voltage_V;
	float temp_sensor_readings_V[FEB_NUM_TEMP_SENSE_PER_BANK];
	cell_t cells[FEB_NUM_CELLS_PER_BANK];
} bank_t;

typedef struct {
	float total_voltage_V;
	bool balance_done;
	bank_t banks[FEB_NUM_BANKS];
	cell_asic IC_Config[FEB_NUM_IC];
} accumulator_t;

// ******************************** Functions ********************************

void FEB_ADBMS_Init();
void FEB_ADBMS_AcquireData();

// ******************************** Voltage ********************************

void start_adc_cell_voltage_measurements();
void read_cell_voltages();
void store_cell_voltages();
void validate_voltages();

// ******************************** Temperature ********************************

void configure_gpio_bits(uint8_t channel);
void start_aux_voltage_measurements();
void read_aux_voltages();
void store_cell_temps(uint8_t channel);

// ******************************** UART ********************************

void FEB_ADBMS_UART_Transmit();

#endif /* INC_FEB_ADBMS6830B_H_ */
