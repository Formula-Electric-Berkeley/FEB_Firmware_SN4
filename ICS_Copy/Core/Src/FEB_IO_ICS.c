#include "FEB_IO_ICS.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart3;

#define IOEXP_ADDR ((uint16_t) 0x20)
#define BTN_HOLD_TIME ((uint32_t) 2000)
#define RTD_BUZZER_TIME ((uint32_t) 2000)
#define RTD_BUZZER_EXIT_TIME ((uint32_t)500)

static uint32_t rtd_press_start_time;
static uint32_t rtd_buzzer_start_time = 0;
static uint8_t set_rtd_buzzer = 1;
static uint8_t IO_state = 0xFF;
static uint8_t r2d = 0;

static FEB_SM_ST_t bms_state;
static FEB_SM_ST_t prev_state;
static uint8_t entered_drive = 0;
static uint8_t exited_drive = 0;
static uint32_t exit_buzzer_start_time = 0;

static uint32_t datalog_press_start_time = 0;
static uint8_t datalog_active = 0;
//static uint8_t button2_last = 0;

// **************************************** Functions ****************************************

void FEB_IO_ICS_Init(void) {
	// initialize RTD (P0)and all pins to High (there is a physical 'not gate' on the board that
	// makes it low so buzzer doesn't go peep)
	uint8_t initial_io_exp_state = 0xF;
	HAL_I2C_Master_Transmit(&hi2c1, IOEXP_ADDR << 1, &initial_io_exp_state, sizeof(initial_io_exp_state), HAL_MAX_DELAY);
	bms_state = FEB_CAN_BMS_Get_State();
}

void FEB_IO_ICS_Loop(void) {
	// receive IO expander data from I2C
	uint8_t received_data = 0x00;
	HAL_I2C_Master_Receive(&hi2c1, IOEXP_ADDR << 1, &received_data, 1, HAL_MAX_DELAY);

	IO_state = 0;
	uint8_t brake_pressure = FEB_CAN_APPS_Get_Brake_Pos();
	uint8_t inv_enabled = FEB_CAN_APPS_Get_Enabled();

	prev_state = bms_state;
	bms_state = FEB_CAN_BMS_Get_State();

	// TSSI
	if (FEB_CAN_BMS_GET_FAULTS()) {
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
	}

	// Buzzer Logic
	if (prev_state == FEB_SM_ST_ENERGIZED && bms_state == FEB_SM_ST_DRIVE) {
		entered_drive = 1;
	} else if (prev_state == FEB_SM_ST_DRIVE && bms_state == FEB_SM_ST_ENERGIZED){
		exited_drive = 1;
	}

	// R2D Button UI
	if(bms_state == FEB_SM_ST_DRIVE){
		if (inv_enabled) {
			lv_obj_set_style_bg_color(ui_ButtonRTD, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
		} else {
			lv_obj_set_style_bg_color(ui_ButtonRTD, lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT );
		}
	}else if (bms_state == FEB_SM_ST_ENERGIZED) {
		lv_obj_set_style_bg_color(ui_ButtonRTD, lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT );
	}else {
		lv_obj_set_style_bg_color(ui_ButtonRTD, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	}

	// Button 1 - Ready-to-Drive (RTD) button
	if (received_data & (1 << 1)) {
	    if ((HAL_GetTick() - rtd_press_start_time) >= BTN_HOLD_TIME &&
	        brake_pressure >= 20 &&
	        (bms_state == FEB_SM_ST_ENERGIZED || bms_state == FEB_SM_ST_DRIVE)) {

	        // Attempt to enter or exit drive
	        if (bms_state == FEB_SM_ST_ENERGIZED) {
	            r2d = 1; // Try entering Drive
	        } else if (bms_state == FEB_SM_ST_DRIVE) {
	            r2d = 0; // Try exiting Drive
	        }

	        // Send R2D over CAN
	        IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
	        FEB_CAN_ICS_Transmit_Button_State(IO_state);
	        rtd_press_start_time = HAL_GetTick(); // reset timer

	    } else {
	        IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
	    }
	} else {
	    rtd_press_start_time = HAL_GetTick();
	    IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
	}

	// Button 2 - Data Logger
	if (received_data & (1 << 2)) {
		    if ((HAL_GetTick() - datalog_press_start_time) >= BTN_HOLD_TIME) {
		    	if (datalog_active == 0){
		    		datalog_active = 1;
		    	} else {
		    		datalog_active = 0;
		    	}
		    	IO_state = (uint8_t) set_n_bit(IO_state, 2, datalog_active);
		    	datalog_press_start_time = HAL_GetTick();
		    } else {
		        IO_state = (uint8_t) set_n_bit(IO_state, 2, datalog_active);
		    }
		} else {
			datalog_press_start_time = HAL_GetTick();
		    IO_state = (uint8_t) set_n_bit(IO_state, 2, datalog_active);
		}

	if(datalog_active){
		lv_obj_set_style_bg_color(ui_ButtonDataLog, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );

	}else{
		lv_obj_set_style_bg_color(ui_ButtonDataLog, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	}

	// Button 3
	if (received_data & (1<<3)) {
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
//	uint8_t buffer[2];
//	buffer[0] = IO_state;
//	buffer[1] = '\0';  // Null termination
//	HAL_UART_Transmit(&huart3, buffer, 1, 100);

	if (entered_drive == 1 && bms_state ==  FEB_SM_ST_DRIVE && inv_enabled) {
		if (rtd_buzzer_start_time == 0) {
			rtd_buzzer_start_time = HAL_GetTick();
		}
		set_rtd_buzzer = 0;
		IO_state = set_n_bit(IO_state, 0, set_rtd_buzzer);
	} else if (exited_drive == 1 && bms_state == FEB_SM_ST_ENERGIZED && !inv_enabled) {
		if (exit_buzzer_start_time == 0) {
			exit_buzzer_start_time = HAL_GetTick();
		}
		set_rtd_buzzer = 0;
		IO_state = set_n_bit(IO_state, 0, set_rtd_buzzer);
	} else {
		set_rtd_buzzer = 1;
		IO_state = set_n_bit(IO_state, 0, set_rtd_buzzer);
	}

	if (((HAL_GetTick() - rtd_buzzer_start_time) >= RTD_BUZZER_TIME && rtd_buzzer_start_time > 0) ||
			((HAL_GetTick() - exit_buzzer_start_time) >= RTD_BUZZER_EXIT_TIME && exit_buzzer_start_time > 0))
	{

		rtd_buzzer_start_time = 0;
		exit_buzzer_start_time = 0;
		entered_drive = 0;
		exited_drive = 0;
		set_rtd_buzzer = 1;
		IO_state = set_n_bit(IO_state,0, set_rtd_buzzer);
	}

//	//r2d should trigger the color not the buzzer state.
//	if (r2d == 1){
//		//lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
//
//	}else{
//		//lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
//
//	}


	uint8_t transmit_rtd = (0b1111111 << 1) + set_rtd_buzzer;
	HAL_I2C_Master_Transmit(&hi2c1, IOEXP_ADDR << 1, &transmit_rtd, 1, HAL_MAX_DELAY); //sizeof(transmit_rtd)

	//display button state on UI
//	char button_state_str[9];
//	uint8_to_binary_string(IO_state, button_state_str);
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

void enable_r2d(){
	r2d = 1;
}

bool is_r2d(){
	if(r2d == 1 ){
		return true;
	}
	return false;
}
