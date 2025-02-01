#include "FEB_ADBMS6830B_Driver.h"

/* Helper function to initialize CFG variables */
void ADBMS6830B_init_cfg(uint8_t total_ic, //Number of ICs in the system
					  cell_asic *ic //A two dimensional array that stores the data
					  )
{
	for (uint8_t current_ic = 0; current_ic<total_ic;current_ic++)
	{
		for (int j =0; j<6; j++)
		{
		  ic[current_ic].configa.tx_data[j] = 0;
          ic[current_ic].configb.tx_data[j] = 0;
		}
	}
}

/* Helper Function to reset PEC counters */
void ADBMS6830B_reset_crc_count(uint8_t total_ic, //Number of ICs in the system
							 cell_asic *ic //A two dimensional array that stores the data
							 )
{
	for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
	{
		ic[current_ic].crc_count.pec_count = 0;
		ic[current_ic].crc_count.cfgr_pec = 0;
		for (int i=0; i<6; i++)
		{
			ic[current_ic].crc_count.cell_pec[i]=0;

		}
		for (int i=0; i<4; i++)
		{
			ic[current_ic].crc_count.aux_pec[i]=0;
		}
		for (int i=0; i<2; i++)
		{
			ic[current_ic].crc_count.stat_pec[i]=0;
		}
	}
}

/* Helper function to set CFGR variable */
void ADBMS6830B_set_cfgr(uint8_t nIC, // Current IC
					 cell_asic *ic, // A two dimensional array that stores the data
					 bool refon, // The REFON bit
					 bool cth[3], // The CTH bits
					 bool gpio[10], // The GPIO bits
					 bool dcc[12], // The DCC bits
					 bool dcto[4], // The Dcto bits
					 uint16_t uv, // The UV value
					 uint16_t  ov // The OV value
					 )
{
	ADBMS6830B_set_cfgr_refon(nIC,ic,refon);
	ADBMS6830B_set_cfgr_cth(nIC,ic,cth);
	ADBMS6830B_set_cfgr_gpio(nIC,ic,gpio);
	ADBMS6830B_set_cfgr_dis(nIC,ic,dcc);
	ADBMS6830B_set_cfgr_dcto(nIC,ic,dcto);
	ADBMS6830B_set_cfgr_uv(nIC, ic, uv);
	ADBMS6830B_set_cfgr_ov(nIC, ic, ov);
}

/* Helper function to set the REFON bit */
void ADBMS6830B_set_cfgr_refon(uint8_t nIC, cell_asic *ic, bool refon)
{
	if (refon) ic[nIC].configa.tx_data[0] = ic[nIC].configa.tx_data[0]|0x80;
	else ic[nIC].configa.tx_data[0] = ic[nIC].configa.tx_data[0]&0x7F;
}

/* Helper function to set CTH bits */
void ADBMS6830B_set_cfgr_cth(uint8_t nIC, cell_asic *ic, bool cth[3])
{
	for (int i = 0; i < 3; i++) {
        if (cth[i]) {
            ic[nIC].configa.tx_data[0] = ic[nIC].configa.tx_data[0] | (0b01 << i);
        } else {
            ic[nIC].configa.tx_data[0] = ic[nIC].configa.tx_data[0] & ~(0b01 << i);
        }
    }
}

/* Helper function to set GPIO bits */
void ADBMS6830B_set_cfgr_gpio(uint8_t nIC, cell_asic *ic,bool gpio[10])
{
	for (int i = 0; i < 8; i++) {
		if (gpio[i]) {
            ic[nIC].configa.tx_data[3] = ic[nIC].configa.tx_data[3] | (0b01 << i);
        } else {
             ic[nIC].configa.tx_data[3] = ic[nIC].configa.tx_data[3] & ~(0b01 << i);
        }
	}

    for (int i = 0; i < 2; i++) {
        if (gpio[i + 8]) {
            ic[nIC].configa.tx_data[4] = ic[nIC].configa.tx_data[4] | (0b01 << i);
        } else {
             ic[nIC].configa.tx_data[4] = ic[nIC].configa.tx_data[4] & ~(0b01 << i);
        }
    }
}

