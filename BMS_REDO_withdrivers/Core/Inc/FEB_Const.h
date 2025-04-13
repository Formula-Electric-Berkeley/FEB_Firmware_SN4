#ifndef INC_FEB_CONST_H_
#define INC_FEB_CONST_H_

#define FEB_SM_ST_DEBUG 1

#define FEB_NUM_CELLS_PER_IC 10
#define FEB_NUM_TEMP_SENSE_PER_IC 10
#define FEB_ERROR_THRESH 3
#define FEB_NUM_ICPBANK 2
#define FEB_NBANKS 1
#define FEB_NUM_IC (FEB_NUM_ICPBANK*FEB_NBANKS)
#define FEB_NUM_CELL_PER_BANK (FEB_NUM_ICPBANK*FEB_NUM_CELLS_PER_IC)

#define FEB_CONFIG_CELL_MIN_VOLTAGE_mV 2500
#define FEB_CONFIG_CELL_MAX_VOLTAGE_mV 4200
#define FEB_CONFIG_CELL_MIN_VOLTAGE_100uV (FEB_CONFIG_CELL_MIN_VOLTAGE_mV * 10)
#define FEB_CONFIG_CELL_MAX_VOLTAGE_100uV (FEB_CONFIG_CELL_MAX_VOLTAGE_mV * 10)
#define FEB_CONST_PRECHARGE_PCT 0.9
#define FEB_NUM_CAN_DEV 6
// ******************************** Voltage Reading Configuration ***************************
#define CONTVR CONTINUOUS
#define DCPVR DCP_ON
#define RSTFVR RSTF_OFF
#define OWVR OW_OFF_ALL_CH
#define RDVR RD_ON
//#define READALL
// *********************** States ***********************
typedef enum {
	FEB_SM_ST_BOOT,
	FEB_SM_ST_LV,
	FEB_SM_ST_HEALTH_CHECK,
	FEB_SM_ST_PRECHARGE,
	FEB_SM_ST_ENERGIZED,
	FEB_SM_ST_DRIVE,
	FEB_SM_ST_FREE,
	FEB_SM_ST_CHARGING,
	FEB_SM_ST_BALANCE,
	FEB_SM_ST_FAULT_BMS,
	FEB_SM_ST_FAULT_BSPD,
	FEB_SM_ST_FAULT_IMD,
	FEB_SM_ST_FAULT_CHARGING,
	FEB_SM_ST_DEFAULT
} FEB_SM_ST_t;

typedef enum {
	FEB_RELAY_STATE_OPEN,
	FEB_RELAY_STATE_CLOSE

}FEB_Relay_State;
typedef struct {
	float voltage_V;
	float voltage_S;
	uint8_t violations;
} cell_t;

typedef struct {
	uint8_t tempRead;
	uint8_t badReadV;
	float total_voltage_V;
	float temp_sensor_readings_V[FEB_NUM_TEMP_SENSE_PER_IC*FEB_NUM_ICPBANK];
	uint8_t temp_violations[FEB_NUM_TEMP_SENSE_PER_IC*FEB_NUM_ICPBANK];
	cell_t cells[FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK];
} bank_t;

typedef struct {
	float total_voltage_V;
	bank_t banks[FEB_NBANKS];
} accumulator_t;

static accumulator_t FEB_ACC = {0};
#endif /* INC_FEB_CONST_H_ */
