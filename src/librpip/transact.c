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
 

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

#include "rpi.h"
#include "error.h"
#include "transact.h"
#include "i2c.h"
#include "spi.h"
#include "uart.h"


extern uint32_t librpip_error_code;
extern uint32_t librpip_error_data;
extern uint32_t librpip_error_extra;
extern float 	librpip_error_extraf;
extern uint32_t librpip_flags;



struct librpip_transaction_t* librpipTransactionCreate(uint8_t mode, uint8_t bpw) {

	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(mode > LIBRPIP_TX_MODE_UART ) {
		librpip_error_code=0x500;
		librpip_error_data=mode;
	} 
	
	if(bpw != 8 ) {
		librpip_error_code=0x501;
		librpip_error_data=bpw;
	} 

	if(!librpip_error_code) {
		struct librpip_transaction_t* t = malloc(sizeof(struct librpip_transaction_t));
	
		t->mode=mode;	
		t->bpw=bpw;	
		t->status=LIBRPIP_TX_STATUS_NEW;
		t->first_msg=0;
		t->curr_msg=0;
		t->last_msg=0;
		t->len=0;
	
		return t;
	} else {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;
	}
}

uint32_t librpipTransactionMsgAddRegRead(struct librpip_transaction_t* t, uint8_t reg, uint16_t len) {

	uint8_t buf[1];
	
	buf[0]=reg;
	
	if(librpipTransactionMsgAdd(t, LIBRPIP_TX_MSG_TX, &buf[0], 1))
		return librpipTransactionMsgAdd(t, LIBRPIP_TX_MSG_RX, 0, len);
	else
		return 0;

}

uint32_t librpipTransactionMsgAddRegWrite(struct librpip_transaction_t* t, uint8_t reg, uint8_t value) {

	uint8_t buf[2];
	
	buf[0]=reg;
	buf[1]=value;
	
	return librpipTransactionMsgAdd( t, LIBRPIP_TX_MSG_TX, &buf[0], 2);
	
}

