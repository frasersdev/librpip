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

#ifndef LIBRPIP_H
#define LIBRPIP_H

#define LIBRPIP_H_VERSION 		"0.2.0"
#define LIBRPIP_H_VERSION_MAJ 		0
#define LIBRPIP_H_VERSION_MIN 		2
#define LIBRPIP_H_VERSION_SUB		0

#define LIBRPIP_FLAG_DEBUG_ON		0x001
#define LIBRPIP_FLAG_NO_RESET		0x002
#define LIBRPIP_FLAG_SKIP_GPIO		0x004
#define LIBRPIP_FLAG_SKIP_UART0		0x008
#define LIBRPIP_FLAG_SKIP_UART1		0x010
#define LIBRPIP_FLAG_SKIP_PWM0		0x020
#define LIBRPIP_FLAG_SKIP_PWM1		0x040
#define LIBRPIP_FLAG_SKIP_I2C0		0x080
#define LIBRPIP_FLAG_SKIP_I2C1		0x100
#define LIBRPIP_FLAG_SKIP_SPI0		0x200
#define LIBRPIP_FLAG_SKIP_SPI1		0x400

#define LIBRPIP_FEATURE_GPIO		0x001
#define LIBRPIP_FEATURE_UART0		0x002	
#define LIBRPIP_FEATURE_UART1		0x004
#define LIBRPIP_FEATURE_I2C0		0x008	
#define LIBRPIP_FEATURE_I2C1		0x010	
#define LIBRPIP_FEATURE_SPI0		0x020	
#define LIBRPIP_FEATURE_SPI1		0x040	
#define LIBRPIP_FEATURE_PWM0		0x080	
#define LIBRPIP_FEATURE_PWM1		0x100



uint32_t librpipInit(uint32_t board, uint32_t flags, uint32_t ignore_pins);
void librpipClose(void);

uint32_t librpipGetBoardID(void);
void librpipVersionStr(char* version, int len);
void librpipVersionInt(uint8_t* maj, uint8_t* min, uint8_t* sub);


#endif
