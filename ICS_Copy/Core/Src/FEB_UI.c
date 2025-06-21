// **************************************** Includes ****************************************

#include "FEB_UI.h"

// **************************************** Variables ****************************************

Screen_Info_t screen_info;

extern ICS_UI_Values_t ICS_UI_Values;
extern uint16_t lv_voltage;

//const char* BMS_STATE_LABELS[] = {"PRECHARGE", "CHARGE", "BALANCE", "DRIVE", "SHUTDOWN", "NULL"};
//const int BMS_STATE_COLORS[] = {0xFAFF00, 0x33FF00, 0x00F0FF, 0xFA00FF, 0xFF0000, 0xC2C2C2};

const char* HV_STATUS_LABELS[] = {"HV OFF", "HV ON"};
const int HV_STATUS_COLORS[] = {0xFF0000, 0xFF8A00};
uint8_t iter;

#define NUM_SOC_POINTS 21

typedef struct {
    uint8_t soc;
    float voltage;
} SOC_Voltage_Lookup;

SOC_Voltage_Lookup soc_voltage_table[NUM_SOC_POINTS] = {
    {100, 4.10},
    {95, 4.00},
    {90, 3.97},
    {85, 3.95},
    {80, 3.92},
    {75, 3.85},
    {70, 3.81},
    {65, 3.77},
    {60, 3.73},
    {55, 3.70},
    {50, 3.64},
    {45, 3.60},
    {40, 3.55},
    {35, 3.50},
    {30, 3.45},
    {25, 3.40},
    {20, 3.34},
    {15, 3.25},
    {10, 3.1},
    {5,  2.95},
    {0,  2.80}
};

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

	//ICS_UI_Values.pack_voltage /= 100;

	//uint16_t pv = ICS_UI_Values.pack_voltage;

//	if (ICS_UI_Values.pack_voltage < 350) {
//		//lv_bar_set_value(ui_Bar2, 0, LV_ANIM_OFF);
//	} else {
//		//lv_bar_set_value(ui_Bar2, ((ICS_UI_Values.pack_voltage - 350) / 238) * 100, LV_ANIM_OFF);
//	}

	//ICS_UI_Values.min_voltage /= 100;

	//set_soc_value(ICS_UI_Values.min_voltage); (TODO)
	//lv_bar_set_value(ui_Bar1, ((ICS_UI_Values.min_voltage - 25) / 20) * 100, LV_ANIM_OFF);

//	if ((int) ICS_UI_Values.acc_temp % 60 < 30) ICS_UI_Values.acc_temp = 30.0;

//	if (ICS_UI_Values.acc_temp == 0) {
//		//lv_bar_set_value(ui_Bar3, 0, LV_ANIM_OFF);
//		//set_temp_value(0); (TODO)
//	} else {
//		uint8_t temp_value = (int) (((((int) ICS_UI_Values.acc_temp) % 65) - 25) / 35.0 * 100.0);
//		//lv_bar_set_value(ui_Bar3, temp_value, LV_ANIM_OFF);
//		//set_temp_value(((int) ICS_UI_Values.acc_temp) % 65); (TODO)
//	}

	SOC_Set_Value(ICS_UI_Values.pack_voltage, ICS_UI_Values.min_voltage);
	TEMP_Set_Value(ICS_UI_Values.max_acc_temp);
//	ICS_UI_Values.motor_speed /= 100;
	//set_speed_value((((ICS_UI_Values.motor_speed / 3.545) * 1.6358) / 60) * 2.237); (TODO)
	SPEED_Set_Value(ICS_UI_Values.motor_speed);
		}