uint32_t librpipTransactionMsgAdd(struct librpip_transaction_t* t, uint8_t dir, void* txbuf, uint16_t len) {

	librpip_error_code=0;
	librpip_error_data=0;	

	if((dir & LIBRPIP_TX_MSG_TXRX) == 0) {
		librpip_error_code=0x510;
		librpip_error_data=dir;
	} else if(!len) {
		librpip_error_code=0x511;
		librpip_error_data=len;
	} else if(t->status==LIBRPIP_TX_STATUS_SENTOK) {
		librpip_error_code=0x512;
	} else if(dir==LIBRPIP_TX_MSG_TXRX && t->mode !=LIBRPIP_TX_MODE_SPI) {
		librpip_error_code=0x513;
	} else if(dir!=LIBRPIP_TX_MSG_TXRX && t->mode == LIBRPIP_TX_MODE_SPI) {
		librpip_error_code=0x514;
	} else if (t->mode == LIBRPIP_TX_MODE_I2C && t->len >= I2C_RDRW_IOCTL_MAX_MSGS) {
		librpip_error_code=0x515;
		librpip_error_data=I2C_RDRW_IOCTL_MAX_MSGS;		
	}
	
	if(!librpip_error_code) {
	
		struct librpip_msg_t* m = malloc(sizeof(struct librpip_msg_t));	
	
		m->dir = dir;
		m->var = '\0'; //this is not a variable message segment
		m->next=0;  //this message is last
	
		if(dir & LIBRPIP_TX_MSG_TX) {
			m->tx = librpip_transaction_buffer_create((uint8_t*)txbuf,len);
		} else  {
			m->tx=0;
		}
	
		if(dir & LIBRPIP_TX_MSG_RX) {
			m->rx = librpip_transaction_buffer_create(0,len);
		} else {
			m->rx=0;
		}
		
		if(t->len > 0) {
			m->prev = t->last_msg;
			t->last_msg->next = m;	
		} else {
			t->first_msg=m;
			t->curr_msg=m;
			t->status=LIBRPIP_TX_STATUS_MSG;	// t has messages now
			m->prev = 0;  						// first message has no previous 
		}

		t->last_msg = m;	//the transaction structure item 'last message' now points to this new message;
		t->len++; 			//length is one longer now
	}
	
	if(librpip_error_code) {	
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	
	return librpip_error_code ? 0 : 1;
}

uint32_t librpipTransactionMsgVarAdd(struct librpip_transaction_t* t, uint8_t dir, char var) {

	librpip_error_code=0;
	librpip_error_data=0;	

	if((dir & LIBRPIP_TX_MSG_TXRX) == 0) {
		librpip_error_code=0x510;
		librpip_error_data=dir;
	} else if (var == '\0') {
		librpip_error_code=0x517;
	} else if(t->status==LIBRPIP_TX_STATUS_SENTOK) {
		librpip_error_code=0x512;
	} else if(dir==LIBRPIP_TX_MSG_TXRX && t->mode !=LIBRPIP_TX_MODE_SPI) {
		librpip_error_code=0x513;
	} else if(dir!=LIBRPIP_TX_MSG_TXRX && t->mode == LIBRPIP_TX_MODE_SPI) {
		librpip_error_code=0x514;
	} else if (t->mode == LIBRPIP_TX_MODE_I2C && t->len >= I2C_RDRW_IOCTL_MAX_MSGS) {
		librpip_error_code=0x515;
		librpip_error_data=I2C_RDRW_IOCTL_MAX_MSGS;		
	}
	
	if(!librpip_error_code) {
	
		struct librpip_msg_t* m = malloc(sizeof(struct librpip_msg_t));	
	
		m->dir = dir;
		m->var = var; 	//this is a variable message segment
		m->next=0;  	//this message is last
	
		m->tx=0;		//these get set when a varset is called prior to sending the transaction
		m->rx=0;		//
		
		if(t->len > 0) {
			m->prev = t->last_msg;
			t->last_msg->next = m;	
		} else {
			t->first_msg=m;
			t->curr_msg=m;
			t->status=LIBRPIP_TX_STATUS_MSG;	// t has messages now
			m->prev = 0;  						// first message has no previous 
		}

		t->last_msg = m;	//the transaction structure item 'last message' now points to this new message;
		t->len++; 			//length is one longer now
	}
	
	if(librpip_error_code) {	
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	
	return librpip_error_code ? 0 : 1;
}

uint32_t librpipTransactionMsgVarSet(struct librpip_transaction_t* t, char var,  void* txbuf, uint16_t len) {

	if(!len) {
		librpip_error_code=0x516;
		librpip_error_data=len;
		librpip_error_extra=(uint32_t)var;
	}
	
	if(!librpip_error_code) {
	
		struct librpip_msg_t* m = t->curr_msg;
		while(m) {
			if(m->var==var) {
				if(m->dir & LIBRPIP_TX_MSG_TX) {
					if(m->tx) librpip_transaction_buffer_destroy(m->tx);
					m->tx = librpip_transaction_buffer_create((uint8_t*)txbuf,len);
				} else  {
					m->tx=0;
				}
	
				if(m->dir & LIBRPIP_TX_MSG_RX) {
					if(m->rx) librpip_transaction_buffer_destroy(m->rx);
					m->rx = librpip_transaction_buffer_create(0,len);
				} else {
					m->rx=0;
				}				
			}
			m=m->next;		//advance to the next one
		}			
	}

	if(librpip_error_code) {	
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	
	return librpip_error_code ? 0 : 1;
}


uint32_t librpipTransactionSend(struct librpip_transaction_t* t, uint32_t device, uint32_t client) {

	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(t->len==0) {
		librpip_error_code=0x520;
	} else if(t->status==LIBRPIP_TX_STATUS_NEW) {
		librpip_error_code=0x520;
	} 
	
	librpip_transaction_msg_var_check(t);
	
	if(!librpip_error_code) {
		struct spi_ioc_transfer* spi;
		struct i2c_rdwr_ioctl_data* i2c;
		switch(t->mode) {
			case LIBRPIP_TX_MODE_I2C:
				i2c = librpip_transaction_i2c_create(t,client);
				librpip_i2c_transaction(device, client, i2c);	
				librpip_transaction_i2c_destroy(i2c);
				break;
			case LIBRPIP_TX_MODE_SPI:
				spi = librpip_transaction_spi_create(t);
				librpip_spi_transaction(device, client,  spi, t->len);
				librpip_transaction_spi_destroy(spi);
				break;
			case LIBRPIP_TX_MODE_SPI_3W:
				spi = librpip_transaction_spi_create(t);
				librpip_spi_transaction_3w(device, client,  spi, t->len);
				librpip_transaction_spi_destroy(spi);
				break;
			case LIBRPIP_TX_MODE_UART:
				 librpip_uart_tansaction(device, t);
				break;
		}
	}
	if(librpip_error_code) {
		t->status=LIBRPIP_TX_STATUS_FAILED;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	} else {
		t->status=LIBRPIP_TX_STATUS_SENTOK;	
		t->curr_msg=t->first_msg;  //set the read pointer to the first msg.
	}
	
	return librpip_error_code ? 0 : 1;
}



uint32_t librpipTransactionRead(struct librpip_transaction_t* t, void* result, uint16_t len) {

	uint16_t i;
	librpip_error_code=0;
	librpip_error_data=0;
	
	uint8_t* p;	
	if(t) {
		switch(t->status) {
			case LIBRPIP_TX_STATUS_SENTOK:
				if(t->curr_msg) {
					struct librpip_msg_t* m = t->curr_msg;
					while(m && !(m->dir & LIBRPIP_TX_MSG_RX) ) {
						m=m->next;		//advance to the next one
					}	
					if(m) {				// we are pointing at a receive message
						i=0;
						p=(uint8_t*)result;  //give the void pointer a type so we can loop though it
						while(i < len && i < m->rx->len) {
							p[i] = m->rx->buf[i];
							i++;
						}
						t->curr_msg=m->next;
					} else {
						librpip_error_code=0x532;
					}
				} else {
					librpip_error_code=0x531;
				}
				break;
			case LIBRPIP_TX_STATUS_NEW:
			case LIBRPIP_TX_STATUS_MSG:
			case LIBRPIP_TX_STATUS_FAILED:
			default:
				librpip_error_code=0x533;
				librpip_error_data=t->status;
		}

	} else {
		librpip_error_code=0x530;
	}
	if(librpip_error_code) {	
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}	
	return librpip_error_code ? 0 : 1;
}

uint32_t librpipTransactionReadSize(struct librpip_transaction_t* t, uint16_t* size) {

	uint16_t i;
	librpip_error_code=0;
	librpip_error_data=0;
	
	uint8_t* p;

	*size=0;
		
	if(t) {
		switch(t->status) {
			case LIBRPIP_TX_STATUS_SENTOK:
				if(t->curr_msg) {
					struct librpip_msg_t* m = t->curr_msg;
					while(m && !(m->dir & LIBRPIP_TX_MSG_RX) ) {
						m=m->next;		//advance to the next one
					}	
					if(m) {				// we are pointing at a receive message
						*size =  m->rx->len;
					} else {
						librpip_error_code=0x532;
					}
				} else {
					librpip_error_code=0x531;
				}
				break;
			case LIBRPIP_TX_STATUS_NEW:
			case LIBRPIP_TX_STATUS_MSG:
			case LIBRPIP_TX_STATUS_FAILED:
			default:
				librpip_error_code=0x533;
				librpip_error_data=t->status;
		}

	} else {
		librpip_error_code=0x530;
	}
	if(librpip_error_code) {	
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}	
	return librpip_error_code ? 0 : 1;
}

uint32_t librpipTransactionDestroy(struct librpip_transaction_t* t) {

	uint16_t i;
	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(t) {
		if(t->len > 0) {
			struct librpip_msg_t* m = t->last_msg;
			struct librpip_msg_t* p = 0;
		
			for(i=0;i<t->len;i++) {   			// is a while(m) better thing to do here?
				p = m->prev;  				// get the previous message pointer before we start destroying things
				librpip_transaction_message_destroy(m);	// destroy it
				m = p;					// move the pointer to the message before
			}
		}
		free(t);	
	} else {
		librpip_error_code=0x540;
	}
	
	if(librpip_error_code) {	
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	
	return librpip_error_code ? 0 : 1;
}

  
//internal functions


void librpip_transaction_msg_var_check(struct librpip_transaction_t* t) {

	struct librpip_msg_t* m = t->curr_msg;
	while(m && !librpip_error_code) {

		if((m->dir & LIBRPIP_TX_MSG_TX) && !m->tx) {
			librpip_error_code=0x521;
		}
	
		if((m->dir & LIBRPIP_TX_MSG_RX) && !m->rx) {
			librpip_error_code=0x521;
		} 
		m=m->next;		//advance to the next one
	}			
}



struct spi_ioc_transfer* librpip_transaction_spi_create(struct librpip_transaction_t* t) {

	uint16_t i;
	
	struct spi_ioc_transfer* spi = malloc(sizeof(struct spi_ioc_transfer) * t->len);
	struct spi_ioc_transfer* s = spi;
	struct librpip_msg_t* m = t->first_msg;
	
	for(i=0; i<t->len;i++) {
		librpip_spi_struct_init(s);	//zero the structure
		
		if(m->dir & LIBRPIP_TX_MSG_TX) {
			s->tx_buf = (uintptr_t)m->tx->buf; //spi structure wants an int!
		} 
		if(m->dir & LIBRPIP_TX_MSG_RX) {
			s->rx_buf = (uintptr_t)m->rx->buf; //spi structure wants an int!
		} 
		s->len = m->tx->len > m->rx->len ? m->tx->len : m->rx->len ;
		
		s++;
		m=m->next;	//advance to the next message 
	}
	
	return spi;
}

struct i2c_rdwr_ioctl_data* librpip_transaction_i2c_create(struct librpip_transaction_t* t, uint32_t client) {

	uint16_t i;
	
	struct i2c_rdwr_ioctl_data* i2c = malloc(sizeof(struct i2c_rdwr_ioctl_data));
	struct i2c_msg* msgs = malloc(sizeof(struct i2c_msg) * t->len);
	struct i2c_msg* p = msgs;
		
	struct librpip_msg_t* m = t->first_msg;

	i2c->msgs = msgs;	
	i2c->nmsgs = t->len;	
	for(i=0; i<t->len;i++) {
	
		p->addr = client;
		if(m->dir & LIBRPIP_TX_MSG_TX) {
			p->flags = 0; 
			p->buf = m->tx->buf;
			p->len = m->tx->len;
		} else { 
			p->flags = I2C_M_RD;
			p->buf = m->rx->buf;
			p->len = m->rx->len;
		}
		p++;
		m=m->next;	//advance to the next message 		
	}
	return i2c;
}

struct librpip_msg_buffer_t* librpip_transaction_buffer_create(uint8_t* buf,uint16_t len) {

	uint16_t i;
	
	struct librpip_msg_buffer_t* b = malloc(sizeof(struct librpip_msg_buffer_t));
	
	b->buf = malloc((sizeof(uint8_t)*len));
	
	if(buf) {
		for(i=0;i<len;i++) {
			b->buf[i] = buf[i];	
		}
	} else {
		for(i=0;i<len;i++) {
			b->buf[i] = 0;	
		}
	}
	b->len=len;
	
	return b;
}

void librpip_transaction_message_destroy(struct librpip_msg_t* m) {

	if(m->tx) librpip_transaction_buffer_destroy(m->tx);
	if(m->rx) librpip_transaction_buffer_destroy(m->rx);
	free(m);
	m=0;

}

void librpip_transaction_buffer_destroy(struct librpip_msg_buffer_t* b) {

	free(b->buf);
	free(b);
	b=0;

}	

void librpip_transaction_spi_destroy(struct spi_ioc_transfer* spi) {

	free(spi);
	spi=0;
}


void librpip_transaction_i2c_destroy(struct i2c_rdwr_ioctl_data* i2c) {

	free(i2c->msgs);
	free(i2c);
	i2c=0;
}
