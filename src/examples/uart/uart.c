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
 
#include <string.h>
 
#include <unistd.h>
#include <stdio.h>
#include <librpip.h>

/* 
 * A brief example of how to use the uarts
 *
 * Connected like this:
 * txd to rxd  (rPi pins 8 & 10, GPIO pins 14 & 15)
 *
 *
 */


void show_uart_config(uint32_t id);

int main(int argc, char * argv[]) {

	uint32_t feature_set;

	fprintf(stdout,"Initialising librpip...\n"); 	
	feature_set = librpipInit(LIBRPIP_BOARD_PI2_B, LIBRPIP_FLAG_DEBUG_ON, 0 );
	fprintf(stdout,"\n"); 	
	
	if(feature_set == 0) {
		fprintf(stdout,"librpip failed to initialise!\n");
	} else {
	
		if(feature_set & LIBRPIP_FEATURE_UART0) {

			char inbufchar[50], outbufchar[50];		
			uint8_t inbufint[50],outbufint[50];
			
			//ascii mode
			fprintf(stdout,"ASCII Mode\n"); 
			librpipUartConfigWrite(0, LIBRPIP_UART_BAUD_115200,8, 0, 1, LIBRPIP_UART_MODE_ASCII);	
			show_uart_config(0);
			
			strcpy(outbufchar,"This is a test. And this is a longer test.");
			fprintf(stdout,"Out: %s\n",outbufchar); 
			librpipUartWrite(0, &outbufchar[0], strlen(outbufchar));
			librpipUartRead(0, &inbufchar[0], sizeof(inbufchar),0);  
			fprintf(stdout,"In: %s\n\n",inbufchar); 
			
			//binary mode
			fprintf(stdout,"Binary Mode\n"); 			
			librpipUartConfigWrite(0, LIBRPIP_UART_BAUD_115200,8, 0, 1, LIBRPIP_UART_MODE_BINARY);	
			show_uart_config(0);			
					
			outbufint[0]=0xff;
			outbufint[1]=0xbe;
			outbufint[2]=0xaf;
			outbufint[3]=0x00;
		
			fprintf(stdout,"Out: 0x%x 0x%x 0x%x 0x%x\n",outbufint[0],outbufint[1],outbufint[2],outbufint[3]); 
			librpipUartWrite(0, &outbufint[0], 4);
			librpipUartRead(0, &inbufint[0], 4 ,0);  
			fprintf(stdout,"In: 0x%x 0x%x 0x%x 0x%x\n\n",inbufint[0],inbufint[1],inbufint[2],inbufint[3]); 
		
			//as a transaction
			fprintf(stdout,"As a transaction\n"); 
			librpipUartConfigWrite(0, LIBRPIP_UART_BAUD_115200,8, 0, 1, LIBRPIP_UART_MODE_ASCII);
			show_uart_config(0);
				
			//create the transaction
			struct librpip_tx* t = librpipTransactionCreate(LIBRPIP_TX_MODE_UART,8);
			librpipTransactionMsgVarAdd(t, LIBRPIP_TX_MSG_TX, 'a');
			librpipTransactionMsgVarAdd(t, LIBRPIP_TX_MSG_TX, 'b');
			librpipTransactionMsgVarAdd(t, LIBRPIP_TX_MSG_RX, 'c');

			strcpy(outbufchar,"This is transaction a.");
			librpipTransactionMsgVarSet(t, 'a', &outbufchar[0], strlen(outbufchar));
			strcpy(outbufchar,"This is transaction b.");
			librpipTransactionMsgVarSet(t, 'b', &outbufchar[0], strlen(outbufchar));
			librpipTransactionMsgVarSet(t, 'c', 0, sizeof(inbufchar));

			
			//do it
			librpipTransactionSend(t, 0, 0); //send to uart0. client id is ignored
			
			//get the results
			librpipTransactionRead(t, &inbufchar[0], sizeof(inbufchar)); 
			fprintf(stdout,"In: %s\n\n",inbufchar); 
			librpipTransactionDestroy(t);
		}	
	}
		
	librpipClose();	
	return 0;
}



void show_uart_config(uint32_t id) {

	uint32_t baud,csize,parity,stop,mode;

	fprintf(stdout,"Getting UART%u config...\n",id);  	
	if(librpipUartConfigRead(id, &baud, &csize, &parity, &stop, &mode)) {
		fprintf(stdout,"Baud is %u, char size is %u, parity is %u, stop bits is %u, mode is %u\n",baud,csize,parity,stop,mode); 
	}
	fprintf(stdout,"\n");  
}