/* Helper function to control discharge */
void ADBMS6830B_set_cfgr_dis(uint8_t nIC, cell_asic *ic, bool dcc[12])
{
	for (int i = 0; i < 8; i++) {
		if (dcc[i]) {
            ic[nIC].configb.tx_data[4] = ic[nIC].configb.tx_data[4] | (0b01 << i);
        } else {
             ic[nIC].configb.tx_data[4] = ic[nIC].configb.tx_data[4] & ~(0b01 << i);
        }
	}

    for (int i = 0; i < 8; i++) {
        if (dcc[i + 8]) {
            ic[nIC].configb.tx_data[5] = ic[nIC].configb.tx_data[5] | (0b01 << i);
        } else {
             ic[nIC].configb.tx_data[5] = ic[nIC].configb.tx_data[5] & ~(0b01 << i);
        }
    }
}

/* Helper function to control discharge time value */
void ADBMS6830B_set_cfgr_dcto(uint8_t nIC, cell_asic *ic, bool dcto[4])
{
	for (int i = 0; i < 6; i++) {
		if (dcto[i]) {
            ic[nIC].configb.tx_data[3] = ic[nIC].configb.tx_data[3] | (0b01 << i);
        } else {
             ic[nIC].configb.tx_data[3] = ic[nIC].configb.tx_data[3] & ~(0b01 << i);
        }
	}
}

/* Helper Function to set UV value in CFG register */
void ADBMS6830B_set_cfgr_uv(uint8_t nIC, cell_asic *ic, uint16_t uv)
{
	uint16_t tmp = (uv / 16) - 1;
	ic[nIC].configb.tx_data[0] = 0x00FF & tmp;
	ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] & 0xF0;
	ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] | ((0x0F00 & tmp) >> 8);
}

/* Helper function to set OV value in CFG register */
void ADBMS6830B_set_cfgr_ov(uint8_t nIC, cell_asic *ic, uint16_t ov)
{
	uint16_t tmp = (ov / 16);
	ic[nIC].configb.tx_data[2] = 0x00FF & (tmp >> 4);
	ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] & 0x0F;
	ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] | ((0x000F & tmp) << 4);
}

/* Initialize the Register limits */
void ADBMS6830B_init_reg_limits(uint8_t total_ic, //The number of ICs in the system
							cell_asic *ic  //A two dimensional array where data will be written
							)
{
  for (uint8_t cic = 0; cic < total_ic; cic++) {
    ic[cic].ic_reg.cell_channels = 20;
    ic[cic].ic_reg.stat_channels = 4; //Doesn't matter?
    ic[cic].ic_reg.aux_channels = 6; //Doesn't matter?
    ic[cic].ic_reg.num_cv_reg = 6;
    ic[cic].ic_reg.num_gpio_reg = 4;
    ic[cic].ic_reg.num_stat_reg = 5;
  }
}


// ******************************** Voltage ********************************

/* Starts ADC conversion for cell voltage */
void ADBMS6830B_adcv( uint8_t RD, //ADC Mode
				   uint8_t DCP, //Discharge Permit
				   uint8_t CONT, //Cell Channels to be measured
				   uint8_t RSTF, //Reset Filter
				   uint8_t OW //Open-wire bits
                 )
{
	uint8_t cmd[2];

	cmd[0] = 0x02 + RD;
	cmd[1] = 0x60 | (CONT << 7) | (DCP << 4) | (RSTF << 2) | OW;

	cmd_68(cmd);
}

/* This function will block operation until the ADC has finished it's conversion */
uint32_t ADBMS6830B_pollAdc()
{
	uint32_t counter = 0;
	uint8_t finished = 0;
	uint8_t current_time = 0;
	uint8_t cmd[4];
	uint16_t cmd_pec;
	/*
	cmd[0] = 0x07;
	cmd[1] = 0x18;
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t) (cmd_pec >> 8);
	cmd[3] = (uint8_t) (cmd_pec);
	*/
	FEB_cs_low();
	FEB_spi_write_array(4, cmd);

	while ((counter < 200000) && (finished == 0))
	{
		current_time = FEB_spi_read_byte(0xff);
		if (current_time > 0)
		{
			finished = 1;
		}
		else
		{
			counter = counter + 10;
		}
	}

	FEB_cs_high();

	return(counter);
}

