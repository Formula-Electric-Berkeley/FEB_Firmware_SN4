// this file was left here for purposes of showing what tests were run.
// use VS code compare tool to fix any changes made
// Copy / Paste into https://www.onlinegdb.com/online_c_compiler
#define COMPILE_UNIT_TEST 0 
#define COMPILE_THROTTLE_TEST 0
#if COMPILE_UNIT_TEST


#include <stdio.h>
#include <math.h>
#include <stdint.h>
// TORQUE COMMAND PARAMS
#define INIT_VOLTAGE            242     // initial voltage of accumulator (used for capping torque commands)
#define PEAK_CURRENT            95      // max DC current (in amps) we want to pull from accumulator
#define PEAK_CURRENT_REGEN      20      // max DC current (in amps) we want to charge the accumulator
#define MAX_TORQUE              230     // New emrax datasheet says this should be 220???
#define MAX_TORQUE_REGEN        130     // User defined constant (equivalent to continuous torque) -- tune to MechE
#define RPM_TO_RAD_S            0.10472 // 2 pi / 60
#define FADE_SPEED              200.0   // About 3.5mph, don't do regen below here (rely on EEPROM/inverter control loop to properly fade)
#define REGEN_BRAKE_POS_THRESH  0.20    // Minimum brake position (as %) before we start commanding regen torque
#define START_REGEN_SOC         0.95    // State of charge at which regen starts
#define MAX_REGEN_SOC           0.8     // State of charge at which full regen is enabled
#define MAX_CELL_TEMP           45.0    // Cell temp at which we are to stop regen (consider reading this from BMS for single source of truth)
#define TEMP_FILTER_SHARPNESS   1.0     // Make notion page
#define USER_REGEN_FILTER       1.0     // Manually tune how much of our calculated regen we want

// TODO REMOVE
typedef enum
{
    FEB_SM_ST_DRIVE,
    FEB_SM_ST_DRIVE_REGEN,
    FEB_SM_ST_NO_DRIVE
}FEB_SM_ST_t;


typedef struct{
    float HV_Bus_Voltage;
    float Motor_Speed;
    float   BMS_SOC;
    float   BMS_hottest_temp;
    int     BMS_state;
    float   accPos;
    float   brkPos;
    float torque;
} SIM_STATE_S;

static SIM_STATE_S SIM_STATE = 
{
    .HV_Bus_Voltage     = 500,
    .Motor_Speed        = 500,
    .BMS_SOC            = 0.6,
    .BMS_hottest_temp   = 36,
    .BMS_state          = FEB_SM_ST_DRIVE_REGEN,
    .accPos             = 0,
    .brkPos             = 0,
    .torque             = 0,
};

#define min(x1, x2) x1 < x2 ? x1 : x2;

// Returns maximum torque available within 20A limit of the cells
// V_acc * 20 * efficiency = torque / w
float FEB_getElecMaxRegenTorque(){
	float accumulator_voltage = min(INIT_VOLTAGE, (SIM_STATE.HV_Bus_Voltage-50) / 10); // TODO: consider reading from IVT
	float motor_speed_rads = SIM_STATE.Motor_Speed * RPM_TO_RAD_S;

	float maxTorque = min(MAX_TORQUE_REGEN, (accumulator_voltage * PEAK_CURRENT_REGEN) / motor_speed_rads);
	return maxTorque;
}

// Step function at from 0 to 1 at FADE_SPEED
float FEB_regen_filter_Speed(float unfiltered_regen_torque){
    float motor_speed_rpm = SIM_STATE.Motor_Speed; // TODO 

    if (motor_speed_rpm < FADE_SPEED)
    {
        return 0;
    }
    else
    {
        return unfiltered_regen_torque;
    }
}

// Saturated linear function between
// (START_REGEN_SOC, 0) and (MAX_REGEN_SOC, 1)
float FEB_regen_filter_SOC(float unfiltered_regen_torque){
	float state_of_charge = SIM_STATE.BMS_SOC; // TODO 

	// m = (y_1 - y_0) / (x_1 - x_0)
	float slope = (1 - 0) / (MAX_REGEN_SOC - START_REGEN_SOC);
	// y - y_0 = m (x - x_0)
	float k_SOC = slope * (state_of_charge - START_REGEN_SOC);
    
	// Saturate between 0 and 1
	if (k_SOC > 1)
	{
		k_SOC = 1;
	}
	if (k_SOC < 0)
	{
		return 0;
	}
	return k_SOC * unfiltered_regen_torque; // be wary of typing (float vs int)
}

