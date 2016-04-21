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
#include <librpip.h>
#include <errno.h>


/* 
 * A MCP3202 12 bit a/d converter
 *
 * Connected like this:
 * rpi 3.3v		to Vdd/Vref
 * rpi Gnd		to Vss
 * rpi SPI.MOSI 	to DIn
 * rpi SPI.MOSI 	to DOut
 * rpi SPI.SCLK 	to Clock
 * rpi SPI.CE0 		to CS/SHDN
 *
 * channel 0 is sampled.
 *
 */

void set_spi_config(uint32_t spi, uint32_t cs);
struct librpip_tx* setup_transaction();
uint32_t do_sample(uint32_t spi, uint32_t cs, struct librpip_tx* tx);

int main(int argc, char * argv[]) {

	uint32_t feature_set;
	uint16_t sample;
	float volts,ave_volts;
	
	uint32_t spi,cs,initok;
	
	spi=0;	
	cs=0;	
	ave_volts=0;
	initok=0;	

	fprintf(stdout,"Initialising librpip...\n"); 	
	feature_set = librpipInit(LIBRPIP_BOARD_DETECT, LIBRPIP_FLAG_DEBUG_ON, 0);
	fprintf(stdout,"\n"); 	
	
	if(feature_set == 0) {
		fprintf(stdout,"librpip failed to initialise!\n");
	} else {
		if(spi==0  && (feature_set & LIBRPIP_FEATURE_SPI0)) {
			initok=1;
		}		

		if(spi==1  && (feature_set & LIBRPIP_FEATURE_SPI1)) {
			initok=1;
		}	

		if(initok) {			
			set_spi_config(spi,cs);
			struct librpip_tx* tx = setup_transaction();
			while(1) {
				sample = do_sample(spi,cs,tx);
				volts = 3.3*(float)sample/4095.0;   //12 bit
				ave_volts = 9.0*ave_volts/10.0 + volts/10.0;   //essentially a simple filter to calm the jitter
				fprintf(stdout,"sample %u, volts %.3fv, average volts %.3fv\n",sample,volts,ave_volts);
				usleep(10000);
			}
			librpipTransactionDestroy(tx);  //kind of pointless after an infinite loop but its a very good habit to destroy what you create			
		}
	}
	
	librpipClose();	
	return 0;
}


void set_spi_config(uint32_t spi, uint32_t cs) {
	uint32_t spi_mode, lsb_first, bits_per_word,max_speed, spi_flags;

	
	fprintf(stdout,"Setting SPI%u.%u config... ",spi,cs); 		
	if(librpipSpiConfigWrite(spi, cs, LIBRPIP_SPI_MODE_0, 500000, 0)) {
		fprintf(stdout,"Success!\n"); 
	}

	fprintf(stdout,"Getting SPI%u.%u config... ",spi,cs); 	
	if(librpipSpiConfigRead(spi, cs, &spi_mode, &lsb_first, &bits_per_word, &max_speed, &spi_flags)) {
		fprintf(stdout," -> mode is %u \n",spi_mode); 
		fprintf(stdout," -> lsb_first is %u \n",lsb_first); 
		fprintf(stdout," -> bits per word is %u \n",bits_per_word); 
		fprintf(stdout," -> max speed is %u \n",max_speed); 
		fprintf(stdout," -> spi flags are 0x%x \n",spi_flags); 
	}

	fprintf(stdout,"\n"); 

}

struct librpip_tx* setup_transaction() {

	uint8_t buf[3];
		
	buf[0]=0b00000001;	// 7x(0) NULL, (1) Start,
	buf[1]=0b10000000;	// (1) Mode = Single, (0) Channel = 0, (0) MSBF = LSB 
				// Function of MSBF seems reversed to me
				// setting it to 1 results in MSB followed by LSB
				// setting it to 0 results in just MSB
				// this is the opposite as what is described in the documentation... :-/
							
	buf[2]=0;		//just need to keep sending clocks so we get our output.
		
	struct librpip_tx* t = librpipTransactionCreate(LIBRPIP_TX_MODE_SPI,8);
	librpipTransactionMsgAdd(t, LIBRPIP_TX_MSG_TXRX, &buf[0], sizeof(buf));
		
	return t;
}



uint32_t do_sample(uint32_t spi, uint32_t cs, struct librpip_tx* tx) {

	uint8_t response[3];
	uint32_t result;

	librpipTransactionSend(tx, spi, cs); 				//send to /dev/spidev0.0
	librpipTransactionRead(tx, &response[0], sizeof(response)); 	//read the result back

	result= ((uint32_t)response[1] << 8 ) | response[2];
	
	return result;
}


