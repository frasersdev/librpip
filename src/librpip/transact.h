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

#ifndef LIBRPIP_TX_H
#define LIBRPIP_TX_H

#define LIBRPIP_TX_NOBUF		0x0	

#define LIBRPIP_TX_STATUS_NEW		0x1	
#define LIBRPIP_TX_STATUS_MSG		0x2	
#define LIBRPIP_TX_STATUS_SENTOK	0x3	
#define LIBRPIP_TX_STATUS_FAILED	0x4	

#define LIBRPIP_TX_MODE_I2C		0x1	
#define LIBRPIP_TX_MODE_SPI		0x2
#define LIBRPIP_TX_MODE_SPI_3W		0x3
#define LIBRPIP_TX_MODE_UART		0x4	

#define LIBRPIP_TX_MSG_TX		0x1	
#define LIBRPIP_TX_MSG_RX		0x2	
#define LIBRPIP_TX_MSG_TXRX		(LIBRPIP_TX_MSG_TX | LIBRPIP_TX_MSG_RX)	




struct librpip_msg_buffer_t {
	uint8_t* buf;
	uint16_t len;
};

struct librpip_msg_t {
	uint8_t dir;
	char	var;
	struct librpip_msg_buffer_t* tx;
	struct librpip_msg_buffer_t* rx;
	struct librpip_msg_t* next;
	struct librpip_msg_t* prev;
}; 

struct librpip_transaction_t {
	uint8_t mode;
	uint8_t bpw;
	uint8_t status;
	struct librpip_msg_t* first_msg;
	struct librpip_msg_t* curr_msg;
	struct librpip_msg_t* last_msg;
	uint16_t len;
}; 


//exposed functions
struct librpip_transaction_t* 	librpipTransactionCreate(uint8_t mode, uint8_t bpw);
void 				librpipTransactionConfigRead(struct librpip_transaction_t* t, uint8_t* mode, uint8_t* bpw, uint8_t* status, uint16_t* len);
uint32_t 			librpipTransactionMsgAdd(struct librpip_transaction_t* t, uint8_t dir, void* txbuf, uint16_t len);
uint32_t 			librpipTransactionMsgAddRegRead(struct librpip_transaction_t* t, uint8_t reg, uint16_t len);
uint32_t 			librpipTransactionMsgAddRegWrite(struct librpip_transaction_t* t, uint8_t reg, uint8_t value);
uint32_t 			librpipTransactionMsgVarAdd(struct librpip_transaction_t* t, uint8_t dir, char var);
uint32_t 			librpipTransactionMsgVarSet(struct librpip_transaction_t* t, char var,  void* txbuf, uint16_t len);
uint32_t 			librpipTransactionSend(struct librpip_transaction_t* t, uint32_t device, uint32_t client);
uint32_t			librpipTransactionRead(struct librpip_transaction_t* t, void* result, uint16_t len);
uint32_t 			librpipTransactionReadSize(struct librpip_transaction_t* t, uint16_t* size);
uint32_t 			librpipTransactionDestroy(struct librpip_transaction_t* t);

//internal functions
void 				librpip_transaction_msg_var_check(struct librpip_transaction_t* t);
//these create the actual xfer structure as needed by the kernel modules
struct spi_ioc_transfer* 	librpip_transaction_spi_create(struct librpip_transaction_t* t);
struct i2c_rdwr_ioctl_data* 	librpip_transaction_i2c_create(struct librpip_transaction_t* t, uint32_t client);
//utility functions
struct librpip_msg_buffer_t* 	librpip_transaction_buffer_create(uint8_t* buf,uint16_t len);
void 				librpip_transaction_message_destroy(struct librpip_msg_t* m);
void 				librpip_transaction_buffer_destroy(struct librpip_msg_buffer_t* b);
void 				librpip_transaction_spi_destroy(struct spi_ioc_transfer* spi);
void 				librpip_transaction_i2c_destroy(struct i2c_rdwr_ioctl_data* i2c);

#endif