// **************************************** Helper Functions ****************************************
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
		case FEB_SM_ST_LV: return "LV Power";
		case FEB_SM_ST_HEALTH_CHECK: return "Health Check";
		case FEB_SM_ST_PRECHARGE: return "Precharge";
		case FEB_SM_ST_ENERGIZED: return "Energized";
		case FEB_SM_ST_DRIVE: return "Drive";
		case FEB_SM_ST_FREE: return "Battery Free";
		case FEB_SM_ST_CHARGER_PRECHARGE: return "Precharge";
		case FEB_SM_ST_CHARGING: return "Charging";
		case FEB_SM_ST_BALANCE: return "Balance";
		case FEB_SM_ST_FAULT_BMS: return "Fault - BMS";
		case FEB_SM_ST_FAULT_BSPD: return "Fault - BSPD";
		case FEB_SM_ST_FAULT_IMD: return "Fault - IMD";
		case FEB_SM_ST_FAULT_CHARGING: return "Fault - Charging";
		case FEB_SM_ST_DEFAULT: return "Default";
		default: return "Unknown";
	}
}

// Temp Gradient: Yellow (low) to Red (high)
static lv_color_t get_temp_gradient_color(uint8_t value) {
    uint8_t r = 255;
    uint8_t g = 255 - (value * 255 / 100);  // fade green out
    return lv_color_make(r, g, 0);
}

// SoC Gradient: Red (low) to Green (high)
static lv_color_t get_soc_gradient_color(uint8_t value) {
    uint8_t r = 255 - (value * 255 / 100);
    uint8_t g = value * 255 / 100;
    return lv_color_make(r, g, 0);
}

