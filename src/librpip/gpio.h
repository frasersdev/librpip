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

#ifndef LIBRPIP_GPIO_H
#define LIBRPIP_GPIO_H

// gpio addresses					32bit   8bit word
#define LIBRPIP_GPIO_ADR_GPFSEL0	0x00 // 0x00
#define LIBRPIP_GPIO_ADR_GPFSEL1	0x01 // 0x04 
#define LIBRPIP_GPIO_ADR_GPFSEL2	0x02 // 0x08 
#define LIBRPIP_GPIO_ADR_GPFSEL3	0x03 // 0x0c
#define LIBRPIP_GPIO_ADR_GPFSEL4	0x04 // 0x10
#define LIBRPIP_GPIO_ADR_GPFSEL5	0x05 // 0x14 
						//reserved	0x06 // 0x18
#define LIBRPIP_GPIO_ADR_GPSET0		0x07 // 0x1c
#define LIBRPIP_GPIO_ADR_GPSET1		0x08 // 0x20 
						//reserved	0x09 // 0x24
#define LIBRPIP_GPIO_ADR_GPCLR0		0x0a // 0x28 
#define LIBRPIP_GPIO_ADR_GPCLR1		0x0b // 0x2c 
						//reserved	0x0c // 0x30
#define LIBRPIP_GPIO_ADR_GPLEV0 	0x0d // 0x34 
#define LIBRPIP_GPIO_ADR_GPLEV1		0x0e // 0x38 
						//reserved	0x0f // 0x3c
#define LIBRPIP_GPIO_ADR_GPEDS0		0x10 // 0x40 
#define LIBRPIP_GPIO_ADR_GPEDS1		0x11 // 0x44
						//reserved	0x12 // 0x48
#define LIBRPIP_GPIO_ADR_GPREN0		0x13 // 0x4c 
#define LIBRPIP_GPIO_ADR_GPREN1		0x14 // 0x50
						//reserved	0x15 // 0x54
#define LIBRPIP_GPIO_ADR_GPFEN0		0x16 // 0x58 
#define LIBRPIP_GPIO_ADR_GPFEN1		0x17 // 0x5c
						//reserved	0x18 // 0x60
#define LIBRPIP_GPIO_ADR_GPHEN0		0x19 // 0x64
#define LIBRPIP_GPIO_ADR_GPHEN1		0x1a // 0x68
						//reserved	0x1b // 0x6c
#define LIBRPIP_GPIO_ADR_GPLEN0		0x1c // 0x70
#define LIBRPIP_GPIO_ADR_GPLEN1		0x12 // 0x74
						//reserved	0x1e // 0x78
#define LIBRPIP_GPIO_ADR_GPAREN0	0x1f // 0x7c
#define LIBRPIP_GPIO_ADR_GPAREN1	0x20 // 0x80
						//reserved	0x21 // 0x84
#define LIBRPIP_GPIO_ADR_GPAFEN0	0x22 // 0x88
#define LIBRPIP_GPIO_ADR_GPAFEN1	0x23 // 0x8c
						//reserved	0x24 // 0x90
#define LIBRPIP_GPIO_ADR_GPPUD		0x25 // 0x94
#define LIBRPIP_GPIO_ADR_GPPUDCLK0	0x26 // 0x98
#define LIBRPIP_GPIO_ADR_GPPUDCLK1	0x27 // 0x9c
						//reserved	0x28 // 0xa0
						//reserved	0x2b // 0xb0

// pin functions
#define LIBRPIP_GPIO_FNC_IN			0b000
#define LIBRPIP_GPIO_FNC_OUT		0b001 
#define LIBRPIP_GPIO_FNC_ALT0		0b100 
#define LIBRPIP_GPIO_FNC_ALT1		0b101
#define LIBRPIP_GPIO_FNC_ALT2		0b110
#define LIBRPIP_GPIO_FNC_ALT3		0b111
#define LIBRPIP_GPIO_FNC_ALT4		0b011
#define LIBRPIP_GPIO_FNC_ALT5		0b010


