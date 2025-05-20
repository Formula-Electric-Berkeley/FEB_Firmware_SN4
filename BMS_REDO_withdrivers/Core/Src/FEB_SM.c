#include "FEB_CAN.h"
#include "FEB_HW.h"
#include "FEB_SM.h"
#include "FEB_IVT.h" //Include for precharge stuff
#include "FEB_DASH_CAN.h" //Include for r2r from DASH

//extern osMutexId_t FEB_SM_LockHandle;
//extern osMutexId_t FEB_UART_LockHandle;

extern FEB_CAN_IVT_Message_t FEB_CAN_IVT_Message;
extern FEB_CAN_DEV *FEB_CAN_NETWORK;
accumulator_t FEB_ACC = {0};
uint16_t ERROR_TYPE=0; //HEXDIGIT 1 voltage faults; HEXDIGIT 2 temp faults; HEXDIGIT 3 relay faults
/* List of transition functions*/
static void bootTransition(FEB_SM_ST_t);
static void LVPowerTransition(FEB_SM_ST_t);
static void HealthCheckTransition(FEB_SM_ST_t);
static void PrechargeTransition(FEB_SM_ST_t);
static void EnergizedTransition(FEB_SM_ST_t);
static void DriveTransition(FEB_SM_ST_t);
static void FreeTransition(FEB_SM_ST_t);
static void ChargingTransition(FEB_SM_ST_t);
static void BalanceTransition(FEB_SM_ST_t);
static void BMSFaultTransition(FEB_SM_ST_t);
static void BSPDFaultTransition(FEB_SM_ST_t);
static void IMDFaultTransition(FEB_SM_ST_t);
static void ChargingFaultTransition(FEB_SM_ST_t);

static void (*transitionVector[13])(FEB_SM_ST_t)={
		bootTransition,
		LVPowerTransition,
		HealthCheckTransition,
		PrechargeTransition,
		EnergizedTransition,
		DriveTransition,
		FreeTransition,
		ChargingTransition,
		BalanceTransition,
		BMSFaultTransition,
		BSPDFaultTransition,
		IMDFaultTransition,
		ChargingFaultTransition
};

// Shared variable, requires synchronization
static FEB_SM_ST_t SM_Current_State;

/* ******** State Transition Functions ******** */
/* State transition functions assume serial access (SM lock held). */

/* Initiate state transition. Assume SM lock held. */
static void transition(FEB_SM_ST_t next_state) {
	(transitionVector[SM_Current_State])(next_state);
	FEB_Config_Update(SM_Current_State);
}

/* ******** Interface ******** */

/* Function called from initial thread (no other threads created).
 * No synchronization needed. */
void FEB_SM_Init(void) {
	SM_Current_State=FEB_SM_ST_BOOT;
	//FEB_Config_Update(FEB_Current_State);

	// Make sure air plus, and precharge are open.
	// Set BMS Shutdown relay and bms indicator
	FEB_PIN_RST(PN_BMS_IND);
	FEB_PIN_RST(PN_PC_AIR);//FEB_Hw_Set_AIR_Plus_Relay(FEB_HW_RELAY_OPEN);
	FEB_PIN_RST(PN_PC_REL);//FEB_Hw_Set_Precharge_Relay(FEB_HW_RELAY_OPEN);
	FEB_PIN_SET(PN_BMS_SHUTDOWN);//FEB_Hw_Set_BMS_Shutdown_Relay(FEB_HW_RELAY_CLOSE);
//	FEB_PIN_SET(PN_BMS_IND);


	//FEB_CAN_Charger_Init();

	/*
	if (!FEB_CAN_Init()) {
		FEB_Current_State = FEB_SM_ST_FAULT_BMS;
		FEB_Hw_Set_BMS_Shutdown_Relay(FEB_HW_RELAY_OPEN);
		return;
	}
	*/


	//All pins initialized, transition to LV
	FEB_SM_Transition(FEB_SM_ST_LV);

}

/* Get current state of state machine. */
FEB_SM_ST_t FEB_SM_Get_Current_State(void) {
	//while (osMutexAcquire(FEB_SM_LockHandle, UINT32_MAX) != osOK);
	//FEB_SM_ST_t state = SM_Current_State;
	//osMutexRelease(FEB_SM_LockHandle);
	return SM_Current_State;
}

/* Initiate state transition. */
void FEB_SM_Transition(FEB_SM_ST_t next_state) {
	transition(next_state);
}

/* Check for conditions necessary for state transitions. */
void FEB_SM_Process(void) {

	//TODO: Add task queue
	transitionVector[FEB_SM_Get_Current_State()](FEB_SM_ST_DEFAULT);
//	FEB_SM_CAN_Transmit();

}

