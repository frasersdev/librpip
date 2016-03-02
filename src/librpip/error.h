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

#ifndef LIBRPIP_ERROR_H
#define LIBRPIP_ERROR_H

#define LIBRPIP_ERROR_MSG_INFO		1
#define LIBRPIP_ERROR_MSG_WARNING	2
#define LIBRPIP_ERROR_MSG_ERROR		3

//functions
uint32_t librpipErrorGetCode(void);
uint32_t librpipErrorGetSeverity(void);
uint32_t librpipErrorGetDescription(char* desc, int len);
void librpipErrorPrint(void);

#endif