void SOC_Set_Value(float ivt_voltage, float min_cell_voltage) {
    // Calculate SoC using your original logic
    // Option 1: Based on ivt_voltage
    //uint8_t soc_value = abs((100 - (((int)((ivt_voltage / 600.0) * 100)) % 600)) % 100);

    // Option 2: Based on min cell voltage (commented out)
    // uint8_t soc_value = (uint8_t)(((min_cell_voltage - 25) / 20.0) * 100.0);

	// Option 3: Linear (TERRIBLE) (avg_cell_voltage-cell_min) / (cell_max - cell_min)
	// float avg_cell_voltage = ivt_voltage / 140;
	// float soc_f = (avg_cell_voltage - 250) / (420 - 250) * 100;

	// if (soc_f > 100.0) soc_f = 100.0;
	// if (soc_f < 0.0) soc_f = 0.0;

    // uint8_t soc_value = (uint8_t) (soc_f + 0.5f);

    // Option 3: Lookup Table
    float avg_cell_voltage = ICS_UI_Values.pack_voltage;
    float min_voltage = 390.0f;
    float max_voltage = 588.0f;

    uint8_t soc_value = (avg_cell_voltage - min_voltage) / (max_voltage - min_voltage);

//    uint8_t soc_value = lookup_soc_from_voltage(avg_cell_voltage);

    // Update UI
    char soc_label[10];
    sprintf(soc_label, "%d%%", soc_value);
    lv_label_set_text(ui_PackVoltageText, soc_label);

    lv_bar_set_value(ui_PackNumerical, soc_value, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(ui_hvSoC, get_soc_gradient_color(soc_value), LV_PART_INDICATOR | LV_STATE_DEFAULT);
}

void LV_Set_Value(void) {
    // Calculate SoC using your original logic
    // Option 1: Based on ivt_voltage
    //uint8_t soc_value = abs((100 - (((int)((ivt_voltage / 600.0) * 100)) % 600)) % 100);

    // Option 2: Based on min cell voltage (commented out)
    // uint8_t soc_value = (uint8_t)(((min_cell_voltage - 25) / 20.0) * 100.0);

	// Option 3: Linear (TERRIBLE) (avg_cell_voltage-cell_min) / (cell_max - cell_min)
	// float avg_cell_voltage = ivt_voltage / 140;
	// float soc_f = (avg_cell_voltage - 250) / (420 - 250) * 100;

	// if (soc_f > 100.0) soc_f = 100.0;
	// if (soc_f < 0.0) soc_f = 0.0;

    // uint8_t soc_value = (uint8_t) (soc_f + 0.5f);

    // Option 3: Lookup Table
    float avg_voltage = lv_voltage;
    float min_voltage = 21.0f;
    float max_voltage = 27.0f;

    float soc_value = (avg_voltage - min_voltage) / (max_voltage - min_voltage);

//    uint8_t soc_value = lookup_soc_from_voltage(avg_cell_voltage);

    // Update UI
    char soc_label[32];
    sprintf(soc_label, "%.1f V", soc_value);

    lv_label_set_text(ui_LVText1, soc_label);

//    lv_bar_set_value(ui_LVNumerical, soc_value, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(ui_lvSoC, get_soc_gradient_color(soc_value), LV_PART_INDICATOR | LV_STATE_DEFAULT);
}

uint8_t lookup_soc_from_voltage(float voltage) {
    for (int i = 0; i < NUM_SOC_POINTS - 1; i++) {
        if (voltage <= soc_voltage_table[i].voltage && voltage > soc_voltage_table[i+1].voltage) {
            float v1 = soc_voltage_table[i].voltage;
            float v2 = soc_voltage_table[i+1].voltage;
            uint8_t soc1 = soc_voltage_table[i].soc;
            uint8_t soc2 = soc_voltage_table[i+1].soc;

            float soc = soc1 + (voltage - v1) * (soc2 - soc1) / (v2 - v1);
            return (uint8_t)(soc + 0.5f);
        }
    }

    if (voltage > soc_voltage_table[0].voltage) {
        return soc_voltage_table[0].soc;
    } else {
        return soc_voltage_table[NUM_SOC_POINTS-1].soc;
    }
}

void TEMP_Set_Value(float max_acc_temp) {
    // Clamp temperature to expected range
	float max_temp_C = max_acc_temp / 100.0f;
    int max_temp = (int)(max_temp_C);
    // if (max_temp % 60 < 30) max_temp = 30.0;

    // Scale: 0–60°C and 0–100%
    uint8_t temp_value = (uint8_t)(((fminf(max_temp, 60.0f)) / 60.0f) * 100.0f);

    if (temp_value > 100) temp_value = 100;

    // Update UI
    char temp_label[10];
    sprintf(temp_label, "%d°C", (int)max_temp);
    lv_label_set_text(ui_tempNumerical, temp_label);

    lv_bar_set_value(ui_BarTemp, temp_value, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(ui_BarTemp, get_temp_gradient_color(temp_value), LV_PART_INDICATOR | LV_STATE_DEFAULT);
}

void SPEED_Set_Value(float motor_speed_rpm) {
    // Convert RPM to MPH using the provided formula
    float mph = (((motor_speed_rpm / 3.545f) * 1.6358f) / 60.0f) * 2.237f;

    // Clamp to 3-digit value for display
    if (mph < 0) mph = 0;
    if (mph > 999) mph = 999;

    // Format and update label
    char speed_str[10];
    sprintf(speed_str, "%d", (int)(mph + 0.5f));  // Round to nearest int
    lv_label_set_text(ui_speedNumerical, speed_str);
}



//void UI_Demo_Mode(void) {
////	set_soc_value(iter); (Commented)
////	set_temp_value(iter + 7);(Commented)
////	set_speed_value(iter + 13); (Commented)
//
//	//lv_bar_set_value(ui_Bar1, iter, LV_ANIM_OFF);
//	//lv_bar_set_value(ui_Bar3, iter + 7, LV_ANIM_OFF);
//
////	set_tsal_status(iter % 2); (Commented)
////	set_bms_status(iter % 6); (Commented)
////	set_regen_status(iter % 10 > 5); (Commented)
//
//	if (iter % 10 < 5) {
//		//lv_obj_set_style_bg_color(ui_TextArea1, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
//		//lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0x019F02), LV_PART_MAIN | LV_STATE_DEFAULT );
//	} else {
//		//lv_obj_set_style_bg_color(ui_TextArea1, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
//		//lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
//	}
//
//	iter++;
//}
