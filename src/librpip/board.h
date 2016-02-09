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

#ifndef LIBRPIP_BOARD_H
#define LIBRPIP_BOARD_H

#define LIBRPIP_BOARD_PI_A			1
#define LIBRPIP_BOARD_PI_B_R1		2
#define LIBRPIP_BOARD_PI_B_R2		3
#define LIBRPIP_BOARD_PI_A_PLUS		4
#define LIBRPIP_BOARD_PI_B_PLUS		5
#define LIBRPIP_BOARD_PI2_B			6
#define LIBRPIP_BOARD_PI_ZERO		7
#define LIBRPIP_BOARD_PI_COMPUTE	8


//these marry up with the library features in rpi.h
											//PPSSIIUUG
											//WWPP22AAP
											//MMIICCRRI
											//101010TTO
											//      10
#define LIBRPIP_BOARD_PI_A_FEATURE			0b010101111
#define LIBRPIP_BOARD_PI_B_R1_FEATURE		0b010101111
#define LIBRPIP_BOARD_PI_B_R2_FEATURE		0b010110111
#define LIBRPIP_BOARD_PI_A_PLUS_FEATURE		0b111111111
#define LIBRPIP_BOARD_PI_B_PLUS_FEATURE		0b111111111
#define LIBRPIP_BOARD_PI2_B_FEATURE			0b111111111
#define LIBRPIP_BOARD_PI_ZERO_FEATURE		0b111111111
#define LIBRPIP_BOARD_PI_COMPUTE_FEATURE	0b111111111
											//000000000
											//xxxxxxxxx
											//100000000
											//084210000
											//000008421

										//3322222222221111111111
										//10987654321098765432109876543210
#define LIBRPIP_BOARD_PI_A_PINS			0b00000011111001101100111110010011
#define LIBRPIP_BOARD_PI_B_R1_PINS		0b00000011111001101100111110010011
#define LIBRPIP_BOARD_PI_B_R2_PINS		0b11111011110001101100111110011100
#define LIBRPIP_BOARD_PI_A_PLUS_PINS	0b00001111111111111111111111111111
#define LIBRPIP_BOARD_PI_B_PLUS_PINS	0b00001111111111111111111111111111
#define LIBRPIP_BOARD_PI2_B_PINS		0b00001111111111111111111111111111
#define LIBRPIP_BOARD_PI_ZERO_PINS		0b00001111111111111111111111111111
#define LIBRPIP_BOARD_PI_COMPUTE_PINS	0b00001111111111111111111111111111

#define LIBRPIP_BOARD_CPUINFO			"/proc/cpuinfo"


//internal functions
uint32_t librpip_board_get_capabilities(void);
uint32_t librpip_board_get_pins(void);
uint32_t librpip_board_detect(void);
void librpip_board_model_print(void);

#endif



