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
 
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>	
#include "dt.h"
#include "pins.h"
#include "rpi.h"

uint32_t librpip_pins_getpins(uint32_t fid) {
	switch(fid) {
	
		case LIBRPIP_FEATURE_UART0:
			return LIBRPIP_PINS_UART0;
			break;
		case LIBRPIP_FEATURE_UART1:
			return LIBRPIP_PINS_UART1;
			break;		
		case LIBRPIP_FEATURE_I2C0:
		case LIBRPIP_FEATURE_I2C1:
		case LIBRPIP_FEATURE_SPI0:
		case LIBRPIP_FEATURE_SPI1:	
		case LIBRPIP_FEATURE_PWM0:
		case LIBRPIP_FEATURE_PWM1:
			return librpip_pins_getpins_from_dt(fid);
	}
	return 0;
}

uint32_t librpip_pins_getpins_from_dt(uint32_t fid) {
	uint32_t pins;
	uint32_t rlen;	
	uint8_t dtconf[40];
	uint32_t i;
	
	pins=0;
	rlen=0;
	

	switch(fid) {	
		case LIBRPIP_FEATURE_I2C0:
			librpip_dt_file_get_raw(LIBRPIP_DT_MODULE_GPIO_ID, LIBRPIP_DT_FILE_I2C0_PINS_ID, &dtconf[0], 40, &rlen);
			if(rlen==8) {		
				pins = (1 << dtconf[3]) | (1 << dtconf[7]);
			} else {
				pins=LIBRPIP_PINS_I2C0;  //default standard pins - or is it better to fail?
			}
			break;
		case LIBRPIP_FEATURE_I2C1:
			librpip_dt_file_get_raw(LIBRPIP_DT_MODULE_GPIO_ID, LIBRPIP_DT_FILE_I2C1_PINS_ID, &dtconf[0], 40, &rlen);
			if(rlen==8) {
				pins = (1 << dtconf[3]) | (1 << dtconf[7]);
			} else {
				pins=LIBRPIP_PINS_I2C1;  //default standard pins - or is it better to fail?
			}
			break;
		case LIBRPIP_FEATURE_PWM0:
			librpip_dt_file_get_raw(LIBRPIP_DT_MODULE_GPIO_ID, LIBRPIP_DT_FILE_PWM_PINS_ID, &dtconf[0], 40, &rlen);
			if(rlen==4) {
				pins = (1 << dtconf[3]) & LIBRPIP_PINS_PWM0;
			} else if(rlen==8) {
				pins = ((1 << dtconf[3]) | (1 << dtconf[7])) & LIBRPIP_PINS_PWM0;
			}
			break;		
		case LIBRPIP_FEATURE_PWM1:
			librpip_dt_file_get_raw(LIBRPIP_DT_MODULE_GPIO_ID, LIBRPIP_DT_FILE_PWM_PINS_ID, &dtconf[0], 40, &rlen);
			if(rlen==4) {
				pins = (1 << dtconf[3]) & LIBRPIP_PINS_PWM1;
			} else if(rlen==8) {
				pins = ((1 << dtconf[3]) | (1 << dtconf[7])) & LIBRPIP_PINS_PWM1;
			}
			break;	
		case LIBRPIP_FEATURE_SPI0:
			librpip_dt_file_get_raw(LIBRPIP_DT_MODULE_GPIO_ID, LIBRPIP_DT_FILE_SPI0_CSPINS_ID, &dtconf[0], 40, &rlen);		
			librpip_dt_file_get_raw(LIBRPIP_DT_MODULE_GPIO_ID, LIBRPIP_DT_FILE_SPI0_PINS_ID, &dtconf[0], 40, &rlen);
			break;		
		case LIBRPIP_FEATURE_SPI1:
			librpip_dt_file_get_raw(LIBRPIP_DT_MODULE_GPIO_ID, LIBRPIP_DT_FILE_SPI1_CSPINS_ID, &dtconf[0], 40, &rlen);		
			librpip_dt_file_get_raw(LIBRPIP_DT_MODULE_GPIO_ID, LIBRPIP_DT_FILE_SPI1_PINS_ID, &dtconf[0], 40, &rlen);
			break;				
	}
	
	return pins;
}