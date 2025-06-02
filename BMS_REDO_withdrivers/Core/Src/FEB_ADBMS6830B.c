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
void validate_temps();
// ******************************** Balance ********************************
uint64_t determineDischarge();
void determineMinV();
// ******************************** Global Variabls ********************************
cell_asic IC_Config[FEB_NUM_IC];
extern accumulator_t FEB_ACC;
extern UART_HandleTypeDef huart2;
extern uint8_t ERROR_TYPE;
// ******************************** Config Bits ********************************

static bool refon = 0;
static bool cth_bits[3] = {1,1,1};
static bool gpio_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
static uint16_t dcc_bits = 0x0000;
static bool dcto_bits[6] = {1,1,1,1,1,1};
static uint16_t uv = 0x0010;
static uint16_t ov = 0x3FF0;
float MIN_CELL_VOLTAGE=3.1;
float FEB_MIN_SLIPPAGE_V=0.01;

// ******************************** Helper Functions ********************************

static float convert_voltage(int16_t raw_code) {
	return raw_code * 0.000150f + 1.5f;
}

// ******************************** Functions ********************************

void FEB_ADBMS_Init() {
	for (uint8_t bank = 0; bank < FEB_NBANKS; bank ++) {
		FEB_ACC.banks[bank].badReadV=0;
		FEB_ACC.banks[bank].tempRead=0;
		FEB_ACC.banks[bank].total_voltage_V=0;
		for(uint8_t ic =0; ic<FEB_NUM_ICPBANK;ic++){
			for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC; cell ++) {
				FEB_ACC.banks[bank].cells[cell].voltage_V = 0;
				FEB_ACC.banks[bank].cells[cell].voltage_S = 0;
				FEB_ACC.banks[bank].cells[cell].violations = 0;
				FEB_ACC.banks[bank].cells[cell].discharging = 0;
				FEB_ACC.banks[bank].temp_violations[cell] = 0;
			}
		}
	}
	FEB_cs_high();
	ADBMS6830B_init_cfg(FEB_NUM_IC, IC_Config);
	ADBMS6830B_reset_crc_count(FEB_NUM_IC, IC_Config);
	ADBMS6830B_init_reg_limits(FEB_NUM_IC, IC_Config);
	ADBMS6830B_wrALL(FEB_NUM_IC, IC_Config);
}
void FEB_ADBMS_Voltage_Process() {
	start_adc_cell_voltage_measurements();
	read_cell_voltages();
	store_cell_voltages();
	validate_voltages();
}

