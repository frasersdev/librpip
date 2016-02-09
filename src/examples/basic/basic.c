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
 * A little example that initialises librpip and shows what features got enabled 
 */

void display_feature_set(uint32_t fs);
void display_gpio_valid_pins();
void show_pin_config(uint32_t pin);
void show_spi_config(uint32_t id, uint32_t cs);
void show_pwm_config(uint32_t id);
void show_i2c_config(uint32_t id);
void show_uart_config(uint32_t id);

int main(int argc, char * argv[]) {

	uint32_t feature_set;

	fprintf(stdout,"Initialising librpip...\n"); 	
	feature_set = librpipInit(LIBRPIP_BOARD_DETECT, LIBRPIP_FLAG_DEBUG_ON, 0);
	fprintf(stdout,"\n"); 	
	
	if(feature_set == 0) {
		fprintf(stdout,"librpip failed to initialise!\n");
	} else {
		display_feature_set(feature_set);
		
		if(feature_set & LIBRPIP_FEATURE_GPIO) show_pin_config(4);
		if(feature_set & LIBRPIP_FEATURE_PWM0) show_pwm_config(0);
		if(feature_set & LIBRPIP_FEATURE_PWM1) show_pwm_config(1);
		if(feature_set & LIBRPIP_FEATURE_UART0) show_uart_config(0);
		if(feature_set & LIBRPIP_FEATURE_UART1) show_uart_config(1);
		if(feature_set & LIBRPIP_FEATURE_I2C0) show_i2c_config(0);
		if(feature_set & LIBRPIP_FEATURE_I2C1) show_i2c_config(1);
		if(feature_set & LIBRPIP_FEATURE_SPI0) show_spi_config(0,0);
		if(feature_set & LIBRPIP_FEATURE_SPI0) show_spi_config(0,1);
		if(feature_set & LIBRPIP_FEATURE_SPI1) show_spi_config(1,0);
		if(feature_set & LIBRPIP_FEATURE_SPI1) show_spi_config(1,1);
		if(feature_set & LIBRPIP_FEATURE_SPI1) show_spi_config(1,2);
		
	}
		
	librpipClose();	
	return 0;
}



void display_feature_set(uint32_t fs) {

	fprintf(stdout,"Getting feature set...\n"); 	
	if(fs & LIBRPIP_FEATURE_GPIO) 	{fprintf(stdout,"GPIO Functions are available on "); display_gpio_valid_pins(); fprintf(stdout,"\n"); }
	if(fs & LIBRPIP_FEATURE_PWM0) 	fprintf(stdout,"PWM Functions are available on PWM0.\n");
	if(fs & LIBRPIP_FEATURE_PWM1) 	fprintf(stdout,"PWM Functions are available on PWM1.\n");
	if(fs & LIBRPIP_FEATURE_UART0) 	fprintf(stdout,"UART Functions are available on UART0.\n");	
	if(fs & LIBRPIP_FEATURE_UART1) 	fprintf(stdout,"UART Functions are available on UART1.\n");	
	if(fs & LIBRPIP_FEATURE_I2C0) 	fprintf(stdout,"I2C Functions are available on I2C0.\n");	
	if(fs & LIBRPIP_FEATURE_I2C1) 	fprintf(stdout,"I2C Functions are available on I2C1.\n");	
	if(fs & LIBRPIP_FEATURE_SPI0) 	fprintf(stdout,"SPI Functions are available on SPI0.\n");
	if(fs & LIBRPIP_FEATURE_SPI1) 	fprintf(stdout,"SPI Functions are available on SPI1.\n");	

	fprintf(stdout,"\n"); 
}



void display_gpio_valid_pins() {
	uint32_t vp;
	uint8_t i,n;

	n=0;
	vp = librpipGpioGetValidPins();
	for(i=0;i<32;i++) {
		if(vp & (1<<i)) {  //its a valid pin
			if(n) fprintf(stdout,",%u", i); 
			else fprintf(stdout,"%u", i); 
			n++;
		}	
	}
}

void show_pin_config(uint32_t pin) {
	
	uint32_t flags;
	
	fprintf(stdout,"Getting config for GPIO pin %u...\n",pin); 
	if(librpipGpioConfigPinRead(pin, &flags)) {
		if(flags & LIBRPIP_GPIO_FLAG_FNC_IN) fprintf(stdout,"Direction is IN.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_FNC_OUT) fprintf(stdout,"Direction is OUT.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_PUD_OFF) fprintf(stdout,"No PULLUP or PULLDOWN.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_PUD_DOWN) fprintf(stdout,"PULLDOWN is active.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_PUD_UP) fprintf(stdout,"PULLUP is active.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_ED_OFF) fprintf(stdout,"Event Detect is OFF.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_ED_RISE) fprintf(stdout,"Event Detect on RISE.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_ED_FALL) fprintf(stdout,"Event Detect on FALL.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_ED_HIGH) fprintf(stdout,"Event Detect on HIGH.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_ED_LOW) fprintf(stdout,"Event Detect on LOW.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_ED_ARISE) fprintf(stdout,"Event Detect on Async RISE.\n");	
		if(flags & LIBRPIP_GPIO_FLAG_ED_AFALL) fprintf(stdout,"Event Detect on Async FALL.\n");			
	}
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


void show_spi_config(uint32_t id, uint32_t cs) {

	uint32_t spi_mode, lsb_first, bits_per_word, max_speed, flags;
      
	fprintf(stdout,"Getting SPI%u.%u config...\n",id,cs);   
	if(librpipSpiConfigRead(id, cs, &spi_mode, &lsb_first, &bits_per_word, &max_speed, &flags)) {
		fprintf(stdout,"Mode is %u, Lsb_first is %u, Bits per word is %u, Max speed is %u \n",spi_mode,lsb_first,bits_per_word,max_speed); 
		if(flags & LIBRPIP_SPI_FLAG_CS_HIGH) fprintf(stdout,"CS is HIGH.\n");	
		if(flags & LIBRPIP_SPI_FLAG_NO_CS) fprintf(stdout,"No CS line (manually controlled by GPIO?).\n");	
	}
	fprintf(stdout,"\n");  
}

void show_i2c_config(uint32_t id) {

	uint32_t flags;
      
	fprintf(stdout,"Getting I2C%u config...\n",id);   
	if(librpipI2cConfigRead(id, &flags)) {
		if(flags &  LIBRPIP_I2C_FLAG_PEC) 
			fprintf(stdout,"PEC is ON.\n"); 
		else 
			fprintf(stdout,"PEC is OFF.\n");	
	}
	fprintf(stdout,"\n");  
}

void show_uart_config(uint32_t id) {

	uint32_t baud,csize,parity,stop,mode;

	fprintf(stdout,"Getting UART%u config...\n",id);  	
	if(librpipUartConfigRead(id, &baud, &csize, &parity, &stop, &mode)) {
		fprintf(stdout,"Baud is %u, char size is %u, parity is %u, stop bits is %u, mode is %u\n",baud,csize,parity,stop,mode); 
	}
	fprintf(stdout,"\n");  
}