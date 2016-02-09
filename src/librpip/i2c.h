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

#ifndef LIBRPIP_I2C_H
#define LIBRPIP_I2C_H


#define LIBRPIP_I2C_FLAGS_PEC		0x01
#define LIBRPIP_I2C_FLAGS_TENBIT	0x02  //currently unsupported
#define LIBRPIP_I2C_SUPPORTED_FLAGS	LIBRPIP_I2C_FLAGS_PEC

//exposed functions
uint32_t librpipI2cConfigWrite(uint32_t id, uint32_t flags);
uint32_t librpipI2cConfigRead(uint32_t id, uint32_t* flags);


//internal functions
uint32_t librpip_i2c_transaction(uint32_t id, uint32_t slave, struct i2c_rdwr_ioctl_data* i2c);

uint32_t librpip_i2c_init(uint32_t id);
uint32_t librpip_i2c_close(uint32_t id);

uint32_t librpip_i2c_validate_id(uint32_t id, int* fd);
uint32_t librpip_i2c_config_get_funcs(int* fd, unsigned long *funcs);
uint32_t librpip_i2c_config_set_slave(int* fd, uint32_t addr);
uint32_t librpip_i2c_config_set_10bit(int* fd, uint32_t select);
uint32_t librpip_i2c_config_set_pec(int* fd, uint32_t select);

#endif