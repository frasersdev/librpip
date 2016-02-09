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
 
#include <errno.h>	
#include <fcntl.h>			
#include <termios.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>			
#include <sys/types.h>

#include "rpi.h"
#include "device.h"
#include "pins.h"
#include "error.h"
#include "transact.h"
#include "uart.h"


extern uint32_t librpip_feature_set;
extern uint32_t librpip_error_code;
extern uint32_t librpip_error_data;
extern uint32_t librpip_error_extra;
extern uint32_t librpip_board;
extern uint32_t librpip_flags;
extern uint32_t librpip_pins_used;

int librpip_uart_dev[2];
int librpip_uart_dev_loaded[2];
int librpip_uart_frag_wait[2];

/* exposed functions */
uint32_t librpip_uart_init(uint32_t id) {

	librpip_error_code=0;
	librpip_error_data=0;
	
	switch(id) {
		case 0:
			
			if(librpip_device_node_exists(LIBRPIP_DEV_UART0_ID)) {
				librpip_uart_dev_loaded[id]=1;
				librpip_pins_used |= LIBRPIP_PINS_UART0;   //pins are in use
				if(librpip_flags & LIBRPIP_FLAG_SKIP_UART0) { //we have been asked not not use I2C0
					librpip_error_code=0x603;					
					librpip_error_data=id;
				} else {
					if(librpip_device_node_open(LIBRPIP_DEV_UART0_ID, O_RDWR | O_NOCTTY | O_NDELAY, &librpip_uart_dev[0])) {
						librpip_feature_set |= LIBRPIP_FEATURE_UART0;  
					} else {
						//device exists but we cant RW it
						librpip_error_code=0x601;					
						librpip_error_data=id;
					}	
				}
			} else {
				//device does not exist
				librpip_uart_dev_loaded[id]=0;
				librpip_error_code=0x600;						
				librpip_error_data=id;
			}
			break;
		case 1:
			if(librpip_device_node_exists(LIBRPIP_DEV_UART1_ID)) {
				librpip_uart_dev_loaded[id]=1;
				librpip_pins_used |= LIBRPIP_PINS_UART1;   //pins are in use
				if(librpip_flags & LIBRPIP_FLAG_SKIP_UART1) { //we have been asked not not use I2C0
					librpip_error_code=0x603;					
					librpip_error_data=id;
				} else {
					if(librpip_device_node_open(LIBRPIP_DEV_UART1_ID, O_RDWR | O_NOCTTY | O_NDELAY, &librpip_uart_dev[1])) {
						librpip_feature_set |= LIBRPIP_FEATURE_UART1;  
					} else {
						//device exists but we cant RW it
						librpip_error_code=0x601;					
						librpip_error_data=id;
					}	
				}
			} else {
				//device does not exist
				librpip_uart_dev_loaded[id]=0;
				librpip_error_code=0x600;						
				librpip_error_data=id;
			}
			break;
		default:
			librpip_error_code=0x602;
			librpip_error_data=id;
			break;
	}
	
	if(librpip_error_code==0) {
		if(!(librpip_flags & LIBRPIP_FLAG_NO_RESET)) {
			librpip_uart_config_set(&librpip_uart_dev[id], 	LIBRPIP_UART_BAUD_19200, 
									LIBRPIP_UART_SIZE_8, 
									LIBRPIP_UART_PARITY_OFF, 
									LIBRPIP_UART_STOPBITS_1, 
									LIBRPIP_UART_MODE_ASCII);
			librpip_uart_frag_wait[id] = librpip_uart_frag_wait_calc(LIBRPIP_UART_BAUD_19200);
		} 
	}	
	
	return librpip_error_code  ? 0 : 1;
}


uint32_t librpip_uart_close(uint32_t id) {

	librpip_error_code=0;
	librpip_error_data=0;
	
	switch(id) {
		case 0:
		case 1:
			close(librpip_uart_dev[id]);
			break;
		default:
			librpip_error_code=0x602;
			librpip_error_data=id;
			break;
	}	
	return librpip_error_code ? 0 : 1;
}


uint32_t librpip_uart_usable(void) {

	librpip_error_code=0;
	librpip_error_data=0;

	if(!librpip_uart_dev_loaded[0] && !librpip_uart_dev_loaded[1]) {
		librpip_error_code=0x604;						
	}
	
	return librpip_error_code ? 0 : 1;	
	
}