//FAULT HELPER FUNCTION
static void fault(FEB_SM_ST_t FAULT_TYPE) {
	SM_Current_State = FAULT_TYPE;
	if(ERROR_TYPE==0)ERROR_TYPE=0x100;
	//FEB_Config_Update(SM_Current_State);

	FEB_PIN_RST(PN_BMS_SHUTDOWN);//FEB_Hw_Set_BMS_Shutdown_Relay(FEB_RELAY_STATE_OPEN);
	//FEB_PIN_SET(PN_BUZZER);
	FEB_PIN_SET(PN_INDICATOR);

	//Delay before resetting signals
	HAL_Delay(500);

	//Reset air plus and precharge for redundancy
	FEB_PIN_RST(PN_PC_AIR); //FEB_Hw_Set_Precharge_Relay(FEB_RELAY_STATE_OPEN)
	FEB_PIN_RST(PN_PC_REL); //FEB_Hw_Set_Precharge_Relay(FEB_RELAY_STATE_OPEN);
//	FEB_Siren_Activate();

}

// This simply seems to toggle the state.
/** Transition Functions **/
static FEB_SM_ST_t updateStateProtected(FEB_SM_ST_t next_state){
	//while (osMutexAcquire(FEB_SM_LockHandle, UINT32_MAX) != osOK);
	if(SM_Current_State==FEB_SM_ST_FAULT_BMS)
		return FEB_SM_ST_FAULT_BMS;
	SM_Current_State=next_state;
	//osMutexRelease(FEB_SM_LockHandle);
	return next_state;
}

static void bootTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_LV:
		updateStateProtected(FEB_SM_ST_LV);
		break;
	case FEB_SM_ST_DEFAULT:
//		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)){
		if(FEB_PIN_RD(PN_AIRP_SENSE) == FEB_RELAY_STATE_OPEN && FEB_PIN_RD(PN_PC_REL) == FEB_RELAY_STATE_OPEN && FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_CLOSE){
//				updateStateProtected(FEB_SM_ST_PRECHARGE);
			bootTransition(FEB_SM_ST_LV);
		}
//		}

		break;
	default:
		return;
	}

}

static void LVPowerTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_HEALTH_CHECK:
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_FREE:
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_DEFAULT:
		//Make sure shutdown loop is completed before going to idle
		if (FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_CLOSE){
			LVPowerTransition(FEB_SM_ST_HEALTH_CHECK);

			//Add an else case to output that shutdown loop was not completed.
		}

		//I'm not sure if this is actually good. Maybe we should use the
		// charge sense input instead
		bool free = true;

		for ( size_t i = 0; i < FEB_NUM_CAN_DEV; ++i ) {
			free &= FEB_CAN_NETWORK[i].FAck;
		}

		if ( free ) {
			LVPowerTransition(FEB_SM_ST_FREE);
		}

		break;

	default:
		return;
	}
}

static void HealthCheckTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_LV:
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_PRECHARGE:
		//Should be open but included for redundancy
		FEB_PIN_RST(PN_PC_AIR);//FEB_Hw_Set_AIR_Plus_Relay(FEB_RELAY_STATE_OPEN);
		FEB_PIN_SET(PN_PC_REL);//FEB_Hw_Set_Precharge_Relay(FEB_RELAY_STATE_CLOSE);
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_DEFAULT:

		//Go back to LV if shutdown not completed
		//This should be a general safety check
		if(FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_OPEN)
					HealthCheckTransition(FEB_SM_ST_LV);

		// AIR minus closed, air plus opened, precharge open make sure tsms
		else if(FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_CLOSE &&
				FEB_PIN_RD(PN_AIRP_SENSE) == FEB_RELAY_STATE_OPEN &&
				FEB_PIN_RD(PN_PC_REL) == FEB_RELAY_STATE_OPEN
		){
			HealthCheckTransition(FEB_SM_ST_PRECHARGE);
		}
		break;

	default:
		return;
	}


}

