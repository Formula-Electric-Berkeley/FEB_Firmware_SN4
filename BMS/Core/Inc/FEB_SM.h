#ifndef INC_FEB_SM_H_
#define INC_FEB_SM_H_

// ******************************** Includes and External ********************************

#include <stdint.h>

// ******************************** Arrays and Enums ********************************

/* FEB_State_Code and FEB_States must be in sync */
typedef enum {
	FEB_STATE_STARTUP,
	FEB_STATE_IDLE,
	FEB_STATE_CHARGE,
	FEB_STATE_BALANCE,
	FEB_STATE_STANDBY,
	FEB_STATE_PRECHARGE,
	FEB_STATE_DRIVE,
	FEB_STATE_FAULT,
	FEB_STATE_DRIVE_STANDBY,
	FEB_STATE_DRIVE_REGEN
} FEB_State_Code;

// ******************************** Struct ********************************

typedef struct {
	FEB_State_Code src_state;
	uint8_t (*state_func) (void);
	FEB_State_Code dest_state;
} FEB_State_Transition_t;

// ******************************** Functions ********************************

void FEB_SM_Init();
FEB_State_Code FEB_SM_Get_Current_State();
uint8_t FEB_SM_Set_State(FEB_State_Code next_state);


#endif /* INC_FEB_SM_H_ */
