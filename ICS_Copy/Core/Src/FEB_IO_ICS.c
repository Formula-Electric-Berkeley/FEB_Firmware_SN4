#include "FEB_IO_ICS.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart3;

#define IOEXP_ADDR ((uint16_t) 0x20)
#define BTN_HOLD_TIME ((uint32_t) 2000)
#define RTD_BUZZER_TIME ((uint32_t) 2000)

static uint32_t rtd_press_start_time;
static uint32_t rtd_buzzer_start_time = 0;
static uint8_t set_rtd_buzzer = 1;
static uint8_t IO_state = 0xFF;
static uint8_t r2d = 0;

static uint32_t datalog_press_start_time = 0;
static uint8_t button2_last_state = 0;
static uint8_t datalog_active = 0;
//static uint8_t button2_last = 0;

// **************************************** Functions ****************************************

void FEB_IO_ICS_Init(void) {
	// initialize RTD (P0)and all pins to High (there is a physical 'not gate' on the board that
	// makes it low so buzzer doesn't go peep)
	uint8_t initial_io_exp_state = 0xE;

	HAL_I2C_Master_Transmit(&hi2c1, IOEXP_ADDR << 1, &initial_io_exp_state, sizeof(initial_io_exp_state), HAL_MAX_DELAY);
}

