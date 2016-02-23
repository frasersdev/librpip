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
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "rpi.h"
#include "board.h"
#include "device.h"
#include "dt.h"
#include "pins.h"
#include "error.h"
#include "pwm.h"
#include "gpio.h"

extern uint32_t librpip_feature_set;
extern uint32_t librpip_error_code;
extern uint32_t librpip_error_data;
extern uint32_t librpip_error_extra;
extern uint32_t librpip_board;
extern uint32_t librpip_flags;
extern uint32_t librpip_pins_used;

uint32_t librpip_gpio_valid_pins=0;
static volatile uint32_t  *gpio_map = MAP_FAILED;

/* exposed functions */
uint32_t librpip_gpio_init(uint32_t ignore_pins) {

	int gpiodev;
	uint32_t i;

	librpip_error_code=0;
	librpip_error_data=0;

	if(librpip_flags & LIBRPIP_FLAG_SKIP_GPIO) { //we have been asked not not use GPIO
		librpip_error_code=0x105;					
	} else {
	
		if(librpip_device_node_exists(LIBRPIP_DEV_GPIO_ID)) {
			if(librpip_device_node_open(LIBRPIP_DEV_GPIO_ID, (O_RDWR | O_SYNC), &gpiodev)) {
				gpio_map = (uint32_t *)mmap(NULL, 0xB4, PROT_READ|PROT_WRITE, MAP_SHARED, gpiodev, 0);
				close(gpiodev);
				if (gpio_map == MAP_FAILED) {
					librpip_error_code = 0x102;
			  	} else {
					librpip_feature_set|=LIBRPIP_FEATURE_GPIO;

					if(librpip_gpio_set_valid_pins()) {	
						librpip_gpio_valid_pins &= (~ignore_pins);
						librpip_gpio_confirm_valid_pins();
						if(~(librpip_flags & LIBRPIP_FLAG_NO_RESET)) {
							for(i=0;i<32;i++) {
								if(librpip_gpio_valid_pins & (1<<i)) {
									librpip_gpio_pin_mode_set(i, LIBRPIP_GPIO_FNC_OUT); 
									librpip_gpio_pin_clear(i);
									librpip_gpio_pin_mode_set(i, LIBRPIP_GPIO_FNC_IN); 
								}
							}
						}
					}
				}
			} else {
				librpip_error_code = 0x101;   //device exists but we can't read it.
			}
		} else {
			librpip_error_code = 0x100;  //device does not exist
		}
	}
	return librpip_error_code ? 0 : 1;
}

uint32_t librpipGpioGetValidPins() {
	return librpip_gpio_valid_pins;
}


uint32_t librpip_gpio_close(void) {
	//since this is only called when exiting there is no need to try an unmap 
	return 1;
}

uint32_t librpipGpioConfigPinWrite(uint32_t pin, uint32_t flags) {
	librpip_error_code=0;
	librpip_error_data=0;	

	if(librpip_gpio_validate_pin(pin,0)) {
		if(flags & LIBRPIP_GPIO_FLAG_FNC_IN) {
			librpip_gpio_pin_mode_set(pin, LIBRPIP_GPIO_FNC_IN);
		}
		if(flags & LIBRPIP_GPIO_FLAG_FNC_OUT) {
			librpip_gpio_pin_mode_set(pin, LIBRPIP_GPIO_FNC_OUT);
			librpip_gpio_pin_clear(pin); // zero the output
		}
		if(flags & LIBRPIP_GPIO_FLAG_PUD_OFF) {
			librpip_gpio_pin_pud_set(pin, LIBRPIP_GPIO_PUD_OFF);
		}
		if(flags & LIBRPIP_GPIO_FLAG_PUD_DOWN) {
			librpip_gpio_pin_pud_set(pin, LIBRPIP_GPIO_PUD_DOWN);
		}
		if(flags & LIBRPIP_GPIO_FLAG_PUD_UP) {
			librpip_gpio_pin_pud_set(pin, LIBRPIP_GPIO_PUD_UP);
		}
		if(flags & LIBRPIP_GPIO_FLAG_ED_OFF) {
			librpip_gpio_pin_event_unset(pin);
		}
		if(flags & LIBRPIP_GPIO_FLAG_ED_RISE) {
			librpip_gpio_pin_event_set(pin, LIBRPIP_GPIO_ED_RISE);
		}
		if(flags & LIBRPIP_GPIO_FLAG_ED_FALL) {
			librpip_gpio_pin_event_set(pin, LIBRPIP_GPIO_ED_FALL);
		}
		if(flags & LIBRPIP_GPIO_FLAG_ED_HIGH) {
			librpip_gpio_pin_event_set(pin, LIBRPIP_GPIO_ED_HIGH);
		}
		if(flags & LIBRPIP_GPIO_FLAG_ED_LOW) {
			librpip_gpio_pin_event_set(pin, LIBRPIP_GPIO_ED_LOW);
		}
		if(flags & LIBRPIP_GPIO_FLAG_ED_ARISE) {
			librpip_gpio_pin_event_set(pin, LIBRPIP_GPIO_ED_ARISE);
		}
		if(flags & LIBRPIP_GPIO_FLAG_ED_AFALL) {
			librpip_gpio_pin_event_set(pin, LIBRPIP_GPIO_ED_AFALL);
		}

		return 1;
	} else {
		librpip_error_data=pin;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;
	}
}

