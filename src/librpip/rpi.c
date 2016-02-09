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
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/spi/spidev.h>

#include "board.h"
#include "pins.h"
#include "device.h"
#include "sysfs.h"
#include "error.h"
#include "gpio.h"
#include "pwm.h"
#include "transact.h"
#include "i2c.h"
#include "spi.h"
#include "uart.h"
#include "rpi.h"

uint32_t librpip_board_capabilities;
uint32_t librpip_feature_set;
uint32_t librpip_error_code;
uint32_t librpip_error_data;
uint32_t librpip_error_extra;
float 	 librpip_error_extraf;
uint32_t librpip_board;
uint32_t librpip_flags;

uint32_t librpip_pins_used;


uint32_t librpipInit(uint32_t board, uint32_t flags, uint32_t ignore_pins) {

	librpip_error_code=0;
	librpip_error_data=0;
	librpip_feature_set=0;
	librpip_pins_used=0;
	librpip_flags=flags;	
	
	if(!getuid() || !geteuid()) {
		//running as root!!!!
		librpip_error_code=0x001;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;		
	}
	
	if(!board) {
		if(!librpip_board_detect()) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
			return 0;
		} else {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpip_board_model_print();
		}
	} else {
		librpip_board=board;
	} 
		
	librpip_board_capabilities=librpip_board_get_capabilities();

	if(librpip_board_capabilities & LIBRPIP_FEATURE_UART0) {
		if(!librpip_uart_init(0)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}
	
	if(librpip_board_capabilities & LIBRPIP_FEATURE_UART1) {	
		if(!librpip_uart_init(1)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}	
	}
	
	if(librpip_board_capabilities & (LIBRPIP_FEATURE_UART0 | LIBRPIP_FEATURE_UART1 )) {
		//this is here as uart0 and 1 share the same pins so we don't know is the pins are free until both have been checked.
		if(!librpip_uart_usable()) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}	
	
	if(librpip_board_capabilities & LIBRPIP_FEATURE_PWM0) {
		if(!librpip_pwm_init(0)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}
	
	if(librpip_board_capabilities & LIBRPIP_FEATURE_PWM1) {
		if(!librpip_pwm_init(1)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}
	
	if(librpip_board_capabilities & LIBRPIP_FEATURE_I2C0) {
		if(!librpip_i2c_init(0)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}

	if(librpip_board_capabilities & LIBRPIP_FEATURE_I2C1) {
		if(!librpip_i2c_init(1)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}

	if(librpip_board_capabilities & LIBRPIP_FEATURE_SPI0) {
		if(!librpip_spi_init(0)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}
	
	if(librpip_board_capabilities & LIBRPIP_FEATURE_SPI1) {
		if(!librpip_spi_init(1)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}

	if(librpip_board_capabilities & LIBRPIP_FEATURE_GPIO) {
		if(!librpip_gpio_init(ignore_pins)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}
	
	return librpip_feature_set;
}


void librpipClose(void) {
	if(librpip_feature_set & LIBRPIP_FEATURE_SPI0) {
		if(!librpip_spi_close(0)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}
	
	if(librpip_feature_set & LIBRPIP_FEATURE_SPI1) {
		if(!librpip_spi_close(1)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}

	if(librpip_feature_set & LIBRPIP_FEATURE_I2C0) {
		if(!librpip_i2c_close(0)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}
	
	if(librpip_feature_set & LIBRPIP_FEATURE_I2C1) {
		if(!librpip_i2c_close(1)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}
	
	if(librpip_feature_set & LIBRPIP_FEATURE_UART0) {
		if(!librpip_uart_close(0)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}

	if(librpip_feature_set & LIBRPIP_FEATURE_UART1) {
		if(!librpip_uart_close(1)) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}
	if(librpip_feature_set & LIBRPIP_FEATURE_GPIO) {
		if(!librpip_gpio_close()) {
			if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		}
	}
}

uint32_t librpipGetBoardID(void) {
	return librpip_board;
}

void librpipVersionStr(char* version, int len) {
	snprintf(version, len, LIBRPIP_H_VERSION);
}

void librpipVersionInt(uint8_t* maj, uint8_t* min, uint8_t* sub) {
	*maj=LIBRPIP_H_VERSION_MAJ;
	*min=LIBRPIP_H_VERSION_MIN;
	*sub=LIBRPIP_H_VERSION_SUB;
}


