#include "FEB_ADBMS6830B.h"

// ******************************** Global Variabls ********************************

accumulator_t accumulator = {0};
extern UART_HandleTypeDef huart2;

// ******************************** Config Bits ********************************

static bool refon = 1;
static bool cth_bits[3] = {0, 0, 1};
static bool gpio_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static bool dcc_bits[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static bool dcto_bits[10] = {0, 0, 0, 0};
static uint8_t gpio_map[4] = {0,1,5,6};
static uint16_t uv = 0x800;
static uint16_t ov = 0x7FF;


// ******************************** Temp Mapping ********************************

/*
Sensor (- terminal)				00	01	02	03	04	05	06	07	08	09	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31
Multiplexer 					0	0	0	0	0	0	0	0	1	1	1	1	1	1	1	1	2	2	2	2	2	2	2	2	3	3	3	3	3	3	3	3
Channel							0	1	2	3	4	5	6	7	0	1	2	3	4	5	6	7	0	1	2	3	4	5	6	7	0	1	2	3	4	5	6	7
*/


// ******************************** Helper Functions ********************************


static uint8_t get_gpio_pin(uint8_t mux) {
	if(mux<0||mux>3)return -1;

	return gpio_map[mux];
}

static uint8_t get_sensor(uint8_t mux, uint8_t channel) {
	return mux * 8 + channel;
}

static float convert_voltage(uint16_t raw_code) {
	return raw_code * 0.000150f - 1.5f;
}

// ******************************** Functions ********************************

void FEB_ADBMS_Init() {
	FEB_cs_high();
	ADBMS6830B_init_cfg(FEB_NUM_IC, accumulator.IC_Config);
	for (uint8_t ic = 0; ic < FEB_NUM_IC; ic++) {
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
	//validate_voltages();

	/* Temperature */
	for (uint8_t channel = 0; channel < 8; channel++) {
		configure_gpio_bits(channel);
		start_aux_voltage_measurements();
		read_aux_voltages();
		store_cell_temps(channel);
	}

}

// ******************************** Voltage ********************************

void start_adc_cell_voltage_measurements() {
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_adcv(RDVR, DCPVR, CONTVR, RSTFVR, OWVR);
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
			float actual_voltage = convert_voltage(accumulator.IC_Config[bank].cells.c_codes[ic]);
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



void configure_gpio_bits(uint8_t channel) {
	gpio_bits[0] = 0b1; /* ADC Channel */
	gpio_bits[1] = 0b1; /* ADC Channel */
	gpio_bits[2] = (channel >> 0) & 0b1; /* MUX Sel 1 */
	gpio_bits[3] = (channel >> 1) & 0b1; /* MUX Sel 1 */
	gpio_bits[4] = (channel >> 2) & 0b1; /* MUX Sel 1 */
	gpio_bits[5] = 0b1; /* ADC Channel */
	gpio_bits[6] = 0b1; /* ADC Channel */
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

void store_cell_temps(uint8_t channel) {
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank++) {
		for (uint8_t mux = 0; mux < 4; mux++) {
			uint8_t gpio = get_gpio_pin(mux);
			uint16_t raw_code = accumulator.IC_Config[bank].aux.a_codes[gpio];
			uint8_t sensor = get_sensor(mux, channel);
			accumulator.banks[bank].temp_sensor_readings_V[sensor] = convert_voltage(raw_code);
		}
	}
}

void FEB_ADBMS_UART_Transmit() {
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank++) {
		char UART_head[256];
		char UART_str[256];
		char UART_temp[256];
		int offset[3];
		offset[0]=sprintf((char*)UART_head,"|Bnk %d|",bank);
		offset[1]=sprintf((char*)UART_str,"|Vlt  |");
		offset[2]=sprintf((char*)UART_temp,"|Temp |");


		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_BANK; cell++) {
			offset[0]+=sprintf((char*)(UART_head + offset[0]), (cell>=10)?"Cell  %d|":"Cell   %d|",cell);
			offset[1]+=sprintf((char*)(UART_str + offset[1]), "%.6f|",accumulator.banks[bank].cells[cell].voltage_V);
			offset[2]+=sprintf((char*)(UART_temp + offset[2]), "%.6f|",accumulator.banks[bank].temp_sensor_readings_V[cell]);
		}
		offset[0]+=sprintf((char*)(UART_head + offset[0]), "\n\r");
		offset[1]+=sprintf((char*)(UART_str + offset[1]), "\n\r");
		offset[2]+=sprintf((char*)(UART_temp + offset[2]), "\n\r\n\r\n\r");
		HAL_UART_Transmit(&huart2, (uint8_t*) UART_head, offset[0]+1, 100);
		HAL_UART_Transmit(&huart2, (uint8_t*) UART_str, offset[1]+1, 100);
		HAL_UART_Transmit(&huart2, (uint8_t*) UART_temp, offset[2]+1, 100);
	}
}