void FEB_ADBMS_Temperature_Process(){
	gpio_bits[9]^=0b1;
	for (uint8_t channel = 0; channel < 5; channel++) {
			configure_gpio_bits(channel);
			start_aux_voltage_measurements();
			read_aux_voltages();
			store_cell_temps(channel);
	}
	validate_temps();
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
	for (uint8_t bank = 0; bank < FEB_NBANKS; bank ++) {
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
	for (uint8_t bank = 0; bank < FEB_NBANKS; bank ++) {
		FEB_ACC.banks[bank].badReadV=0;
		for (uint8_t cell = 0; cell < FEB_NUM_CELL_PER_BANK; cell ++) {
			float voltageC = FEB_ACC.banks[bank].cells[cell].voltage_V*1000;
			float voltageS = FEB_ACC.banks[bank].cells[cell].voltage_V*1000;
			if (voltageC > vMax|| voltageC < vMin) {
				if(voltageC==-1000){
					FEB_ACC.banks[bank].badReadV+=1;
				}
				if(voltageS > vMax && voltageS < vMin){
					FEB_ACC.banks[bank].cells[cell].violations+=1;
					if(FEB_ACC.banks[bank].cells[cell].violations == FEB_ERROR_THRESH) {
						ERROR_TYPE=0x30;
						FEB_SM_Transition(FEB_SM_ST_FAULT_BMS);

					}
				} else {
					FEB_ACC.banks[bank].cells[cell].violations = 0;
				}
			} else {
				FEB_ACC.banks[bank].cells[cell].violations=0;
			}
		}
	}
}
void configure_gpio_bits(uint8_t channel) {
	gpio_bits[0] = 0b1; /* ADC Channel */
	gpio_bits[1] = 0b1; /* ADC Channel */
	gpio_bits[2] = ((channel >> 0) & 0b1 ); /* MUX Sel 1 */
	gpio_bits[3] = ((channel >> 1) & 0b1 ); /* MUX Sel 1 */
	gpio_bits[4] = ((channel >> 2) & 0b1 ); /* MUX Sel 1 */
	gpio_bits[5] = 0b1; /* ADC Channel */
	gpio_bits[6] = 0b1; /* ADC Channel */
	for (uint8_t icn = 0; icn < FEB_NUM_IC; icn++) {
		ADBMS6830B_set_cfgr(icn, IC_Config, refon, cth_bits, gpio_bits, 0, dcto_bits, uv, ov);
	}
	ADBMS6830B_wrcfga(FEB_NUM_IC, IC_Config);
}

// ******************************** Temperature ********************************
void start_aux_voltage_measurements() {
	ADBMS6830B_adax(AUX_OW_OFF, PUP_DOWN, 1);
	HAL_Delay(2);
	//ADBMS6830B_pollAdc();
	ADBMS6830B_adax(AUX_OW_OFF, PUP_DOWN, 2);
	HAL_Delay(2);
}

void read_aux_voltages() {
	ADBMS6830B_rdaux(FEB_NUM_IC, IC_Config);
}

void store_cell_temps(uint8_t channel) {
	int totalReads=0;
	for (uint8_t bank = 0; bank < FEB_NBANKS; bank++) {
		for (uint8_t icn = 0; icn < FEB_NUM_ICPBANK; icn++) {
			uint16_t mux1 = IC_Config[FEB_NUM_ICPBANK*bank+icn].aux.a_codes[0];
			uint16_t mux2 = IC_Config[FEB_NUM_ICPBANK*bank+icn].aux.a_codes[1];
			float V1=(convert_voltage(mux1)*1000);
			float V2=(convert_voltage(mux2)*1000);
			FEB_ACC.banks[bank].temp_sensor_readings_V[icn*FEB_NUM_TEMP_SENSE_PER_IC+channel] = FEB_Temp_LUT_Get_Temp_100mC( (int) V1)*0.1;
			FEB_ACC.banks[bank].temp_sensor_readings_V[icn*FEB_NUM_TEMP_SENSE_PER_IC+channel+5] = FEB_Temp_LUT_Get_Temp_100mC( (int) V2)*0.1;
		}
	}
}
void validate_temps() {
	uint16_t tMax = FEB_Config_Get_Cell_Max_Temperature_dC();
	uint16_t tMin = FEB_Config_Get_Cell_Min_Temperature_dC();
	int totalReads=0;
	int totalErrors = 0;
	for (uint8_t bank = 0; bank < FEB_NBANKS; bank ++) {
		FEB_ACC.banks[bank].tempRead = 0;
		for (uint8_t cell = 0; cell < FEB_NUM_CELL_PER_BANK; cell ++) {
			float temp = FEB_ACC.banks[bank].temp_sensor_readings_V[cell]*10;
			if(temp!=-12.0f)FEB_ACC.banks[bank].tempRead+=1;
			else continue;
			if (temp > tMax || temp < (float) tMin) {
				FEB_ACC.banks[bank].temp_violations[cell]++;
				if(FEB_ACC.banks[bank].temp_violations[cell] == FEB_ERROR_THRESH){
					ERROR_TYPE=0x10;
					FEB_SM_Transition(FEB_SM_ST_FAULT_BMS);

				}
			} else {
				FEB_ACC.banks[bank].temp_violations[cell] = 0;
			}
		}
		totalReads+=FEB_ACC.banks[bank].tempRead;
	}
	if(totalReads/(float)(FEB_NUM_CELL_PER_BANK*FEB_NBANKS) < 0.2){
		ERROR_TYPE=0x20;
		FEB_SM_Transition(FEB_SM_ST_FAULT_BMS);

	}
}

//************************** Cell Balancing **********************
void FEB_Cell_Balance_Start(){
	FEB_cs_high();
	ADBMS6830B_init_cfg(FEB_NUM_IC, IC_Config);
	ADBMS6830B_wrALL(FEB_NUM_IC, IC_Config);
	FEB_Cell_Balance_Process();
}
int cycle=0;
uint16_t mask=0xAAAA;
void FEB_Cell_Balance_Process(){
	determineMinV();
	if(cycle == 3 ){
		mask=~mask;
		cycle=0;
	}
	cycle++;
	for (uint8_t icn = 0; icn < FEB_NUM_IC; icn ++) {
		uint16_t bits=0x0000;
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC; cell ++) {
			float volt = FEB_ACC.banks[icn/FEB_NUM_ICPBANK].cells[cell+FEB_NUM_CELLS_PER_IC*(icn%2)].voltage_V;
			float diff = volt-MIN_CELL_VOLTAGE;
			if(diff>FEB_MIN_SLIPPAGE_V){
				bits|=(0b1<<cell);
				FEB_ACC.banks[icn/FEB_NUM_ICPBANK].cells[cell+FEB_NUM_CELLS_PER_IC*(icn%2)].discharging=0b1&((mask&bits)>>cell);
			} else {
				FEB_ACC.banks[icn/FEB_NUM_ICPBANK].cells[cell+FEB_NUM_CELLS_PER_IC*(icn%2)].discharging=0b0;
			}
		}
		ADBMS6830B_set_cfgr(icn, IC_Config, refon, cth_bits, gpio_bits, (bits & mask), dcto_bits, uv, ov);
	}
	//ADBMS6830B_wrcfga(FEB_NUM_IC, IC_Config);
	ADBMS6830B_wrcfgb(FEB_NUM_IC, IC_Config);
}
void FEB_Stop_Balance(){
	for (uint8_t ic = 0; ic < FEB_NUM_IC; ic++) {
		ADBMS6830B_set_cfgr(ic, IC_Config, refon, cth_bits, gpio_bits, 0, dcto_bits, uv, ov);
	}
	ADBMS6830B_wrALL(FEB_NUM_IC, IC_Config);
	transmitCMD(ADCV|AD_DCP);
}
void determineMinV(){
	FEB_Stop_Balance();
	transmitCMD(ADCV|AD_CONT|AD_RD);
	HAL_Delay(1);
	ADBMS6830B_rdcv(FEB_NUM_IC, IC_Config);
	ADBMS6830B_rdsv(FEB_NUM_IC, IC_Config);
	store_cell_voltages();
	validate_voltages();

	MIN_CELL_VOLTAGE=FEB_ACC.banks[0].cells[0].voltage_V;
	for (uint8_t bank = 0; bank < FEB_NBANKS; bank ++) {
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC; cell ++) {
			float volt =FEB_ACC.banks[bank].cells[cell].voltage_V;
			if(volt<0){
				continue;
			}
			MIN_CELL_VOLTAGE= MIN_CELL_VOLTAGE>volt?volt:MIN_CELL_VOLTAGE;
		}
	}
}


