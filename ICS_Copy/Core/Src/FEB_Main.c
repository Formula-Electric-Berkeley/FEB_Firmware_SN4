// **************************************** Includes ****************************************

#include "FEB_Main.h"

uint64_t bms_errors;
extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;
extern I2C_HandleTypeDef hi2c1;

// ********************************** Variables **********************************
char buf[128];
uint8_t buf_len; //stolen from Main_Setup (SN2)

static TPS2482_Configuration tps2482_configurations[1];
uint8_t tps2482_i2c_addresses[1];
static uint16_t tps2482_ids[1];

// **************************************** Functions ****************************************

static void FEB_Variable_Init(void) {
	tps2482_i2c_addresses[0] = TPS2482_I2C_ADDR(TPS2482_I2C_ADDR_GND, TPS2482_I2C_ADDR_GND);
	tps2482_configurations[0].config = TPS2482_CONFIG_DEFAULT;
	tps2482_configurations[0].mask = TPS2482_MASK_SOL;
	tps2482_configurations[0].cal = TPS2482_CAL_EQ(TPS2482_CURRENT_LSB_EQ((double)(5)),(double)(.012));
}

void FEB_Main_Setup(void) {

	FEB_IO_ICS_Init();

	FEB_UI_Init();

	FEB_CAN_Init();

	FEB_CAN_HEARTBEAT_Init();

	FEB_Variable_Init();
	bool tps2482_init_res[1];
	TPS2482_Init(&hi2c1, tps2482_i2c_addresses, tps2482_configurations, tps2482_ids, tps2482_init_res, 1);

	// FEB_TPS2482_Setup();
	// memset(&bms_errors, 0xFF, sizeof(uint64_t));
}

void FEB_Main_Loop(void) {
	FEB_UI_Update();

	FEB_IO_ICS_Loop();

	FEB_Radio_Transmit();

	FEB_CAN_TPS_Transmit();

//	FEB_CAN_HEARTBEAT_Transmit();

	HAL_Delay(10);
	// FEB_CAN_HEARTBEAT_Transmit(&hcan1, &bms_errors);
}


