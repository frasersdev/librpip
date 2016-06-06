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

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "board.h"

extern uint32_t librpip_error_code;
extern uint32_t librpip_error_data;
extern uint32_t librpip_board;

uint32_t librpip_board_get_capabilities(void) {
	switch(librpip_board) {
		case LIBRPIP_BOARD_PI_A:
			return LIBRPIP_BOARD_PI_A_FEATURE;
			break;
		case LIBRPIP_BOARD_PI_B_R1:
			return LIBRPIP_BOARD_PI_B_R1_FEATURE;
			break;
		case LIBRPIP_BOARD_PI_B_R2:
			return LIBRPIP_BOARD_PI_B_R2_FEATURE;
			break;
		case LIBRPIP_BOARD_PI_A_PLUS:
			return LIBRPIP_BOARD_PI_A_PLUS_FEATURE;
			break;
		case LIBRPIP_BOARD_PI_B_PLUS:
			return LIBRPIP_BOARD_PI_B_PLUS_FEATURE;
			break;
		case LIBRPIP_BOARD_PI_ZERO:
			return LIBRPIP_BOARD_PI_ZERO_FEATURE;
			break;
		case LIBRPIP_BOARD_PI_COMPUTE:
			return  LIBRPIP_BOARD_PI_COMPUTE_FEATURE;
			break;
		case LIBRPIP_BOARD_PI2_B:
			return LIBRPIP_BOARD_PI2_B_FEATURE;
			break;	
		case LIBRPIP_BOARD_PI3_B:
			return LIBRPIP_BOARD_PI3_B_FEATURE;
			break;					
	}
	return 0;
}

uint32_t librpip_board_get_pins(void) {
	switch(librpip_board) {
		case LIBRPIP_BOARD_PI_A:
			return LIBRPIP_BOARD_PI_A_PINS;
			break;
		case LIBRPIP_BOARD_PI_B_R1:
			return LIBRPIP_BOARD_PI_B_R1_PINS;
			break;
		case LIBRPIP_BOARD_PI_B_R2:
			return LIBRPIP_BOARD_PI_B_R2_PINS;
			break;
		case LIBRPIP_BOARD_PI_A_PLUS:
			return LIBRPIP_BOARD_PI_A_PLUS_PINS;
			break;
		case LIBRPIP_BOARD_PI_B_PLUS:
			return LIBRPIP_BOARD_PI_B_PLUS_PINS;
			break;
		case LIBRPIP_BOARD_PI_ZERO:
			return LIBRPIP_BOARD_PI_ZERO_PINS;
			break;
		case LIBRPIP_BOARD_PI_COMPUTE:
			return  LIBRPIP_BOARD_PI_COMPUTE_PINS;
			break;
		case LIBRPIP_BOARD_PI2_B:
			return LIBRPIP_BOARD_PI2_B_PINS;
			break;	
		case LIBRPIP_BOARD_PI3_B:
			return LIBRPIP_BOARD_PI3_B_PINS;
			break;						
	}
	return 0;
}

uint32_t librpip_board_detect(void) {

	//best way to detect a board properly seems to be /proc/cpuinfo...
	struct stat fileStat;
	FILE* fd;
	char line[256];
	uint8_t i,keep_going; 
	unsigned long revision;
	
	revision = 0;
	librpip_error_code=0;
	librpip_error_data=0;
	
	if(stat(LIBRPIP_BOARD_CPUINFO,&fileStat)>=0) {
		//proc/cpuinfo exists
		//using library functions here as we are not talking direct to hardware
		fd = fopen(LIBRPIP_BOARD_CPUINFO, "r");
		if(fd) {
			keep_going=1;
			while (fgets(line, 256, fd) && keep_going) {
				if (!strncmp(line, "Revision", 8)) {
					for(i=0;i<strlen(line);i++) { 
						if(line[i]==':') {	
							revision = strtoul(&line[i+1],NULL,16);
							break;
						}
					}
					keep_going=0;
				} 
			}
			fclose(fd);
			
			switch(revision & 0xffffff) {  //mask out the warranty bits
				case 0x7:
				case 0x8:
				case 0x9:
					librpip_board=LIBRPIP_BOARD_PI_A;
					break;
				case 0x2:
				case 0x3:
					librpip_board=LIBRPIP_BOARD_PI_B_R1;
					break;
				case 0x4:
				case 0x5:
				case 0x6:
				case 0xd:
				case 0xe:
				case 0xf:
					librpip_board=LIBRPIP_BOARD_PI_B_R2;
					break;
				case 0x12:
				case 0x15:
					librpip_board=LIBRPIP_BOARD_PI_A_PLUS;
					break;
				case 0x10:
				case 0x13:
					librpip_board=LIBRPIP_BOARD_PI_B_PLUS;
					break;
				case 0x11:
				case 0x14:
					librpip_board=LIBRPIP_BOARD_PI_COMPUTE;
					break;
				case 0x900092:
				case 0x900093:
					librpip_board=LIBRPIP_BOARD_PI_ZERO;
					break;
				case 0xa01041:
				case 0xa21041:
					librpip_board=LIBRPIP_BOARD_PI2_B;
					break;	
				case 0xa02082:											
				case 0xa22082:	
					librpip_board=LIBRPIP_BOARD_PI3_B;
					break;								
				case 0:
				default:
					librpip_error_code=0x012;
					librpip_error_data=(uint32_t)revision;
			}
			
		} else {
			librpip_error_code=0x011;
		}
	} else {
		librpip_error_code=0x010;
	}		
	
	return librpip_error_code  ? 0 : 1;
}	

void librpip_board_model_print() {
	switch(librpip_board) {

		case LIBRPIP_BOARD_PI_A:
			fprintf(stderr,"[librpip INFO   ] Raspberry PI Model A detected.\n");
			break;
		case LIBRPIP_BOARD_PI_B_R1:
			fprintf(stderr,"[librpip INFO   ] Raspberry PI Model B Revision 1 detected.\n");
			break;
		case LIBRPIP_BOARD_PI_B_R2:
			fprintf(stderr,"[librpip INFO   ] Raspberry PI Model B Revision 2 detected.\n");
			break;
		case LIBRPIP_BOARD_PI_A_PLUS:
			fprintf(stderr,"[librpip INFO   ] Raspberry PI Model A+ detected.\n");
			break;
		case LIBRPIP_BOARD_PI_B_PLUS:
			fprintf(stderr,"[librpip INFO   ] Raspberry PI Model B+ detected.\n");
			break;
		case LIBRPIP_BOARD_PI_ZERO:
			fprintf(stderr,"[librpip INFO   ] Raspberry PI Zero detected.\n");
			break;
		case LIBRPIP_BOARD_PI_COMPUTE:
			fprintf(stderr,"[librpip INFO   ] Raspberry PI Compute Module detected.\n");
			break;	
		case LIBRPIP_BOARD_PI2_B:
			fprintf(stderr,"[librpip INFO   ] Raspberry PI 2 Model B detected.\n");
			break;	
		case LIBRPIP_BOARD_PI3_B:
			fprintf(stderr,"[librpip INFO   ] Raspberry PI 3 Model B detected.\n");
			break;						
		default:
			fprintf(stderr,"[librpip ERROR  ] Unable to detect board.\n");
	}
}
