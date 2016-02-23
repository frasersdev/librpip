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
#include <string.h>
#include "dt.h"


uint32_t librpip_dt_module_enabled(uint32_t moduleid) {

	char status[10];

	if(librpip_dt_module_exists(moduleid)) {
		if(librpip_dt_file_get_str(moduleid, LIBRPIP_DT_FILE_STATUS_ID, &status[0], 10)) {				
			if(strcmp((const char *)&status[0],"disabled")) {			
				return 1;
			}
		}
	}
	return 0;
}

uint32_t librpip_dt_module_exists(uint32_t moduleid) {

	struct stat fileStat;
	int result;

	switch(moduleid) {
		case LIBRPIP_DT_MODULE_GPIOMEM_ID:
			result=stat(LIBRPIP_DT_MODULE_GPIOMEM,&fileStat);
			break;	
		case LIBRPIP_DT_MODULE_GPIO_ID:
			result=stat(LIBRPIP_DT_MODULE_GPIO,&fileStat);
			break;		
		case LIBRPIP_DT_MODULE_I2C0_ID:
			result=stat(LIBRPIP_DT_MODULE_I2C0,&fileStat);
			break;			
		case LIBRPIP_DT_MODULE_I2C1_ID:
			result=stat(LIBRPIP_DT_MODULE_I2C1,&fileStat);
			break;			
		case LIBRPIP_DT_MODULE_I2S_ID:
			result=stat(LIBRPIP_DT_MODULE_I2S,&fileStat);
			break;			
		case LIBRPIP_DT_MODULE_PWM_ID:
			result=stat(LIBRPIP_DT_MODULE_PWM,&fileStat);
			break;			
		case LIBRPIP_DT_MODULE_SPI0_ID:
			result=stat(LIBRPIP_DT_MODULE_SPI0,&fileStat);
			break;	
		case LIBRPIP_DT_MODULE_SPI0_SPIDEV0_ID:
			result=stat(LIBRPIP_DT_MODULE_SPI0_SPIDEV0,&fileStat);
			break;			
		case LIBRPIP_DT_MODULE_SPI0_SPIDEV1_ID:	
			result=stat(LIBRPIP_DT_MODULE_SPI0_SPIDEV1,&fileStat);
			break;							
		case LIBRPIP_DT_MODULE_SPI1_ID:
			result=stat(LIBRPIP_DT_MODULE_SPI1,&fileStat);
			break;	
		case LIBRPIP_DT_MODULE_SPI1_SPIDEV0_ID:
			result=stat(LIBRPIP_DT_MODULE_SPI1_SPIDEV0,&fileStat);
			break;			
		case LIBRPIP_DT_MODULE_SPI1_SPIDEV1_ID:	
			result=stat(LIBRPIP_DT_MODULE_SPI1_SPIDEV1,&fileStat);
			break;			
		case LIBRPIP_DT_MODULE_SPI1_SPIDEV2_ID:	
			result=stat(LIBRPIP_DT_MODULE_SPI1_SPIDEV2,&fileStat);
			break;										
		case LIBRPIP_DT_MODULE_UART0_ID:
			result=stat(LIBRPIP_DT_MODULE_UART0,&fileStat);
			break;			
		case LIBRPIP_DT_MODULE_UART1_ID:
			result=stat(LIBRPIP_DT_MODULE_UART1,&fileStat);
			break;
		case LIBRPIP_DT_MODULE_1WIRE_ID:
			result=stat(LIBRPIP_DT_MODULE_1WIRE,&fileStat);
			break;
		default:
			result=-1;
	}
	return result < 0 ? 0 : 1;  
} 

