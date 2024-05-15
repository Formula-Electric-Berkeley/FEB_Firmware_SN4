#include <stdint.h>
#include <stdlib.h>
#include "ADBMS6830B.h"
#include "bms_hardware.h"

const uint16_t crc15Table[256] = {0x0,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,  // precomputed CRC15 Table
                                0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
                                0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
                                0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
                                0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
                                0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
                                0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d,
                                0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
                                0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
                                0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
                                0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
                                0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
                                0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
                                0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
                                0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
                                0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
                                0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
                                0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
                                0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
                                0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453,
                                0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
                                0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
                                0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
                               };

/* Calculates  and returns the CRC15 */
uint16_t pec15_calc(uint8_t len, //Number of bytes that will be used to calculate a PEC
                    uint8_t *data //Array of data that will be used to calculate  a PEC
                   )
{
	uint16_t remainder,addr;
	remainder = 16;//initialize the PEC

	for (uint8_t i = 0; i<len; i++) // loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
		remainder = (remainder<<8)^crc15Table[addr];
	}

	return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}

/* Generic function to write 68xx commands. Function calculates PEC for tx_cmd data. */
void cmd_68(uint8_t tx_cmd[2]) //The command to be transmitted
{
	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t md_bits;

	cmd[0] = tx_cmd[0];
	cmd[1] =  tx_cmd[1];
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	cs_low();
	spi_write_array(4,cmd);
	cs_high();
}

/*
Generic function to write 68xx commands and write payload data.
Function calculates PEC for tx_cmd data and the data to be transmitted.
 */
void write_68(uint8_t total_ic, //Number of ICs to be written to
			  uint8_t tx_cmd[2], //The command to be transmitted
			  uint8_t data[] // Payload Data
			  )
{
	const uint8_t BYTES_IN_REG = 6;
	const uint8_t CMD_LEN = 4+(8*total_ic);
	uint8_t *cmd;
	uint16_t data_pec;
	uint16_t cmd_pec;
	uint8_t cmd_index;

	cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));
	cmd[0] = tx_cmd[0];
	cmd[1] = tx_cmd[1];
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	cmd_index = 4;
	for (uint8_t current_ic = total_ic; current_ic > 0; current_ic--)               // Executes for each ADBMS630B, this loops starts with the last IC on the stack.
    {	                                                                            //The first configuration written is received by the last IC in the daisy chain
		for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
		{
			cmd[cmd_index] = data[((current_ic-1)*6)+current_byte];
			cmd_index = cmd_index + 1;
		}

		data_pec = (uint16_t)pec15_calc(BYTES_IN_REG, &data[(current_ic-1)*6]);    // Calculating the PEC for each ICs configuration register data
		cmd[cmd_index] = (uint8_t)(data_pec >> 8);
		cmd[cmd_index + 1] = (uint8_t)data_pec;
		cmd_index = cmd_index + 2;
	}

	cs_low();
	spi_write_array(CMD_LEN, cmd);
	cs_high();

	free(cmd);
}

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
					 bool cth[3], // The ADCOPT bit
					 bool gpio[5], // The GPIO bits
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

/* Helper function to set the ADCOPT bit */
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
void LTC681x_set_cfgr_gpio(uint8_t nIC, cell_asic *ic,bool gpio[10])
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
void LTC681x_set_cfgr_dis(uint8_t nIC, cell_asic *ic,bool dcc[12])
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
void LTC681x_set_cfgr_dcto(uint8_t nIC, cell_asic *ic,bool dcto[4])
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
void LTC681x_set_cfgr_uv(uint8_t nIC, cell_asic *ic,uint16_t uv)
{
	uint16_t tmp = (uv/16)-1;
	ic[nIC].configb.tx_data[0] = 0x00FF & tmp;
	ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]&0xF0;
	ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]|((0x0F00 & tmp)>>8);
}

/* Helper function to set OV value in CFG register */
void LTC681x_set_cfgr_ov(uint8_t nIC, cell_asic *ic,uint16_t ov)
{
	uint16_t tmp = (ov/16);
	ic[nIC].configb.tx_data[2] = 0x00FF & (tmp>>4);
	ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]&0x0F;
	ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]|((0x000F & tmp)<<4);
}