uint32_t librpipUartConfigWrite(uint32_t id, uint32_t baud, uint32_t csize, uint32_t parity, uint32_t stop, uint32_t mode) {
	
	int fd;
	fd=0;
	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(librpip_uart_validate_device(id, &fd)) {
	
		if(!librpip_uart_config_convert_l2b(baud)) {
			librpip_error_code=0x620;
			librpip_error_data=id;
			librpip_error_extra=baud;
		}
		if(mode > 2) {  
			librpip_error_code=0x621;
			librpip_error_data=id;
			librpip_error_extra=mode;
		}		
		switch(id) {
			case 0:
				if(csize < 5 || csize > 8) {
					librpip_error_code=0x622;
					librpip_error_data=id;
					librpip_error_extra=csize;
				}
				if(parity > 2) {
					librpip_error_code=0x623;
					librpip_error_data=id;
					librpip_error_extra=parity;
				}
				if(stop > 2) {
					librpip_error_code=0x624;
					librpip_error_data=id;
					librpip_error_extra=stop;
				}
				break;
			case 1:
				if(csize < 7 || csize > 8) {
					librpip_error_code=0x622;
					librpip_error_data=id;
					librpip_error_extra=csize;
				}
				if(parity > 0) {
					librpip_error_code=0x623;
					librpip_error_data=id;
					librpip_error_extra=parity;
				}
				if(stop > 0) {
					librpip_error_code=0x624;
					librpip_error_data=id;
					librpip_error_extra=stop;
				}
				break;
		}

		if(!librpip_error_code) {
			librpip_uart_config_set(&fd, baud, csize, parity, stop, mode);
		}	
	}
	
	if(librpip_error_code) {
		librpip_error_data=id;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	} else {
		librpip_uart_frag_wait[id]=librpip_uart_frag_wait_calc(baud);
	}
	
	return librpip_error_code ? 0 : 1;
}


