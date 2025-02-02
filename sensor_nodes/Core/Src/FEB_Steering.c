/*
 * FEB_Steering.c
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

// **************************************** Includes & External ****************************************

#include "FEB_Steering.h"

extern CAN_HandleTypeDef hcan1;
extern I2C_HandleTypeDef hi2c3;
extern TIM_HandleTypeDef htim6;

// **************************************** Configuration ****************************************

typedef struct {
	uint8_t zmco;
	uint16_t zpos;
	uint16_t mpos;
	uint16_t mang;
	uint16_t conf;
	uint8_t i2c_addr;
	uint8_t i2c_updt;
	uint16_t raw_angle;
	uint16_t angle;
	uint8_t status;
	uint8_t agc;
	uint16_t magnitude;

} AS5600L_Data;

typedef struct {
	uint32_t can_counter;
	uint16_t flags;
	uint16_t angle;

} CAN_Data;


// FEB constants
#define FEB_CAN_ID_Steer_Wheel_ENC \
								(uint16_t) 0xAA // Encoder-specific ID

// AS5600L Register Table
#define AS5600L_CONFIG_ZMCO 	(uint8_t) 0x00
#define AS5600L_CONFIG_ZPOS 	(uint8_t) 0x01 	// Start POS
#define AS5600L_CONFIG_MPOS 	(uint8_t) 0x03 	// End POS
#define AS5600L_CONFIG_MANG 	(uint8_t) 0x05 	// Angular Range (>18 deg)
#define AS5600L_CONFIG_CONF 	(uint8_t) 0x07
#define AS5600L_CONFIG_I2CADDR 	(uint8_t) 0x20
#define AS5600L_CONFIG_I2CUPDT 	(uint8_t) 0x21

#define AS5600L_OUT_ANG_RAW 	(uint8_t) 0x0C
#define AS5600L_OUT_ANG 		(uint8_t) 0x0E

#define AS5600L_STATUS_STATUS 	(uint8_t) 0x0B
#define AS5600L_STATUS_AGC 		(uint8_t) 0x1A
#define AS5600L_STATUS_MAG 		(uint8_t) 0x1B

#define AS5600L_BURN	 		(uint8_t) 0xFF


static const uint16_t conf_val = 0x0300; 				// least filtering possible

static const uint16_t conf_pm_mask 		= 0x03; 		// Power Mode mask
static const uint16_t conf_hyst_mask 	= 0x03 << 2; 	// Hysteresis mask
static const uint16_t conf_outs_mask 	= 0x03 << 4;	// Output Stage mask
static const uint16_t conf_pwmf_mask 	= 0x03 << 6; 	// PWM Frequency mask
static const uint16_t conf_sf_mask 		= 0x03 << 8; 	// Slow Filter mask
static const uint16_t conf_fth_mask 	= 0x07 << 10; 	// Fast Filter Threshold mask
static const uint16_t conf_wd_mask 		= 0x01 << 13; 	// Watch Dog mask

static const uint8_t status_mag_mask 	= 0x07 << 3; 	// Magnet Detection bits

static uint8_t conf_i2c_addr = 0x40 << 1; // 0x40 by default
static AS5600L_Data i2c_data;
static CAN_Data can_data;
static uint8_t canTx[8];


// **************************************** Functions ****************************************

bool Steer_ENC_I2C_Init(void) {
	uint8_t buf[3];
	buf[0] = AS5600L_CONFIG_CONF;
	buf[1] = (conf_val >> 8) & 0xFF;
	buf[2] = conf_val & 0xFF;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 3, HAL_MAX_DELAY) != HAL_OK ) {
		/* Error*/
	}

	memset(buf, 0, 3 * sizeof(*buf));

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
		/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[1], 2, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	uint16_t recieved_config = (uint16_t)(((uint16_t)buf[1] & 0x3F) << 8 | buf[2]);
	bool proper_init = 0;

	proper_init |= (conf_pm_mask | recieved_config) == (conf_pm_mask | conf_val);
	proper_init |= (conf_hyst_mask | recieved_config) == (conf_hyst_mask | conf_val);
	proper_init |= (conf_outs_mask | recieved_config) == (conf_outs_mask | conf_val);
	proper_init |= (conf_pwmf_mask | recieved_config) == (conf_pwmf_mask | conf_val);
	proper_init |= (conf_sf_mask | recieved_config) == (conf_sf_mask | conf_val);
	proper_init |= (conf_fth_mask | recieved_config) == 	(conf_fth_mask | conf_val);
	proper_init |= (conf_wd_mask | recieved_config) == (conf_wd_mask | conf_val);

	return proper_init;
}