static void PrechargeTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_LV:
		break;

	//TODO: Make sure this transition is needed
	case FEB_SM_ST_HEALTH_CHECK:
		FEB_PIN_RST(PN_PC_AIR);//FEB_Hw_Set_AIR_Plus_Relay(FEB_RELAY_STATE_OPEN);
		FEB_PIN_RST(PN_PC_REL);//FEB_Hw_Set_Precharge_Relay(FEB_RELAY_STATE_OPEN);
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_ENERGIZED:
		FEB_PIN_SET(PN_PC_AIR);//FEB_Hw_Set_AIR_Plus_Relay(FEB_RELAY_STATE_CLOSE);
		HAL_Delay(500);//osDelay(100);
		FEB_PIN_RST(PN_PC_REL);//FEB_Hw_Set_Precharge_Relay(FEB_RELAY_STATE_OPEN);
		updateStateProtected(FEB_SM_ST_ENERGIZED);
		break;

	case FEB_SM_ST_DEFAULT:

		if ( FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_OPEN ) {
			PrechargeTransition(FEB_SM_ST_FAULT_BMS);
		}

		//Keep air plus open for redundancy
		FEB_PIN_RST(PN_PC_AIR);

		//Hold precharge relay closed for redundancy
		FEB_PIN_SET(PN_PC_REL);

//		  TODO: Move to precharge file
		float voltage_V = (float) FEB_CAN_IVT_Message.voltage_1_mV * 0.001;
//		float target_voltage_V = FEB_ADBMS_Get_Total_Voltage() * FEB_CONST_PRECHARGE_PCT;
		if (voltage_V >= 0.9 * FEB_ACC.total_voltage_V) {
			PrechargeTransition(FEB_SM_ST_ENERGIZED);
		}


		//The conditions below are good to have just in case. Transition to energized should probably just use
		// IVT process... but I can also just move that stuff here

//		if( FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_OPEN ||
//			FEB_PIN_RD(PN_AIRM_SENSE)==FEB_RELAY_STATE_OPEN//FEB_Hw_AIR_Minus_Sense()==FEB_RELAY_STATE_OPEN
//			)PrechargeTransition(FEB_SM_ST_LV);
//		else if(0//FEB_Hw_Precharge_Sense()==FEB_RELAY_STATE_OPEN
//				)PrechargeTransition(FEB_SM_ST_ESC);
//		else if (FEB_PIN_RD(PN_AIRP_SENSE)==FEB_RELAY_STATE_CLOSE//Precharge complete
//				)PrechargeTransition(FEB_SM_ST_ENERGIZED);
		break;

	default:
		return;
	}


}

static void EnergizedTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_DRIVE:
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_LV:

	//How the fuck does that work
	case FEB_SM_ST_HEALTH_CHECK:
		FEB_PIN_RST(PN_PC_AIR);//FEB_Hw_Set_AIR_Plus_Relay(FEB_RELAY_STATE_OPEN);
		FEB_PIN_RST(PN_PC_REL);//FEB_Hw_Set_Precharge_Relay(FEB_RELAY_STATE_OPEN);
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_DEFAULT:
		if ( FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_OPEN ) {
			PrechargeTransition(FEB_SM_ST_FAULT_BMS);
		}

		if(FEB_CAN_DASH_Get_R2R()){
			EnergizedTransition(FEB_SM_ST_DRIVE);
		}

//		if( FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_OPEN||
//			FEB_PIN_RD(PN_AIRM_SENSE)==FEB_RELAY_STATE_OPEN//FEB_Hw_AIR_Minus_Sense()==FEB_RELAY_STATE_OPEN
//			)EnergizedTransition(FEB_SM_ST_LV);
//		else if(0//FEB_CAN_ICS_Ready_To_Drive() <-TODO:replace with task
//				)
//			EnergizedTransition(FEB_SM_ST_DRIVE);
		break;

	default:
		return;
	}

}

static void DriveTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_LV:
		//This transition should only happen by a fault
		break;

	case FEB_SM_ST_HEALTH_CHECK:
		FEB_PIN_RST(PN_PC_AIR);//FEB_Hw_Set_AIR_Plus_Relay(FEB_RELAY_STATE_OPEN);
		FEB_PIN_RST(PN_PC_REL);//FEB_Hw_Set_Precharge_Relay(FEB_RELAY_STATE_OPEN);
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_ENERGIZED:
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_DEFAULT:
		//If shutdown fails, or air minus is no longer connected
		if( FEB_PIN_RD(PN_SHS_IN)== FEB_RELAY_STATE_OPEN \
				|| FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_OPEN ){
			fault(FEB_SM_ST_FAULT_BMS);


		// if driver no longer requests r2r, set back to energized
		}else if(!FEB_CAN_DASH_Get_R2R()){
			//Toggle to energized
			DriveTransition(FEB_SM_ST_ENERGIZED);
		}
		break;

	default:
		return;
	}
}