// Consider linear function similar to the above if driver doesn't like the exponential
// This will depend on how fast cells heat up
// Function with a vertical asymptote at 45 deg C
float FEB_regen_filter_Temp(float unfiltered_regen_torque){
	float hottest_cell_temp_C = SIM_STATE.BMS_hottest_temp; // TODO
	float e = 2.71828;
	float exponent = TEMP_FILTER_SHARPNESS * (hottest_cell_temp_C - MAX_CELL_TEMP);
	float k_temp = 1 - pow(e, exponent);
	if (k_temp < 0)
	{
		return 0;
	}
	return k_temp * unfiltered_regen_torque;
}

// Wrapper function for various filters
float FEB_regen_filter(float regen_torque_max){
	float filtered_regen_torque = regen_torque_max;
	filtered_regen_torque = FEB_regen_filter_Speed(filtered_regen_torque);
	filtered_regen_torque = FEB_regen_filter_SOC(filtered_regen_torque);
	filtered_regen_torque = FEB_regen_filter_Temp(filtered_regen_torque);
	return filtered_regen_torque * USER_REGEN_FILTER;
}

// Returns MAGNITUDE of regen torque
float FEB_CAN_RMS_getFilteredTorque_Regen(void){
	float present_regen_max = FEB_getElecMaxRegenTorque();
	return FEB_regen_filter(present_regen_max);
}


float FEB_CAN_RMS_getMaxTorque(void){
// 	return 20; // what the
	float accumulator_voltage = min(INIT_VOLTAGE, (SIM_STATE.HV_Bus_Voltage-50) / 10); // TODO: consider reading from IVT
	float motor_speed = SIM_STATE.Motor_Speed * RPM_TO_RAD_S;
  // If speed is less than 15, we should command max torque
  // This catches divide by 0 errors and also negative speeds (which may create very high negative torque values)
	if (motor_speed < 15) {
		return MAX_TORQUE;
	}
	float maxTorque = min(MAX_TORQUE, (accumulator_voltage * PEAK_CURRENT) / motor_speed);
	return maxTorque;
}

void FEB_CAN_RMS_Torque(void){
	FEB_SM_ST_t current_BMS_state = SIM_STATE.BMS_state;
	float accPos = SIM_STATE.accPos;
	float brkPos = SIM_STATE.brkPos;
	if (brkPos > REGEN_BRAKE_POS_THRESH) // brake identified
	{
		if ((current_BMS_state == FEB_SM_ST_DRIVE_REGEN))
		{
		    // Brake detected, regen allowed
			// Multiply by -1 to regen (opposite direction)
			SIM_STATE.torque = -1 * 10 * brkPos * FEB_CAN_RMS_getFilteredTorque_Regen();
		}
		else
		{
			// Brake detected, but regen not allowed -> command 0 torque
			SIM_STATE.torque = 0;
		}
	}
	else
	{
	    if ((current_BMS_state == FEB_SM_ST_DRIVE) ||
	        (current_BMS_state == FEB_SM_ST_DRIVE_REGEN))
	    {
	        // No braking detected, send throttle command
    		SIM_STATE.torque = 10 * accPos * FEB_CAN_RMS_getMaxTorque();
	    }
	    else
	    {
	        // No braking detected, but driving not allowed by BMS state
	        SIM_STATE.torque = 0;
	    }
	}
	// TODO::: UNCOMMENT AFTER UNIT TESTING
// 	FEB_CAN_RMS_Transmit_updateTorque();
}

#define TEST_SOC_FILTER     1
#define TEST_TEMP_FILTER    1
#define TEST_SPEED_FILTER   1
#define TEST_DRIVE_CONDS    1