void Steer_ENC_I2C_Read(void) {
	uint8_t buf[3];

	memset(buf, 0, 3 * sizeof(*buf));

	buf[0] = AS5600L_OUT_ANG;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[1], 2, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	i2c_data.angle = (uint16_t)(((uint16_t)buf[1] & 0x0F) << 8 | buf[2]);
}

void Steer_ENC_I2C_Full_Read(void) {
	// Number of bytes to fill in i2c_data plus 6 pointer changes
	uint8_t buf[6 + sizeof(i2c_data)];

	memset(buf, 0, (6 + sizeof(i2c_data)) * sizeof(*buf));

	// Make use of auto-pointer incrementing
	buf[0] = AS5600L_CONFIG_ZMCO;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[1], 9, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	// Make use of auto-pointer incrementing
	buf[10] = AS5600L_CONFIG_I2CADDR;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[11], 2, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	// Doesn't make use of pointer incrementing due to special register
	buf[13] = AS5600L_OUT_ANG_RAW;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[14], 2, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	// Doesn't make use of pointer incrementing due to special register
	buf[16] = AS5600L_OUT_ANG;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[17], 2, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	// Make use of auto-pointer incrementing
	buf[19] = AS5600L_STATUS_STATUS;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[11], 4, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	i2c_data.zmco = 		(uint8_t)	(buf[1] & 0x3);
	i2c_data.zpos = 		(uint16_t)	(((uint16_t)buf[2] & 0x0F) << 8 | buf[3]);
	i2c_data.mpos = 		(uint16_t)	(((uint16_t)buf[4] & 0x0F) << 8 | buf[5]);
	i2c_data.mang = 		(uint16_t) 	(((uint16_t)buf[6] & 0x0F) << 8 | buf[7]);
	i2c_data.conf = 		(uint16_t)	(((uint16_t)buf[8] & 0x3F) << 8 | buf[9]);
	i2c_data.i2c_addr = 	(uint8_t)	(buf[11] & 0xFE);
	i2c_data.i2c_updt = 	(uint8_t)	(buf[12] & 0xFE);
	i2c_data.raw_angle = 	(uint16_t)	(((uint16_t)buf[14] & 0x0F) << 8 | buf[15]);
	i2c_data.angle = 		(uint16_t)	(((uint16_t)buf[17] & 0x0F) << 8 | buf[18]);
	i2c_data.status =		(uint8_t)	(buf[20] & 0x38);
	i2c_data.agc =			(uint8_t)	(buf[21] & 0xFF);
	i2c_data.magnitude = 	(uint16_t)	(((uint16_t)buf[23] & 0x0F) << 8 | buf[24]);
}

void Steer_ENC_CAN_Message(uint8_t *canTx) {
	memset(canTx, 0, 8 * sizeof(*canTx));

	can_data.flags = (uint16_t)(i2c_data.status | status_mag_mask) >> 3;
	can_data.angle = i2c_data.angle;

	memcpy(canTx, &can_data, sizeof(can_data));
}

void Steer_ENC_CAN_Transmit(void) {
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;

	TxHeader.DLC = 8; // Data length
	TxHeader.IDE = CAN_ID_STD; // Standard ID
	TxHeader.RTR = CAN_RTR_DATA; // Data frame
	TxHeader.StdId = FEB_CAN_ID_Steer_Wheel_ENC; // CAN ID
	TxHeader.ExtId = 0; // Not used with standard ID

	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, canTx, &TxMailbox) != HAL_OK) {
		// Transmission request error
//		Error_Handler();
	}
}

void Steer_ENC_Main(void) {
	Steer_ENC_I2C_Read();
	Steer_ENC_CAN_Message(canTx);
	Steer_ENC_CAN_Transmit();
}


