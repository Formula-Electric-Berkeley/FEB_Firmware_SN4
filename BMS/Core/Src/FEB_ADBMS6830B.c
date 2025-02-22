#include "FEB_ADBMS6830B.h"
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
// ******************************** Balance ********************************
uint64_t determineDischarge();
void determineMinV();
// ******************************** Global Variabls ********************************


extern UART_HandleTypeDef huart2;
extern accumulator_t FEB_ACC;
// ******************************** Config Bits ********************************

static bool refon = 1;
static bool cth_bits[3] = {0, 0, 1};
static bool gpio_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static bool dcc_bits[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static bool dcto_bits[10] = {0, 0, 0, 0};
static uint8_t gpio_map[4] = {0,1,5,6};
static uint16_t uv = 0x800;
static uint16_t ov = 0x7FF;
float MIN_CELL_VOLTAGE=0;
float FEB_MIN_SLIPPAGE_V=0.01;
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

static float convert_voltage(int16_t raw_code) {
	return raw_code * 0.000150 + 1.5;
}

static float convert_to_temp(float voltage){
	return voltage;
}
//TODO: Convert voltage to temperature
// ******************************** Functions ********************************

void FEB_ADBMS_Init() {
	FEB_cs_high();
	ADBMS6830B_init_cfg(FEB_NUM_IC, FEB_ACC.IC_Config);
	for (uint8_t ic = 0; ic < FEB_NUM_IC; ic++) {
		ADBMS6830B_set_cfgr(ic, FEB_ACC.IC_Config, refon, cth_bits, gpio_bits, dcc_bits, dcto_bits, uv, ov);
	}
	ADBMS6830B_reset_crc_count(FEB_NUM_IC, FEB_ACC.IC_Config);
	ADBMS6830B_init_reg_limits(FEB_NUM_IC, FEB_ACC.IC_Config);
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_wrcfga(FEB_NUM_IC, FEB_ACC.IC_Config);
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_wrcfgb(FEB_NUM_IC, FEB_ACC.IC_Config);
	//TODO:read back config
	//wakeup_sleep(FEB_NUM_IC);
	//start_adc_cell_voltage_measurements();

}

void FEB_ADBMS_Voltage_Process() {
	start_adc_cell_voltage_measurements();
	read_cell_voltages();
	store_cell_voltages();
	//validate_voltages();
	FEB_ADBMS_UART_Transmit(FEB_ACC);
	/*
	for (uint8_t channel = 0; channel < 8; channel++) {
		//configure_gpio_bits(channel);
		//start_aux_voltage_measurements();
		//read_aux_voltages();
		//store_cell_temps(channel);
	}*/

}

void FEB_ADBMS_Temperature_Process(){
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
	ADBMS6830B_adcv(1, DCPVR, CONTVR, RSTFVR, OWVR);
	HAL_Delay(1);
	//ADBMS6830B_pollAdc();
}

void read_cell_voltages() {
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_rdcv(FEB_NUM_IC, FEB_ACC.IC_Config);
}

void store_cell_voltages() {
	FEB_ACC.total_voltage_V = 0;
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank ++) {
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_BANK; cell ++) {
			uint8_t ic = bank;
			float CVoltage = convert_voltage(FEB_ACC.IC_Config[bank].cells.c_codes[ic]);
			FEB_ACC.banks[bank].cells[cell].voltage_V = CVoltage;
			FEB_ACC.banks[bank].cells[cell].voltage_S = convert_voltage(FEB_ACC.IC_Config[bank].cells.s_codes[ic]);
			FEB_ACC.total_voltage_V+=CVoltage;
		}
	}
}

void validate_voltages() {
	uint16_t vMax = FEB_Config_Get_Cell_Max_Voltage_mV();
	uint16_t vMin = FEB_Config_Get_Cell_Min_Voltage_mV();
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank ++) {
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_BANK; cell ++) {
			float voltage = FEB_ACC.banks[bank].cells[cell].voltage_V;
			if (voltage > vMax || voltage < vMin) {
				//FEB_SM_Transition(FEB_SM_ST_FAULT_BMS);
			}
		}
	}
}
void configure_gpio_bits(uint8_t channel) {
	gpio_bits[0] = 0b1; /* ADC Channel */
	gpio_bits[1] = 0b1; /* ADC Channel */
	gpio_bits[2] = (channel >> 0) & 0b1; /* MUX Sel 1 */
	gpio_bits[3] = (channel >> 1) & 0b1; /* MUX Sel 1 */
	gpio_bits[4] = (channel >> 2) & 0b1; /* MUX Sel 1 */
	gpio_bits[5] = 0b1; /* ADC Channel */
	gpio_bits[6] = 0b1; /* ADC Channel */
	for (uint8_t ic = 0; ic < FEB_NUM_IC; ic++) {
		ADBMS6830B_set_cfgr(ic, FEB_ACC.IC_Config, refon, cth_bits, gpio_bits, dcc_bits, dcto_bits, uv, ov);
	}
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_wrcfga(FEB_NUM_IC, FEB_ACC.IC_Config);
	ADBMS6830B_wrcfgb(FEB_NUM_IC, FEB_ACC.IC_Config);

}

// ******************************** Temperature ********************************
void start_aux_voltage_measurements() {
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_adax(AUX_OW_OFF, PUP_DOWN, AUX_ALL);
	//ADBMS6830B_pollAdc();
}