void FEB_IO_ICS_Loop(void) {
	// receive IO expander data from I2C
	uint8_t received_data = 0x00;
	HAL_I2C_Master_Receive(&hi2c1, IOEXP_ADDR << 1, &received_data, 1, HAL_MAX_DELAY);

	IO_state = 0;
	uint8_t brake_pressure = FEB_CAN_APPS_Get_Brake_Pos();
	uint8_t inv_enabled = FEB_CAN_APPS_Get_Enabled();
	FEB_SM_ST_t bms_state = FEB_CAN_BMS_Get_State();
//
//	if(r2d == 1  && (!(bms_state == FEB_SM_ST_DRIVE || bms_state == FEB_SM_ST_DRIVE_REGEN) || FEB_CAN_BMS_is_stale())){
//		r2d = 0; //Reset r2d if we're no longer in drive or drive regen.
//	}
//	if (r2d == 1 && (bms_state != FEB_SM_ST_DRIVE || FEB_CAN_BMS_is_stale())) {
//		r2d = 0;
//	}

	if(r2d == 0 && bms_state == FEB_SM_ST_ENERGIZED){
		//lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT );
	}

	// Button 1 - Ready-to-Drive (RTD) button
	if ((received_data & (1 << 1))) {
	    if ((HAL_GetTick() - rtd_press_start_time) >= BTN_HOLD_TIME &&
	        brake_pressure >= 4 &&
	        (bms_state == FEB_SM_ST_ENERGIZED || bms_state == FEB_SM_ST_DRIVE)) {

	        uint8_t previous_r2d = r2d;

	        // Attempt to enter or exit drive
	        if (bms_state == FEB_SM_ST_ENERGIZED) {
	            r2d = 1; // Try entering Drive
	        } else if (bms_state == FEB_SM_ST_DRIVE) {
	            r2d = 0; // Try exiting Drive
	        }

	        // Send R2D over CAN
	        IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
	        FEB_CAN_ICS_Transmit_Button_State(IO_state);

	        // Delay for BMS to get the message
	        HAL_Delay(100);

	        // Re-read BMS state
	        bms_state = FEB_CAN_BMS_Get_State();

	        if ((previous_r2d == 0 && bms_state == FEB_SM_ST_DRIVE) ||
	            (previous_r2d == 1 && bms_state == FEB_SM_ST_ENERGIZED)) {

	            // SUCCESS: BMS accepted the change, buzz and update UI
	            set_rtd_buzzer = 0;
	            rtd_buzzer_start_time = HAL_GetTick();

	            if (r2d == 1) {
	                lv_obj_set_style_bg_color(ui_ButtonRTD, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT); // Green
	            } else {
	                lv_obj_set_style_bg_color(ui_ButtonRTD, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT); // Red
	            }

	        } else {
	            // FAILURE: BMS rejected it, revert r2d and update UI
	            r2d = previous_r2d;
	            IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
	            FEB_CAN_ICS_Transmit_Button_State(IO_state); // Re-send correct state

	            if (r2d == 1) {
	                lv_obj_set_style_bg_color(ui_ButtonRTD, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT); // Green
	            } else {
	                lv_obj_set_style_bg_color(ui_ButtonRTD, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT); // Red
	            }
	        }

	        rtd_press_start_time = HAL_GetTick(); // reset timer
	    } else {
	        IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
	    }
	} else {
	    rtd_press_start_time = HAL_GetTick();
	    IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
	}

//	if ((received_data & (1<<1))) {
//		if (((HAL_GetTick() - rtd_press_start_time) >= BTN_HOLD_TIME) &&
//		     brake_pressure >= 4 &&
//		     (bms_state == FEB_SM_ST_ENERGIZED ||
//		      bms_state == FEB_SM_ST_DRIVE)) {
//
//			//Flio ready to drive if pressed again to turn it off
//			if (r2d == 1) {
//				r2d = 0;
//			} else {
//				r2d = 1;
//			}
//
//			IO_state = (uint8_t) set_n_bit(IO_state, 1, 1);
//			if(r2d == 1){
//				set_rtd_buzzer = 0;
//			} else{
//				set_rtd_buzzer = 1;
//			}
//
//			//Restart the start time so we don't constantly toggle r2d
//			rtd_press_start_time = HAL_GetTick();
//
//			if (r2d) {
//				lv_obj_set_style_bg_color(ui_ButtonRTD, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
//			}
//			else {
//				lv_obj_set_style_bg_color(ui_ButtonRTD, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
//			}
//
//		} else {
//			IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
//		}
//	} else {
//		IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
//		rtd_press_start_time = HAL_GetTick();
//	}

//	//BUTTON 2 - Datalogger (Latest)
//	if ((received_data & (1 << 2))) { // If button 2 is currently pressed
//	    if (button2_last_state == 0) {
//	        // Only toggle if previously was not pressed
//	        datalog_active ^= 1; // Toggle ON/OFF
//
//	        // Update UI immediately
//	        if (datalog_active) {
//	            lv_obj_set_style_bg_color(ui_ButtonDataLog, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT); // Green
//	        } else {
//	            lv_obj_set_style_bg_color(ui_ButtonDataLog, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT); // Red
//	        }
//	    }
//	    button2_last_state = 1;
//	    IO_state = (uint8_t) set_n_bit(IO_state, 2, 1);
//	} else {
//	    button2_last_state = 0;
//	    IO_state = (uint8_t) set_n_bit(IO_state, 2, 0);
//	}


//	if ((received_data & (1<<2))) {
//		if (((HAL_GetTick() - datalog_press_start_time) >= BTN_HOLD_TIME)) {
//
//			//Flio ready to drive if pressed again to turn it off
//			datalog_active ^= 1;
//			IO_state = (uint8_t) set_n_bit(IO_state, 2, 1);
//
//			//Restart the start time so we don't constantly toggle r2d
//			datalog_press_start_time = HAL_GetTick();
//
//			if ( datalog_active ) {
//				lv_obj_set_style_bg_color(ui_ButtonDataLog, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
//			}
//			else {
//				lv_obj_set_style_bg_color(ui_ButtonDataLog, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
//			}
//
//		} else {
//			IO_state = (uint8_t) set_n_bit(IO_state, 2, datalog_active);
//		}
//	} else {
//		IO_state = (uint8_t) set_n_bit(IO_state, 2, datalog_active);
//		datalog_press_start_time = HAL_GetTick();
//	}

	// Button 3
	if ((received_data & (1<<3))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 3, 1);
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 3, 0);
	}

	// Switch 1 - (Coolant Pump)
	if ((received_data & (1<<5))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 5, 1);
		lv_obj_set_style_bg_color(ui_ButtonCoolPump, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 5, 0);
		lv_obj_set_style_bg_color(ui_ButtonCoolPump, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	}

	// Switch 2 - Radiator Fans
	if ((received_data & (1<<6))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 6, 1);
		lv_obj_set_style_bg_color(ui_ButtonRADFan, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT ); //ui_ButtonCoolPump
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 6, 0);
		lv_obj_set_style_bg_color(ui_ButtonRADFan, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	}

	// Switch 3 - Accumulator Fans
	if ((received_data & (1<<7))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 7, 1);
		lv_obj_set_style_bg_color(ui_ButtonAccFan, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 7, 0);
		lv_obj_set_style_bg_color(ui_ButtonAccFan, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	}


	// Handle buzzer
	uint8_t buffer[2];
	buffer[0] = IO_state;
	buffer[1] = '\0';  // Null termination

	HAL_UART_Transmit(&huart3, buffer, 1, 100);

	if (set_rtd_buzzer == 0 && (bms_state ==  FEB_SM_ST_DRIVE) && inv_enabled == 1) {
		if (rtd_buzzer_start_time == 0) {
			rtd_buzzer_start_time = HAL_GetTick();
		}
		IO_state = set_n_bit(IO_state, 0, 0);
		//lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
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
		//lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );

	}else{
		//lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );

	}


	uint8_t transmit_rtd = (0b1111111 << 1) + set_rtd_buzzer;
	HAL_I2C_Master_Transmit(&hi2c1, IOEXP_ADDR << 1, &transmit_rtd, 1, HAL_MAX_DELAY); //sizeof(transmit_rtd)

	//display button state on UI
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
