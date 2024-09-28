// ******************************** Includes ********************************

#include <stdbool.h>
#include "FEB_ADBMS6830B.h"
#include "FEB_ADBMS6830B_Driver.h"
#include "FEB_Const.h"

// ******************************** Struct ********************************

typedef struct {
	float voltage_V;
	float temp_C[2];
} cell_t;

typedef struct {
	float total_voltage_V;
	cell_t cells[FEB_NUM_CELLS_PER_BANK];
} bank_t;

typedef struct {
	float total_voltage_V;
	bool balance_done;
	bank_t banks[FEB_NUM_BANKS];
	cell_asic IC_Config[FEB_NUM_IC];
} accumulator_t;

// ******************************** Global Variabls ********************************

accumulator_t accumulator = {0};

// ******************************** Config Bits ********************************

static bool refon = 1;
static bool cth_bits[3] = {0, 0, 1};
static bool gpio_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static bool dcc_bits[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static bool dcto_bits[10] = {0, 0, 0, 0};
static uint16_t uv = 0x800;
static uint16_t ov = 0x7FF;

// ******************************** Temp Mapping ********************************

/*
Sensor (- terminal)				00	01	02	03	04	05	06	07	08	09	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31
Multiplexer - Sensor Layer		0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1
Multiplexer - Chip Layer		0	0	0	0	0	0	0	0	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	1	1	1	1	1	1	1	1
Channel							0	1	2	3	4	5	6	7	0	1	2	3	4	5	6	7	0	1	2	3	4	5	6	7	0	1	2	3	4	5	6	7
*/


// ******************************** Helper Functions ********************************

static uint8_t get_cell(uint8_t sensor) {
	return sensor / 2;
}

static uint8_t get_channel(uint8_t sensor) {
	return sensor % 8;
}

static uint8_t get_mux(uint8_t sensor) {
	if (sensor >= 0 && sensor <= 15) {
		return 0;
	} else if (sensor >= 16 && sensor <= 31) {
		return 1;
	} else {
		return -1; /* Error */
	}
}

static float convert_voltage(uint16_t raw_code) {
	return raw_code * 0.0001;
}

// ******************************** Functions ********************************

void FEB_ADBMS_Init() {
	ADBMS6830B_init_cfg(FEB_NUM_IC, accumulator.IC_Config);
	for (uint8_t ic; ic < FEB_NUM_IC; ic++) {
		ADBMS6830B_set_cfgr(ic, accumulator.IC_Config, refon, cth_bits, gpio_bits, dcc_bits, dcto_bits, uv, ov);
	}
	ADBMS6830B_reset_crc_count(FEB_NUM_IC, accumulator.IC_Config);
	ADBMS6830B_init_reg_limits(FEB_NUM_IC, accumulator.IC_Config);
}

void FEB_ADBMS_AcquireData() {

	/* Voltage */
	start_adc_cell_voltage_measurements();
	read_cell_voltages();
	store_cell_voltages();
	validate_voltages();

	/* Temperature */
	for (uint8_t mux = 0; mux < 2; mux++) {
		for (uint8_t channel = 0; channel < 8; channel++) {
			configure_gpio_bits(mux, channel);
			start_aux_voltage_measurements();
			read_aux_voltages();
			store_cell_temps(mux, channel);
		}
	}

}

// ******************************** Voltage ********************************

void start_adc_cell_voltage_measurements() {
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_adcv(RD_ON, DCP_ON, CONTINUOUS, RSTF_OFF, OW_OFF_ALL_CH);
	ADBMS6830B_pollAdc();
}

void read_cell_voltages() {
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_rdcv(FEB_NUM_IC, accumulator.IC_Config);
}

void store_cell_voltages() {
	accumulator.total_voltage_V = 0;
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank ++) {
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_BANK; cell ++) {
			uint8_t ic = bank;
			float actual_voltage = convert_voltage(accumulator.IC_Config->cells.c_codes[ic]);
			accumulator.banks[bank].cells[cell].voltage_V = actual_voltage;
		}
	}
}

void validate_voltages() {
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank ++) {
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_BANK; cell ++) {
			float voltage = accumulator.banks[bank].cells[cell].voltage_V;
			if (voltage > FEB_CELL_MAX_VOLT || voltage < FEB_CELL_MIN_VOLT) {
				/* Some error handling */
			}
		}
	}
}

// ******************************** Temperature ********************************



void configure_gpio_bits(uint8_t mux, uint8_t channel) {
	gpio_bits[0] = 0b1;
	gpio_bits[1] = 0b1;
	gpio_bits[2] = (channel >> 0) & 0b1;
	gpio_bits[3] = (channel >> 1) & 0b1;
	gpio_bits[4] = (channel >> 2) & 0b1;
	gpio_bits[5] = mux & 0b1;
	for (uint8_t ic = 0; ic < FEB_NUM_IC; ic++) {
		ADBMS6830B_set_cfgr(ic, accumulator.IC_Config, refon, cth_bits, gpio_bits, dcc_bits, dcto_bits, uv, ov);
	}
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_wrcfga(FEB_NUM_IC, accumulator.IC_Config);
	ADBMS6830B_wrcfgb(FEB_NUM_IC, accumulator.IC_Config);

}

void start_aux_voltage_measurements() {
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_adax(AUX_OW_OFF, PUP_DOWN, AUX_ALL);
	ADBMS6830B_pollAdc();
}

void read_aux_voltages() {
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_rdaux(FEB_NUM_IC, accumulator.IC_Config);
}

void store_cell_temps(uint8_t mux, uint8_t channel) {
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank ++) {
		for (uint8_t sensor = 0; sensor < FEB_NUM_TEMP_SENSE_PER_BANK; sensor++) {
			if (get_mux(sensor) == mux && get_channel(sensor) == channel) {
				uint16_t raw_voltage = accumulator.IC_Config->aux.a_codes[mux];
				uint8_t cell = get_cell(sensor);
				if (sensor % 2 == 0) {
					accumulator.banks[bank].cells[cell].temp_C[0] = raw_voltage; /* Need to figure out conversion */
				} else {
					accumulator.banks[bank].cells[cell].temp_C[1] = raw_voltage; /* Need to figure out conversion */
				}
			}
		}
	}
}

//Functions Go Here