void read_aux_voltages() {
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_rdaux(FEB_NUM_IC, FEB_ACC.IC_Config);
}

void store_cell_temps(uint8_t channel) {
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank++) {
		for (uint8_t mux = 0; mux < 4; mux++) {
			uint8_t gpio = get_gpio_pin(mux);
			uint16_t raw_code = FEB_ACC.IC_Config[bank].aux.a_codes[gpio];
			uint8_t sensor = get_sensor(mux, channel);
			FEB_ACC.banks[bank].temp_sensor_readings_V[sensor] = convert_voltage(raw_code);
		}
	}
}

//************************** Cell Balancing **********************
void BalanceUart(){
	char buffer[32];
	for(int i=0;i<6;i++){
		sprintf(buffer,"|PWM %d|%02X",i,FEB_ACC.IC_Config[0].pwm.tx_data[i]);
		HAL_UART_Transmit(&huart2, (uint8_t*) buffer, 32, 100);
	}
	for(int i=0;i<2;i++){
		sprintf(buffer,"|PWM %d|%02X",i+6,FEB_ACC.IC_Config[0].pwmb.tx_data[i]);
		HAL_UART_Transmit(&huart2, (uint8_t*) buffer, 32, 100);
	}
}

void FEB_Cell_Balance_Start(){
	FEB_cs_high();
	ADBMS6830B_init_cfg(FEB_NUM_IC, FEB_ACC.IC_Config);
	determineMinV();
	FEB_Cell_Balance_Process();
}
void FEB_Cell_Balance_Process(){
	uint64_t PWMBITS=determineDischarge();
	for (uint8_t ic = 0; ic < FEB_NUM_IC; ic++) {
		ADBMS6830B_set_cfgr(ic, FEB_ACC.IC_Config, refon, cth_bits, gpio_bits, dcc_bits, dcto_bits, uv, ov);
		for(int i=0;i<6;i++)
			FEB_ACC.IC_Config[ic].pwm.tx_data[i]=*(((int8_t*)(PWMBITS))+i);
		for(int i=0;i<2;i++)
			FEB_ACC.IC_Config[ic].pwmb.tx_data[i]=*(((int8_t*)(PWMBITS))+6+i);
	}
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_wrcfga(FEB_NUM_IC, FEB_ACC.IC_Config);
	ADBMS6830B_wrcfgb(FEB_NUM_IC, FEB_ACC.IC_Config);
	//TODO:read back config
	transmitCMD(ADCV|AD_CONT);
	BalanceUART();
}
void FEB_Stop_Balance(){
	uint64_t PWMBITS=0;
	for (uint8_t ic = 0; ic < FEB_NUM_IC; ic++) {
		ADBMS6830B_set_cfgr(ic, FEB_ACC.IC_Config, refon, cth_bits, gpio_bits, dcc_bits, dcto_bits, uv, ov);
		for(int i=0;i<6;i++)
			FEB_ACC.IC_Config[ic].pwm.tx_data[i]=*(((int8_t*)(PWMBITS))+i);
		for(int i=0;i<2;i++)
			FEB_ACC.IC_Config[ic].pwmb.tx_data[i]=*(((int8_t*)(PWMBITS))+6+i);
	}
	wakeup_sleep(FEB_NUM_IC);
	ADBMS6830B_wrcfga(FEB_NUM_IC, FEB_ACC.IC_Config);
	ADBMS6830B_wrcfgb(FEB_NUM_IC, FEB_ACC.IC_Config);
	transmitCMD(ADCV|AD_DCP);
}
void determineMinV(){
	transmitCMD(ADCV|AD_CONT);
	HAL_Delay(8);
	read_cell_voltages();
	store_cell_voltages();
	validate_voltages();
	MIN_CELL_VOLTAGE=FEB_ACC.banks[0].cells[0].voltage_V;
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank ++) {
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_BANK; cell ++) {
			float volt =FEB_ACC.banks[FEB_NUM_BANKS].cells[FEB_NUM_CELLS_PER_BANK].voltage_V;
			MIN_CELL_VOLTAGE= MIN_CELL_VOLTAGE<volt?volt:MIN_CELL_VOLTAGE;
		}
	}
}
uint64_t determineDischarge(){
	transmitCMD(ADCV|AD_CONT);
	HAL_Delay(8);
	read_cell_voltages();
	store_cell_voltages();
	validate_voltages();
	uint64_t bits=0;
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank ++) {
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_BANK; cell ++) {
			float volt =FEB_ACC.banks[FEB_NUM_BANKS].cells[FEB_NUM_CELLS_PER_BANK].voltage_V;
			float diff = volt-MIN_CELL_VOLTAGE;
			uint8_t dutyCycle=0b1111;
			if(diff<FEB_MIN_SLIPPAGE_V)
				dutyCycle=0;
			if(diff>FEB_MIN_SLIPPAGE_V*10)
				dutyCycle=0b0100;
			if(diff>FEB_MIN_SLIPPAGE_V*50)
				dutyCycle=0b1000;
			if(diff>FEB_MIN_SLIPPAGE_V*100)
				dutyCycle=0b1111;
			FEB_ACC.banks[bank].cells[cell].dischargeAmount=dutyCycle;
			*(((uint8_t*)bits)+(cell/2))|=dutyCycle<<(4*(cell%2));
		}
	}
	return bits;
}


