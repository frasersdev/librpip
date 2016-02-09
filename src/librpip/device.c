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
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "device.h"


uint32_t librpip_device_node_exists(uint32_t devid) {

	struct stat fileStat;
	int result;

    switch(devid) {
		case LIBRPIP_DEV_GPIO_ID:
			result=stat(LIBRPIP_DEV_GPIO,&fileStat);
			break;
		case  LIBRPIP_DEV_UART0_ID:
			result=stat(LIBRPIP_DEV_UART0,&fileStat);
			break;
		case  LIBRPIP_DEV_UART1_ID:
			result=stat(LIBRPIP_DEV_UART1,&fileStat);
			break;
		case LIBRPIP_DEV_I2C0_ID:
			result=stat(LIBRPIP_DEV_I2C0,&fileStat);
			break;
		case LIBRPIP_DEV_I2C1_ID:	
			result=stat(LIBRPIP_DEV_I2C1,&fileStat);
			break;
		case LIBRPIP_DEV_SPI0_CS0_ID:	
			result=stat(LIBRPIP_DEV_SPI0_CS0,&fileStat);
			break;
		case LIBRPIP_DEV_SPI0_CS1_ID:	
			result=stat(LIBRPIP_DEV_SPI0_CS1,&fileStat);
			break;
		case LIBRPIP_DEV_SPI1_CS0_ID:	
			result=stat(LIBRPIP_DEV_SPI1_CS0,&fileStat);
			break;
		case LIBRPIP_DEV_SPI1_CS1_ID:	
			result=stat(LIBRPIP_DEV_SPI1_CS1,&fileStat);
			break;
		case LIBRPIP_DEV_SPI1_CS2_ID:	
			result=stat(LIBRPIP_DEV_SPI1_CS1,&fileStat);
			break;
		default:
			result=-1;
	}
	return result < 0 ? 0 : 1;  
} 


uint32_t librpip_device_node_open(uint32_t devid, int flags, int* dev) {

    switch(devid) {
		case LIBRPIP_DEV_GPIO_ID:
			*dev=open(LIBRPIP_DEV_GPIO, flags);
			break;
		case  LIBRPIP_DEV_UART0_ID:
			*dev=open(LIBRPIP_DEV_UART0, flags);
			break;
		case  LIBRPIP_DEV_UART1_ID:
			*dev=open(LIBRPIP_DEV_UART1, flags);
			break;
		case LIBRPIP_DEV_I2C0_ID:
			*dev=open(LIBRPIP_DEV_I2C0, flags);
			break;
		case LIBRPIP_DEV_I2C1_ID:	
			*dev=open(LIBRPIP_DEV_I2C1, flags);
			break;
		case LIBRPIP_DEV_SPI0_CS0_ID:	
			*dev=open(LIBRPIP_DEV_SPI0_CS0, flags);
			break;
		case LIBRPIP_DEV_SPI0_CS1_ID:	
			*dev=open(LIBRPIP_DEV_SPI0_CS1, flags);
			break;
		case LIBRPIP_DEV_SPI1_CS0_ID:	
			*dev=open(LIBRPIP_DEV_SPI1_CS0, flags);
			break;
		case LIBRPIP_DEV_SPI1_CS1_ID:	
			*dev=open(LIBRPIP_DEV_SPI1_CS1, flags);
			break;
		case LIBRPIP_DEV_SPI1_CS2_ID:	
			*dev=open(LIBRPIP_DEV_SPI1_CS2, flags);
			break;					
		default:
			*dev=-1;
	}
	return *dev < 0 ? 0 : 1;  
} 