static void FreeTransition(FEB_SM_ST_t next_state){
	switch(next_state){

	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
		fault(FEB_SM_ST_FAULT_CHARGING);
		break;

	case FEB_SM_ST_FREE:
	case FEB_SM_ST_LV:
		FEB_PIN_RST(PN_PC_AIR);//FEB_Hw_Set_AIR_Plus_Relay(FEB_RELAY_STATE_OPEN);
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_CHARGING:
		HAL_Delay(10);//osDelay(100);
		FEB_PIN_SET(PN_PC_AIR);//FEB_Hw_Set_AIR_Plus_Relay(FEB_RELAY_STATE_CLOSE);
		FEB_CAN_Charger_Init();
		FEB_CAN_Charger_Start_Charge();

		updateStateProtected(next_state);
		break;
	case FEB_SM_ST_BALANCE:
		updateStateProtected(next_state);
		break;
	case FEB_SM_ST_DEFAULT:
		if ( FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_CLOSE && FEB_CAN_Charger_Received() ) {
			FreeTransition(FEB_SM_ST_CHARGING);
		}

		bool free = true;

		for ( size_t i = 0; i < FEB_NUM_CAN_DEV; ++i ) {
			free &= FEB_CAN_NETWORK[i].FAck;
		}

		if ( !free ) {
			FreeTransition(FEB_SM_ST_LV);
		}
	default:
		return;
	}

}

static void ChargingTransition(FEB_SM_ST_t next_state){
	switch(next_state){

	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
		FEB_CAN_Charger_Stop_Charge();
		fault(FEB_SM_ST_FAULT_CHARGING);
		break;

	case FEB_SM_ST_LV:
	case FEB_SM_ST_FREE:
		FEB_PIN_RST(PN_PC_AIR);//FEB_Hw_Set_AIR_Plus_Relay(FEB_RELAY_STATE_OPEN);
		FEB_PIN_RST(PN_PC_REL);//FEB_Hw_Set_Precharge_Relay(FEB_RELAY_STATE_OPEN);
		FEB_CAN_Charger_Stop_Charge();
		updateStateProtected(FEB_SM_ST_FREE);
		break;

	case FEB_SM_ST_DEFAULT:
		if(FEB_PIN_RD(PN_AIRM_SENSE)==FEB_RELAY_STATE_OPEN//FEB_Hw_AIR_Minus_Sense()==FEB_RELAY_STATE_OPEN
			)
			ChargingTransition(FEB_SM_ST_FREE);
	default:
		break;
	}

}

static void BalanceTransition(FEB_SM_ST_t next_state){
	switch(next_state){

	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
		fault(FEB_SM_ST_FAULT_CHARGING);
		//osMutexRelease(FEB_SM_LockHandle);
		FEB_Stop_Balance();
		//while (osMutexAcquire(FEB_SM_LockHandle, UINT32_MAX) != osOK);
		break;

	case FEB_SM_ST_LV:
	case FEB_SM_ST_FREE:
		FEB_PIN_RST(PN_PC_AIR);//FEB_Hw_Set_AIR_Plus_Relay(FEB_RELAY_STATE_OPEN);
		FEB_PIN_RST(PN_PC_REL);//FEB_Hw_Set_Precharge_Relay(FEB_RELAY_STATE_OPEN);
		//osMutexRelease(FEB_SM_LockHandle);
		FEB_Stop_Balance();//FEB_LTC6811_Stop_Balance();
		updateStateProtected(FEB_SM_ST_FREE);
		break;

	case FEB_SM_ST_DEFAULT:
		if (FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_OPEN/*FEB_Hw_AIR_Minus_Sense()==FEB_RELAY_STATE_OPEN*/)
			BalanceTransition(FEB_SM_ST_FREE);

	default:
		break;

	}
}

/** Hard Fault Functions **/
static void BMSFaultTransition(FEB_SM_ST_t next_state){

	switch(next_state){
	case FEB_SM_ST_DEFAULT:
		// perpetually fault until cleared
		FEB_PIN_SET(PN_BMS_IND);
		fault(FEB_SM_ST_FAULT_BMS);
//		if(FEB_PIN_RD(PN_RST) == FEB_RELAY_STATE_CLOSE){
//			updateStateProtected(FEB_SM_ST_LV);
//		}
		break;

	default:
		break;
	}

}

static void BSPDFaultTransition(FEB_SM_ST_t next_state){
	if (next_state==FEB_SM_ST_DEFAULT)return;
	fault(SM_Current_State);
}
static void IMDFaultTransition(FEB_SM_ST_t next_state){
	if (next_state==FEB_SM_ST_DEFAULT)return;
	fault(SM_Current_State);
}
static void ChargingFaultTransition(FEB_SM_ST_t next_state){
	if (next_state==FEB_SM_ST_DEFAULT)return;
	FEB_PIN_SET(PN_BMS_IND);
	fault(SM_Current_State);
}



