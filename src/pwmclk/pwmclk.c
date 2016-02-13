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
 
/* 
 * This program starts up the pwm clock if ti isn't already running
 *
 * Do I need to stop the clock and start it again if it is already running? well time will tell I guess...
 *
 */

 
#define PWMCLK_MMAP		"/sys/firmware/devicetree/base/soc/ranges" 
 
#define PWMCLK_SPEED		0x00010000 // divide clock by 16
#define PWMCLK_PASSWORD     	(0x5A << 24) 	
#define PWMCLK_REG_CTRL   	0x1010a0 	//https://www.scribd.com/doc/127599939/BCM2835-Audio-clocks
#define PWMCLK_REG_DIV    	0x1010a4 	//https://www.scribd.com/doc/127599939/BCM2835-Audio-clocks

#define PWMCLK_STOPMASK		0x0000070f //0b11100001111  just sets enable to zero without affecting other values
#define PWMCLK_BUSY_BIT		0x00000080 //0b   10000000
#define PWMCLK_ENABLE_BIT	0x00000010 //0b   00010000
#define PWMCLK_OSC_BIT		0x00000001 //0b   00000001
 
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>	
#include <sys/mman.h>
#include <sys/types.h>	


uint32_t get_and_map_mem();
void start_pwmclk();
uint32_t reg_read(volatile uint32_t* address);
void reg_write(volatile uint32_t* address,uint32_t value);

uint32_t map_err;
static volatile uint32_t*  mem_map = MAP_FAILED;
 
 
int main(int argc, char * argv[]) {

	if(get_and_map_mem()) {
		start_pwmclk();
	}
	return map_err ;
}
 
uint32_t get_and_map_mem() {

	int fd, dev;
	uint8_t buf[4];
	uint32_t mem_base, mem_size;
    
	map_err=0;
	mem_base=0;
	mem_size=0;

	fd = open(PWMCLK_MMAP, O_RDONLY);
	if(fd>=0)   {
		lseek(fd, 4, SEEK_SET);
		if (read(fd, buf, sizeof(buf)) == sizeof(buf)) {
			mem_base = (buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3] );
			fprintf(stderr, "[pwmclk INFO] Mem Base is 0x%x\n",mem_base);
		}
		lseek(fd, 8, SEEK_SET);
		if (read(fd, buf, sizeof(buf)) == sizeof(buf)) {
			mem_size = (buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3] );
			fprintf(stderr, "[pwmclk INFO] Mem Size is 0x%x\n",mem_size);	
		}	
		close(fd);
		 if(mem_base && mem_size) {
			if ((dev = open("/dev/mem", O_RDWR | O_SYNC) ) >= 0)     {
				mem_map = (uint32_t *)mmap(NULL, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, dev, mem_base);
    			if (mem_map == MAP_FAILED) {
			    	fprintf(stderr, "[pwmclk ERROR] Clock Not Set. Failure mapping to /dev/mem: %s\n", strerror(errno));
    				map_err=4;
    			} else {
			    	fprintf(stderr, "[pwmclk INFO] Mapped to memory\n");
    			}
			} else {
				fprintf(stderr, "[pwmclk ERROR] Clock Not Set. Failure opening /dev/mem: %s\n", strerror(errno));
    				map_err=3;
    			}        
    		} else {
			fprintf(stderr, "[pwmclk ERROR] Clock Not Set. Error reading memory settings from /sys/firmware/devicetree/base/soc/ranges: %s\n", strerror(errno));
    			map_err=2;
    		}
	} else {
		fprintf(stderr, "[pwmclk ERROR] Clock Not Set. Failure opening /sys/firmware/devicetree/base/soc/ranges: %s\n", strerror(errno));
		map_err=1;
	}
	 return map_err ? 0 : 1;
}

void start_pwmclk() {
	uint32_t current_reg;
	
	current_reg=reg_read(mem_map + PWMCLK_REG_CTRL/4);

	if(current_reg & PWMCLK_BUSY_BIT) {  									//clock is running!
		fprintf(stderr, "[pwmclk INFO] Clock already running...\n");						
	} else {
		fprintf(stderr, "[pwmclk INFO] Starting clock...\n");
		reg_write(mem_map + PWMCLK_REG_DIV/4, PWMCLK_PASSWORD | PWMCLK_SPEED); 				//set clock
		reg_write(mem_map + PWMCLK_REG_CTRL/4, PWMCLK_PASSWORD | PWMCLK_ENABLE_BIT | PWMCLK_OSC_BIT); 	//start clock
		while(!(current_reg & PWMCLK_BUSY_BIT)) {
			current_reg=reg_read(mem_map + PWMCLK_REG_CTRL/4);
		}
		fprintf(stderr, "[pwmclk INFO] Clock is Running!\n");
	}
}

uint32_t reg_read(volatile uint32_t* address) {
	return *address;
}

void reg_write(volatile uint32_t* address,uint32_t value)  {
	*address = value;
} 	



