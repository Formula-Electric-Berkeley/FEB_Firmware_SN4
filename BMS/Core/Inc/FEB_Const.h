#ifndef INC_FEB_CONST_H_
#define INC_FEB_CONST_H_

#define FEB_SM_ST_DEBUG 1

#define FEB_NUM_CELLS_PER_BANK 16
#define FEB_NUM_TEMP_SENSE_PER_BANK 32
#define FEB_NUM_BANKS 1
#define FEB_NUM_IC 1
#define FEB_CELL_MIN_VOLT 2.5
#define FEB_CELL_MAX_VOLT 4.2

// ******************************** Voltage Reading Configuration ***************************
#define CONTVR CONTINUOUS
#define DCPVR DCP_OFF
#define RSTFVR RSTF_OFF
#define OWVR OW_OFF_ALL_CH
#define RDVR RD_OFF

// *********************** States ***********************
typedef enum {
	FEB_SM_ST_BOOT,
	FEB_SM_ST_LV,
	FEB_SM_ST_ESC,
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

#endif /* INC_FEB_CONST_H_ */