uint32_t librpipGpioConfigPinRead(uint32_t pin, uint32_t* flags) {
	librpip_error_code=0;
	librpip_error_data=0;	
	
	uint32_t temp,result;
	
	result=0;

	if(librpip_gpio_validate_pin(pin,0)) {
		temp=librpip_gpio_pin_mode_get(pin);
		switch(temp) {
			case LIBRPIP_GPIO_FNC_IN:
				result |= LIBRPIP_GPIO_FLAG_FNC_IN;
				break;
			case LIBRPIP_GPIO_FNC_OUT:
				result |= LIBRPIP_GPIO_FLAG_FNC_OUT;
				break;
			default:
				librpip_error_code=0x114;
				librpip_error_data=pin;
				librpip_error_extra=temp;
		}
		result |= librpip_gpio_pin_event_get(pin);
	} else {
		librpip_error_data=pin;
	}
	if(librpip_error_code) {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		*flags=0;
		return 0;
	} else {
		*flags=result;
		return 1;
	}
	
}

uint32_t librpipGpioPinWrite(uint32_t pin, uint32_t value) {
	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(librpip_gpio_validate_pin(pin,LIBRPIP_GPIO_FNC_OUT)) {
		if(value) {
			librpip_gpio_pin_set(pin);
		} else {
			librpip_gpio_pin_clear(pin);
		}
		return 1;
	} else {
		librpip_error_data=pin;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;
	}
}

uint32_t librpipGpioPinToggle(uint32_t pin) {

	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(librpip_gpio_validate_pin(pin,LIBRPIP_GPIO_FNC_OUT)) {
		librpip_gpio_pin_switch(pin);
		return 1;
	} else {
		librpip_error_data=pin;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;
	}
}

uint32_t librpipGpioPinPulse(uint32_t pin, uint32_t length) {

	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(librpip_gpio_validate_pin(pin,LIBRPIP_GPIO_FNC_OUT)) {
		librpip_gpio_pin_switch(pin);
		usleep(length);
		librpip_gpio_pin_switch(pin);
		return 1;
	} else {
		librpip_error_data=pin;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;
	}
}

uint32_t librpipGpioPinRead(uint32_t pin, uint32_t* value) {

	librpip_error_code=0;
	librpip_error_data=0;	
	
	if(librpip_gpio_validate_pin(pin,LIBRPIP_GPIO_FNC_IN)) {
		*value = librpip_gpio_pin_get(pin) > 0 ? 1 : 0;
		return 1;
	} else {
		librpip_error_data=pin;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;
	}
}

uint32_t librpipGpioPinEvent(uint32_t pin, uint32_t* event) {
	librpip_error_code=0;
	librpip_error_data=0;	

	if(librpip_gpio_validate_pin(pin,0)) {
		*event=librpip_gpio_pin_event_check(pin);
		if(*event) librpip_gpio_pin_event_clear(pin); 
		return 1;
	} else {
		librpip_error_data=pin;
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
		return 0;
	}
}

