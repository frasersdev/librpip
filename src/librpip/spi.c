/*
 * librpip - a Library to facilitate using Raspberry PI peripherals 
 * from userland by utilising kernel modules.
 * 
 * Copyright (C) 2016 Fraser Stuart

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
 
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>	
#include <errno.h>
#include <sys/types.h>		
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "rpi.h"
#include "device.h"
#include "error.h"
#include "dt.h"
#include "pins.h"
#include "transact.h"
#include "spi.h"
#include "config.h"

extern uint32_t librpip_feature_set;
extern uint32_t librpip_error_code;
extern uint32_t librpip_error_data;
extern uint32_t librpip_error_extra;
extern uint32_t librpip_board;
extern uint32_t librpip_flags;
extern uint32_t librpip_pins_used;

uint32_t librpip_spi_cs[2][3];
int 	librpip_spi_cs_dev[2][3];

/* exposed functions */
uint32_t librpip_spi_init(uint32_t id) {

	librpip_error_code=0;
	librpip_error_data=0;
	switch(id) {
		case 0:
			librpip_spi_cs[0][0]=0;
			librpip_spi_cs[0][1]=0;
			librpip_spi_cs[0][2]=0;	
			if(librpip_dt_module_enabled(LIBRPIP_DT_MODULE_SPI0_ID)) {
				librpip_pins_used |= librpip_pins_getpins(LIBRPIP_FEATURE_SPI0);   //pins are in use					
				if(librpip_flags & LIBRPIP_FLAG_SKIP_SPI0) { //we have been asked not not use SPI0
						librpip_error_code=0x205;					
						librpip_error_data=id;
				} else {
					if(librpip_spi_validate_device_nodes_exist(0)) {
						if(librpip_spi_validate_device_nodes_writeable(0) ) {
							librpip_feature_set |= LIBRPIP_FEATURE_SPI0;  //device exists and we can RW it
						} else {
							librpip_error_code=0x201; //device exists but we cant RW it
							librpip_error_data=id;
						}

					} else {
						librpip_error_code=0x200;	//device does not exist, pins are in use
						librpip_error_data=id;
					}
				} 
			} else {
				librpip_error_code=0x207;	//module not loaded
				librpip_error_data=id;			
			}
			break;
		case 1:
			librpip_spi_cs[1][0]=0;
			librpip_spi_cs[1][1]=0;
			librpip_spi_cs[1][2]=0;	
			if(librpip_dt_module_enabled(LIBRPIP_DT_MODULE_SPI1_ID)) {
				librpip_pins_used |= librpip_pins_getpins(LIBRPIP_FEATURE_SPI1);   //pins are in use					
				if(librpip_flags & LIBRPIP_FLAG_SKIP_SPI1) { //we have been asked not not use SPI0
						librpip_error_code=0x205;					
						librpip_error_data=id;
				} else {
					if(librpip_spi_validate_device_nodes_exist(1)) {
						if(librpip_spi_validate_device_nodes_writeable(1) ) {
							librpip_feature_set |= LIBRPIP_FEATURE_SPI1;  //device exists and we can RW it
						} else {
							librpip_error_code=0x201; //device exists but we cant RW it
							librpip_error_data=id;
						}

					} else {
						librpip_error_code=0x200;	//device does not exist, pins are in use
						librpip_error_data=id;
					}
				} 
			} else {
				librpip_error_code=0x207;	//module not loaded
				librpip_error_data=id;			
			}
			break;
		default:
			librpip_error_code=0x202;
			librpip_error_data=id;
			break;
	}
	if(librpip_error_code==0) {
		if(!(librpip_flags & LIBRPIP_FLAG_NO_RESET)) {
			if(librpip_spi_cs[id][0]) librpip_spi_config_set(&librpip_spi_cs_dev[id][0], SPI_MODE_0, 0, 8, 1000000);
			if(librpip_spi_cs[id][1]) librpip_spi_config_set(&librpip_spi_cs_dev[id][1], SPI_MODE_0, 0, 8, 1000000);  
			if(librpip_spi_cs[id][2]) librpip_spi_config_set(&librpip_spi_cs_dev[id][2], SPI_MODE_0, 0, 8, 1000000);
		}
	}
	
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_spi_close(uint32_t id) {

	librpip_error_code=0;
	librpip_error_data=0;
	
	switch(id) {
		case 0:
			if(librpip_spi_cs[0][0]) close(librpip_spi_cs_dev[0][0]);
			if(librpip_spi_cs[0][1]) close(librpip_spi_cs_dev[0][1]);
			break;
		case 1:
			if(librpip_spi_cs[1][0]) close(librpip_spi_cs_dev[1][0]);
			if(librpip_spi_cs[1][1]) close(librpip_spi_cs_dev[1][1]);
			if(librpip_spi_cs[1][2]) close(librpip_spi_cs_dev[1][2]);
			break;
		default:
			librpip_error_code=0x202;
			librpip_error_data=id;
			break;
	}	
	return librpip_error_code ? 0 : 1;
}



uint32_t librpipSpiConfigRead(uint32_t id, uint32_t cs, uint32_t* mode, uint32_t* lsb_first, uint32_t* bits_per_word, uint32_t* max_speed, uint32_t* flags) {

	int fd;
	fd=0;
	librpip_error_code=0;
	librpip_error_data=0;
	
	if(librpip_spi_validate_spixcsx(id, cs, &fd)) {
		librpip_spi_config_get(&fd, flags, lsb_first, bits_per_word, max_speed);
		*mode  = *flags & 0b11; // mask out the spi_flags from the mode
		*flags &= ~((uint32_t)0b11); // mask out the spi_mode from the flags
		return 1;
	} else {
		librpip_error_data=id;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;
	}
}

uint32_t librpipSpiConfigWrite(uint32_t id, uint32_t cs, uint32_t mode, uint32_t max_speed, uint32_t flags) {

	int fd;
	fd=0;
	librpip_error_code=0;
	librpip_error_data=0;
	
	if(librpip_spi_validate_spixcsx(id, cs, &fd)) {
		if(mode>3) {
			librpip_error_code=0x227;
			librpip_error_data=id;
			librpip_error_extra=mode;
		}
		if(max_speed < 8000 || max_speed > 125000000) {  //8khz to 125mhz are valid speeds
			librpip_error_code=0x228;
			librpip_error_data=id;
			librpip_error_extra=max_speed;
		}		
		librpip_spi_validate_flags(flags);
		if(!librpip_error_code) {
			flags |= mode; // add the spi_mode to the flags
			librpip_spi_config_set(&fd, flags, 0, 8, max_speed); //only supported values for lsb_first and bits_per_word are 0 and 8 respectively.
		}
	}
	if(librpip_error_code) {
		librpip_error_data=id;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	
	return librpip_error_code ? 0 : 1;
}



//internal functions
uint32_t librpip_spi_validate_spixcsx(uint32_t id, uint32_t cs, int* fd) {
	switch(id) {
		case 0:
			if(librpip_feature_set & LIBRPIP_FEATURE_SPI0) {
				if(cs < 2) {
					if(librpip_spi_cs[0][cs])  {
						*fd=librpip_spi_cs_dev[0][cs];
					} else {
						librpip_error_code=0x208;
						librpip_error_data=id;
						librpip_error_extra=cs;
					}
				} else {
					librpip_error_code=0x203;
					librpip_error_data=id;
					librpip_error_extra=cs;
				}			
			} else {
				librpip_error_code=0x204;
				librpip_error_data=id;
			}

			break;
		case 1:
			if(librpip_feature_set & LIBRPIP_FEATURE_SPI1) {
				if(cs < 3) {
					if(librpip_spi_cs[1][cs])  {
						*fd=librpip_spi_cs_dev[1][cs];
					} else {
						librpip_error_code=0x208;
						librpip_error_data=id;
						librpip_error_extra=cs;
					}
				} else {
					librpip_error_code=0x203;
					librpip_error_data=id;
					librpip_error_extra=cs;
				}
			} else {
				librpip_error_code=0x204;
				librpip_error_data=id;
			}				
			break; 
		default:
			librpip_error_code=0x202;
			librpip_error_data=id;
		break;
	}
	return librpip_error_code ? 0 : 1;
}

uint32_t librpip_spi_validate_spixcsx_3wire(uint32_t id, uint32_t cs, int* fd) {
	switch(id) {
		case 0:
			librpip_spi_validate_spixcsx(id, cs, fd);
			break;
		case 1:
			librpip_error_code=0x206;
			librpip_error_data=id;
		default:
			librpip_error_code=0x202;
			librpip_error_data=id;
		break;
	}
	return librpip_error_code ? 0 : 1;
}

uint32_t librpip_spi_validate_device_nodes_exist(uint32_t id) {

	uint32_t result;
	
	result=0;
	switch(id) {
		case 0:
			if(librpip_spi_cs[0][0] || librpip_spi_cs[0][1]) result=1; 
			if(librpip_spi_cs[0][0]) {
				result &= librpip_device_node_exists(LIBRPIP_DEV_SPI0_CS0_ID);
			}
			if(librpip_spi_cs[0][1]) {
				result &= librpip_device_node_exists(LIBRPIP_DEV_SPI0_CS1_ID);
			}
			break;	
		case 1:
			if(librpip_spi_cs[1][0] || librpip_spi_cs[1][1] || librpip_spi_cs[1][2]) result=1; 
			if(librpip_spi_cs[1][0]) {
				result &= librpip_device_node_exists(LIBRPIP_DEV_SPI1_CS0_ID);
			}
			if(librpip_spi_cs[1][1]) {
				result &= librpip_device_node_exists(LIBRPIP_DEV_SPI1_CS1_ID);
			}
			if(librpip_spi_cs[1][2]) {
				result &= librpip_device_node_exists(LIBRPIP_DEV_SPI1_CS2_ID);
			}			
			break;						
	}
	return result;
}

uint32_t librpip_spi_validate_device_nodes_writeable(uint32_t id) {

	uint32_t result;
	
	result=0;
	switch(id) {
		case 0:
			if(librpip_spi_cs[0][0] || librpip_spi_cs[0][1]) result=1; 
			if(librpip_spi_cs[0][0]) {
				result &= librpip_device_node_open(LIBRPIP_DEV_SPI0_CS0_ID, O_RDWR, &librpip_spi_cs_dev[0][0]);
			}
			if(librpip_spi_cs[0][1]) {
				result &= librpip_device_node_open(LIBRPIP_DEV_SPI0_CS1_ID, O_RDWR, &librpip_spi_cs_dev[0][1]);
			}
			break;	
		case 1:
			if(librpip_spi_cs[1][0] || librpip_spi_cs[1][1] || librpip_spi_cs[1][2]) result=1; 
			if(librpip_spi_cs[1][0]) {
				result &= librpip_device_node_open(LIBRPIP_DEV_SPI1_CS0_ID, O_RDWR, &librpip_spi_cs_dev[1][0]);
			}
			if(librpip_spi_cs[1][1]) {
				result &= librpip_device_node_open(LIBRPIP_DEV_SPI1_CS1_ID, O_RDWR, &librpip_spi_cs_dev[1][1]);
			}
			if(librpip_spi_cs[1][2]) {
				result &= librpip_device_node_open(LIBRPIP_DEV_SPI1_CS2_ID, O_RDWR, &librpip_spi_cs_dev[1][2]);
			}			
			break;						
	}
	return result;
}




uint32_t librpip_spi_validate_flags(uint32_t spi_flags) {

	uint32_t masked_flags;

	masked_flags = (spi_flags & LIBRPIP_SPI_SUPPORTED_FLAGS);
	if(spi_flags != masked_flags) { 
		librpip_error_code=0x225; 
		librpip_error_extra=spi_flags; 
	}
	return librpip_error_code ? 0 : 1;
}


void librpip_spi_struct_init(struct spi_ioc_transfer* tr) {

			tr->tx_buf = 0;
			tr->rx_buf = 0;  
			tr->len = 0;
			tr->delay_usecs = 0;
			tr->speed_hz = 0;
			tr->bits_per_word = 0;
			tr->cs_change = 0;
#ifdef LIBRPIP_HAVE_SPI_STRUCT_NBITS
			tr->tx_nbits = 0;
			tr->rx_nbits = 0;
#endif
			tr->pad = 0;
}

uint32_t librpip_spi_transaction(uint32_t id, uint32_t cs, struct spi_ioc_transfer* spi, uint16_t len) {
	int fd;

	if(librpip_spi_validate_spixcsx(id, cs, &fd)) {
		if(ioctl(fd, SPI_IOC_MESSAGE(len), spi) < 0 ) {
			librpip_error_code=0x230;
			librpip_error_data=id;
			librpip_error_extra=errno;
		}
	}
	
	if(librpip_error_code) {	
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	
	return librpip_error_code ? 0 : 1;
}

uint32_t librpip_spi_transaction_3w(uint32_t id, uint32_t cs, struct spi_ioc_transfer* spi, uint16_t len) {
	int fd;

	uint32_t mode,newmode;
		
	if(librpip_spi_validate_spixcsx_3wire(id, cs, &fd)) {
	
		librpip_spi_config_get_mode(&fd, &mode);
		newmode = mode | SPI_3WIRE;  //add 3Wire mode to the current flags
		librpip_spi_config_set_mode(&fd, newmode);
		
		if(ioctl(fd, SPI_IOC_MESSAGE(len), spi) < 0 ) {
			librpip_error_code=0x230;
			librpip_error_data=id;
			librpip_error_extra=errno;
		}
		
		librpip_spi_config_set_mode(&fd, mode);	//put the previous mode back
	}
	
	if(librpip_error_code) {	
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	
	return librpip_error_code ? 0 : 1;
}



//these functions do not perform any validation and already have the fd set
uint32_t librpip_spi_config_get(int* fd, uint32_t* spi_mode, uint32_t* lsb_first, uint32_t* bits_per_word, uint32_t* max_speed) {

	librpip_spi_config_get_mode(fd, spi_mode);
	librpip_spi_config_get_lsb(fd, lsb_first);
	librpip_spi_config_get_bpw(fd, bits_per_word);
	librpip_spi_config_get_maxspeed(fd, max_speed); 
	
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_spi_config_get_mode(int* fd, uint32_t* spi_mode) {

	*spi_mode=0;

	if (ioctl(*fd, SPI_IOC_RD_MODE, spi_mode) < 0) {
		librpip_error_code=0x210;
		librpip_error_extra=errno;
	} 
	
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_spi_config_get_lsb(int* fd, uint32_t* lsb_first) {


	*lsb_first=0;


	if (ioctl(*fd, SPI_IOC_RD_LSB_FIRST, lsb_first) < 0) {
		librpip_error_code=0x211;
		librpip_error_extra=errno;
	} 
	
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_spi_config_get_bpw(int* fd, uint32_t* bits_per_word) {

	*bits_per_word=0;

	if (ioctl(*fd, SPI_IOC_RD_BITS_PER_WORD, bits_per_word) < 0) {
		librpip_error_code=0x212;
		librpip_error_extra=errno;
	} 
	
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_spi_config_get_maxspeed(int* fd, uint32_t* max_speed) {

	*max_speed=0;

	if (ioctl(*fd, SPI_IOC_RD_MAX_SPEED_HZ, max_speed) < 0) {
		librpip_error_code=0x213;
		librpip_error_extra=errno;
	} 
	
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_spi_config_set(int* fd, uint32_t spi_flags, uint32_t lsb_first, uint32_t bits_per_word, uint32_t max_speed) {

	librpip_spi_config_set_mode(fd, spi_flags);
	librpip_spi_config_set_lsb(fd, lsb_first);		
	librpip_spi_config_set_bpw(fd, bits_per_word);
	librpip_spi_config_set_maxspeed(fd, max_speed);
	
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_spi_config_set_mode(int* fd, uint32_t spi_mode) {
	
	if (ioctl(*fd, SPI_IOC_WR_MODE, &spi_mode) < 0) {
		librpip_error_code=0x220;
		librpip_error_extra=errno;
	} 
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_spi_config_set_lsb(int* fd, uint32_t lsb_first) {
	
	if (ioctl(*fd, SPI_IOC_WR_LSB_FIRST, &lsb_first) < 0) {
		librpip_error_code=0x221;
		librpip_error_extra=errno;
	}
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_spi_config_set_bpw(int* fd, uint32_t bits_per_word) {
	
	if (ioctl(*fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word) < 0) {
		librpip_error_code=0x222;
		librpip_error_extra=errno;
	} 
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_spi_config_set_maxspeed(int* fd, uint32_t max_speed) {
	
	if (ioctl(*fd, SPI_IOC_WR_MAX_SPEED_HZ, &max_speed) < 0) {
		librpip_error_code=0x223;
		librpip_error_extra=errno;
	} 
	return librpip_error_code  ? 0 : 1;
}

