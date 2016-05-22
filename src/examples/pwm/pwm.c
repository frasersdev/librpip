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

/*
 * A little example that pulses pwm0 and moves a servo on pwm1 
 *
 * You will need to have enabled by PWM by following these instructions:
 * http://librpip.frasersdev.net/peripheral-config/pwm0and1/
 *
 */

void display_feature_set(uint32_t fs);
void show_pwm_config(uint32_t id);

int main(int argc, char * argv[]) {

	uint32_t feature_set;

	fprintf(stdout,"Initialising librpip...\n"); 	
	feature_set = librpipInit(LIBRPIP_BOARD_DETECT, LIBRPIP_FLAG_DEBUG_ON, 0);
	fprintf(stdout,"\n"); 	
	
	if(feature_set == 0) {
		fprintf(stdout,"librpip failed to initialise!\n");
	} else {
		display_feature_set(feature_set);
		
		if(feature_set & LIBRPIP_FEATURE_PWM0) show_pwm_config(0);
		if(feature_set & LIBRPIP_FEATURE_PWM1) show_pwm_config(1);

	
		librpipPwmConfigWrite(0, 10000, 0, LIBRPIP_PWM_FLAG_POLARITY_NORMAL);
		librpipPwmStatusWrite(0, LIBRPIP_PWM_STATUS_ON);
		
		librpipPwmConfigWrite(1, 20000, 0, LIBRPIP_PWM_FLAG_POLARITY_NORMAL);
		librpipPwmStatusWrite(1, LIBRPIP_PWM_STATUS_ON);
		//a servo with 180 degrees of movement, .9ms pulse = -90 and 2.1ms pulse = +90 	
		librpipServoConfigWrite(1, 180, 900, 2100);	


		int i;
		float dc;
		fprintf(stdout,"Pulsing PWM0 and moving a servo on PWM1.\nPress CTRL-C to stop!\n");
		while(1) {
			librpipServoPositionWrite(1, -45);
			for(i=0;i<1000;i++) {
				dc=(float)i/10;
				librpipPwmDutyPercentWrite(0, dc);
				usleep(1000);
			}
					
			librpipServoPositionWrite(1, 45);
			for(i=1000;i>0;i--) {
				dc=(float)i/10;
				librpipPwmDutyPercentWrite(0, dc);
				usleep(1000);
			}
		}
	}
		
	librpipClose();	
	return 0;
}



void display_feature_set(uint32_t fs) {

	fprintf(stdout,"Getting feature set...\n"); 	
	if(fs & LIBRPIP_FEATURE_GPIO) 	fprintf(stdout,"GPIO Functions are available.\n");
	if(fs & LIBRPIP_FEATURE_UART0) 	fprintf(stdout,"UART Functions are available on UART0.\n");	
	if(fs & LIBRPIP_FEATURE_I2C0) 	fprintf(stdout,"I2C Functions are available on I2C0.\n");	
	if(fs & LIBRPIP_FEATURE_I2C1) 	fprintf(stdout,"I2C Functions are available on I2C1.\n");	
	if(fs & LIBRPIP_FEATURE_SPI0) 	fprintf(stdout,"SPI Functions are available on SPI0.\n");
	if(fs & LIBRPIP_FEATURE_SPI1) 	fprintf(stdout,"SPI Functions are available on SPI1.\n");	
	if(fs & LIBRPIP_FEATURE_PWM0) 	fprintf(stdout,"PWM Functions are available on PWM0.\n");
	if(fs & LIBRPIP_FEATURE_PWM1) 	fprintf(stdout,"PWM Functions are available on PWM1.\n");
	fprintf(stdout,"\n"); 
}




void show_pwm_config(uint32_t id) {
	uint32_t pin, period, duty_cycle, flags, status;
      
	fprintf(stdout,"Getting PWM%u config...\n",id);   
	if(librpipPwmConfigRead(id, &pin, &period, &duty_cycle, &flags)) {
		fprintf(stdout,"Pin is 0x%x, Period is %u, Duty cycle is %u.\n",pin, period, duty_cycle); 
		if(flags & LIBRPIP_PWM_FLAG_POLARITY_NORMAL) fprintf(stdout,"Polarity is NORMAL.\n");	
		if(flags & LIBRPIP_PWM_FLAG_POLARITY_INVERTED) fprintf(stdout,"Polarity is INVERTED.\n");	
	}
	if(librpipPwmStatusRead(id, &status)) {	
		fprintf(stdout,"PWM%u is ",id);	
		switch(status) {
			case LIBRPIP_PWM_STATUS_OFF:
				fprintf(stdout,"OFF.\n");
				break;	
			case LIBRPIP_PWM_STATUS_ON:
				fprintf(stdout,"ON.\n");
				break;	
		}	
	}
	fprintf(stdout,"\n");  

}





