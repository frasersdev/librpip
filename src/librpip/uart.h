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
 
#ifndef LIBRPIP_UART_H
#define LIBRPIP_UART_H

#define LIBRPIP_UART_BAUD_1200 			1200
#define LIBRPIP_UART_BAUD_2400 			2400
#define LIBRPIP_UART_BAUD_4800 			4800
#define LIBRPIP_UART_BAUD_9600 			9600
#define LIBRPIP_UART_BAUD_19200 		19200
#define LIBRPIP_UART_BAUD_38400 		38400
#define LIBRPIP_UART_BAUD_57600 		57600
#define LIBRPIP_UART_BAUD_115200 		115200
#define LIBRPIP_UART_BAUD_230400 		230400
#define LIBRPIP_UART_BAUD_460800 		460800
#define LIBRPIP_UART_BAUD_500000 		500000
#define LIBRPIP_UART_BAUD_576000 		576000
#define LIBRPIP_UART_BAUD_921600 		921600
#define LIBRPIP_UART_BAUD_1000000 		1000000
#define LIBRPIP_UART_BAUD_1152000 		1152000
#define LIBRPIP_UART_BAUD_1500000 		1500000
#define LIBRPIP_UART_BAUD_2000000 		2000000
#define LIBRPIP_UART_BAUD_2500000 		2500000
#define LIBRPIP_UART_BAUD_3000000 		3000000
#define LIBRPIP_UART_BAUD_3500000 		3500000
#define LIBRPIP_UART_BAUD_4000000 		4000000

#define LIBRPIP_UART_SIZE_5				5  /* not valid for uart1 */
#define LIBRPIP_UART_SIZE_6				6  /* not valid for uart1 */
#define LIBRPIP_UART_SIZE_7				7
#define LIBRPIP_UART_SIZE_8				8

#define LIBRPIP_UART_PARITY_OFF			0
#define LIBRPIP_UART_PARITY_ODD			1  /* not valid for uart1 */
#define LIBRPIP_UART_PARITY_EVEN		2  /* not valid for uart1 */

#define LIBRPIP_UART_STOPBITS_1			1
#define LIBRPIP_UART_STOPBITS_2			2  /* not valid for uart1 */

#define LIBRPIP_UART_MODE_BINARY		0
#define LIBRPIP_UART_MODE_ASCII			1




//exposed functions
uint32_t librpipUartConfigWrite(uint32_t id, uint32_t baud, uint32_t csize, uint32_t parity, uint32_t stop, uint32_t mode);
uint32_t librpipUartConfigRead(uint32_t id, uint32_t* baud, uint32_t* csize, uint32_t* parity, uint32_t* stop, uint32_t* mode);
uint32_t librpipUartWrite(uint32_t id, void* buf, uint16_t len); 
uint32_t librpipUartRead(uint32_t id, void* buf, uint16_t len, uint32_t timeout); 

//internal functions
uint32_t librpip_uart_init(uint32_t id);
uint32_t librpip_uart_close(uint32_t id);
uint32_t librpip_uart_usable(void);
uint32_t librpip_uart_validate_device(uint32_t id, int* fd);
uint32_t librpip_uart_tansaction(uint32_t id, struct librpip_transaction_t* t);
uint32_t librpip_uart_write_raw(uint32_t id, uint8_t* buf, uint16_t len); 
uint32_t librpip_uart_read_raw(uint32_t id, uint8_t* buf, uint16_t len, uint32_t timeout);
uint32_t librpip_uart_read_fragment(int* fd, uint16_t* size, uint32_t* delta, uint8_t* buf, uint16_t len, uint32_t frag_wait);
uint32_t librpip_uart_config_set(int* fd, uint32_t baud, uint32_t csize, uint32_t parity, uint32_t stop, uint32_t mode);
uint32_t librpip_uart_config_get(int* fd, uint32_t* baud, uint32_t* csize, uint32_t* parity, uint32_t* stop, uint32_t* mode);
uint32_t librpip_uart_config_convert_l2b(uint32_t baud);
uint32_t librpip_uart_config_convert_b2l(uint32_t baud);
uint32_t librpip_uart_frag_wait_calc(uint32_t baud);

#endif