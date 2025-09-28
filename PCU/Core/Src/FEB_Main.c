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
uint8_t loop_counter = 0;

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
	bool ready_to_drive = FEB_Ready_To_Drive();
	
	// Debug logging for state transitions
	static FEB_SM_ST_t last_bms_state = FEB_SM_ST_DEFAULT;
	static bool last_ready_to_drive = false;
	
	if (bms_state != last_bms_state || ready_to_drive != last_ready_to_drive) {
		buf_len = snprintf(buf, sizeof(buf), "[STATE_CHANGE] BMS=%d->%d, R2D=%d->%d, Auto=%d\r\n",
						  last_bms_state, bms_state, last_ready_to_drive, ready_to_drive, auto_on);
		if (buf_len > 0 && buf_len < sizeof(buf)) {
			HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, HAL_MAX_DELAY);
		}
		last_bms_state = bms_state;
		last_ready_to_drive = ready_to_drive;
	}

	if (!auto_on){
		#if TORQUE_TEST_MODE
		// Test mode: Allow torque calculation for testing, but still require drive state for enable
		// Always update acceleration for testing torque calculations
		FEB_Normalized_updateAcc();
		
		// Only enable RMS if in proper drive state (same as normal mode)
		if (ready_to_drive && (bms_state == FEB_SM_ST_DRIVE)) {
			FEB_CAN_RMS_Process();  // This sets RMSControl.enabled = 1
			buf_len = snprintf(buf, sizeof(buf), "[TEST_MODE] RMS ENABLED: R2D=%d, BMS=%d\r\n",
					  ready_to_drive, bms_state);
		} else {
			FEB_CAN_RMS_Disable();  // This sets RMSControl.enabled = 0
			buf_len = snprintf(buf, sizeof(buf), "[TEST_MODE] RMS DISABLED: R2D=%d, BMS=%d\r\n",
					  ready_to_drive, bms_state);
		}
		
		if (buf_len > 0 && buf_len < sizeof(buf)) {
			HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, HAL_MAX_DELAY);
		}
		#else
		// Normal mode: Require both ready_to_drive and DRIVE state
		if (ready_to_drive && (bms_state == FEB_SM_ST_DRIVE /*|| bms_state == FEB_SM_ST_DRIVE_REGEN*/)) {
			FEB_Normalized_updateAcc();
			FEB_CAN_RMS_Process();
	//		FEB_TPS2482_sendReadings();
			
			buf_len = snprintf(buf, sizeof(buf), "[MAIN_LOOP] DRIVE mode: R2D=1, BMS=%d\r\n", bms_state);
			if (buf_len > 0 && buf_len < sizeof(buf)) {
				HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, HAL_MAX_DELAY);
			}

		} else {
			FEB_Normalized_setAcc0();
			FEB_CAN_RMS_Disable();
			
			buf_len = snprintf(buf, sizeof(buf), "[MAIN_LOOP] NOT ready: R2D=%d, BMS=%d\r\n", 
							  ready_to_drive, bms_state);
			if (buf_len > 0 && buf_len < sizeof(buf)) {
				HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, HAL_MAX_DELAY);
			}
		}
		#endif
	//	FEB_Normalized_updateAcc();
	//	FEB_CAN_RMS_Process();

		FEB_HECS_update();
		
		// Always call torque function in manual mode - it will handle safety checks internally
		if (loop_counter == 10) {
			loop_counter = 0;
			FEB_CAN_RMS_Torque();
		}
		loop_counter++;


	} else {
		// Auto mode
		if (bms_state == FEB_SM_ST_ENERGIZED) {
			FEB_CAN_RMS_Process();
			FEB_CAN_RMS_AUTO_Torque(torque);
		} else {
			FEB_Normalized_setAcc0();
			FEB_CAN_RMS_Disable();
			FEB_CAN_RMS_Disable_Torque();  // Ensure torque is disabled
		}
	}

	FEB_Normalized_CAN_sendBrake();
	FEB_CAN_HEARTBEAT_Transmit();
	FEB_CAN_ACC();
	FEB_CAN_TPS_Transmit();
	FEB_CAN_Send_Diagnostics();  // Send diagnostic info for debugging

	HAL_Delay(10);
}
