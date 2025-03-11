#include "FEB_Config.h"

/* State variables tied to state machine. Requires synchronization (SM lock held). */
static uint16_t cell_min_voltage_mV;		// mili-volts
static uint16_t cell_max_voltage_mV;
static int16_t cell_min_temperature_dC;		// deci-celsius
static int16_t cell_max_temperature_dC;
static int32_t pack_min_current_mA = 0;		// mili-amps
static int32_t pack_max_current_mA = 0;

/* Assume serial access (SM lock held). */
void FEB_Config_Update(FEB_SM_ST_t current_state) {
	int32_t current_resolution_mA = 50;
	switch(current_state) {
		case FEB_SM_ST_BALANCE:
			cell_min_voltage_mV = 3200; /* Higher minimum voltage */
			cell_max_voltage_mV = 4200;
			cell_min_temperature_dC = -200;
			cell_max_temperature_dC = 600;
			pack_min_current_mA = 0 - current_resolution_mA;
			pack_max_current_mA = 0 + current_resolution_mA;
			break;
		case FEB_SM_ST_CHARGING:
			cell_min_voltage_mV = 2500;
			cell_max_voltage_mV = 4200;
			cell_min_temperature_dC = 0;
			cell_max_temperature_dC = 450;
			pack_min_current_mA = -20000;
			pack_max_current_mA = 0 + current_resolution_mA;
			break;
		case FEB_SM_ST_PRECHARGE:
			cell_min_voltage_mV = 2500;
			cell_max_voltage_mV = 4200;
			cell_min_temperature_dC = -200;
			cell_max_temperature_dC = 600;
			pack_min_current_mA = 0 - current_resolution_mA;
			pack_max_current_mA = 100;
			break;
		case FEB_SM_ST_DRIVE:
			cell_min_voltage_mV = 2500;
			cell_max_voltage_mV = 4200;
			cell_min_temperature_dC = -200;
			cell_max_temperature_dC = 600;
			pack_min_current_mA = 0 - current_resolution_mA;
			pack_max_current_mA = 60000;
			break;
			/*REGEN
		case FEB_SM_ST_DRIVE_REGEN:
			cell_min_voltage_mV = 2500;
			cell_max_voltage_mV = 4200;
			cell_min_temperature_dC = 0;
			cell_max_temperature_dC = 450;
			pack_min_current_mA = -20000;
			pack_max_current_mA = 60000;
			break;*/
		default:
			cell_min_voltage_mV = 2500;
			cell_max_voltage_mV = 4200;
			cell_min_temperature_dC = -200;
			cell_max_temperature_dC = 600;
			pack_min_current_mA = 0 - current_resolution_mA;
			pack_max_current_mA = 0 + current_resolution_mA;
			break;
	}
}

/* ******** Interface ******** */

uint16_t FEB_Config_Get_Cell_Min_Voltage_mV(void) {
	//while (osMutexAcquire(FEB_SM_LockHandle, UINT32_MAX) != osOK);
	uint16_t value = cell_min_voltage_mV;
	//osMutexRelease(FEB_SM_LockHandle);
	return value;
}

uint16_t FEB_Config_Get_Cell_Max_Voltage_mV(void) {
	//while (osMutexAcquire(FEB_SM_LockHandle, UINT32_MAX) != osOK);
	uint16_t value = cell_max_voltage_mV;
	//osMutexRelease(FEB_SM_LockHandle);
	return value;
}


int16_t FEB_Config_Get_Cell_Min_Temperature_dC(void) {
	//while (osMutexAcquire(FEB_SM_LockHandle, UINT32_MAX) != osOK);
	int16_t value = cell_min_temperature_dC;
	//osMutexRelease(FEB_SM_LockHandle);
	return value;
}

int16_t FEB_Config_Get_Cell_Max_Temperature_dC(void) {
	//while (osMutexAcquire(FEB_SM_LockHandle, UINT32_MAX) != osOK);
	int16_t value = cell_max_temperature_dC;
	//osMutexRelease(FEB_SM_LockHandle);
	return value;
}

int32_t FEB_Config_Get_Pack_Min_Current_mA(void) {
	//while (osMutexAcquire(FEB_SM_LockHandle, UINT32_MAX) != osOK);
	int32_t value = pack_min_current_mA;
	//osMutexRelease(FEB_SM_LockHandle);
	return value;
}

int32_t FEB_Config_Get_Pack_Max_Current_mA(void) {
	//while (osMutexAcquire(FEB_SM_LockHandle, UINT32_MAX) != osOK);
	int32_t value = pack_max_current_mA;
	//osMutexRelease(FEB_SM_LockHandle);
	return value;
}