/*
Reads and parses the ADBMS6830B cell voltage registers.
The function is used to read the parsed Cell voltages codes of the LTC681x.
This function will send the requested read commands parse the data
and store the cell voltages in c_codes variable.
*/
uint8_t ADBMS6830B_rdcv(uint8_t total_ic, // The number of ICs in the system
                     	   cell_asic *ic // Array of the parsed cell codes
                    	  )
{
	/* OLD CODE
	 * //parse data
		int8_t c_ic=0;
		for (int curr_ic = 0; curr_ic < total_ic; curr_ic++) {
			if (ic->isospi_reverse == false) {
				c_ic = curr_ic;
			} else {
				c_ic = total_ic - curr_ic - 1;
			}
			//pec_error += parse_cells(c_ic, CELL, cell_data, &ic[c_ic].cells.c_codes[0], &ic[c_ic].cells.pec_match[0]);
		}*/
	uint8_t *cell_data;
	cell_data = (uint8_t *) malloc(34 * sizeof(uint8_t));
	transmitCMDR(RDACALL,cell_data,34);
	memcpy(&(ic->cells.c_codes),cell_data,(size_t)32);
	uint16_t data_pec=pec10_calc(32,cell_data);
	uint16_t rx_pec=*(uint16_t*)(cell_data+32);
	free(cell_data);
	return(data_pec!=rx_pec);
}

/* Helper function that parses voltage measurement registers */
int8_t parse_cells(uint8_t current_ic, // Current IC
					uint8_t cell_reg,  // Type of register
					uint8_t cell_data[], // Unparsed data
					uint16_t *cell_codes, // Parsed data
					uint8_t *ic_pec // PEC error
					)
{
	const uint8_t BYT_IN_REG = 6;
	const uint8_t CELL_IN_REG = 3;
	int8_t pec_error = 0;
	uint16_t parsed_cell;
	uint16_t received_pec;
	uint16_t data_pec;
	uint8_t data_counter = current_ic * NUM_RX_BYT; //data counter


	for (uint8_t current_cell = 0; current_cell < CELL_IN_REG; current_cell++) // This loop parses the read back data into the register codes, it
	{																		// loops once for each of the 3 codes in the register

		parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);//Each code is received as two bytes and is combined to
																				   // create the parsed code
		cell_codes[current_cell + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;

		data_counter = data_counter + 2;                       //Because the codes are two bytes, the data counter
															  //must increment by two for each parsed code
	}
	received_pec = ((cell_data[data_counter] & 0x03) << 8) | cell_data[data_counter + 1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
																			   //after the 6 cell voltage data bytes. Command counter is first 6 bits of first byte, which we don't care bout, so we do & 0x03
	data_pec = pec10_calc(BYT_IN_REG, &cell_data[(current_ic) * NUM_RX_BYT]);

	if (received_pec != data_pec)
	{
		pec_error = 1;                             //The pec_error variable is simply set negative if any PEC errors
		ic_pec[cell_reg-1]=1;
	}
	else
	{
		ic_pec[cell_reg-1]=0;
	}
	data_counter=data_counter+2;

	return(pec_error);
}

// ******************************** Temperature ********************************

/* Write the ADBMS6830B CFGRA */
void ADBMS6830B_wrcfga(uint8_t total_ic, //The number of ICs being written to
                   cell_asic ic[]  // A two dimensional array of the configuration data that will be written
                  )
{
	uint8_t cmd[2] = {0x00 , 0x01} ;
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;

	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ic->isospi_reverse == false)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}

		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = ic[c_ic].configa.tx_data[data];
			write_count++;
		}
	}
	write_68(total_ic, cmd, write_buffer);
}

/* Write the ADBMS6830B CFGRB */
void ADBMS6830B_wrcfgb(uint8_t total_ic, //The number of ICs being written to
                    cell_asic ic[] // A two dimensional array of the configuration data that will be written
                   )
{
	uint8_t cmd[2] = {0x00 , 0x24} ;
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;

	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ic->isospi_reverse == false)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}

		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = ic[c_ic].configb.tx_data[data];
			write_count++;
		}
	}
	write_68(total_ic, cmd, write_buffer);
}

/* Start ADC Conversion for GPIO and Vref2  */
void ADBMS6830B_adax(uint8_t OW, //Open Wire Detection
				  uint8_t PUP, //Pull up/pull down current sources during measurement
				  uint8_t CH //GPIO Channels to be measured
				  )
{
	uint8_t cmd[4];

	cmd[0] = OW + 0x04;
	cmd[1] = (PUP << 7) + ((CH & 0x10) << 2) + (CH & 0xF) + 0x10;

	cmd_68(cmd);
}

