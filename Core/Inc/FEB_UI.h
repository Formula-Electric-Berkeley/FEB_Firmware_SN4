#ifndef INC_FEB_UI_H_
#define INC_FEB_UI_H_

// **************************************** Includes ****************************************

#include "FEB_CAN_ICS.h"

#include <stdlib.h>
#include <stdio.h>

#include "lvgl.h"
#include "screen_driver.h"
#include "touch_sensor_driver.h"
#include "ui.h"

#include "stm32f4xx_hal.h"

typedef struct {
	char * test_str;
} Screen_Info_t;

// **************************************** Functions ****************************************

void FEB_UI_Init(void);

void FEB_UI_Update(void);

void UI_Demo_Mode(void);

void FEB_UI_Set_Values(void);

#endif /* INC_FEB_UI_H_ */
