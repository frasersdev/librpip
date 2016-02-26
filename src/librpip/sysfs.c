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
#include "sysfs.h"

extern uint32_t librpip_error_code;

uint32_t librpip_sysfs_node_exists(uint32_t sysfsid) {

	struct stat fileStat;
	int result;

	switch(sysfsid) {
		case LIBRPIP_SYSFS_PWM0_ID:						
			result=stat(LIBRPIP_SYSFS_PWM0,&fileStat);
			break;
		case LIBRPIP_SYSFS_PWM1_ID:
			result=stat(LIBRPIP_SYSFS_PWM1,&fileStat);
			break;
		default:
			result=-1;
	}
	return result < 0 ? 0 : 1;  
} 

uint32_t librpip_sysfs_node_file_validate(uint32_t sysfsid) {

	int fd1,fd2,fd3,fd4;
	uint32_t result;
	
	result=0;
	
    switch(sysfsid) {
		case LIBRPIP_SYSFS_PWM0_ID:						
		case LIBRPIP_SYSFS_PWM1_ID:
			if(librpip_sysfs_node_file_open(sysfsid, LIBRPIP_SYSFS_PWM_DUTYCYCLE_ID, O_RDWR, &fd1) &&
			   librpip_sysfs_node_file_open(sysfsid, LIBRPIP_SYSFS_PWM_ENABLE_ID, O_RDWR, &fd2) &&
			   librpip_sysfs_node_file_open(sysfsid, LIBRPIP_SYSFS_PWM_PERIOD_ID, O_RDWR, &fd3) &&
			   librpip_sysfs_node_file_open(sysfsid, LIBRPIP_SYSFS_PWM_POLARITY_ID, O_RDWR, &fd4)) {
				result=1;
			   }
			if(fd1>0) close(fd1);
			if(fd2>0) close(fd2);
			if(fd3>0) close(fd3);
			if(fd4>0) close(fd4);
			break;
	}
	return result;
	
}



uint32_t librpip_sysfs_node_file_open(uint32_t sysfsid, uint32_t fileid, int flags, int* fd) {

	const char* node=NULL;
	const char* file=NULL;
	char	sfile[200];

	switch(sysfsid) {
		case LIBRPIP_SYSFS_PWM0_ID:						
			node=LIBRPIP_SYSFS_PWM0;
			break;
		case LIBRPIP_SYSFS_PWM1_ID:
			node=LIBRPIP_SYSFS_PWM1;
			break;
	}
	
	switch(fileid) {
		case LIBRPIP_SYSFS_PWM_DUTYCYCLE_ID:
			file=LIBRPIP_SYSFS_PWM_DUTYCYCLE;
			break;
		case LIBRPIP_SYSFS_PWM_ENABLE_ID:
			file=LIBRPIP_SYSFS_PWM_ENABLE;
			break;		
		case LIBRPIP_SYSFS_PWM_PERIOD_ID:
			file=LIBRPIP_SYSFS_PWM_PERIOD;
			break;		
		case LIBRPIP_SYSFS_PWM_POLARITY_ID:					
			file=LIBRPIP_SYSFS_PWM_POLARITY;
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

uint32_t librpip_sysfs_node_file_get_int(uint32_t sysfsid, uint32_t fileid, int* value) {

	int fd,result;
	char buf[20];
	result=0;

	if(librpip_sysfs_node_file_open(sysfsid, fileid, O_RDONLY, &fd)) {
    		if(read(fd, buf, sizeof(buf))) {
			*value=atoi(buf);
			result=1;
		}
		close(fd);
	} 
	return result;  
} 

uint32_t librpip_sysfs_node_file_get_str(uint32_t sysfsid, uint32_t fileid, char* value, uint32_t len) {

	int fd,result;
	result=0;

	if(librpip_sysfs_node_file_open(sysfsid, fileid, O_RDONLY, &fd)) {
    		if(read(fd, value, len)) {
			result=1;
		}
		close(fd);
	} 
	return result;  
} 	

uint32_t librpip_sysfs_node_file_set_int(uint32_t sysfsid, uint32_t fileid, int value) {

	int fd,result;
	char buf[20];
	result=0;
	
	sprintf(buf,"%u",value); //an int will fit into 20 chars

	if(librpip_sysfs_node_file_open(sysfsid, fileid, O_RDWR, &fd)) {
    		if(write(fd, buf, sizeof(buf)))  {
			result=1;
		}
		close(fd);
	}
    
	return result;     
} 		
	
uint32_t librpip_sysfs_node_file_set_str(uint32_t sysfsid, uint32_t fileid, char* value, uint32_t len) {

	int fd,result;
	result=0;
	
	if(librpip_sysfs_node_file_open(sysfsid, fileid, O_RDWR, &fd)) {
    		if(write(fd, value, len))  {
			result=1;
		}
		close(fd);
	}
    
	return result;     
} 			