int main()
{
#if TEST_SOC_FILTER
    for (float i = 1.2; i > 0.6; i -= 0.05)
    {
        SIM_STATE.BMS_SOC = i;
        printf("SOC %f: %f pct of unfiltered\n", SIM_STATE.BMS_SOC, FEB_regen_filter_SOC(100));
    }
    SIM_STATE.BMS_SOC = -0.05;
    printf("SOC %f: %f pct of unfiltered\n", -0.05, FEB_regen_filter_SOC(100));
    SIM_STATE.BMS_SOC = 0.6;
#endif // TEST_SOC_FILTER
    
#if TEST_TEMP_FILTER
    for (float i = 30; i < 50; i += 2)
    {
        SIM_STATE.BMS_hottest_temp = i;
        printf("Temp %f: %f pct of unfiltered\n", SIM_STATE.BMS_hottest_temp, FEB_regen_filter_Temp(100));
    }
    for (float i = 42; i < 46; i += 0.2)
    {
        SIM_STATE.BMS_hottest_temp = i;
        printf("Temp %f: %f pct of unfiltered\n", SIM_STATE.BMS_hottest_temp, FEB_regen_filter_Temp(100));
    }
#endif //TEST_TEMP_FILTER
    
#if TEST_SPEED_FILTER
    for (float i = -100; i < 400; i += 20)
    {
        SIM_STATE.Motor_Speed = i;
        printf("Speed %f: %f pct of unfiltered\n", SIM_STATE.Motor_Speed, FEB_regen_filter_Speed(100));
    }
#endif // TEST_SPEED_FILTER
    
#if TEST_DRIVE_CONDS
//    // Test various driving conditions
    SIM_STATE.HV_Bus_Voltage     = 500;
    SIM_STATE.Motor_Speed        = 500; 
    SIM_STATE.BMS_SOC            = 0.6;
    SIM_STATE.BMS_hottest_temp   = 36;
    SIM_STATE.BMS_state          = FEB_SM_ST_DRIVE_REGEN;
    SIM_STATE.accPos             = 0;
    SIM_STATE.brkPos             = 0;
    SIM_STATE.torque             = 0;
    
//    // Isolated accel
    printf("\nTesting isolated accel\n");
    for (float accPedal = 0; accPedal < 1; accPedal += 0.1)
    {
        SIM_STATE.accPos = accPedal;
        FEB_CAN_RMS_Torque();
        printf("accPedal: %f, Torque: %f\n", SIM_STATE.accPos, SIM_STATE.torque);
    }
    SIM_STATE.accPos = 0;
    
//    // Isolated brake
    printf("\nTesting isolated brake\n");
    for (float brkPedal = 0; brkPedal < 1; brkPedal += 0.1)
    {
        SIM_STATE.brkPos = brkPedal;
        FEB_CAN_RMS_Torque();
        printf("brkPedal: %f, Torque: %f\n", SIM_STATE.brkPos, SIM_STATE.torque);
    }
    SIM_STATE.brkPos = 0;
    
//    // brake takes over accel
    printf("\nTesting brake takes over accel\n");
    SIM_STATE.accPos = 0.4;
    for (float brkPedal = 0; brkPedal < 1; brkPedal += 0.1)
    {
        SIM_STATE.brkPos = brkPedal;
        FEB_CAN_RMS_Torque();
        printf("brkPedal: %f, Torque: %f\n", SIM_STATE.brkPos, SIM_STATE.torque);
    }
    SIM_STATE.accPos = 0;
    SIM_STATE.brkPos = 0;
    
//    // Magnitude of torque decreases with HV_Bus_Voltage
    printf("\nTesting Magnitude of torque decreases with HV_Bus_Voltage\n");
    for (float voltage = 588; voltage > 400; voltage-= 10)
    {
        SIM_STATE.HV_Bus_Voltage = voltage;
        SIM_STATE.accPos = 1;
        SIM_STATE.brkPos = 0;
        FEB_CAN_RMS_Torque();
        printf("Voltage: %f, Torque: %f\n", SIM_STATE.HV_Bus_Voltage, SIM_STATE.torque);
        SIM_STATE.brkPos = 1;
        FEB_CAN_RMS_Torque();
        printf("Voltage: %f, Torque: %f\n", SIM_STATE.HV_Bus_Voltage, SIM_STATE.torque);
    }
    SIM_STATE.HV_Bus_Voltage     = 500;
    
    // State Machine blocks regen and accel
    printf("\nTesting State Machine blocks regen/accel\n");
    SIM_STATE.accPos = 0.5;
    SIM_STATE.brkPos = 0.5;
    FEB_CAN_RMS_Torque();
    printf("Regen Allowed, brakes engaged, Torque: %f\n", SIM_STATE.torque);
    SIM_STATE.BMS_state = FEB_SM_ST_DRIVE;
    FEB_CAN_RMS_Torque();
    printf("Regen Not allowed, brakes engaged, Torque: %f\n", SIM_STATE.torque);
    SIM_STATE.BMS_state = FEB_SM_ST_NO_DRIVE;
    FEB_CAN_RMS_Torque();
    printf("Drive Not allowed, brakes engaged, Torque: %f\n", SIM_STATE.torque);
    SIM_STATE.brkPos = 0.5;
    FEB_CAN_RMS_Torque();
    printf("Drive Not allowed, brakes not engaged, accel active, Torque: %f\n", SIM_STATE.torque);
    
    // Magnitude follows expected values with Speed
    printf("\nTesting Magnitude follows expected behavior w/ respect to speed\n");
    SIM_STATE.accPos = 1;
    SIM_STATE.BMS_state = FEB_SM_ST_DRIVE_REGEN;
    for (float speed = -10; speed < 3000; speed += 10)
    {
        SIM_STATE.Motor_Speed = speed;
        SIM_STATE.brkPos = 1;
        FEB_CAN_RMS_Torque();
        printf("speed: %f, regen torque: %f\n", speed, SIM_STATE.torque);
    }
    for (float speed = -10; speed < 3000; speed += 10)
    {
        SIM_STATE.Motor_Speed = speed;
        SIM_STATE.brkPos = 0;
        FEB_CAN_RMS_Torque();
        printf("speed: %f, throttle torque: %f\n", speed, SIM_STATE.torque);
    }
    
#endif // TEST_DRIVE_CONDS
    
    
    return 0;
}
#endif // COMPILE_REGEN_TEST
#if COMPILE_THROTTLE_TEST
#include <stdio.h>

