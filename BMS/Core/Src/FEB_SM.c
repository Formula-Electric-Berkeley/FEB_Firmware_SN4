// ******************************** Includes ********************************

#include "FEB_SM.h"

// ******************************** State Functions ********************************

uint8_t FEB_Startup_State();
uint8_t FEB_Idle_State();
uint8_t FEB_Charge_State();
uint8_t FEB_Balance_State();
uint8_t FEB_Standby_State();
uint8_t FEB_Precharge_State();
uint8_t FEB_Drive_State();
uint8_t FEB_Fault_State();
uint8_t FEB_Drive_Standby_State();
uint8_t FEB_Drive_Regen_State();

// *****************************a** Global Variables ********************************

FEB_State_Code current_state = FEB_STATE_STARTUP;
uint8_t (*current_state_func) (void);
uint8_t num_transitions = 18;
FEB_State_Transition_t transitions[] = {
		{FEB_STATE_STARTUP, FEB_Idle_State, FEB_STATE_IDLE},
		{FEB_STATE_STARTUP, FEB_Fault_State, FEB_STATE_FAULT},

		{FEB_STATE_IDLE, FEB_Standby_State, FEB_STATE_STANDBY},
		{FEB_STATE_IDLE, FEB_Fault_State, FEB_STATE_FAULT},

		{FEB_STATE_STANDBY, FEB_Precharge_State, FEB_STATE_PRECHARGE},
		{FEB_STATE_STANDBY, FEB_Charge_State, FEB_STATE_CHARGE},
		{FEB_STATE_STANDBY, FEB_Balance_State, FEB_STATE_BALANCE},
		{FEB_STATE_STANDBY, FEB_Fault_State, FEB_STATE_FAULT},

		{FEB_STATE_CHARGE, FEB_Charge_State, FEB_STATE_STANDBY},
		{FEB_STATE_CHARGE, FEB_Fault_State, FEB_STATE_FAULT},

		{FEB_STATE_BALANCE, FEB_Charge_State, FEB_STATE_STANDBY},
		{FEB_STATE_BALANCE, FEB_Fault_State, FEB_STATE_FAULT},

		{FEB_STATE_PRECHARGE, FEB_Drive_State, FEB_STATE_DRIVE},
		{FEB_STATE_PRECHARGE, FEB_Fault_State, FEB_STATE_FAULT},

		{FEB_STATE_DRIVE, FEB_Drive_State, FEB_STATE_DRIVE_STANDBY},
		{FEB_STATE_DRIVE, FEB_Fault_State, FEB_STATE_FAULT},

		{FEB_STATE_DRIVE, FEB_Drive_State, FEB_STATE_DRIVE_REGEN},
		{FEB_STATE_DRIVE, FEB_Fault_State, FEB_STATE_FAULT},

};

// *****************************a** Functions ********************************

void FEB_SM_Init() {
	FEB_Startup_State();
}

FEB_State_Code FEB_SM_Get_Current_State() {
	return current_state;
}

uint8_t FEB_SM_Set_State(FEB_State_Code next_state) {
	if (current_state == next_state) {
		return 1;
	}
	for (uint8_t i = 0; i < num_transitions; i++) {
		if (transitions[i].src_state == current_state &&
			transitions[i].dest_state == next_state ) {
			transitions[i].state_func();
			return 1;
		}
	}
	return 0; /* Invalid Transition */
}


// ******************************** State Functions ********************************

uint8_t FEB_Startup_State() {
	/* Code Goes Here */
	return 1;
}

uint8_t FEB_Idle_State() {
	/* Code Goes Here */
	return 1;
}

uint8_t FEB_Charge_State() {
	/* Code Goes Here */
	return 1;
}

uint8_t FEB_Balance_State() {
	/* Code Goes Here */
	return 1;
}

uint8_t FEB_Standby_State() {
	/* Code Goes Here */
	return 1;
}

uint8_t FEB_Precharge_State() {
	/* Code Goes Here */
	return 1;
}

uint8_t FEB_Drive_State() {
	/* Code Goes Here */
	return 1;
}

uint8_t FEB_Fault_State() {
	/* Code Goes Here */
	return 1;
}

uint8_t FEB_Drive_Standby_State() {
	/* Code Goes Here */
	return 1;
}

uint8_t FEB_Drive_Regen_State() {
	/* Code Goes Here */
	return 1;
}
