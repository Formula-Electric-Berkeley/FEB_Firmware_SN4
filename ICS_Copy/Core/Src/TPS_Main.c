/*
 * TPS_Main.c
 *
 *  Created on: Apr 29, 2025
 *      Author: root
 */
#include "TPS_Main.h"

// Stores TPS2482 configurations
/* 		LV - Low voltage bus
* 		CP - Coolant Pump
* 		AF - Accumulator Fans
* 		RF - Radiator Fans
* 		SH - Shutdown Source
* 		L - LIDAR
* 		AS - Autonomous Steering
* 		AB - Autonomous Braking
*/
/*
uint8_t tps2482_i2c_addresses[NUM_TPS2482];
uint16_t tps2482_ids[NUM_TPS2482];

TPS2482_Configuration tps2482_configurations[NUM_TPS2482];
TPS2482_Configuration *lv_config = &tps2482_configurations[0];

GPIO_TypeDef *tps2482_en_ports[NUM_TPS2482 - 1]; // LV doesn't have an EN pin
uint16_t tps2482_en_pins[NUM_TPS2482 - 1]; // LV doesn't have an EN pin

GPIO_TypeDef *tps2482_pg_ports[NUM_TPS2482];
uint16_t tps2482_pg_pins[NUM_TPS2482];

GPIO_TypeDef *tps2482_alert_ports[NUM_TPS2482];
uint16_t tps2482_alert_pins[NUM_TPS2482];

uint16_t tps2482_current_raw[NUM_TPS2482];
uint16_t tps2482_bus_voltage_raw[NUM_TPS2482];
uint16_t tps2482_shunt_voltage_raw[NUM_TPS2482];

int32_t tps2482_current_filter[NUM_TPS2482];
bool tps2482_current_filter_init[NUM_TPS2482];

int16_t tps2482_current[NUM_TPS2482];
uint16_t tps2482_bus_voltage[NUM_TPS2482];
double tps2482_shunt_voltage[NUM_TPS2482];

static void FEB_Variable_Conversion(void) {
	for ( uint8_t i = 0; i < NUM_TPS2482; i++ ) {
		tps2482_bus_voltage[i] = FLOAT_TO_UINT16_T(tps2482_bus_voltage_raw[i] * TPS2482_CONV_VBUS);
//		tps2482_shunt_voltage[i] = (SIGN_MAGNITUDE(tps2482_shunt_voltage_raw[i]) * TPS2482_CONV_VSHUNT);
	}

	tps2482_current[0] = FLOAT_TO_INT16_T(SIGN_MAGNITUDE(tps2482_current_raw[0]) * LV_CURRENT_LSB);


	FEB_Current_IIR(tps2482_current, tps2482_current, tps2482_current_filter, NUM_TPS2482, tps2482_current_filter_init);
}

static void FEB_Variable_Init(void) {
	tps2482_i2c_addresses[0] = LV_ADDR;

	for ( uint8_t i = 0; i < NUM_TPS2482; i++ ) {
		tps2482_configurations[i].config = TPS2482_CONFIG_DEFAULT;
		tps2482_configurations[i].mask = TPS2482_MASK_SOL;
	}

	lv_config->cal = LV_CAL_VAL;


	lv_config->alert_lim = LV_ALERT_LIM_VAL;


	tps2482_pg_pins[0] = LV_PG_Pin;


	tps2482_alert_ports[0] = LV_ALERT_GPIO_Port;

	tps2482_alert_pins[0] = LV_ALERT_Pin;

	can_data.ids[0] = FEB_CAN_LVPDB_FLAGS_BUS_VOLTAGE_LV_CURRENT_FRAME_ID;
	can_data.ids[1] = FEB_CAN_LVPDB_CP_AF_RF_SH_CURRENT_FRAME_ID;
	can_data.ids[2] = FEB_CAN_LVPDB_L_AS_AB_CURRENT_FRAME_ID;

	memset(tps2482_current_raw, 0, NUM_TPS2482 * sizeof(uint16_t));
	memset(tps2482_bus_voltage_raw, 0, NUM_TPS2482 * sizeof(uint16_t));
	memset(tps2482_shunt_voltage_raw, 0, NUM_TPS2482 * sizeof(uint16_t));
	memset(tps2482_current, 0, NUM_TPS2482 * sizeof(uint16_t));
	memset(tps2482_bus_voltage, 0, NUM_TPS2482 * sizeof(uint16_t));
	memset(tps2482_shunt_voltage, 0, NUM_TPS2482 * sizeof(uint16_t));

	memset(tps2482_current_filter, 0, NUM_TPS2482 * sizeof(int32_t));
	memset(tps2482_current_filter_init, false, NUM_TPS2482 * sizeof(bool));
}
**/
