#include "FEB_SM.h"
#include "FEB_ADBMS6830B_Driver.h"
#include "stm32f4xx_hal.h"
#include "FEB_ADBMS6830B.h"
#include "FEB_HW.h"
#include "FEB_Temp_LUT.h"
#include "FEB_UART_Transmit.h"
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
cell_asic IC_Config[FEB_NUM_IC];
accumulator_t FEB_ACC = {0};
extern UART_HandleTypeDef huart2;

// ******************************** Config Bits ********************************

static bool refon = 1;
static bool cth_bits[3] = {0, 1, 1};
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
	//if(FEB_PIN_RD(P_PB0))FEB_Siren_Activate();
	ADBMS6830B_init_cfg(FEB_NUM_IC, IC_Config);
	ADBMS6830B_reset_crc_count(FEB_NUM_IC, IC_Config);
	ADBMS6830B_init_reg_limits(FEB_NUM_IC, IC_Config);
	ADBMS6830B_wrALL(FEB_NUM_IC, IC_Config);
	//ADBMS6830B_rdALL(FEB_NUM_IC, IC_Config);


}
#define POLL_RATE 1
int poll = POLL_RATE;

void FEB_ADBMS_Voltage_Process() {
	start_adc_cell_voltage_measurements();
	read_cell_voltages();
	store_cell_voltages();
	validate_voltages();
	if(poll-- == 0){
		FEB_MONITOR_UART_Transmit(&FEB_ACC);
		poll=POLL_RATE;
	}

}

void FEB_ADBMS_Temperature_Process(){
	gpio_bits[9] = !gpio_bits[9];
	for (uint8_t channel = 0; channel < 5; channel++) {
			configure_gpio_bits(channel);
			start_aux_voltage_measurements();
			read_aux_voltages();
			store_cell_temps(channel);
	}
}

// ******************************** Voltage ********************************

void start_adc_cell_voltage_measurements() {
	ADBMS6830B_adcv(1, 0, 1, 0, OWVR);
	HAL_Delay(1);
	//ADBMS6830B_pollAdc();
}

void read_cell_voltages() {
	ADBMS6830B_rdcv(FEB_NUM_IC, IC_Config);
	ADBMS6830B_rdsv(FEB_NUM_IC, IC_Config);
}

void store_cell_voltages() {
	FEB_ACC.total_voltage_V = 0;
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank ++) {
		for(uint8_t ic =0; ic<FEB_NUM_ICPBANK;ic++){
			for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC; cell ++) {
				float CVoltage = convert_voltage(IC_Config[ic+bank*FEB_NUM_ICPBANK].cells.c_codes[cell]);
				FEB_ACC.banks[bank].cells[cell+ic*FEB_NUM_CELLS_PER_IC].voltage_V = CVoltage;
				FEB_ACC.banks[bank].cells[cell+ic*FEB_NUM_CELLS_PER_IC].voltage_S = convert_voltage(IC_Config[ic+bank*FEB_NUM_ICPBANK].cells.s_codes[cell]);
				FEB_ACC.total_voltage_V+=CVoltage;
			}
		}

	}
}

void validate_voltages() {
	uint16_t vMax = FEB_Config_Get_Cell_Max_Voltage_mV();
	uint16_t vMin = FEB_Config_Get_Cell_Min_Voltage_mV();
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank ++) {
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC; cell ++) {
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
		ADBMS6830B_set_cfgr(ic, IC_Config, refon, cth_bits, gpio_bits, dcc_bits, dcto_bits, uv, ov);
	}
	ADBMS6830B_wrALL(FEB_NUM_IC, IC_Config);
}

// ******************************** Temperature ********************************
void start_aux_voltage_measurements() {
	ADBMS6830B_adax(AUX_OW_OFF, PUP_DOWN, 1);
	HAL_Delay(1);
	//ADBMS6830B_pollAdc();
	ADBMS6830B_adax(AUX_OW_OFF, PUP_DOWN, 2);
	HAL_Delay(1);
}

void read_aux_voltages() {
	ADBMS6830B_rdaux(FEB_NUM_IC, IC_Config);
}

void store_cell_temps(uint8_t channel) {
	for (uint8_t bank = 0; bank < FEB_NUM_IC; bank++) {
		for (uint8_t icn = 0; icn < FEB_NUM_ICPBANK; icn++) {
			uint16_t mux1 = IC_Config[FEB_NUM_ICPBANK*bank+icn].aux.a_codes[0];
			uint16_t mux2 = IC_Config[FEB_NUM_ICPBANK*bank+icn].aux.a_codes[1];
			FEB_ACC.banks[bank].temp_sensor_readings_V[icn*FEB_NUM_TEMP_SENSE_PER_IC+channel] = FEB_Temp_LUT_Get_Temp_100mC( (int) (convert_voltage(mux1)*1000))*0.1;
			FEB_ACC.banks[bank].temp_sensor_readings_V[icn*FEB_NUM_TEMP_SENSE_PER_IC+channel+5] = FEB_Temp_LUT_Get_Temp_100mC( (int) (convert_voltage(mux2)*1000))*0.1;
		}
	}
}

//************************** Cell Balancing **********************
void FEB_Cell_Balance_Start(){
	FEB_cs_high();
	ADBMS6830B_init_cfg(FEB_NUM_IC, IC_Config);
	determineMinV();
	FEB_Cell_Balance_Process();
}
void FEB_Cell_Balance_Process(){
	uint64_t PWMBITS=determineDischarge();
	for (uint8_t ic = 0; ic < FEB_NUM_IC; ic++) {
		ADBMS6830B_set_cfgr(ic, IC_Config, refon, cth_bits, gpio_bits, dcc_bits, dcto_bits, uv, ov);
		//memcpy( &(IC_Config[ic].pwm), (int8_t*)PWMBITS, 6);
		//memcpy( &(IC_Config[ic].pwmb), ((int8_t*)PWMBITS)+6, 2);
	}
	ADBMS6830B_wrcfga(FEB_NUM_IC, IC_Config);
	ADBMS6830B_wrcfgb(FEB_NUM_IC, IC_Config);
	transmitCMD(ADCV|AD_CONT);
}
void FEB_Stop_Balance(){
	uint64_t PWMBITS=0;
	for (uint8_t ic = 0; ic < FEB_NUM_IC; ic++) {
		ADBMS6830B_set_cfgr(ic, IC_Config, refon, cth_bits, gpio_bits, dcc_bits, dcto_bits, uv, ov);
		//memcpy( &(IC_Config[ic].pwm), (int8_t*)PWMBITS, 6);
		//memcpy( &(IC_Config[ic].pwmb), ((int8_t*)PWMBITS)+6, 2);
	}

	ADBMS6830B_wrcfga(FEB_NUM_IC, IC_Config);
	ADBMS6830B_wrcfgb(FEB_NUM_IC, IC_Config);
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
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC; cell ++) {
			float volt =FEB_ACC.banks[bank].cells[cell].voltage_V;
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
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC; cell ++) {
			float volt = FEB_ACC.banks[bank].cells[cell].voltage_V;
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

float FEB_ADBMS_Get_Total_Voltage(){
	return FEB_ACC.total_voltage_V;
}



