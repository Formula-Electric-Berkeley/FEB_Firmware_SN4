#include "FEB_IO_ICS.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart3;

#define IOEXP_ADDR ((uint16_t) 0x40)
#define BTN_HOLD_TIME ((uint32_t) 2000)
#define RTD_BUZZER_TIME ((uint32_t) 2000)

static uint32_t rtd_press_start_time;
static uint32_t rtd_buzzer_start_time = 0;
static uint8_t set_rtd_buzzer = 1;
static uint8_t IO_state = 0xFF;

static uint8_t r2d = 0;

// **************************************** Functions ****************************************

void FEB_IO_ICS_Init(void) {
	// initialize RTD (P0)and all pins to High (there is a physical 'not gate' on the board that
	// makes it low so buzzer doesn't go peep)
	uint8_t initial_io_exp_state = IO_state;

	HAL_I2C_Master_Transmit(&hi2c1, IOEXP_ADDR, &initial_io_exp_state, sizeof(initial_io_exp_state), HAL_MAX_DELAY);
}

void FEB_IO_ICS_Loop(void) {
	// receive IO expander data from I2C
	uint8_t received_data;
	HAL_I2C_Master_Receive(&hi2c1, IOEXP_ADDR, &received_data, 1, HAL_MAX_DELAY);

	IO_state = 0;
	uint8_t brake_pressure = FEB_CAN_APPS_Get_Brake_Pos();
	uint8_t inv_enabled = FEB_CAN_APPS_Get_Enabled();
	FEB_SM_ST_t bms_state = FEB_CAN_BMS_Get_State();
//
//	if(r2d == 1  && (!(bms_state == FEB_SM_ST_DRIVE || bms_state == FEB_SM_ST_DRIVE_REGEN) || FEB_CAN_BMS_is_stale())){
//		r2d = 0; //Reset r2d if we're no longer in drive or drive regen.
//	}

	if(r2d == 0 && bms_state == FEB_SM_ST_DRIVE_STANDBY){
		lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT );

	}

	// Button 1 - Ready-to-Drive (RTD) button
	if (!(received_data & (1<<1))) {
		if (((HAL_GetTick() - rtd_press_start_time) >= BTN_HOLD_TIME) && brake_pressure >= 4 && (bms_state == FEB_SM_ST_DRIVE_STANDBY || bms_state == FEB_SM_ST_DRIVE_REGEN || bms_state == FEB_SM_ST_DRIVE)) {

			//Flio ready to drive if pressed again to turn it off
			r2d = (r2d == 1) ? 0 : 1;
			IO_state = (uint8_t) set_n_bit(IO_state, 1, 1);
			if(r2d == 1){
				set_rtd_buzzer = 0;
			}else{
				set_rtd_buzzer = 1;
			}

			//Restart the start time so we don't constantly toggle r2d
			rtd_press_start_time = HAL_GetTick();
			lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT );

		} else {
			IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
		}
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
		rtd_press_start_time = HAL_GetTick();
	}

	// Button 2
	if (!(received_data & (1<<2))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 2, 1);
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 2, 0);
	}

	// Button 3
	if (!(received_data & (1<<3))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 3, 1);
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 3, 0);
	}

	// Button 4
	if (!(received_data & (1<<4))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 4, 1);
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 4, 0);
	}

	// Switch 1 - Coolant Pump
	if (!(received_data & (1<<7))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 7, 1);
		lv_obj_set_style_bg_color(ui_TextArea4, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 7, 0);
		lv_obj_set_style_bg_color(ui_TextArea4, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	}

	// Switch 2 - Radiator Fans
	if (!(received_data & (1<<5))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 5, 1);
		lv_obj_set_style_bg_color(ui_TextArea1, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 5, 0);
		lv_obj_set_style_bg_color(ui_TextArea1, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	}

	// Switch 3 - Accumulator Fans
	if (!(received_data & (1<<6))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 6, 1);
		lv_obj_set_style_bg_color(ui_TextArea2, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 6, 0);
		lv_obj_set_style_bg_color(ui_TextArea2, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	}


	// Handle buzzer


	if (set_rtd_buzzer == 0 && (bms_state ==  FEB_SM_ST_DRIVE || bms_state ==FEB_SM_ST_DRIVE_REGEN) && inv_enabled == 1) {
		if (rtd_buzzer_start_time == 0) {
			rtd_buzzer_start_time = HAL_GetTick();
		}
		IO_state = set_n_bit(IO_state, 0, 0);
		lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
	} else {
		IO_state = set_n_bit(IO_state, 0, 1);
	}


	//I'm pretty sure this should go here
	if ((HAL_GetTick() - rtd_buzzer_start_time) >= RTD_BUZZER_TIME && rtd_buzzer_start_time > 0) {
		rtd_buzzer_start_time = 0;
		set_rtd_buzzer = 1;
		IO_state = set_n_bit(IO_state,0,0);
	}

	//r2d should trigger the color not the buzzer state.
	if (r2d == 1){
		lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );

	}else{
		lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );

	}

	// ---------------------- Additional logic for NEW BMS STATES ----------------------
	if (bms_state == FEB_SM_ST_CHARGE) {
	    // existing "charge" state => maybe show blue
	    lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT );
	 }
	 else if (bms_state == FEB_SM_ST_BALANCE) {
	    // existing "balance" => maybe purple
	    lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x7700FF), LV_PART_MAIN | LV_STATE_DEFAULT );
	 }
	 // ------- new states below -------
	 else if (bms_state == FEB_SM_ST_ORIGIN) {
	    lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT );
	 }
	 else if (bms_state == FEB_SM_ST_LV_POWER) {
	    lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT );
	 }
	 else if (bms_state == FEB_SM_ST_ESC_COMPLETED) {
	    lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
	 }
	 else if (bms_state == FEB_SM_ST_BATTERY_FREE) {
	    lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFF7700), LV_PART_MAIN | LV_STATE_DEFAULT );
	 }
	 // new fault sub-states
	 else if (bms_state == FEB_SM_ST_BMS_FAULT ||
	          bms_state == FEB_SM_ST_BSPD_FAULT ||
	          bms_state == FEB_SM_ST_IMD_FAULT ||
	          bms_state == FEB_SM_ST_BMS_CHARGING_FAULT)
	 {
	    lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	 }
	 // End new states


	uint8_t transmit_rtd = (0b1111111 << 1) + set_rtd_buzzer;
	HAL_I2C_Master_Transmit(&hi2c1, IOEXP_ADDR, &transmit_rtd, sizeof(transmit_rtd), HAL_MAX_DELAY);

	// display button state on UI
	char button_state_str[9];
	uint8_to_binary_string(IO_state, button_state_str);
//	lv_label_set_text(ui_buttonField, button_state_str);

	FEB_CAN_ICS_Transmit_Button_State(IO_state);
}

// set nth bit
uint8_t set_n_bit(uint8_t x, uint8_t n, uint8_t bit_value) {
	return (x & (~(1 << n))) | (bit_value << n);
}

// convert uint8_t to binary string
void uint8_to_binary_string(uint8_t value, char *binary_string) {
    for (int i = 7; i >= 0; i--) {
    	binary_string[7 - i] = ((value >> i) & 1) ? '1' : '0';
    }
    binary_string[8] = '\0';
}

void disable_r2d(){
	r2d = 0;
}

bool is_r2d(){
	if(r2d ==1 ){
		return true;
	}
	return false;
}