uint32_t librpip_dt_file_open(uint32_t moduleid, uint32_t fileid, int flags, int* fd) {

	const char* node=NULL;
	const char* file=NULL;	

	char	sfile[200];

	switch(moduleid) {
		case LIBRPIP_DT_MODULE_GPIOMEM_ID:
			node=LIBRPIP_DT_MODULE_GPIOMEM;
			break;	
		case LIBRPIP_DT_MODULE_GPIO_ID:
			node=LIBRPIP_DT_MODULE_GPIO;
			break;		
		case LIBRPIP_DT_MODULE_I2C0_ID:
			node=LIBRPIP_DT_MODULE_I2C0;
			break;			
		case LIBRPIP_DT_MODULE_I2C1_ID:
			node=LIBRPIP_DT_MODULE_I2C1;
			break;			
		case LIBRPIP_DT_MODULE_I2S_ID:
			node=LIBRPIP_DT_MODULE_I2S;
			break;			
		case LIBRPIP_DT_MODULE_PWM_ID:
			node=LIBRPIP_DT_MODULE_PWM;
			break;			
		case LIBRPIP_DT_MODULE_SPI0_ID:
			node=LIBRPIP_DT_MODULE_SPI0;
			break;	
		case LIBRPIP_DT_MODULE_SPI0_SPIDEV0_ID:
			node=LIBRPIP_DT_MODULE_SPI0_SPIDEV0;
			break;			
		case LIBRPIP_DT_MODULE_SPI0_SPIDEV1_ID:	
			node=LIBRPIP_DT_MODULE_SPI0_SPIDEV1;
			break;							
		case LIBRPIP_DT_MODULE_SPI1_ID:
			node=LIBRPIP_DT_MODULE_SPI1;
			break;	
		case LIBRPIP_DT_MODULE_SPI1_SPIDEV0_ID:
			node=LIBRPIP_DT_MODULE_SPI1_SPIDEV0;
			break;			
		case LIBRPIP_DT_MODULE_SPI1_SPIDEV1_ID:	
			node=LIBRPIP_DT_MODULE_SPI1_SPIDEV1;
			break;			
		case LIBRPIP_DT_MODULE_SPI1_SPIDEV2_ID:	
			node=LIBRPIP_DT_MODULE_SPI1_SPIDEV2;
			break;										
		case LIBRPIP_DT_MODULE_UART0_ID:
			node=LIBRPIP_DT_MODULE_UART0;
			break;			
		case LIBRPIP_DT_MODULE_UART1_ID:
			node=LIBRPIP_DT_MODULE_UART1;	
			break;
		case LIBRPIP_DT_MODULE_1WIRE_ID:
			node=LIBRPIP_DT_MODULE_1WIRE;	
			break;			
	}
	
	switch(fileid) {
		case LIBRPIP_DT_FILE_STATUS_ID:
			file=LIBRPIP_DT_FILE_STATUS;
			break;		
		case LIBRPIP_DT_FILE_I2C0_PINS_ID:
			file=LIBRPIP_DT_FILE_I2C0_PINS;
			break;			
		case LIBRPIP_DT_FILE_I2C1_PINS_ID:
			file=LIBRPIP_DT_FILE_I2C1_PINS;
			break;			
		case LIBRPIP_DT_FILE_I2S_PINS_ID:
			file=LIBRPIP_DT_FILE_I2S_PINS;
			break;			
		case LIBRPIP_DT_FILE_PWM_PINS_ID:
			file=LIBRPIP_DT_FILE_PWM_PINS;
			break;			
		case LIBRPIP_DT_FILE_SPI0_CSPINS_ID:
			file=LIBRPIP_DT_FILE_SPI0_CSPINS;
			break;			
		case LIBRPIP_DT_FILE_SPI0_PINS_ID:
			file=LIBRPIP_DT_FILE_SPI0_PINS;
			break;			
		case LIBRPIP_DT_FILE_SPI1_CSPINS_ID:
			file=LIBRPIP_DT_FILE_SPI1_CSPINS;
			break;			
		case LIBRPIP_DT_FILE_SPI1_PINS_ID:
			file=LIBRPIP_DT_FILE_SPI1_PINS;
			break;	
		case LIBRPIP_DT_FILE_1WIRE_PINS_ID:
			file=LIBRPIP_DT_FILE_1WIRE_PINS;
			break;						
	}	
	
	if(node && file) {
		sprintf(sfile,"%s/%s",node,file);
		*fd=open(sfile, flags);
		return *fd < 0 ? 0 : 1;  
	} else {
		return 0;
	}	
	
}

 

uint32_t librpip_dt_file_get_str(uint32_t moduleid, uint32_t fileid, char* value, uint32_t len) {

	int fd,result;
	result=0;

	if(librpip_dt_file_open(moduleid, fileid, O_RDONLY, &fd)) {
    		if(read(fd, value, len)) {
			result=1;
		}
		close(fd);
	} 
	return result;  
} 

uint32_t librpip_dt_file_get_raw(uint32_t moduleid, uint32_t fileid, uint8_t* buf, uint32_t len, uint32_t* rlen) {

	int fd,result;
	result=0;

	if(librpip_dt_file_open(moduleid, fileid, O_RDONLY, &fd)) {
		*rlen=read(fd, buf, len);
    		if(*rlen) {
			result=1;
		}
		close(fd);
	} 
	return result;  
} 