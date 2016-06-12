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
 
#include "config.h"
 
#include <stdio.h> 
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>	
#include <errno.h>	
#include <sys/types.h>	
#include <sys/ioctl.h>
#ifndef LIBRPIP_HAVE_I2CMSG_IN_I2CDEV
#include <linux/i2c.h>
#endif
#include <linux/i2c-dev.h>


#include "rpi.h"
#include "device.h"
#include "dt.h"
#include "error.h"
#include "pins.h"
#include "i2c.h"


extern uint32_t librpip_feature_set;
extern uint32_t librpip_error_code;
extern uint32_t librpip_error_data;
extern uint32_t librpip_error_extra;
extern uint32_t librpip_board;
extern uint32_t librpip_flags;
extern uint32_t librpip_pins_used;

int librpip_i2c_dev[2];
unsigned long librpip_i2c_functions[2];

uint32_t librpip_i2c_flags[2];

/* exposed functions */
uint32_t librpip_i2c_init(uint32_t id) {

	librpip_error_code=0;
	librpip_error_data=0;
	
	switch(id) {
		case 0:
			if(librpip_dt_module_enabled(LIBRPIP_DT_MODULE_I2C0_ID)) {
				if(librpip_device_node_exists(LIBRPIP_DEV_I2C0_ID)) {
					librpip_pins_used |= librpip_pins_getpins(LIBRPIP_FEATURE_I2C0);   //pins are in use
					if(librpip_flags & LIBRPIP_FLAG_SKIP_I2C0) { //we have been asked not not use I2C0
						librpip_error_code=0x304;					
						librpip_error_data=id;
					} else {
						if(librpip_device_node_open(LIBRPIP_DEV_I2C0_ID, O_RDWR, &librpip_i2c_dev[0])) {
							//finally check the i2c hardware supports the functions we need
							if(librpip_i2c_config_get_funcs(&librpip_i2c_dev[0], &librpip_i2c_functions[0])) {
								if(librpip_i2c_functions[0] & I2C_FUNC_I2C ) {
									//device exists and we can RW it and use it
									librpip_feature_set |= LIBRPIP_FEATURE_I2C0;  
								} else {
									//device exists and we can RW it but it doesn't support I2C functions we need
									librpip_error_code=0x311;					
									librpip_error_data=id;
								}
							} else {
								//device exists and we can RW it but we couldn't discover what it supports so we have to ignore it
								//code got set in function
								librpip_error_data=0;
							}
						} else {
							//device exists but we cant RW it
							librpip_error_code=0x301;					
							librpip_error_data=id;
						}	
					}
				} else {
					//device node does not exist
					librpip_error_code=0x300;						
					librpip_error_data=id;
				}
			} else {
				//module not enabled
				librpip_error_code=0x305;						
				librpip_error_data=id;			
			}
			break;
		case 1:
			if(librpip_dt_module_enabled(LIBRPIP_DT_MODULE_I2C1_ID)) {		
				if(librpip_device_node_exists(LIBRPIP_DEV_I2C1_ID)) {
					librpip_pins_used |= librpip_pins_getpins(LIBRPIP_FEATURE_I2C1);   //pins are in use
					if(librpip_flags & LIBRPIP_FLAG_SKIP_I2C1) { //we have been asked not not use I2C1
						librpip_error_code=0x304;					
						librpip_error_data=id;
					} else {
						if(librpip_device_node_open(LIBRPIP_DEV_I2C1_ID, O_RDWR, &librpip_i2c_dev[1])) {
							//finally check the i2c hardware supports the functions we need
							if(librpip_i2c_config_get_funcs(&librpip_i2c_dev[1], &librpip_i2c_functions[1])) {
								if(librpip_i2c_functions[1] & I2C_FUNC_I2C) {
									//device exists and we can RW it and use it
									librpip_feature_set |= LIBRPIP_FEATURE_I2C1;  
								} else {
									//device exists and we can RW it but it doesn't support I2C functions we need
									librpip_error_code=0x311;					
									librpip_error_data=id;
								}
							} else {
								//device exists and we can RW it but we couldn't discover what it supports so we have to ignore it
								//code got set in function
								librpip_error_data=1;
							}
						} else {
							//device exists but we cant RW it
							librpip_error_code=0x301;					
							librpip_error_data=id;
						}
					}
				} else {
					//device node does not exist
					librpip_error_code=0x300;						
					librpip_error_data=id;
				}
			} else {
				//module not enabled
				librpip_error_code=0x305;						
				librpip_error_data=id;			
			}			
			break;
		default:
			librpip_error_code=0x302;
			librpip_error_data=id;
			break;
	}
	
	if(librpip_error_code==0) {
		if(!(librpip_flags & LIBRPIP_FLAG_NO_RESET)) {
			librpip_i2c_flags[id]=0;
			librpip_i2c_config_set_pec(&librpip_i2c_dev[id], 0);
		} //note there is no way to know what the i2c config is through ioctls
	}	
	
	
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_i2c_close(uint32_t id) {

	librpip_error_code=0;
	librpip_error_data=0;
	
	switch(id) {
		case 0:
		case 1:
			close(librpip_i2c_dev[id]);
			break;
		default:
			librpip_error_code=0x302;
			librpip_error_data=id;
			break;
	}	
	return librpip_error_code ? 0 : 1;
}


//exposed functions
uint32_t librpipI2cConfigWrite(uint32_t id, uint32_t flags) {

	librpip_error_code=0;
	librpip_error_data=0;

	int fd;
	if(librpip_i2c_validate_id(id, &fd)) {
	
	
		if((flags & LIBRPIP_I2C_SUPPORTED_FLAGS) != flags)  {
			librpip_error_code=0x320;
			librpip_error_data=id;
			librpip_error_extra=flags;
		}
		
		if(flags & LIBRPIP_I2C_FLAGS_PEC) {
			if(!(librpip_i2c_functions[id] & I2C_FUNC_SMBUS_PEC))  {
				librpip_error_code=0x321;
				librpip_error_data=id;
				librpip_error_extra=flags;
			}
		}
		if(flags & LIBRPIP_I2C_FLAGS_TENBIT) {
			if(!(librpip_i2c_functions[id] & I2C_FUNC_10BIT_ADDR))  {
				librpip_error_code=0x322;
				librpip_error_data=id;
				librpip_error_extra=flags;
			}
		}

		if(!librpip_error_code) {
			if(librpip_i2c_config_set_pec(&fd, flags)) 	librpip_i2c_flags[id]=flags; //only pec is supported
		}
		
	}
	
	if(librpip_error_code) {
		librpip_error_data=id;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;
	}

	return librpip_error_code ? 0 : 1;
}

uint32_t librpipI2cConfigRead(uint32_t id, uint32_t* flags) {

	librpip_error_code=0;
	librpip_error_data=0;

	int fd;
	if(librpip_i2c_validate_id(id, &fd)) {
	
		*flags=librpip_i2c_flags[id];
		
	} else {
		librpip_error_data=id;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;
	}
	return librpip_error_code  ? 0 : 1;
}


//internal functions 
uint32_t librpip_i2c_validate_id(uint32_t id, int* fd) {
	switch(id) {
		case 0:
			if(librpip_feature_set & LIBRPIP_FEATURE_I2C0) {
				*fd=librpip_i2c_dev[0];
			} else {
				librpip_error_code=0x303;
				librpip_error_data=id;
			}
			break;
		case 1:
			if(librpip_feature_set & LIBRPIP_FEATURE_I2C1) {
				*fd=librpip_i2c_dev[1];
			} else {
				librpip_error_code=0x303;
				librpip_error_data=id;
			}			
			break; 
		default:
			librpip_error_code=0x302;
			librpip_error_data=id;
		break;
	}
	return librpip_error_code ? 0 : 1;
}

uint32_t librpip_i2c_validate_slave(uint32_t id, uint32_t slave) {

	if(librpip_i2c_flags[id] & LIBRPIP_I2C_FLAGS_TENBIT) {
		if(slave > 0b1111111111) {
			librpip_error_code=0x323;
			librpip_error_data=id;
			librpip_error_extra=slave;
		} 
	} else {
		if(slave > 0b1111111) {
			librpip_error_code=0x324;
			librpip_error_data=id;
			librpip_error_extra=slave;
		} 
	}

	return librpip_error_code ? 0 : 1;
}



uint32_t librpip_i2c_transaction(uint32_t id, uint32_t slave, struct i2c_rdwr_ioctl_data* i2c) {

	int fd,result;
	
	if(librpip_i2c_validate_id(id, &fd)) {
		if(librpip_i2c_validate_slave(id, slave)) {
			if(librpip_i2c_config_set_slave(&fd, slave)) {
				result=ioctl(fd, I2C_RDWR, i2c);
				if(result<0) {
					librpip_error_code=0x350;	
					librpip_error_data=id;
					librpip_error_extra=errno;			
				}
			}
		}
	}
	return librpip_error_code ? 0 : 1;
}

// these don't do any validation
uint32_t librpip_i2c_config_get_funcs(int* fd, unsigned long *funcs) {

	int result;
		
	result=ioctl(*fd, I2C_FUNCS, funcs);
	if(result<0) {
		librpip_error_code=0x330;	
		librpip_error_extra=errno;			
	}
	return librpip_error_code  ? 0 : 1;
}


uint32_t librpip_i2c_config_set_slave(int* fd, uint32_t addr) {
	int result;
		
	result=ioctl(*fd, I2C_SLAVE, (long)addr);
	if(result<0) {
		librpip_error_code=0x340;	
		librpip_error_extra=errno;			
	}
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_i2c_config_set_10bit(int* fd, uint32_t select) {

	int result;
		
	result=ioctl(*fd, I2C_TENBIT, (long)(select & LIBRPIP_I2C_FLAGS_TENBIT));
	if(result<0) {
		librpip_error_code=0x341;	
		librpip_error_extra=errno;		
	}
	return librpip_error_code ? 0 : 1;
}

uint32_t librpip_i2c_config_set_pec(int* fd, uint32_t select) {

	int result;
		
	result=ioctl(*fd, I2C_PEC, (long)(select & LIBRPIP_I2C_FLAGS_PEC));
	if(result<0) {
		librpip_error_code=0x342;	
		librpip_error_extra=errno;			
	}
	return librpip_error_code  ? 0 : 1;
}