uint32_t librpipGpioPinEventWait(uint32_t pin, uint32_t timeout) {

	librpip_error_code=0;
	librpip_error_data=0;
	
	uint32_t event=0;	

	if(librpip_gpio_validate_pin(pin,0)) {
		event=librpip_gpio_pin_event_check(pin);
		while(timeout && !event) {
			usleep(1);
			timeout--;
			event=librpip_gpio_pin_event_check(pin);
		}
		if(event) {
			librpip_gpio_pin_event_clear(pin); 
		} else {
			librpip_error_code=0x120;
			librpip_error_data=pin;	
		}
	} else {
		librpip_error_data=pin;
	}
	
	if(librpip_error_code) {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();
	}
	return librpip_error_code ? 0 : 1;
}


/* internal functions */
uint32_t librpip_gpio_set_valid_pins(void) {

	//first check for some other modules that might be running and using gpios
	if(librpip_dt_module_enabled(LIBRPIP_DT_MODULE_I2S_ID)) 
		librpip_pins_used |= librpip_pins_getextpins_from_dt(LIBRPIP_DT_FILE_I2S_PINS_ID);
	if(librpip_dt_module_enabled(LIBRPIP_DT_MODULE_1WIRE_ID)) 
		librpip_pins_used |= librpip_pins_getextpins_from_dt(LIBRPIP_DT_FILE_1WIRE_PINS_ID);

	librpip_gpio_valid_pins=librpip_board_get_pins();
	if(librpip_gpio_valid_pins==0) {
		librpip_error_code=0x103;
	} else {
		librpip_gpio_valid_pins &= (~librpip_pins_used);	
	}

	return librpip_error_code ? 0 : 1;
}

uint32_t librpip_gpio_confirm_valid_pins(void) {

	uint32_t i,mode,pins;
	
	pins=0;
	
	for (i=0;i<32;i++) {
		if(librpip_gpio_valid_pins & (1<<i)) {
			mode = librpip_gpio_pin_mode_get(i);
			if(mode>1) { // there is an unexpected alternate function active on this pin!!!
				pins |= (1<<i) ;
			}
		}
		
	}
	if(pins>0) {
		librpip_gpio_valid_pins &= ~pins;
		librpip_error_code=0x104;
		librpip_error_data=pins;
	}

	return librpip_error_code ? 0 : 1;
}


/* validation function. this is run in all exposed gpio functions aside from init */

uint32_t librpip_gpio_validate_pin(uint32_t pin, uint32_t mode) {

	if(!(librpip_feature_set & LIBRPIP_FEATURE_GPIO)) {
		librpip_error_code=0x110;
	} else if(pin > 31) {
		librpip_error_code=0x111;
	} else if(!(librpip_gpio_valid_pins & (1 << pin))) {
		librpip_error_code=0x112;
	} else if(mode == LIBRPIP_GPIO_FNC_OUT && librpip_gpio_pin_mode_get(pin) != LIBRPIP_GPIO_FNC_OUT) {
		librpip_error_code=0x113;
	} 

	return librpip_error_code ? 0 : 1;
}


/* primitive functions that have no error checking. data is validated before these are called */

void librpip_gpio_pin_switch(uint32_t pin) { 
	if(librpip_gpio_pin_get(pin)) {
		librpip_gpio_pin_clear(pin);
	} else {
		librpip_gpio_pin_set(pin);
	}
}

void librpip_gpio_pin_set(uint32_t pin) {
	 *(gpio_map + LIBRPIP_GPIO_ADR_GPSET0) =  ( 1 << pin)  ;
}

void librpip_gpio_pin_clear(uint32_t pin) { 
	 *(gpio_map + LIBRPIP_GPIO_ADR_GPCLR0) =  ( 1 << pin)  ;
}

uint32_t librpip_gpio_pin_get(uint32_t pin) { 
	 return *(gpio_map + LIBRPIP_GPIO_ADR_GPLEV0) & ( 1 << pin);
}


uint32_t librpip_gpio_pin_mode_get(uint32_t pin) {

	uint32_t byte_offset;
	uint32_t bit_offset;
	
	librpip_gpio_calc_offset(pin, &byte_offset, &bit_offset);
   
   return ( *(gpio_map + LIBRPIP_GPIO_ADR_GPFSEL0 + byte_offset) >> bit_offset) &   0b111;

}

