#include "../ui.h"
#include "../../Core/Inc/FEB_UI.h"

void ui_Screen1_screen_init(void) {
	ui_Screen1 = lv_obj_create(NULL);
	lv_obj_clear_flag( ui_Screen1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

	lv_obj_set_style_bg_color(ui_Screen1, lv_color_hex(0x000000), LV_PART_MAIN);

//	ui_Container2 = lv_obj_create(ui_Screen1);
//	lv_obj_remove_style_all(ui_Container2);
//	lv_obj_set_width( ui_Container2, 800);
//	lv_obj_set_height( ui_Container2, 85);
//	lv_obj_set_x( ui_Container2, 0 );
//	lv_obj_set_y( ui_Container2, 200 );
//	lv_obj_set_align( ui_Container2, LV_ALIGN_CENTER );
//	lv_obj_clear_flag( ui_Container2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
//	lv_obj_set_style_bg_color(ui_Container2, lv_color_hex(0x01FF01), LV_PART_MAIN | LV_STATE_DEFAULT );
//	lv_obj_set_style_bg_opa(ui_Container2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
//
//    img_regen_status = lv_img_create(ui_Screen1);
//    set_regen_status(0);

	ui_Bar2 = lv_bar_create(ui_Screen1);
	lv_bar_set_value(ui_Bar2,50,LV_ANIM_OFF);
	lv_bar_set_start_value(ui_Bar2, 0, LV_ANIM_OFF);
	lv_obj_set_style_base_dir(ui_Bar2, LV_BASE_DIR_RTL, 0);
	lv_obj_set_width( ui_Bar2, 800);
	lv_obj_set_height( ui_Bar2, 85);
	lv_obj_set_x( ui_Bar2, 0 );
	lv_obj_set_y( ui_Bar2, 200 );
	lv_obj_set_align( ui_Bar2, LV_ALIGN_CENTER );
	lv_obj_set_style_radius(ui_Bar2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_Bar2, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_Bar2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	lv_obj_set_style_radius(ui_Bar2, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_Bar2, lv_color_hex(0xffe100), LV_PART_INDICATOR | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_Bar2, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_img_tiled(ui_Bar2, false, LV_PART_INDICATOR| LV_STATE_DEFAULT);


	ui_Bar1 = lv_bar_create(ui_Screen1);
	lv_bar_set_value(ui_Bar1,50,LV_ANIM_OFF);
	lv_bar_set_start_value(ui_Bar1, 0, LV_ANIM_OFF);
	lv_obj_set_style_base_dir(ui_Bar1, LV_BASE_DIR_RTL, 0);
	lv_obj_set_width( ui_Bar1, 800);
	lv_obj_set_height( ui_Bar1, 55);
	lv_obj_set_x( ui_Bar1, 0 );
	lv_obj_set_y( ui_Bar1, 125 );
	lv_obj_set_align( ui_Bar1, LV_ALIGN_CENTER );
	lv_obj_set_style_radius(ui_Bar1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_Bar1, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_Bar1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	lv_obj_set_style_radius(ui_Bar1, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_Bar1, lv_color_hex(0x007BFF), LV_PART_INDICATOR | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_Bar1, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_img_tiled(ui_Bar1, false, LV_PART_INDICATOR| LV_STATE_DEFAULT);

	img_ones_place = lv_img_create(ui_Screen1);
    img_tens_place = lv_img_create(ui_Screen1);
    img_v = lv_img_create(ui_Screen1);
	set_soc_value(00);

	ui_Bar3 = lv_bar_create(ui_Screen1);
	lv_bar_set_value(ui_Bar3,95,LV_ANIM_OFF);
	lv_bar_set_start_value(ui_Bar3, 0, LV_ANIM_OFF);
	lv_obj_set_style_base_dir(ui_Bar3, LV_BASE_DIR_RTL, 0);
	lv_obj_set_width( ui_Bar3, 800);
	lv_obj_set_height( ui_Bar3, 55);
	lv_obj_set_x( ui_Bar3, 0 );
	lv_obj_set_y( ui_Bar3, 65 );
	lv_obj_set_align( ui_Bar3, LV_ALIGN_CENTER );
	lv_obj_set_style_radius(ui_Bar3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_Bar3, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_Bar3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	lv_obj_set_style_radius(ui_Bar3, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui_Bar3, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_Bar3, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_color(ui_Bar3, lv_color_hex(0xFFFA00), LV_PART_INDICATOR | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_grad_dir(ui_Bar3, LV_GRAD_DIR_HOR, LV_PART_INDICATOR| LV_STATE_DEFAULT);

	img_ones_place1 = lv_img_create(ui_Screen1);
    img_tens_place1 = lv_img_create(ui_Screen1);
    img_degrees = lv_img_create(ui_Screen1);
	set_temp_value(00);

    img_tsal_status = lv_img_create(ui_Screen1);
    set_tsal_status(0);

    img_bms_status = lv_img_create(ui_Screen1);
    set_bms_status(0);

	img_ones_place2 = lv_img_create(ui_Screen1);
    img_tens_place2 = lv_img_create(ui_Screen1);
	set_speed_value(00);

	ui_Label6 = lv_label_create(ui_Screen1);
	lv_obj_set_width( ui_Label6, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_height( ui_Label6, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_x( ui_Label6, 18 );
	lv_obj_set_y( ui_Label6, 103 );
	lv_obj_set_align( ui_Label6, LV_ALIGN_CENTER );
	lv_label_set_text(ui_Label6,".");
	lv_obj_set_style_text_color(ui_Label6, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_text_opa(ui_Label6, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui_Label6, &lv_font_montserrat_40, LV_PART_MAIN| LV_STATE_DEFAULT);

	LV_IMG_DECLARE(tsalmphbms_label);
    lv_obj_t * img_tsalmphbms_label = lv_img_create(ui_Screen1);
    lv_img_set_src(img_tsalmphbms_label, &tsalmphbms_label);
    lv_obj_align(img_tsalmphbms_label, LV_ALIGN_CENTER, 0, -60);
    lv_img_set_angle(img_tsalmphbms_label, 1800);

	ui_TextArea2 = lv_obj_create(ui_Screen1);
	lv_obj_remove_style_all(ui_TextArea2);
	lv_obj_set_width( ui_TextArea2, 100);
	lv_obj_set_height( ui_TextArea2, 75);
	lv_obj_set_x( ui_TextArea2, 250 );
	lv_obj_set_y( ui_TextArea2, -125 );
	lv_obj_set_align( ui_TextArea2, LV_ALIGN_CENTER );
	lv_obj_clear_flag( ui_TextArea2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_style_bg_color(ui_TextArea2, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_TextArea2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	ui_TextArea1 = lv_obj_create(ui_Screen1);
	lv_obj_remove_style_all(ui_TextArea1);
	lv_obj_set_width( ui_TextArea1, 100);
	lv_obj_set_height( ui_TextArea1, 75);
	lv_obj_set_x( ui_TextArea1, 85 );
	lv_obj_set_y( ui_TextArea1, -125 );
	lv_obj_set_align( ui_TextArea1, LV_ALIGN_CENTER );
	lv_obj_clear_flag( ui_TextArea1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_style_bg_color(ui_TextArea1, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_TextArea1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	ui_TextArea4 = lv_obj_create(ui_Screen1);
	lv_obj_remove_style_all(ui_TextArea4);
	lv_obj_set_width( ui_TextArea4, 100);
	lv_obj_set_height( ui_TextArea4, 75);
	lv_obj_set_x( ui_TextArea4, -85 );
	lv_obj_set_y( ui_TextArea4, -125 );
	lv_obj_set_align( ui_TextArea4, LV_ALIGN_CENTER );
	lv_obj_clear_flag( ui_TextArea4, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_style_bg_color(ui_TextArea4, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_TextArea4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

	ui_TextArea3 = lv_obj_create(ui_Screen1);
	lv_obj_remove_style_all(ui_TextArea3);
	lv_obj_set_width( ui_TextArea3, 100);
	lv_obj_set_height( ui_TextArea3, 75);
	lv_obj_set_x( ui_TextArea3, -250 );
	lv_obj_set_y( ui_TextArea3, -125 );
	lv_obj_set_align( ui_TextArea3, LV_ALIGN_CENTER );
	lv_obj_clear_flag( ui_TextArea3, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_style_bg_color(ui_TextArea3, lv_color_hex(0xFE0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	lv_obj_set_style_bg_opa(ui_TextArea3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

    LV_IMG_DECLARE(io_label);
    lv_obj_t * img_io_label = lv_img_create(ui_Screen1);
    lv_img_set_src(img_io_label, &io_label);
    lv_obj_align(img_io_label, LV_ALIGN_CENTER, 0, -125);
    lv_img_set_angle(img_io_label, 1800);
}

void set_soc_value(uint16_t value) {
	uint16_t x = value;
	uint8_t x_pos_zero = 0;
	uint8_t y_pos_zero = 125;
	uint8_t x_pos_one = 35;
	uint8_t y_pos_one = 125;

    switch (value % 10) {
    	case 1:
    	    LV_IMG_DECLARE(one);
    	    lv_img_set_src(img_ones_place, &one);
    	    lv_obj_align(img_ones_place, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place, 1800);
    		break;
    	case 2:
    	    LV_IMG_DECLARE(two);
    	    lv_img_set_src(img_ones_place, &two);
    	    lv_obj_align(img_ones_place, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place, 1800);
    		break;
    	case 3:
    	    LV_IMG_DECLARE(three);
    	    lv_img_set_src(img_ones_place, &three);
    	    lv_obj_align(img_ones_place, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place, 1800);
    		break;
    	case 4:
    	    LV_IMG_DECLARE(four);
    	    lv_img_set_src(img_ones_place, &four);
    	    lv_obj_align(img_ones_place, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place, 1800);
    		break;
    	case 5:
    	    LV_IMG_DECLARE(five);
    	    lv_img_set_src(img_ones_place, &five);
    	    lv_obj_align(img_ones_place, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place, 1800);
    		break;
    	case 6:
    	    LV_IMG_DECLARE(six);
    	    lv_img_set_src(img_ones_place, &six);
    	    lv_obj_align(img_ones_place, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place, 1800);
    		break;
    	case 7:
    	    LV_IMG_DECLARE(seven);
    	    lv_img_set_src(img_ones_place, &seven);
    	    lv_obj_align(img_ones_place, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place, 1800);
    		break;
    	case 8:
    	    LV_IMG_DECLARE(eight);
    	    lv_img_set_src(img_ones_place, &eight);
    	    lv_obj_align(img_ones_place, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place, 1800);
    		break;
    	case 9:
    	    LV_IMG_DECLARE(nine);
    	    lv_img_set_src(img_ones_place, &nine);
    	    lv_obj_align(img_ones_place, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place, 1800);
    		break;
        default:
    	    LV_IMG_DECLARE(zero);
    	    lv_img_set_src(img_ones_place, &zero);
    	    lv_obj_align(img_ones_place, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place, 1800);
    		break;
    }

    value /= 10;

    switch (value % 10) {
    	case 1:
    	    LV_IMG_DECLARE(one);
    	    lv_img_set_src(img_tens_place, &one);
    	    lv_obj_align(img_tens_place, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place, 1800);
    		break;
    	case 2:
    	    LV_IMG_DECLARE(two);
    	    lv_img_set_src(img_tens_place, &two);
    	    lv_obj_align(img_tens_place, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place, 1800);
    		break;
    	case 3:
    	    LV_IMG_DECLARE(three);
    	    lv_img_set_src(img_tens_place, &three);
    	    lv_obj_align(img_tens_place, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place, 1800);
    		break;
    	case 4:
    	    LV_IMG_DECLARE(four);
    	    lv_img_set_src(img_tens_place, &four);
    	    lv_obj_align(img_tens_place, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place, 1800);
    		break;
    	case 5:
    	    LV_IMG_DECLARE(five);
    	    lv_img_set_src(img_tens_place, &five);
    	    lv_obj_align(img_tens_place, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place, 1800);
    		break;
    	case 6:
    	    LV_IMG_DECLARE(six);
    	    lv_img_set_src(img_tens_place, &six);
    	    lv_obj_align(img_tens_place, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place, 1800);
    		break;
    	case 7:
    	    LV_IMG_DECLARE(seven);
    	    lv_img_set_src(img_tens_place, &seven);
    	    lv_obj_align(img_tens_place, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place, 1800);
    		break;
    	case 8:
    	    LV_IMG_DECLARE(eight);

    	    lv_img_set_src(img_tens_place, &eight);
    	    lv_obj_align(img_tens_place, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place, 1800);
    		break;
    	case 9:
    	    LV_IMG_DECLARE(nine);
    	    lv_img_set_src(img_tens_place, &nine);
    	    lv_obj_align(img_tens_place, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place, 1800);
    		break;
        default:
    	    LV_IMG_DECLARE(zero);
    	    lv_img_set_src(img_tens_place, &zero);
    	    lv_obj_align(img_tens_place, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place, 1800);
    		break;
    }

	lv_obj_set_x( img_ones_place, x_pos_zero );
	lv_obj_set_y( img_ones_place, y_pos_zero );
	lv_obj_set_x( img_tens_place, x_pos_one );
	lv_obj_set_y( img_tens_place, y_pos_one );

    LV_IMG_DECLARE(v);
    lv_img_set_src(img_v, &v);
    lv_obj_align(img_v, LV_ALIGN_CENTER, -30, 125);
    lv_img_set_angle(img_v, 1800);

//    LV_IMG_DECLARE(decimal);
//    lv_img_set_src(img_decimal, &decimal);
//    lv_obj_align(img_decimal, LV_ALIGN_CENTER, 15, 125);
//    lv_img_set_angle(img_decimal, 1800);
}

void set_temp_value(uint8_t value) {
	uint8_t x_pos_zero = 0;
	uint8_t y_pos_zero = 65;
	uint8_t x_pos_one = 25;
	uint8_t y_pos_one = 65;

    switch (value % 10) {
    	case 1:
    	    LV_IMG_DECLARE(one);
    	    lv_img_set_src(img_ones_place1, &one);
    	    lv_obj_align(img_ones_place1, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place1, 1800);
    		break;
    	case 2:
    	    LV_IMG_DECLARE(two);
    	    lv_img_set_src(img_ones_place1, &two);
    	    lv_obj_align(img_ones_place1, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place1, 1800);
    		break;
    	case 3:
    	    LV_IMG_DECLARE(three);
    	    lv_img_set_src(img_ones_place1, &three);
    	    lv_obj_align(img_ones_place1, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place1, 1800);
    		break;
    	case 4:
    	    LV_IMG_DECLARE(four);
    	    lv_img_set_src(img_ones_place1, &four);
    	    lv_obj_align(img_ones_place1, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place1, 1800);
    		break;
    	case 5:
    	    LV_IMG_DECLARE(five);
    	    lv_img_set_src(img_ones_place1, &five);
    	    lv_obj_align(img_ones_place1, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place1, 1800);
    		break;
    	case 6:
    	    LV_IMG_DECLARE(six);
    	    lv_img_set_src(img_ones_place1, &six);
    	    lv_obj_align(img_ones_place1, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place1, 1800);
    		break;
    	case 7:
    	    LV_IMG_DECLARE(seven);
    	    lv_img_set_src(img_ones_place1, &seven);
    	    lv_obj_align(img_ones_place1, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place1, 1800);
    		break;
    	case 8:
    	    LV_IMG_DECLARE(eight);
    	    lv_img_set_src(img_ones_place1, &eight);
    	    lv_obj_align(img_ones_place1, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place1, 1800);
    		break;
    	case 9:
    	    LV_IMG_DECLARE(nine);
    	    lv_img_set_src(img_ones_place1, &nine);
    	    lv_obj_align(img_ones_place1, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place1, 1800);
    		break;
        default:
    	    LV_IMG_DECLARE(zero);
    	    lv_img_set_src(img_ones_place1, &zero);
    	    lv_obj_align(img_ones_place1, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
    	    lv_img_set_angle(img_ones_place1, 1800);
    		break;
    }

    value /= 10;

    switch (value % 10) {
    	case 1:
    	    LV_IMG_DECLARE(one);
    	    lv_img_set_src(img_tens_place1, &one);
    	    lv_obj_align(img_tens_place1, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place1, 1800);
    		break;
    	case 2:
    	    LV_IMG_DECLARE(two);
    	    lv_img_set_src(img_tens_place1, &two);
    	    lv_obj_align(img_tens_place1, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place1, 1800);
    		break;
    	case 3:
    	    LV_IMG_DECLARE(three);
    	    lv_img_set_src(img_tens_place1, &three);
    	    lv_obj_align(img_tens_place1, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place1, 1800);
    		break;
    	case 4:
    	    LV_IMG_DECLARE(four);
    	    lv_img_set_src(img_tens_place1, &four);
    	    lv_obj_align(img_tens_place1, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place1, 1800);
    		break;
    	case 5:
    	    LV_IMG_DECLARE(five);
    	    lv_img_set_src(img_tens_place1, &five);
    	    lv_obj_align(img_tens_place1, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place1, 1800);
    		break;
    	case 6:
    	    LV_IMG_DECLARE(six);
    	    lv_img_set_src(img_tens_place1, &six);
    	    lv_obj_align(img_tens_place1, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place1, 1800);
    		break;
    	case 7:
    	    LV_IMG_DECLARE(seven);
    	    lv_img_set_src(img_tens_place1, &seven);
    	    lv_obj_align(img_tens_place1, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place1, 1800);
    		break;
    	case 8:
    	    LV_IMG_DECLARE(eight);

    	    lv_img_set_src(img_tens_place1, &eight);
    	    lv_obj_align(img_tens_place1, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place1, 1800);
    		break;
    	case 9:
    	    LV_IMG_DECLARE(nine);
    	    lv_img_set_src(img_tens_place1, &nine);
    	    lv_obj_align(img_tens_place1, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place1, 1800);
    		break;
        default:
    	    LV_IMG_DECLARE(zero);
    	    lv_img_set_src(img_tens_place1, &zero);
    	    lv_obj_align(img_tens_place1, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
    	    lv_img_set_angle(img_tens_place1, 1800);
    		break;
    }

	lv_obj_set_x( img_ones_place1, x_pos_zero );
	lv_obj_set_y( img_ones_place1, y_pos_zero );
	lv_obj_set_x( img_tens_place1, x_pos_one );
	lv_obj_set_y( img_tens_place1, y_pos_one );

    LV_IMG_DECLARE(degrees);
    lv_img_set_src(img_degrees, &degrees);
    lv_obj_align(img_degrees, LV_ALIGN_CENTER, -38, 65);
    lv_img_set_angle(img_degrees, 1800);
}

void set_speed_value(uint16_t value) {
	uint16_t x = value;
	uint8_t x_pos_zero = 0;
	uint8_t y_pos_zero = 0;
	uint8_t x_pos_one = 0;
	uint8_t y_pos_one = 0;

	switch (value % 10) {
		case 1:
			LV_IMG_DECLARE(one);
			lv_img_set_src(img_ones_place2, &one);
			lv_obj_align(img_ones_place2, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
			lv_img_set_angle(img_ones_place2, 1800);
			break;
		case 2:
			LV_IMG_DECLARE(two);
			lv_img_set_src(img_ones_place2, &two);
			lv_obj_align(img_ones_place2, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
			lv_img_set_angle(img_ones_place2, 1800);
			break;
		case 3:
			LV_IMG_DECLARE(three);
			lv_img_set_src(img_ones_place2, &three);
			lv_obj_align(img_ones_place2, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
			lv_img_set_angle(img_ones_place2, 1800);
			break;
		case 4:
			LV_IMG_DECLARE(four);
			lv_img_set_src(img_ones_place2, &four);
			lv_obj_align(img_ones_place2, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
			lv_img_set_angle(img_ones_place2, 1800);
			break;
		case 5:
			LV_IMG_DECLARE(five);
			lv_img_set_src(img_ones_place2, &five);
			lv_obj_align(img_ones_place2, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
			lv_img_set_angle(img_ones_place2, 1800);
			break;
		case 6:
			LV_IMG_DECLARE(six);
			lv_img_set_src(img_ones_place2, &six);
			lv_obj_align(img_ones_place2, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
			lv_img_set_angle(img_ones_place2, 1800);
			break;
		case 7:
			LV_IMG_DECLARE(seven);
			lv_img_set_src(img_ones_place2, &seven);
			lv_obj_align(img_ones_place2, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
			lv_img_set_angle(img_ones_place2, 1800);
			break;
		case 8:
			LV_IMG_DECLARE(eight);
			lv_img_set_src(img_ones_place2, &eight);
			lv_obj_align(img_ones_place2, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
			lv_img_set_angle(img_ones_place2, 1800);
			break;
		case 9:
			LV_IMG_DECLARE(nine);
			lv_img_set_src(img_ones_place2, &nine);
			lv_obj_align(img_ones_place2, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
			lv_img_set_angle(img_ones_place2, 1800);
			break;
		default:
			LV_IMG_DECLARE(zero);
			lv_img_set_src(img_ones_place2, &zero);
			lv_obj_align(img_ones_place2, LV_ALIGN_CENTER, x_pos_zero, y_pos_zero);
			lv_img_set_angle(img_ones_place2, 1800);
			break;
	}

	value /= 10;

	switch (value % 10) {
		case 1:
			LV_IMG_DECLARE(one);
			lv_img_set_src(img_tens_place2, &one);
			lv_obj_align(img_tens_place2, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
			lv_img_set_angle(img_tens_place2, 1800);
			break;
		case 2:
			LV_IMG_DECLARE(two);
			lv_img_set_src(img_tens_place2, &two);
			lv_obj_align(img_tens_place2, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
			lv_img_set_angle(img_tens_place2, 1800);
			break;
		case 3:
			LV_IMG_DECLARE(three);
			lv_img_set_src(img_tens_place2, &three);
			lv_obj_align(img_tens_place2, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
			lv_img_set_angle(img_tens_place2, 1800);
			break;
		case 4:
			LV_IMG_DECLARE(four);
			lv_img_set_src(img_tens_place2, &four);
			lv_obj_align(img_tens_place2, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
			lv_img_set_angle(img_tens_place2, 1800);
			break;
		case 5:
			LV_IMG_DECLARE(five);
			lv_img_set_src(img_tens_place2, &five);
			lv_obj_align(img_tens_place2, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
			lv_img_set_angle(img_tens_place2, 1800);
			break;
		case 6:
			LV_IMG_DECLARE(six);
			lv_img_set_src(img_tens_place2, &six);
			lv_obj_align(img_tens_place2, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
			lv_img_set_angle(img_tens_place2, 1800);
			break;
		case 7:
			LV_IMG_DECLARE(seven);
			lv_img_set_src(img_tens_place2, &seven);
			lv_obj_align(img_tens_place2, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
			lv_img_set_angle(img_tens_place2, 1800);
			break;
		case 8:
			LV_IMG_DECLARE(eight);

			lv_img_set_src(img_tens_place2, &eight);
			lv_obj_align(img_tens_place2, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
			lv_img_set_angle(img_tens_place2, 1800);
			break;
		case 9:
			LV_IMG_DECLARE(nine);
			lv_img_set_src(img_tens_place2, &nine);
			lv_obj_align(img_tens_place2, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
			lv_img_set_angle(img_tens_place2, 1800);
			break;
		default:
			LV_IMG_DECLARE(zero);
			lv_img_set_src(img_tens_place2, &zero);
			lv_obj_align(img_tens_place2, LV_ALIGN_CENTER, x_pos_one, y_pos_one);
			lv_img_set_angle(img_tens_place2, 1800);
			break;
	}

	lv_obj_set_x( img_ones_place2, x_pos_zero - 15 );
	lv_obj_set_y( img_ones_place2, y_pos_zero - 15 );
	lv_obj_set_x( img_tens_place2, x_pos_one + 15 );
	lv_obj_set_y( img_tens_place2, y_pos_one - 15 );

	lv_img_set_zoom(img_ones_place2, 300);
	lv_img_set_zoom(img_tens_place2, 300);
}

void set_tsal_status(uint8_t value) {
	if (value == 1) {
		LV_IMG_DECLARE(hvon);
		lv_img_set_src(img_tsal_status, &hvon);
	} else {
		LV_IMG_DECLARE(hvoff);
		lv_img_set_src(img_tsal_status, &hvoff);
	}

	lv_obj_align(img_tsal_status, LV_ALIGN_CENTER, 250, -15);
	lv_img_set_angle(img_tsal_status, 1800);
}

void set_bms_status(uint8_t value) {
	switch (value) {
		case 0:
			LV_IMG_DECLARE(STRTUP);
			lv_img_set_src(img_bms_status, &STRTUP);
			lv_img_set_zoom(img_bms_status, 220);
			break;
		case 1:
			LV_IMG_DECLARE(STDBY);
			lv_img_set_src(img_bms_status, &STDBY);
			lv_img_set_zoom(img_bms_status, 256);
			break;
		case 2:
			LV_IMG_DECLARE(balance);
			lv_img_set_src(img_bms_status, &balance);
			lv_img_set_zoom(img_bms_status, 256);
			break;
		case 3:
			LV_IMG_DECLARE(charge);
			lv_img_set_src(img_bms_status, &charge);
			lv_img_set_zoom(img_bms_status, 256);
			break;
		case 4:
			LV_IMG_DECLARE(precharge);
			lv_img_set_src(img_bms_status, &precharge);
			lv_img_set_zoom(img_bms_status, 220);
			break;
		case 5:
			LV_IMG_DECLARE(DR_STDBY);
			lv_img_set_src(img_bms_status, &DR_STDBY);
			lv_img_set_zoom(img_bms_status, 220);
			break;
		case 6:
			LV_IMG_DECLARE(drive);
			lv_img_set_src(img_bms_status, &drive);
			lv_img_set_zoom(img_bms_status, 220);
			break;
		case 7:
			LV_IMG_DECLARE(DR_REGEN);
			lv_img_set_src(img_bms_status, &DR_REGEN);
			lv_img_set_zoom(img_bms_status, 220);
			break;
		case 8:
			LV_IMG_DECLARE(FAULT);
			lv_img_set_src(img_bms_status, &FAULT);
			lv_img_set_zoom(img_bms_status, 220);
			break;

		default:
			LV_IMG_DECLARE(null);
			lv_img_set_src(img_bms_status, &null);
			lv_img_set_zoom(img_bms_status, 256);
			break;
	}

	lv_obj_align(img_bms_status, LV_ALIGN_CENTER, -250, -15);
	lv_img_set_angle(img_bms_status, 1800);
}

void set_regen_status(uint8_t value) {
	if (value == 1) {
		LV_IMG_DECLARE(regen_ready);
		lv_img_set_src(img_regen_status, &regen_ready);
		lv_obj_set_style_bg_color(ui_Container2, lv_color_hex(0x01FF01), LV_PART_MAIN | LV_STATE_DEFAULT );
	} else {
		LV_IMG_DECLARE(regen_off);
		lv_img_set_src(img_regen_status, &regen_off);
		lv_obj_set_style_bg_color(ui_Container2, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT );
	}

	lv_obj_align(img_regen_status, LV_ALIGN_CENTER, 0, 200);
	lv_img_set_angle(img_regen_status, 1800);
}