uint32_t librpipUartConfigRead(uint32_t id, uint32_t* baud, uint32_t* csize, uint32_t* parity, uint32_t* stop, uint32_t* mode) {

	int fd;
	fd=0;
	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(librpip_uart_validate_device(id, &fd)) {
		librpip_uart_config_get(&fd, baud,  csize,  parity,  stop,  mode);
	}
	
	if(librpip_error_code) {
		librpip_error_data=id;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	
	return librpip_error_code ? 0 : 1;
	
	
}



uint32_t librpipUartWrite(uint32_t id, void* buf, uint16_t len) {
	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(len > 0) {
		librpip_uart_write_raw(id, (uint8_t*)buf, len);
	} else {
		librpip_error_code=0x632;
		librpip_error_data=id;	
		librpip_error_data=len;	
	}
	
	if(librpip_error_code) {
		librpip_error_data=id;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	
	return librpip_error_code ? 0 : 1;
}

uint32_t librpipUartRead(uint32_t id, void* buf, uint16_t len, uint32_t timeout) {


	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(len > 0) {
		memset(buf, '\0',len);
		librpip_uart_read_raw(id, (uint8_t*)buf, len, timeout);
	} else {
		librpip_error_code=0x642;
		librpip_error_data=id;	
		librpip_error_data=len;	
	}

	
	if(librpip_error_code) {
		librpip_error_data=id;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	
	return librpip_error_code ? 0 : 1;
}

uint32_t librpip_uart_tansaction(uint32_t id, struct librpip_transaction_t* t) {


	struct librpip_msg_t* m = t->first_msg;
	uint16_t i;
	
	for(i=0; i<t->len;i++) {
		
		if(!librpip_error_code) {
			if(m->dir & LIBRPIP_TX_MSG_TX) {
				librpip_uart_write_raw(id, m->tx->buf, m->tx->len);
			} 
			if(m->dir & LIBRPIP_TX_MSG_RX) {
				librpip_uart_read_raw(id, m->rx->buf,m->rx->len, 0);
			} 
			m=m->next;	//advance to the next message 
		}
	}
	
	return librpip_error_code ? 0 : 1;
}

// internal functions
uint32_t librpip_uart_validate_device(uint32_t id, int* fd) {

	switch(id) {
		case 0:
			if(librpip_feature_set & LIBRPIP_FEATURE_UART0) {
				*fd=librpip_uart_dev[0];
			} else {
				librpip_error_code=0x605;
				librpip_error_data=id;
			}
			break;
		case 1:
			if(librpip_feature_set & LIBRPIP_FEATURE_UART1) {
				*fd=librpip_uart_dev[1];
			} else {
				librpip_error_code=0x605;
				librpip_error_data=id;
			}
			break;
		default:
			librpip_error_code=0x602;
			librpip_error_data=id;
		break;
	}
	return librpip_error_code ? 0 : 1;
}



uint32_t librpip_uart_write_raw(uint32_t id, uint8_t* buf, uint16_t len) {

	int num,fd;
	fd=0;

	
	if(librpip_uart_validate_device(id, &fd)) {
		
		num = write(fd, buf, len);
		if(num<0) { 
			librpip_error_code=0x630;
			librpip_error_data=id;
			librpip_error_extra=errno;
		}
		else if(num != len) {
			librpip_error_code=0x631;
			librpip_error_data=id;
			librpip_error_extra=num;
		}
	}

	return librpip_error_code ? 0 : 1;
}


uint32_t librpip_uart_read_raw(uint32_t id, uint8_t* buf, uint16_t len, uint32_t timeout) {

	uint8_t keep_waiting;
	uint16_t frag_size,len_left;
	uint32_t delta, result;
	int fd;
	fd=0;
	
	uint8_t* p;
	int atimeout;
	
	keep_waiting=1;
	p = buf;
	len_left=len;
	atimeout = (int)timeout+(int)(librpip_uart_frag_wait[id] * ((len/8)+1) ) ;


	if(librpip_uart_validate_device(id, &fd)) {
		
		while(keep_waiting) {
			frag_size=0;
			result=librpip_uart_read_fragment(&fd, &frag_size, &delta, p, len_left, librpip_uart_frag_wait[id]);
			if(frag_size > 0) {
				p += frag_size;
				len_left -= frag_size;
			} 
			if(!result) {
				// something wrong with the read
				librpip_error_data=id;
				keep_waiting=0;
			} else if(len_left==0) {
				//hmmm we've run out of space, exit loop 
				keep_waiting=0;
			} else {
				//get ready to try again
				atimeout-=delta;
				if(atimeout<=0) {
					//hmmm we've run out of time, exit loop 
					keep_waiting=0;
				} 
			}
		}
		
		if(len_left==len) {
			//nothing got read
			librpip_error_code=0x641;
			librpip_error_data=id;
		}
	}
	return librpip_error_code ? 0 : 1;
}

uint32_t librpip_uart_read_fragment(int* fd, uint16_t* size, uint32_t* delta, uint8_t* buf, uint16_t len, uint32_t frag_wait) {

	uint8_t keep_waiting;
	uint32_t ltimeout;
	int num;

	keep_waiting=1;
	ltimeout=frag_wait;
	num=0;
	*size=0;
	*delta=0;

	while(keep_waiting) {
		num = read(*fd, buf, len);
		if(num<0 && errno!=EAGAIN) {
			// bad error, exit the loop
			keep_waiting=0;
			librpip_error_code=0x640;
			librpip_error_extra=errno;
		} else if(num>0) { 
			// got something, lets exit the loop
			*size=num;
			keep_waiting=0;
		} else if(ltimeout==0) {
			//hmmm we've run out of time, exit loop 
			keep_waiting=0;
		} else {
			//sleep a bit and try again
			usleep(1);
			ltimeout--;
		}
	}
	*delta = frag_wait - ltimeout;
	return librpip_error_code ? 0 : 1;
}

// these functions perform no validation

uint32_t librpip_uart_config_set(int* fd, uint32_t baud, uint32_t csize, uint32_t parity, uint32_t stop, uint32_t mode) {

	struct termios options;
	if(!tcgetattr(*fd, &options)) {	
	
		options.c_cflag = 0;
		options.c_iflag = 0;
		options.c_oflag = 0;
		options.c_lflag = 0;

		switch(csize) { //i could do some maths and bit shifting instead but this is guaranteed to work if the structure changes change.
			case LIBRPIP_UART_SIZE_5:
				options.c_cflag |= CS5;
				break;
			case LIBRPIP_UART_SIZE_6:
				options.c_cflag |= CS6;
				break;
			case LIBRPIP_UART_SIZE_7:
				options.c_cflag |= CS7;
				break;
			case LIBRPIP_UART_SIZE_8:
				options.c_cflag |= CS8;
				break;
		}
	
		if(parity) {
			options.c_cflag |= PARENB;
			options.c_cflag |= (parity == LIBRPIP_UART_PARITY_ODD ?  PARODD : 0);
		}
		options.c_cflag |= (stop == LIBRPIP_UART_STOPBITS_2 ? CSTOPB : 0 );  
		options.c_cflag |= CLOCAL | CREAD;
		options.c_iflag |= mode ? ICRNL : 0;
	
		cfsetspeed(&options,librpip_uart_config_convert_l2b(baud));
	
		if(!tcflush(*fd, TCIFLUSH)) {
			if(tcsetattr(*fd, TCSANOW, &options) < 0)  {
				librpip_error_code=0x612;
				librpip_error_data=errno;
			}
		} else {
			librpip_error_code=0x611;
			librpip_error_data=errno;
		}
	} else { 
		librpip_error_code=0x610;
		librpip_error_data=errno;
	}
	
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_uart_config_get(int* fd, uint32_t* baud, uint32_t* csize, uint32_t* parity, uint32_t* stop, uint32_t* mode) {

	*baud = 0; 
	*csize = 0; 
	*parity = 0; 
	*stop = 0; 
	*mode = 0;
	
	struct termios options;
	if(!tcgetattr(*fd, &options)) {	
	
		*baud = librpip_uart_config_convert_b2l(cfgetispeed(&options));
	
		switch(options.c_cflag & CS8) {
			case CS5:	//0x0
				*csize=LIBRPIP_UART_SIZE_5;
				break;
			case CS6:   //0x20
				*csize=LIBRPIP_UART_SIZE_6;
				break;
			case CS7:   //0x40
				*csize=LIBRPIP_UART_SIZE_7;
				break;
			case CS8:   //0x60
				*csize=LIBRPIP_UART_SIZE_8;
				break;
		}
	
		if(options.c_cflag & PARENB) {  //parity is on
			*parity = options.c_cflag & PARODD ? LIBRPIP_UART_PARITY_ODD : LIBRPIP_UART_PARITY_EVEN ;	
		}
	
		*stop =  options.c_cflag & CSTOPB ? LIBRPIP_UART_STOPBITS_2 : LIBRPIP_UART_STOPBITS_1 ;	
	
		*mode =  options.c_iflag & ICRNL ? LIBRPIP_UART_MODE_ASCII : LIBRPIP_UART_MODE_BINARY ;
		
	} else { 
		librpip_error_code=0x610;
		librpip_error_data=errno;
	}
	
	return librpip_error_code  ? 0 : 1;
	
}

uint32_t librpip_uart_config_convert_l2b(uint32_t baud) {
	switch(baud) {
		case LIBRPIP_UART_BAUD_1200:
			return B1200;
			break;
		case LIBRPIP_UART_BAUD_2400:
			return B2400;
			break;
		case LIBRPIP_UART_BAUD_4800:
			return B4800;
			break;
		case LIBRPIP_UART_BAUD_9600:
			return B9600;
			break;
		case LIBRPIP_UART_BAUD_19200:
			return B19200;
			break;
		case LIBRPIP_UART_BAUD_38400:
			return B38400;
			break;
		case LIBRPIP_UART_BAUD_57600:
			return B57600;
			break;
		case LIBRPIP_UART_BAUD_115200:
			return B115200;
			break;
		case LIBRPIP_UART_BAUD_230400:
			return B230400;
			break;
		case LIBRPIP_UART_BAUD_460800:
			return B460800;
			break;
		case LIBRPIP_UART_BAUD_500000:
			return B500000;
			break;
		case LIBRPIP_UART_BAUD_576000:
			return B576000;
			break;
		case LIBRPIP_UART_BAUD_921600:
			return B921600;
			break;
		case LIBRPIP_UART_BAUD_1000000:
			return B1000000;
			break;
		case LIBRPIP_UART_BAUD_1152000:
			return B1152000;
			break;
		case LIBRPIP_UART_BAUD_1500000:
			return B1500000;
			break;
		case LIBRPIP_UART_BAUD_2000000:
			return B2000000;
			break;
		case LIBRPIP_UART_BAUD_2500000:
			return B2500000;
			break;
		case LIBRPIP_UART_BAUD_3000000:
			return B3000000;
			break;
		case LIBRPIP_UART_BAUD_3500000:
			return B3500000;
			break;
		case LIBRPIP_UART_BAUD_4000000:
			return B4000000;
			break;
			
	}
	return 0;
}

uint32_t librpip_uart_config_convert_b2l(uint32_t baud) {
	switch(baud) {
		case B1200:
			return LIBRPIP_UART_BAUD_1200;
			break;
		case B2400:
			return LIBRPIP_UART_BAUD_2400;
			break;
		case B4800:
			return LIBRPIP_UART_BAUD_4800;
			break;
		case B9600:
			return LIBRPIP_UART_BAUD_9600;
			break;
		case B19200:
			return LIBRPIP_UART_BAUD_19200;
			break;
		case B38400:
			return LIBRPIP_UART_BAUD_38400;
			break;
		case B57600:
			return LIBRPIP_UART_BAUD_57600;
			break;
		case B115200:
			return LIBRPIP_UART_BAUD_115200;
			break;
		case B230400:
			return LIBRPIP_UART_BAUD_230400;
			break;
		case B460800:
			return LIBRPIP_UART_BAUD_460800;
			break;
		case B500000:
			return LIBRPIP_UART_BAUD_500000;
			break;
		case B576000:
			return LIBRPIP_UART_BAUD_576000;
			break;
		case B921600:
			return LIBRPIP_UART_BAUD_921600;
			break;
		case B1000000:
			return LIBRPIP_UART_BAUD_1000000;
			break;
		case B1152000:
			return LIBRPIP_UART_BAUD_1152000;
			break;
		case B1500000:
			return LIBRPIP_UART_BAUD_1500000;
			break;
		case B2000000:
			return LIBRPIP_UART_BAUD_2000000;
			break;
		case B2500000:
			return LIBRPIP_UART_BAUD_2500000;
			break;
		case B3000000:
			return LIBRPIP_UART_BAUD_3000000;
			break;
		case B3500000:
			return LIBRPIP_UART_BAUD_3500000;
			break;
		case B4000000:
			return LIBRPIP_UART_BAUD_4000000;
			break;
	}
	return 0;
}

uint32_t librpip_uart_frag_wait_calc(uint32_t baud) {
	switch(baud) {
		case LIBRPIP_UART_BAUD_1200:
			return 1600;
			break;
		case LIBRPIP_UART_BAUD_2400:
			return 800;
			break;
		case LIBRPIP_UART_BAUD_4800:
			return 400;
			break;
		case LIBRPIP_UART_BAUD_9600:
			return 180;
			break;
		case LIBRPIP_UART_BAUD_19200:
			return 100;
			break;
		case LIBRPIP_UART_BAUD_38400:
			return 50;
			break;
		case LIBRPIP_UART_BAUD_57600:
			return 33;
			break;
		case LIBRPIP_UART_BAUD_115200:
			return 17;
			break;
		case LIBRPIP_UART_BAUD_230400:
			return 8;
			break;
		case LIBRPIP_UART_BAUD_460800:
		case LIBRPIP_UART_BAUD_500000:
		case LIBRPIP_UART_BAUD_576000:
			return 4;
			break;
		case LIBRPIP_UART_BAUD_921600:
		case LIBRPIP_UART_BAUD_1000000:
		case LIBRPIP_UART_BAUD_1152000:
		case LIBRPIP_UART_BAUD_1500000:
		case LIBRPIP_UART_BAUD_2000000:
			return 2;
			break;
		case LIBRPIP_UART_BAUD_2500000:
		case LIBRPIP_UART_BAUD_3000000:
		case LIBRPIP_UART_BAUD_3500000:
		case LIBRPIP_UART_BAUD_4000000:
			return 1;
			break;
			
	}
	return 0;
}
