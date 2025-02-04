#ifndef INC_FEB_ADBMS6830B_H_
#define INC_FEB_ADBMS6830B_H_

// ******************************** Includes ********************************

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "FEB_Const.h"
#include "FEB_SM.h"
#include "FEB_ADBMS6830B_Driver.h"


// ********************************** ADBMS6830B Configuration **********************************

typedef enum {
	RD_OFF = 0x00,
	RD_ON
} RD;

typedef enum {
	DCP_OFF = 0x00,
	DCP_ON
} DCP;

typedef enum {
	SINGLE = 0x00,
	CONTINUOUS
} CONT;


typedef enum {
	RSTF_OFF = 0x00,
	RSTF_ON
} RSTF;

typedef enum {
	OW_OFF_ALL_CH = 0x00,
	OW_ON_EVEN_CH,
	OW_ON_ODD_CH,
	OW_ON_ALL_CH,
} OW;

typedef enum {
	AUX_OW_OFF = 0x00,
	AUX_OW_ON
} AUX_OW;

typedef enum {
	PUP_DOWN = 0x00,
	PUP_UP
} PUP;

typedef enum {
	AUX_ALL = 0x00,
	GPIO1,
	GPIO2,
	GPIO3,
	GPIO4,
	GPIO5,
	GPIO6,
	GPIO7,
	GPIO8,
	GPIO9,
	GPIO10,
	VREF2,
	VD,
	VA,
	ITEMP,
	VPV,
	VMV,
	VRES
} AUX_CH;

// ******************************** Functions ********************************

void FEB_ADBMS_Init();
void FEB_ADBMS_AcquireData();


// ******************************** UART ********************************

void FEB_ADBMS_UART_Transmit();

#endif /* INC_FEB_ADBMS6830B_H_ */