void librpip_gpio_pin_mode_set(uint32_t pin, uint32_t dir) {

	uint32_t byte_offset;
	uint32_t bit_offset;
	
	librpip_gpio_calc_offset(pin, &byte_offset, &bit_offset);
	
	*(gpio_map + LIBRPIP_GPIO_ADR_GPFSEL0 + byte_offset) &= ~(0b111<<bit_offset);     
	*(gpio_map + LIBRPIP_GPIO_ADR_GPFSEL0 + byte_offset) |= (dir<<bit_offset);		

}


void librpip_gpio_pin_pud_set(uint32_t pin, uint32_t pud_state) {

	*(gpio_map + LIBRPIP_GPIO_ADR_GPPUD) = pud_state;
	
	//must wait 150 clock cycles - but BCM arm peripherals document is not clear as to what clock. Assuming system clock @ 250mHz
	// = 150/250,000,000 seconds
	// = .6us
	usleep(1);
	
	*(gpio_map + LIBRPIP_GPIO_ADR_GPPUDCLK0) = (1<<pin);
	
	usleep(1);

	*(gpio_map + LIBRPIP_GPIO_ADR_GPPUD) = 0;
	*(gpio_map + LIBRPIP_GPIO_ADR_GPPUDCLK0) = 0;
}

uint32_t librpip_gpio_pin_event_check(uint32_t pin) {

	return *(gpio_map + LIBRPIP_GPIO_ADR_GPEDS0) & (1<<pin);
}

void librpip_gpio_pin_event_clear(uint32_t pin) {

	*(gpio_map + LIBRPIP_GPIO_ADR_GPEDS0) = (1<<pin);
}

void librpip_gpio_pin_event_set(uint32_t pin, uint32_t event) {

	*(gpio_map + event) |= (1<<pin);
}

void librpip_gpio_pin_event_unset(uint32_t pin) {

	uint32_t pinmask = ~(1<<pin);

	*(gpio_map + LIBRPIP_GPIO_ADR_GPREN0) &= pinmask;
	*(gpio_map + LIBRPIP_GPIO_ADR_GPFEN0) &= pinmask;
	*(gpio_map + LIBRPIP_GPIO_ADR_GPHEN0) &= pinmask;
	*(gpio_map + LIBRPIP_GPIO_ADR_GPLEN0) &= pinmask;
	*(gpio_map + LIBRPIP_GPIO_ADR_GPAREN0) &= pinmask;
	*(gpio_map + LIBRPIP_GPIO_ADR_GPAFEN0) &= pinmask;

}

uint32_t librpip_gpio_pin_event_get(uint32_t pin) {

	uint32_t events=0;
	uint32_t shiftpin=1<<pin;

	events |= *(gpio_map + LIBRPIP_GPIO_ADR_GPREN0) & shiftpin ? LIBRPIP_GPIO_FLAG_ED_RISE : 0;
	events |= *(gpio_map + LIBRPIP_GPIO_ADR_GPFEN0) & shiftpin ? LIBRPIP_GPIO_FLAG_ED_FALL : 0;
	events |= *(gpio_map + LIBRPIP_GPIO_ADR_GPHEN0) & shiftpin ? LIBRPIP_GPIO_FLAG_ED_HIGH : 0;
	events |= *(gpio_map + LIBRPIP_GPIO_ADR_GPLEN0) & shiftpin ? LIBRPIP_GPIO_FLAG_ED_LOW : 0;
	events |= *(gpio_map + LIBRPIP_GPIO_ADR_GPAREN0) & shiftpin ? LIBRPIP_GPIO_FLAG_ED_ARISE : 0;
	events |= *(gpio_map + LIBRPIP_GPIO_ADR_GPAFEN0) & shiftpin ? LIBRPIP_GPIO_FLAG_ED_AFALL : 0;
	
	if(!events) events = LIBRPIP_GPIO_FLAG_ED_OFF;
	
	return events;

}


void librpip_gpio_calc_offset(uint32_t pin, uint32_t* byte, uint32_t* bit) {
	*byte =  pin/10;
	*bit  = (pin%10) * 3;
}

