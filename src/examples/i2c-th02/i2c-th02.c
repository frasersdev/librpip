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
 * A th02 temp and humidity module
 * http://www.hoperf.com/upload/sensor/TH02_V1.1.pdf
 *
 * Connected like this:
 * rpi 3.3v			to Vdd
 * rpi Gnd			to Gnd
 * rpi I2C-1.SDA 	to SDA
 * rpi I2C-1.SCL 	to SCK
 *
 *
 */

void setup_transactions(void);
float read_temp(uint8_t i2c_bus, uint8_t client);
float read_humidity(uint8_t i2c_bus, uint8_t client, float temp);
void wait_for_ready(uint8_t i2c_bus, uint8_t client);
void get_conversion(uint8_t i2c_bus, uint8_t client, uint8_t* result, uint16_t length);

struct librpip_tx* startT;
struct librpip_tx* startRH;
struct librpip_tx* waitReady;
struct librpip_tx* getConversion;

int main(int argc, char * argv[]) {

	uint32_t feature_set;


	fprintf(stdout,"Initialising librpip...\n"); 	
	feature_set = librpipInit(LIBRPIP_BOARD_DETECT, LIBRPIP_FLAG_DEBUG_ON, 0);
	fprintf(stdout,"\n"); 	
	
	if(feature_set == 0) {
		fprintf(stdout,"librpip failed to initialise!\n");
	} else {
		if(feature_set & LIBRPIP_FEATURE_I2C1) {
	
			uint8_t client=0x40;
			uint8_t i2c_bus=1;
			float temp,humidity;

			setup_transactions();
			
			while (1) {
				temp = read_temp(i2c_bus, client);
				humidity = read_humidity(i2c_bus, client, temp);
				fprintf(stdout,"Temp is: %0.2f, Humidity is: %0.1f\n", temp, humidity );
				sleep(1);
			}

			librpipTransactionDestroy(startT);  		//kind of pointless after an infinite loop but its a very good habit to destroy what you create
			librpipTransactionDestroy(startRH);  		//ditto
			librpipTransactionDestroy(waitReady);  		//ditto
			librpipTransactionDestroy(getConversion); 	//ditto
		}

	}
		
	librpipClose();	
	return 0;
}

void setup_transactions(void) {

	startT = librpipTransactionCreate(LIBRPIP_TX_MODE_I2C,8);
	librpipTransactionMsgAddRegWrite(startT, 0x03, 0x11);	

	startRH = librpipTransactionCreate(LIBRPIP_TX_MODE_I2C,8);
	librpipTransactionMsgAddRegWrite(startRH, 0x03, 0x01);	

	waitReady = librpipTransactionCreate(LIBRPIP_TX_MODE_I2C,8);
	librpipTransactionMsgAddRegRead(waitReady, 0x00, 1);	

	getConversion = librpipTransactionCreate(LIBRPIP_TX_MODE_I2C,8);
	librpipTransactionMsgAddRegRead(getConversion, 0x01, 3);		//note how we have to read in 3 bytes

}

float read_temp(uint8_t i2c_bus, uint8_t client ) {

	uint8_t raw_result[3];
	uint16_t result;
	float temp;

	librpipTransactionSend(startT, i2c_bus, client);
	wait_for_ready(i2c_bus, client);
	get_conversion(i2c_bus, client, &raw_result[0], 3);

	// I unexpectedly get a zero byte first (ie raw_result[0]=0) no matter what approach I take. 
	// only this device does it so I suspect its device related
	result = (((uint16_t)raw_result[1]) << 6) | (raw_result[2] >> 2);
	
	temp = (float)result/32.0 - 50.0;
	

		
	return temp;

}

float read_humidity(uint8_t i2c_bus, uint8_t client, float temp) {

	uint8_t raw_result[3];
	uint16_t result;
	float humidity;

	librpipTransactionSend(startRH, i2c_bus, client);
	wait_for_ready(i2c_bus, client);
	get_conversion(i2c_bus, client, &raw_result[0], 3);
	
	result = (((uint16_t)raw_result[1]) << 4) | (raw_result[2] >> 4);
	
	humidity = (float)result/16.0 - 24.0;
	
	//linear compensation as per page 11 of datasheet
	humidity = humidity - ((humidity * humidity) * -0.00393 + humidity * 0.4008  + -4.7844);
	
	//temperature compensation as per page 12 of datasheet
	humidity = humidity + (temp - 30) * (humidity * 0.00237 + 0.1973);

	return humidity;

}

void wait_for_ready(uint8_t i2c_bus, uint8_t client) {

	uint8_t result;

	librpipTransactionSend(waitReady, i2c_bus, client);
	librpipTransactionRead(waitReady, &result, 1);
	while(result & 0b00000001) {
		usleep(50);
		librpipTransactionSend(waitReady, i2c_bus, client);
		librpipTransactionRead(waitReady, &result, 1);
	}
}


void get_conversion(uint8_t i2c_bus, uint8_t client, uint8_t* result, uint16_t length) {
	librpipTransactionSend(getConversion, i2c_bus, client);
	librpipTransactionRead(getConversion, result, length);
}

