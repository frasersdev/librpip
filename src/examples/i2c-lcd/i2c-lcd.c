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
#include <stdio.h>
#include <string.h>
#include <librpip.h>
#include <errno.h>


/* 
 * A midas i2c lcd module - MCCOG22005A
 * Useful because: 
 * 1) it is a 3.3v module
 * 2) is uses i2c (no bodgy bit banging)
 * http://au.element14.com/midas/mccog22005a6w-fptlwi/lcd-alpha-num-20-x-2-white/dp/2425758
 * http://www.farnell.com/datasheets/1821132.pdf
 *
 * Connected like this:
 * rpi 3.3v			to Vdd, RST & Vout (via 4.7uf cap)
 * rpi Gnd			to Vss
 * rpi I2C-1.SDA 	to SDA
 * rpi I2C-1.SCL 	to SCL
 *                     CapIn & CapOut connect via 4.7uf Cap
 *
 *
 */
 
#define LCD_SEND_CMD_LAST 	0x00
#define LCD_SEND_DATA_LAST	0x40	
#define LCD_SEND_CMD	 	0x80
#define LCD_SEND_DATA		0xc0	

#define CMD_DISPLAY_CLEAR	0x01
#define CMD_DISPLAY_HOME	0x02
#define CMD_DISPLAY_OFF		0x08
#define CMD_DISPLAY_ON 		0x0C
#define CMD_BIAS_OSC_SET	0x14 
#define CMD_INSTUCT_TBL0 	0x38 
#define CMD_INSTUCT_TBL1 	0x39 
#define CMD_PWR_BOOST_ON	0x54
#define CMD_FOLLOWER_ON		0x6F
#define CMD_CONTRAST_SET 	0x74

#define CMD_IGRAM	 		0x40	//instruction table 1
#define CMD_CGRAM	 		0x40	//instruction table 0
#define CMD_DDRAM_1 		0x80
#define CMD_DDRAM_2			0xc0


#define WAIT_INIT			1000	//1ms

void setup_transactions(void);
void lcd_init(void);
void lcd_write(uint8_t line, char* data);

struct librpip_tx* lcdInit;
struct librpip_tx* lcdWriteLine1;
struct librpip_tx* lcdWriteLine2;

uint8_t client=0x3c;
uint8_t i2c_bus=1;

int main(int argc, char * argv[]) {

	uint32_t feature_set;
	
	fprintf(stdout,"Initialising librpip...\n"); 	
	feature_set = librpipInit(LIBRPIP_BOARD_DETECT, LIBRPIP_FLAG_DEBUG_ON, 0);
	fprintf(stdout,"\n"); 	
	
	if(feature_set == 0) {
		fprintf(stdout,"librpip failed to initialise!\n");
	} else {
		if(feature_set & LIBRPIP_FEATURE_I2C1) {

			setup_transactions();
			librpipTransactionSend(lcdInit, i2c_bus, client);
			usleep(WAIT_INIT);
			
			lcd_write(1, "G'day");
			lcd_write(2, "Hows it goin?");
			
			sleep(5);
			
			lcd_write(1, "This is a LCD");
			lcd_write(2, "Powered by i2c");
		
			librpipTransactionDestroy(lcdInit);  		
			librpipTransactionDestroy(lcdWriteLine1); 
			librpipTransactionDestroy(lcdWriteLine2); 
		}

	}
		
	librpipClose();	
	return 0;
}

void setup_transactions(void) {
		
	uint8_t buf[8];
	lcdInit = librpipTransactionCreate(LIBRPIP_TX_MODE_I2C,8);
	buf[0]=LCD_SEND_CMD;
	buf[1]=CMD_INSTUCT_TBL0;
	librpipTransactionMsgAdd(lcdInit, LIBRPIP_TX_MSG_TX, &buf[0], 2);
	buf[0]=LCD_SEND_CMD_LAST;
	buf[1]=CMD_INSTUCT_TBL1;
	buf[2]=CMD_BIAS_OSC_SET;
	buf[3]=CMD_CONTRAST_SET;
	buf[4]=CMD_PWR_BOOST_ON;
	buf[5]=CMD_FOLLOWER_ON;
	buf[6]=CMD_DISPLAY_ON;
	buf[7]=CMD_DISPLAY_CLEAR;
	librpipTransactionMsgAdd(lcdInit, LIBRPIP_TX_MSG_TX, &buf[0], 8);
	
	lcdWriteLine1=librpipTransactionCreate(LIBRPIP_TX_MODE_I2C, 8);	
	buf[0]=LCD_SEND_CMD; 
	buf[1]=CMD_DDRAM_1;
	librpipTransactionMsgAdd(lcdWriteLine1, LIBRPIP_TX_MSG_TX, &buf[0], 2);
	librpipTransactionMsgVarAdd(lcdWriteLine1, LIBRPIP_TX_MSG_TX, '1');
	
	lcdWriteLine2=librpipTransactionCreate(LIBRPIP_TX_MODE_I2C, 8);	
	buf[0]=LCD_SEND_CMD; 
	buf[1]=CMD_DDRAM_2;   
	librpipTransactionMsgAdd(lcdWriteLine2, LIBRPIP_TX_MSG_TX, &buf[0], 2);
	librpipTransactionMsgVarAdd(lcdWriteLine2, LIBRPIP_TX_MSG_TX, '2');
	

}

void lcd_write(uint8_t line, char* data) {

	//note that the control byte and data must be in the same message segment. 
	//so we merge the two here. first byte is the control byte.
	
	uint8_t buf[21];
	uint8_t i,len;
	
	len=strlen(data);
	
	if(len>20) len=20;
	
	buf[0]=LCD_SEND_DATA_LAST;

	for(i=0;i<20;i++) {
		buf[i+1] = i<len ? data[i] : ' ' ;
	}

	if(line==1) {
		librpipTransactionMsgVarSet(lcdWriteLine1, '1', &buf, sizeof(buf));	//update transaction with the data	
		librpipTransactionSend(lcdWriteLine1, i2c_bus, client);	
	} else if(line==2) {
		librpipTransactionMsgVarSet(lcdWriteLine2, '2', &buf, sizeof(buf)); //update transaction with the data
		librpipTransactionSend(lcdWriteLine2, i2c_bus, client);
	}

}
