#ifndef INC_FEB_ADBMS6830B_DRIVER_H_
#define INC_FEB_ADBMS6830B_DRIVER_H_

// ******************************** Includes ********************************
#include "FEB_CMDCODES.h"
#include "FEB_AD68xx_Interface.h"
#include "FEB_Hw.h"
// ******************************** Macros ********************************

#define NUM_RX_BYT 8
#define CFGRA 0
#define CFGRB 1
#define CELL 2
#define AUX 3
#define STAT 4

// ******************************** Struct ********************************


// ******************************** Initialization ********************************

/*!
 Helper Function to initialize the CFGR data structures
 @return void
 */
void ADBMS6830B_init_cfg(uint8_t total_ic, //!< Number of ICs in the daisy chain
                      cell_asic *ic //!< A two dimensional array that will store the data
					           );

/* Initialize the Register limits */
void ADBMS6830B_init_reg_limits(uint8_t total_ic, //The number of ICs in the system
							cell_asic *ic  //A two dimensional array where data will be written
							);

/*!
 Helper Function that resets the PEC error counters
 @return void
 */
void ADBMS6830B_reset_crc_count(uint8_t total_ic, //!< Number of ICs in the daisy chain
                             cell_asic *ic //!< A two dimensional array that will store the data
							 );

void ADBMS6830B_set_cfgr(uint8_t nIC, //!< Current IC
                      cell_asic *ic, //!< A two dimensional array that will store the data
                      bool refon,  //!< The REFON bit
                      bool cth[3], //!< The CTH bits
                      bool gpio[10],//!< The GPIO bits
                      bool dcc[12],//!< The DCC bits
					  bool dcto[4],//!< The Dcto bits
					  uint16_t uv, //!< The UV value
					  uint16_t ov  //!< The OV value
					  );

/*!
 Helper function to turn the REFON bit HIGH or LOW
 @return void
 */
void ADBMS6830B_set_cfgr_refon(uint8_t nIC, //!< Current IC
                            cell_asic *ic, //!< A two dimensional array that will store the data
                            bool refon //!< The REFON bit
							);

/*!
 Helper function to turn the ADCOPT bit HIGH or LOW
 @return void
 */
void ADBMS6830B_set_cfgr_cth(uint8_t nIC, //!< Current IC
                             cell_asic *ic, //!< A two dimensional array that will store the data
                             bool cth[3] //!< The ADCOPT bit
							 );

/*!
 Helper function to turn the GPIO bits HIGH or LOW
 @return void
 */
void ADBMS6830B_set_cfgr_gpio(uint8_t nIC, //!< Current IC
                           cell_asic *ic, //!< A two dimensional array that will store the data
                           bool gpio[10] //!< The GPIO bits
						   );

/*!
 Helper function to turn the DCC bits HIGH or LOW
 @return void
 */
void ADBMS6830B_set_cfgr_dis(uint8_t nIC, //!< Current IC
                          cell_asic *ic, //!< A two dimensional array that will store the data
                          bool dcc[12] //!< The DCC bits
						  );

/*!
 Helper function to control discharge time value
 @return void
 */
void ADBMS6830B_set_cfgr_dcto(uint8_t nIC,  //!< Current IC
						   cell_asic *ic, //!< A two dimensional array that will store the data
						   bool dcto[4] //!< The Dcto bits
						   );

/*!
 Helper function to set uv field in CFGRA register
 @return void
 */
void ADBMS6830B_set_cfgr_uv(uint8_t nIC, //!< Current IC
                         cell_asic *ic, //!< A two dimensional array that will store the data
                         uint16_t uv //!< The UV value
						 );

/*!
 Helper function to set ov field in CFGRA register
 @return void
 */
void ADBMS6830B_set_cfgr_ov(uint8_t nIC, //!< Current IC
                         cell_asic *ic, //!< A two dimensional array that will store the data
                         uint16_t ov //!< The OV value
						 );

// ******************************** Voltage ********************************

/*!
 Starts cell voltage conversion
 Starts ADC conversions of the ADBMS6830B Cpin inputs.
 The type of ADC conversion executed can be changed by setting the following parameters:
 @return void
 */
void ADBMS6830B_adcv( uint8_t RD, //ADC Mode
				   uint8_t DCP, //Discharge Permit
				   uint8_t CONT, //Cell Channels to be measured
				   uint8_t RSTF, //Reset Filter
				   uint8_t OW //Open-wire bits
                 );

/*!
  This function will block operation until the ADC has finished it's conversion
  @returns uint32_t counter The approximate time it took for the ADC function to complete.
  */
uint32_t ADBMS6830B_pollAdc();

/*!
 Reads and parses the LTC681x cell voltage registers.
 The function is used to read the cell codes of the LTC681x.
 This function will send the requested read commands parse the data and store the cell voltages in the cell_asic structure.
 @return uint8_t, PEC Status.
  0: No PEC error detected
 -1: PEC error detected, retry read
 */
uint8_t ADBMS6830B_rdcv(uint8_t total_ic, //!< The number of ICs in the system
                     cell_asic *ic //!< Array of the parsed cell codes
                    );

/*!
 Helper function that parses voltage measurement registers
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t parse_cells(uint8_t current_ic, //!< Current IC
                   uint8_t cell_reg, //!< Type of register
                   uint8_t cell_data[], //!< Unparsed data
                   uint16_t *cell_codes, //!< Parsed data
                   uint8_t *ic_pec //!< PEC error
				   );

// ******************************** Temperature Functions ********************************

/*!
 Write the ADBMS6830B CFGRA register
 This command will write the configuration registers of the LTC681xs connected in a daisy chain stack.
 The configuration is written in descending order so the last device's configuration is written first.
 @return void
 */
void ADBMS6830B_wrcfga(uint8_t total_ic, //!< The number of ICs being written to
                   cell_asic *ic //!< A two dimensional array of the configuration data that will be written
                  );

/*!
 Write the ADBMS6830B CFGRB register
 This command will write the configuration registers of the LTC681xs connected in a daisy chain stack.
 The configuration is written in descending order so the last device's configuration is written first.
 @return void
 */
void ADBMS6830B_wrcfgb(uint8_t total_ic, //!< The number of ICs being written to
                    cell_asic *ic //!< A two dimensional array of the configuration data that will be written
                   );

/* Start ADC Conversion for GPIO and Vref2  */
void ADBMS6830B_adax(uint8_t OW, //Open Wire Detection
				  uint8_t PUP, //Pull up/pull down current sources during measurement
				  uint8_t CH //GPIO Channels to be measured
				  );

/*
The function is used to read the parsed GPIO codes of the ADBMS6830B.
This function will send the requested read commands parse the data
and store the gpio voltages in a_codes variable.
*/

uint8_t ADBMS6830B_rdaux(uint8_t total_ic, // The number of ICs in the system
                     	   cell_asic *ic // Array of the parsed cell codes
                    	  );

// ******************************** Auxilary Functions ********************************
/*!
 Wake the ADBMS6830B from the sleep state
 @return void
 */
void wakeup_sleep(uint8_t total_ic); //!< Number of ICs in the daisy chain

/*!
 Helper Function that counts overall PEC errors and register/IC PEC errors
 @return void
 */
void ADBMS6830B_check_pec(uint8_t total_ic, //!< Number of ICs in the daisy chain
                       uint8_t reg, //!< Type of register
                       cell_asic *ic //!< A two dimensional array that will store the data
					   );

#endif /* INC_FEB_ADBMS6830B_DRIVER_H_ */