/*
The function is used to read the  parsed GPIO codes of the ADBMS6830B.
This function will send the requested read commands parse the data
and store the gpio voltages in a_codes variable.
*/

uint8_t ADBMS6830B_rdaux(uint8_t total_ic, // The number of ICs in the system
                     	   cell_asic *ic // Array of the parsed cell codes
                    	  )
{
	const uint8_t REG_LEN = 8; //Number of bytes in each ICs register + 2 bytes for the PEC

	int8_t pec_error = 0;
	uint8_t *cell_data;
	uint8_t c_ic = 0;
	cell_data = (uint8_t *) malloc((NUM_RX_BYT * total_ic) * sizeof(uint8_t));

	for (uint8_t cell_reg = 1; cell_reg <= ic[0].ic_reg.num_cv_reg; cell_reg++) {
		uint8_t cmd[4];
		switch(cell_reg) {
			case 1: //Reg A
				cmd[0] = 0x00;
				cmd[1] = 0x19;
				break;
			case 2: //Reg B
				cmd[0] = 0x00;
				cmd[1] = 0x1A;
				break;
			case 3: //Reg C
				cmd[0] = 0x00;
				cmd[1] = 0x1B;
				break;
			case 4: //Reg D
				cmd[0] = 0x00;
				cmd[1] = 0x1F;
				break;
		}
		uint16_t cmd_pec = pec15_calc(2, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
		FEB_cs_low();
		FEB_spi_write_read(cmd, 4, cell_data, (REG_LEN * total_ic));
		FEB_cs_high();

		//parse data
		for (int curr_ic = 0; curr_ic < total_ic; curr_ic++) {
			if (ic->isospi_reverse == false) {
				c_ic = curr_ic;
			} else {
				c_ic = total_ic - curr_ic - 1;
			}
			pec_error += parse_cells(c_ic, cell_reg, cell_data, &ic[c_ic].aux.a_codes[0], &ic[c_ic].aux.pec_match[0]);
		}
	}

	ADBMS6830B_check_pec(total_ic, CELL, ic);
	free(cell_data);
	return(pec_error);
}

/* Generic wakeup command to wake the ADBMS6830B from sleep state */
void wakeup_sleep(uint8_t total_ic) //Number of ICs in the system
{
	for (int i = 0; i < total_ic; i++) {
	   FEB_cs_low(); //TODO: changed  from low
	   FEB_delay_u(300); // Guarantees the ADBMS6830B will be in standby
	   FEB_cs_high();
	   FEB_delay_u(10);
	}
}

void ADBMS6830B_check_pec(uint8_t total_ic, //Number of ICs in the system
					   uint8_t reg, //Type of Register
					   cell_asic *ic //A two dimensional array that stores the data
					   )
{
	switch (reg)
	{
		case CFGRA:
		  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
		  {
			ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].configa.rx_pec_match;
			ic[current_ic].crc_count.cfgr_pec = ic[current_ic].crc_count.cfgr_pec + ic[current_ic].configa.rx_pec_match;
		  }
		break;

		case CFGRB:
		  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
		  {
			ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].configb.rx_pec_match;
			ic[current_ic].crc_count.cfgr_pec = ic[current_ic].crc_count.cfgr_pec + ic[current_ic].configb.rx_pec_match;
		  }
		break;
		case CELL:
		  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
		  {
			for (int i=0; i<ic[0].ic_reg.num_cv_reg; i++)
			{
			  ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].cells.pec_match[i];
			  ic[current_ic].crc_count.cell_pec[i] = ic[current_ic].crc_count.cell_pec[i] + ic[current_ic].cells.pec_match[i];
			}
		  }
		break;
		case AUX:
		  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
		  {
			for (int i=0; i<ic[0].ic_reg.num_gpio_reg; i++)
			{
			  ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + (ic[current_ic].aux.pec_match[i]);
			  ic[current_ic].crc_count.aux_pec[i] = ic[current_ic].crc_count.aux_pec[i] + (ic[current_ic].aux.pec_match[i]);
			}
		  }

		break;
		case STAT:
		  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
		  {

			for (int i=0; i<ic[0].ic_reg.num_stat_reg-1; i++)
			{
			  ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].stat.pec_match[i];
			  ic[current_ic].crc_count.stat_pec[i] = ic[current_ic].crc_count.stat_pec[i] + ic[current_ic].stat.pec_match[i];
			}
		  }
		break;
		default:
		break;
	}
}
