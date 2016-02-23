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
 
#ifndef LIBRPIP_PINS_H
#define LIBRPIP_PINS_H

//static Pins's
#define LIBRPIP_PINS_UART0	0b00000000000000001100000000000000
#define LIBRPIP_PINS_UART1	0b00000000000000001100000000000000
#define LIBRPIP_PINS_I2C0 	0b00000000000000000000000000000011
#define LIBRPIP_PINS_I2C1	0b00000000000000000000000000001100
#define LIBRPIP_PINS_SPI0	0b00000000000000000000111110000000 //
#define LIBRPIP_PINS_SPI1	0b00000000001111110000000000000000 //
#define LIBRPIP_PINS_PWM0	0b00000000000001000001000000000000  
#define LIBRPIP_PINS_PWM1	0b00000000000010000010000000000000  

uint32_t librpip_pins_getpins(uint32_t fid);
uint32_t librpip_pins_getpins_from_dt(uint32_t fid);

#endif