// ********************************** Includes & External **********************************

#include "FEB_Main.h"

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;

// ********************************** Variables **********************************
char buf[128];
uint8_t buf_len; //stolen from Main_Setup (SN2)
static TPS2482_Configuration tps2482_configurations[1];
uint8_t tps2482_i2c_addresses[1];
static uint16_t tps2482_ids[1];

// ********************************** Functions **********************************

static void FEB_Variable_Init(void) {
	tps2482_i2c_addresses[0] = TPS2482_I2C_ADDR(TPS2482_I2C_ADDR_GND, TPS2482_I2C_ADDR_GND);
	tps2482_configurations[0].config = TPS2482_CONFIG_DEFAULT;
	tps2482_configurations[0].mask = TPS2482_MASK_SOL;
	tps2482_configurations[0].cal = TPS2482_CAL_EQ(TPS2482_CURRENT_LSB_EQ((double)(4)),(double)(.002));
}

void FEB_Main_Setup(void){
	HAL_ADCEx_InjectedStart(&hadc1); //@lovehate - where does this go
//	FEB_Timer_Init();
//	FEB_TPS2482_Setup();
	FEB_CAN_Init(); //FEB_CAN_Init() // The transceiver must be connected otherwise you get sent into an infinite loop
	FEB_CAN_RMS_Setup();
	FEB_CAN_HEARTBEAT_Init();
	FEB_Variable_Init();
	bool tps2482_init_res[1];
	TPS2482_Init(&hi2c1, tps2482_i2c_addresses, tps2482_configurations, tps2482_ids, tps2482_init_res, 1);
}

void FEB_Main_While(void){
//	FEB_CAN_ICS_Transmit();
	FEB_SM_ST_t bms_state = FEB_CAN_BMS_getState();

	if (!auto_on){
		if (FEB_Ready_To_Drive() && (bms_state == FEB_SM_ST_DRIVE /*|| bms_state == FEB_SM_ST_DRIVE_REGEN*/)) {
			FEB_Normalized_updateAcc();
			FEB_CAN_RMS_Process();
	//		FEB_TPS2482_sendReadings();

		} else {
			FEB_Normalized_setAcc0();
			FEB_CAN_RMS_Disable();
		}
	//	FEB_Normalized_updateAcc();
	//	FEB_CAN_RMS_Process();

		FEB_HECS_update();

		FEB_CAN_RMS_Torque();

	} else {
		if (bms_state == FEB_SM_ST_ENERGIZED) {
			FEB_CAN_RMS_Process();
		}else {
			FEB_Normalized_setAcc0();
			FEB_CAN_RMS_Disable();
		}
		
		FEB_CAN_RMS_AUTO_Torque(torque);
	}

	FEB_Normalized_CAN_sendBrake();
	FEB_CAN_HEARTBEAT_Transmit();
	FEB_CAN_ACC();
	FEB_CAN_TPS_Transmit();

	HAL_Delay(10);
}
