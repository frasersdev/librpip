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

#ifndef LIBRPIP_DEVICES_H
#define LIBRPIP_DEVICES_H


//dev files
#define LIBRPIP_DEV_GPIO 				"/dev/gpiomem"
#define LIBRPIP_DEV_UART0				"/dev/ttyAMA0"	
#define LIBRPIP_DEV_UART1				"/dev/ttyAMA1"		
#define LIBRPIP_DEV_I2C0 				"/dev/i2c-0"
#define LIBRPIP_DEV_I2C1 				"/dev/i2c-1"
#define LIBRPIP_DEV_SPI0_CS0			"/dev/spidev0.0"
#define LIBRPIP_DEV_SPI0_CS1			"/dev/spidev0.1"
#define LIBRPIP_DEV_SPI1_CS0			"/dev/spidev1.0"
#define LIBRPIP_DEV_SPI1_CS1			"/dev/spidev1.1"
#define LIBRPIP_DEV_SPI1_CS2			"/dev/spidev1.2"

// the PWM kernel module does not create a device file for userspace access. :(
// however we can use what we find in /sys so long as it has been configured for use.
// see the pwm-init script for details.
// yes, its a bit hacky.
// one day perhaps we'll see a /dev file that we can send ioctl's to.
// yuk


//ID's
#define LIBRPIP_DEV_GPIO_ID				1 
#define LIBRPIP_DEV_UART0_ID			2 
#define LIBRPIP_DEV_UART1_ID			3
#define LIBRPIP_DEV_I2C0_ID 			4
#define LIBRPIP_DEV_I2C1_ID 			5
#define LIBRPIP_DEV_SPI0_CS0_ID			6
#define LIBRPIP_DEV_SPI0_CS1_ID			7
#define LIBRPIP_DEV_SPI1_CS0_ID			8
#define LIBRPIP_DEV_SPI1_CS1_ID			9
#define LIBRPIP_DEV_SPI1_CS2_ID			10



//internal functions
uint32_t librpip_device_node_exists(uint32_t devid);
uint32_t librpip_device_node_open(uint32_t devid, int flags, int* dev);

#endif




