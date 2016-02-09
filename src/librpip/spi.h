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

#ifndef LIBRPIP_SPI_H
#define LIBRPIP_SPI_H


/* 
From spidev.h
SPI_IOC_xx_MODE = the 4 SPI_MODES listed below
SPI_IOC_xx_MODE32 = all the other ones
ie SPI_IOC_xx_MODE32 & 0b11 = SPI_IOC_xx_MODE

#define SPI_CPHA                0x01
#define SPI_CPOL                0x02

#define SPI_MODE_0              (0|0)
#define SPI_MODE_1              (0|SPI_CPHA)
#define SPI_MODE_2              (SPI_CPOL|0)
#define SPI_MODE_3              (SPI_CPOL|SPI_CPHA)

#define SPI_CS_HIGH             0x04
#define SPI_LSB_FIRST           0x08   //unsupported by SPI0 and SPI1
#define SPI_3WIRE               0x10   //unsupported by SPI1
#define SPI_LOOP                0x20   //unsupported by SPI0 and SPI1
#define SPI_NO_CS               0x40
#define SPI_READY               0x80   //unsupported by SPI0 and SPI1
#define SPI_TX_DUAL             0x100  //unsupported by SPI0 and SPI1
#define SPI_TX_QUAD             0x200  //unsupported by SPI0 and SPI1
#define SPI_RX_DUAL             0x400  //unsupported by SPI0 and SPI1
#define SPI_RX_QUAD             0x800  //unsupported by SPI0 and SPI1

*/

#define LIBRPIP_SPI_SUPPORTED_FLAGS (SPI_CS_HIGH | SPI_NO_CS)	//from spi-bcm2835.c & spi-bcm2835aux.c  
																//Note 3wire mode is supported on SPI0 and handled as discrete functions


//exposed functions
//config
uint32_t librpipSpiConfigRead(uint32_t id, uint32_t cs, uint32_t* mode, uint32_t* lsb_first, uint32_t* bits_per_word, uint32_t* max_speed, uint32_t* flags);
uint32_t librpipSpiConfigWrite(uint32_t id, uint32_t cs, uint32_t mode, uint32_t max_speed, uint32_t flags);


//internal functions
uint32_t librpip_spi_init(uint32_t id);
uint32_t librpip_spi_close(uint32_t id);
uint32_t librpip_spi_validate_spixcsx(uint32_t id, uint32_t cs, int* fd);
uint32_t librpip_spi_validate_spixcsx_3wire(uint32_t id, uint32_t cs, int* fd);
uint32_t librpip_spi_validate_flags(uint32_t spi_flags);
void librpip_spi_struct_init(struct spi_ioc_transfer* tr);

uint32_t librpip_spi_transaction(uint32_t id, uint32_t cs, struct spi_ioc_transfer* spi, uint16_t len);
uint32_t librpip_spi_transaction_3w(uint32_t id, uint32_t cs, struct spi_ioc_transfer* spi, uint16_t len);

uint32_t librpip_spi_config_get(int* fd, uint32_t* spi_mode, uint32_t* lsb_first, uint32_t* bits_per_word, uint32_t* max_speed);
uint32_t librpip_spi_config_get_mode(int* fd, uint32_t* spi_mode);
uint32_t librpip_spi_config_get_lsb(int* fd, uint32_t* lsb_first);
uint32_t librpip_spi_config_get_bpw(int* fd, uint32_t* bits_per_word);
uint32_t librpip_spi_config_get_maxspeed(int* fd, uint32_t* max_speed);

uint32_t librpip_spi_config_set(int* fd, uint32_t spi_flags, uint32_t lsb_first, uint32_t bits_per_word, uint32_t max_speed);
uint32_t librpip_spi_config_set_mode(int* fd, uint32_t spi_mode);
uint32_t librpip_spi_config_set_lsb(int* fd, uint32_t lsb_first);
uint32_t librpip_spi_config_set_bpw(int* fd, uint32_t bits_per_word);
uint32_t librpip_spi_config_set_maxspeed(int* fd, uint32_t max_speed);



#endif