#define INIT_VOLTAGE            480     // initial voltage of accumulator (used for capping torque commands) (consider sampling when BMS samples)
#define PEAK_CURRENT            52.0      // max DC current (in amps) we want to pull from accumulator
#define PEAK_CURRENT_REGEN      18.0      // max DC current (in amps) we want to charge the accumulator
#define MAX_TORQUE              230    // New emrax datasheet says this should be 220???
#define MAX_TORQUE_REGEN        1300    // User defined constant (equivalent to continuous torque) -- tune to MechE, nominally this doesn't seem to go above 50
#define RPM_TO_RAD_S            0.10472 // 2 pi / 60
#define FADE_SPEED_RPM          200.0   // About 3.5mph, don't do regen below here (rely on EEPROM/inverter control loop to properly fade)
#define BRAKE_POSITION_THRESH   0.15    // Minimum brake position (as %) before we stop accelerating / start regen
#define START_REGEN_SOC         0.95    // State of charge at which regen starts
#define MAX_REGEN_SOC           0.8     // State of charge at which full regen is enabled
#define MAX_CELL_TEMP           45.0    // Cell temp at which we are to stop regen (consider reading this from BMS for single source of truth)
#define TEMP_FILTER_SHARPNESS   1.0     // Make notion page
#define USER_REGEN_FILTER       0.0     // Manually tune how much of our calculated regen we want
#define min(x1, x2) x1 < x2 ? x1 : x2;

float FEB_get_peak_current_delimiter(float voltage)
{
	float accumulator_voltage = voltage;
	float estimated_voltage_drop_at_peak = PEAK_CURRENT;
	float start_derating_voltage = 400.0 + PEAK_CURRENT; // Assume R_acc = 1ohm
	// Note: Comments are based on start_derating_voltage = 460V and PEAK_CURRENT = 60

	// Saturate outside of 460 and 410
	if (accumulator_voltage > start_derating_voltage)
	{
		return 1;
	}
	if (accumulator_voltage <= 410)
	{
		return (10.0 / PEAK_CURRENT); // limit to only 10A 
	}

	// TODO: add a low-pass filter on this value if speed oscillates at high power

	//   x0    y0            x1    y1
	// (460V, 60/60A) and (410V, 10/60A)

	//      m   = (        y_1           -              y_0)              / (x_1 -          x_0)
	float slope = ((10.0 / PEAK_CURRENT) - (PEAK_CURRENT / PEAK_CURRENT)) / (410.0 - (start_derating_voltage));
	//      y     =   m     (       x            -          x_0          ) + y_0
	float derater = slope * (accumulator_voltage - start_derating_voltage) + 1.0;

	return derater;   
}

float FEB_CAN_RMS_getMaxTorque(float voltage, float speed){
	// float accumulator_voltage = min(INIT_VOLTAGE, (RMS_MESSAGE.HV_Bus_Voltage-50) / 10); // TODO: consider reading from IVT
	float motor_speed = speed * RPM_TO_RAD_S;
	float peak_current_limited = PEAK_CURRENT * FEB_get_peak_current_delimiter(voltage);
	float power_capped = peak_current_limited * 400.0; // Cap power to 24kW (i.e. our min voltage)
 	// If speed is less than 15, we should command max torque
  	// This catches divide by 0 errors and also negative speeds (which may create very high negative torque values)
	if (motor_speed < 15) {
		return MAX_TORQUE;
	}
	float maxTorque = min(MAX_TORQUE, (power_capped) / motor_speed);
	return maxTorque;
}

int main()
{
    for (float v = 605; v > 375; v -= 10)
    {
        for (float s = -100; s < 4000; s+=100)
        {
            printf("%f Volts, %f RPM -> %fA from accumulator\n", v, s, FEB_CAN_RMS_getMaxTorque(v,s) * (s * RPM_TO_RAD_S) / v);    
        }
    }
    
    return 0;
}
#endif // COMPILE_THROTTLE_TEST
