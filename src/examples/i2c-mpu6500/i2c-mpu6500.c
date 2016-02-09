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
 * A mu6500 6 axis accel/gyro
 * http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Components/General%20IC/PS-MPU-6000A.pdf
 * https://dlnmh9ip6v2uc.cloudfront.net/datasheets/Sensors/Accelerometers/RM-MPU-6000A.pdf
 *
 * Connected like this:
 * rpi 3.3v			to Vcc
 * rpi Gnd			to Gnd
 * rpi I2C-0.SDA 	to SDA
 * rpi I2C-0.SCL 	to SCL
 *
 *
 */

void setup_transactions(void);
void get_accel(uint8_t i2c_bus, uint8_t client, float* x, float* y, float* z);

struct librpip_tx* init;
struct librpip_tx* readAccel;
struct librpip_tx* readGyro;

int main(int argc, char * argv[]) {

	uint32_t feature_set;
	



	fprintf(stdout,"Initialising librpip...\n"); 	
	feature_set = librpipInit(LIBRPIP_BOARD_DETECT, LIBRPIP_FLAG_DEBUG_ON, 0);
	fprintf(stdout,"\n"); 	
	
	if(feature_set == 0) {
		fprintf(stdout,"librpip failed to initialise!\n");
	} else {
		if(feature_set & LIBRPIP_FEATURE_I2C1) {
		
			uint8_t client=0x68;
			uint8_t i2c_bus=0;
			float x,y,z;
		
			setup_transactions();
			librpipTransactionSend(init, i2c_bus, client);
		
			while (1) {
				get_accel(i2c_bus, client, &x, &y, &z);
				fprintf(stdout,"x: %0.2f, y: %0.2f, z: %0.2f \n", x,y,z );
				sleep(1);
			}
			
			librpipTransactionDestroy(init);  		//kind of pointless after an infinite loop but its a very good habit to destroy what you create
			librpipTransactionDestroy(readAccel);  	//ditto
			librpipTransactionDestroy(readGyro);  	//ditto
		}

	}
		
	librpipClose();	
	return 0;
}

void setup_transactions(void) {
		
	init = librpipTransactionCreate(LIBRPIP_TX_MODE_I2C,8);
	librpipTransactionMsgAddRegWrite(init, 0x1b, 0x0);
	librpipTransactionMsgAddRegWrite(init, 0x1c, 0x0);
	librpipTransactionMsgAddRegWrite(init, 0x6b, 0x1);	


	readAccel = librpipTransactionCreate(LIBRPIP_TX_MODE_I2C,8);
	librpipTransactionMsgAddRegRead(readAccel, 0x3b, 2);
	librpipTransactionMsgAddRegRead(readAccel, 0x3d, 2);
	librpipTransactionMsgAddRegRead(readAccel, 0x3f, 2);


	readGyro = librpipTransactionCreate(LIBRPIP_TX_MODE_I2C,8);
	librpipTransactionMsgAddRegRead(readGyro, 0x43, 2);
	librpipTransactionMsgAddRegRead(readGyro, 0x45, 2);
	librpipTransactionMsgAddRegRead(readGyro, 0x47, 2);
	
}

void get_accel(uint8_t i2c_bus, uint8_t client, float* x, float* y, float* z) {
	
	uint8_t raw_result[2];
	int16_t temp;

	librpipTransactionSend(readAccel, i2c_bus, client);
	librpipTransactionRead(readAccel, &raw_result[0], 2);
	temp = (((int16_t)raw_result[0])<<8) | raw_result[1];
	*x = (float)temp/16384.0;
	
	librpipTransactionRead(readAccel, &raw_result[0], 2);
	temp = (((int16_t)raw_result[0])<<8) | raw_result[1];
	*y = (float)temp/16384.0;
	
	librpipTransactionRead(readAccel, &raw_result[0], 2);
	temp = (((int16_t)raw_result[0])<<8) | raw_result[1];
	*z = (float)temp/16384.0;

}