// pin pull-up/down
#define LIBRPIP_GPIO_PUD_OFF		0b00
#define LIBRPIP_GPIO_PUD_DOWN		0b01
#define LIBRPIP_GPIO_PUD_UP			0b10 

// pin event detect
#define LIBRPIP_GPIO_ED_RISE		LIBRPIP_GPIO_ADR_GPREN0
#define LIBRPIP_GPIO_ED_FALL		LIBRPIP_GPIO_ADR_GPFEN0
#define LIBRPIP_GPIO_ED_HIGH		LIBRPIP_GPIO_ADR_GPHEN0 
#define LIBRPIP_GPIO_ED_LOW			LIBRPIP_GPIO_ADR_GPLEN0
#define LIBRPIP_GPIO_ED_ARISE		LIBRPIP_GPIO_ADR_GPAREN0
#define LIBRPIP_GPIO_ED_AFALL		LIBRPIP_GPIO_ADR_GPAFEN0


//config flags
#define LIBRPIP_GPIO_FLAG_FNC_IN	0x001
#define LIBRPIP_GPIO_FLAG_FNC_OUT	0x002
#define LIBRPIP_GPIO_FLAG_PUD_OFF	0x004
#define LIBRPIP_GPIO_FLAG_PUD_DOWN	0x008
#define LIBRPIP_GPIO_FLAG_PUD_UP	0x010 
#define LIBRPIP_GPIO_FLAG_ED_OFF	0x020
#define LIBRPIP_GPIO_FLAG_ED_RISE	0x040
#define LIBRPIP_GPIO_FLAG_ED_FALL	0x080
#define LIBRPIP_GPIO_FLAG_ED_HIGH	0x100 
#define LIBRPIP_GPIO_FLAG_ED_LOW	0x200
#define LIBRPIP_GPIO_FLAG_ED_ARISE	0x400
#define LIBRPIP_GPIO_FLAG_ED_AFALL	0x800

//exposed functions
uint32_t librpipGpioGetValidPins(void);
uint32_t librpipGpioConfigPinRead(uint32_t pin, uint32_t* flags);
uint32_t librpipGpioConfigPinWrite(uint32_t pin, uint32_t flags);

uint32_t librpipGpioPinWrite(uint32_t pin, uint32_t value);
uint32_t librpipGpioPinToggle(uint32_t pin);
uint32_t librpipGpioPinPulse(uint32_t pin, uint32_t length);

uint32_t librpipGpioPinRead(uint32_t pin, uint32_t* value);
uint32_t librpipGpioPinEvent(uint32_t pin, uint32_t* event);
uint32_t librpipGpioPinEventWait(uint32_t pin, uint32_t timeout);


//internal functions
uint32_t 	librpip_gpio_init(uint32_t ignore_pins);
uint32_t 	librpip_gpio_close(void);
uint32_t 	librpip_gpio_set_valid_pins(void);
uint32_t 	librpip_gpio_confirm_valid_pins(void);
uint32_t 	librpip_gpio_validate_pin(uint32_t pin, uint32_t mode);
void 		librpip_gpio_pin_switch(uint32_t pin);
void 		librpip_gpio_pin_set(uint32_t pin);
void 		librpip_gpio_pin_clear(uint32_t pin);
uint32_t 	librpip_gpio_pin_get(uint32_t pin);
uint32_t 	librpip_gpio_pin_mode_get(uint32_t pin);
void 		librpip_gpio_pin_mode_set(uint32_t pin, uint32_t dir);
void 		librpip_gpio_pin_pud_set(uint32_t pin, uint32_t pud_state);
uint32_t 	librpip_gpio_pin_event_check(uint32_t pin);
void 		librpip_gpio_pin_event_clear(uint32_t pin);
void 		librpip_gpio_pin_event_set(uint32_t pin, uint32_t event);
void 		librpip_gpio_pin_event_unset(uint32_t pin);
uint32_t 	librpip_gpio_pin_event_get(uint32_t pin);
void 		librpip_gpio_calc_offset(uint32_t pin, uint32_t* byte, uint32_t* bit);

#endif



