// **************************************** Includes ****************************************

#include "FEB_UI.h"

// **************************************** Variables ****************************************

Screen_Info_t screen_info;

extern ICS_UI_Values_t ICS_UI_Values;

const char* BMS_STATE_LABELS[] = {"PRECHARGE", "CHARGE", "BALANCE", "DRIVE", "SHUTDOWN", "NULL"};
const int BMS_STATE_COLORS[] = {0xFAFF00, 0x33FF00, 0x00F0FF, 0xFA00FF, 0xFF0000, 0xC2C2C2};

const char* HV_STATUS_LABELS[] = {"HV OFF", "HV ON"};
const int HV_STATUS_COLORS[] = {0xFF0000, 0xFF8A00};
uint8_t iter;

// **************************************** Functions ****************************************

void FEB_UI_Init(void) {
	screen_info.test_str = "0";
	iter = 1;
	lv_init();

	screen_driver_init();
//	touch_sensor_driver_init();

	ICS_UI_Values.bms_state = 0;

	ui_init();
}

void FEB_UI_Update(void) {
	lv_task_handler();

	FEB_UI_Set_Values();
	//BMS_State_Set();

//	UI_Demo_Mode();
}

void FEB_UI_Set_Values(void) {

	//set_bms_status(ICS_UI_Values.bms_state);
	BMS_State_Set();

	//set_tsal_status(HV_STATUS_COLORS[ICS_UI_Values.ivt_voltage > 60.0]); (TODO)

//	uint8_t soc_value = abs((100 - (((int) ((ICS_UI_Values.ivt_voltage / 600.0) * 100)) % 600)) % 100);
//	set_soc_value(soc_value);

	ICS_UI_Values.pack_voltage /= 100;

	uint16_t pv = ICS_UI_Values.pack_voltage;

	if (ICS_UI_Values.pack_voltage < 350) {
		//lv_bar_set_value(ui_Bar2, 0, LV_ANIM_OFF);
	} else {
		//lv_bar_set_value(ui_Bar2, ((ICS_UI_Values.pack_voltage - 350) / 238) * 100, LV_ANIM_OFF);
	}

	ICS_UI_Values.min_voltage /= 100;

	//set_soc_value(ICS_UI_Values.min_voltage); (TODO)
	//lv_bar_set_value(ui_Bar1, ((ICS_UI_Values.min_voltage - 25) / 20) * 100, LV_ANIM_OFF);

//	if ((int) ICS_UI_Values.acc_temp % 60 < 30) ICS_UI_Values.acc_temp = 30.0;

	if (ICS_UI_Values.acc_temp == 0) {
		//lv_bar_set_value(ui_Bar3, 0, LV_ANIM_OFF);
		//set_temp_value(0); (TODO)
	} else {
		uint8_t temp_value = (int) (((((int) ICS_UI_Values.acc_temp) % 65) - 25) / 35.0 * 100.0);
		//lv_bar_set_value(ui_Bar3, temp_value, LV_ANIM_OFF);
		//set_temp_value(((int) ICS_UI_Values.acc_temp) % 65); (TODO)
	}

//	ICS_UI_Values.motor_speed /= 100;

	//set_speed_value((((ICS_UI_Values.motor_speed / 3.545) * 1.6358) / 60) * 2.237); (TODO)
}

void UI_Demo_Mode(void) {
//	set_soc_value(iter); (Commented)
//	set_temp_value(iter + 7);(Commented)
//	set_speed_value(iter + 13); (Commented)

	//lv_bar_set_value(ui_Bar1, iter, LV_ANIM_OFF);
	//lv_bar_set_value(ui_Bar3, iter + 7, LV_ANIM_OFF);

//	set_tsal_status(iter % 2); (Commented)
//	set_bms_status(iter % 6); (Commented)
//	set_regen_status(iter % 10 > 5); (Commented)

	if (iter % 10 < 5) {
		//lv_obj_set_style_bg_color(ui_TextArea1, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
		//lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
	} else {
		//lv_obj_set_style_bg_color(ui_TextArea1, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
		//lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	}

	iter++;
}

void BMS_State_Set(void) {
    FEB_SM_ST_t bms_state = FEB_CAN_BMS_Get_State();
	//FEB_SM_ST_t bms_state = FEB_SM_ST_PRECHARGE; //If you want to actually see if you change the UI.

    char* bms_str = get_bms_state_string(bms_state);
    lv_label_set_text(ui_BMStateNumerical, bms_str);

    lv_color_t bms_color;

    switch (bms_state) {
        case FEB_SM_ST_DRIVE:
        case FEB_SM_ST_ENERGIZED:
            bms_color = lv_color_hex(0x019F02); // green
            break;
        case FEB_SM_ST_PRECHARGE:
        case FEB_SM_ST_CHARGING:
            bms_color = lv_color_hex(0xFFFF00); // yellow
            break;
        case FEB_SM_ST_FAULT_BMS:
        case FEB_SM_ST_FAULT_BSPD:
        case FEB_SM_ST_FAULT_IMD:
        case FEB_SM_ST_FAULT_CHARGING:
            bms_color = lv_color_hex(0xFF0000); // red
            break;
        default:
            bms_color = lv_color_hex(0xFFFFFF); // white/default
            break;
    }

    lv_obj_set_style_text_color(ui_BMStateNumerical, bms_color, LV_PART_MAIN | LV_STATE_DEFAULT);
}

//Might need it later
char* get_bms_state_string(FEB_SM_ST_t state) {
	switch (state) {
		case FEB_SM_ST_BOOT: return "Boot";
		case FEB_SM_ST_LV_POWER: return "LV Power";
		case FEB_SM_ST_ESC_COMPLETED: return "ESC Completed";
		case FEB_SM_ST_PRECHARGE: return "Precharge";
		case FEB_SM_ST_ENERGIZED: return "Energized";
		case FEB_SM_ST_DRIVE: return "Drive";
		case FEB_SM_ST_BATTERY_FREE: return "Battery Free";
		case FEB_SM_ST_CHARGING: return "Charging";
		case FEB_SM_ST_BALANCE: return "Balance";
		case FEB_SM_ST_FAULT_BMS: return "Fault - BMS";
		case FEB_SM_ST_FAULT_BSPD: return "Fault - BSPD";
		case FEB_SM_ST_FAULT_IMD: return "Fault - IMD";
		case FEB_SM_ST_FAULT_CHARGING: return "Fault - Charging";
		default: return "Unknown";
	}
}
