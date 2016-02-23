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

#ifndef LIBRPIP_DT_H
#define LIBRPIP_DT_H

#define LIBRPIP_DT_MODULE_GPIOMEM		"/sys/firmware/devicetree/base/soc/gpiomem"
#define LIBRPIP_DT_MODULE_GPIO			"/sys/firmware/devicetree/base/soc/gpio@7e200000"
#define LIBRPIP_DT_MODULE_I2C0			"/sys/firmware/devicetree/base/soc/i2c@7e205000"
#define LIBRPIP_DT_MODULE_I2C1 			"/sys/firmware/devicetree/base/soc/i2c@7e804000"
#define LIBRPIP_DT_MODULE_I2S			"/sys/firmware/devicetree/base/soc/i2s@7e203000"
#define LIBRPIP_DT_MODULE_PWM			"/sys/firmware/devicetree/base/soc/pwm@7e20c000"
#define LIBRPIP_DT_MODULE_SPI0			"/sys/firmware/devicetree/base/soc/spi@7e204000"
#define LIBRPIP_DT_MODULE_SPI0_SPIDEV0		"/sys/firmware/devicetree/base/soc/spi@7e204000/spidev@0"
#define LIBRPIP_DT_MODULE_SPI0_SPIDEV1		"/sys/firmware/devicetree/base/soc/spi@7e204000/spidev@1"
#define LIBRPIP_DT_MODULE_SPI1			"/sys/firmware/devicetree/base/soc/spi@7e215080"
#define LIBRPIP_DT_MODULE_SPI1_SPIDEV0		"/sys/firmware/devicetree/base/soc/spi@7e215080/spidev@0"
#define LIBRPIP_DT_MODULE_SPI1_SPIDEV1		"/sys/firmware/devicetree/base/soc/spi@7e215080/spidev@1"
#define LIBRPIP_DT_MODULE_SPI1_SPIDEV2		"/sys/firmware/devicetree/base/soc/spi@7e215080/spidev@2"
#define LIBRPIP_DT_MODULE_UART0			"/sys/firmware/devicetree/base/soc/uart@7e201000"
#define LIBRPIP_DT_MODULE_UART1			"/sys/firmware/devicetree/base/soc/uart@7e215040"

#define LIBRPIP_DT_MODULE_GPIOMEM_ID		1
#define LIBRPIP_DT_MODULE_GPIO_ID		2
#define LIBRPIP_DT_MODULE_I2C0_ID		3
#define LIBRPIP_DT_MODULE_I2C1_ID		4
#define LIBRPIP_DT_MODULE_I2S_ID		5
#define LIBRPIP_DT_MODULE_PWM_ID		6
#define LIBRPIP_DT_MODULE_SPI0_ID		7
#define LIBRPIP_DT_MODULE_SPI0_SPIDEV0_ID	8
#define LIBRPIP_DT_MODULE_SPI0_SPIDEV1_ID	9
#define LIBRPIP_DT_MODULE_SPI1_ID		10
#define LIBRPIP_DT_MODULE_SPI1_SPIDEV0_ID	11
#define LIBRPIP_DT_MODULE_SPI1_SPIDEV1_ID	12
#define LIBRPIP_DT_MODULE_SPI1_SPIDEV2_ID	13	
#define LIBRPIP_DT_MODULE_UART0_ID		14
#define LIBRPIP_DT_MODULE_UART1_ID		15


//component files
#define LIBRPIP_DT_FILE_STATUS			"status"
#define LIBRPIP_DT_FILE_I2C0_PINS		"i2c0/brcm,pins"
#define LIBRPIP_DT_FILE_I2C1_PINS		"i2c1/brcm,pins"
#define LIBRPIP_DT_FILE_I2S_PINS 		"i2s/brcm,pins"
#define LIBRPIP_DT_FILE_PWM_PINS 		"pwm_pins/brcm,pins"
#define LIBRPIP_DT_FILE_SPI0_CSPINS 		"spi0_cs_pins/brcm,pins"
#define LIBRPIP_DT_FILE_SPI0_PINS		"spi0_pins/brcm,pins"
#define LIBRPIP_DT_FILE_SPI1_CSPINS 		"spi1_cs_pins/brcm,pins"
#define LIBRPIP_DT_FILE_SPI1_PINS 		"spi1_pins/brcm,pins"

#define LIBRPIP_DT_FILE_STATUS_ID		1
#define LIBRPIP_DT_FILE_I2C0_PINS_ID		2
#define LIBRPIP_DT_FILE_I2C1_PINS_ID		3
#define LIBRPIP_DT_FILE_I2S_PINS_ID 		4
#define LIBRPIP_DT_FILE_PWM_PINS_ID 		5
#define LIBRPIP_DT_FILE_SPI0_CSPINS_ID 		6
#define LIBRPIP_DT_FILE_SPI0_PINS_ID		7
#define LIBRPIP_DT_FILE_SPI1_CSPINS_ID 		8
#define LIBRPIP_DT_FILE_SPI1_PINS_ID 		9

uint32_t librpip_dt_module_enabled(uint32_t moduleid);
uint32_t librpip_dt_module_exists(uint32_t moduleid);
uint32_t librpip_dt_file_open(uint32_t moduleid, uint32_t fileid, int flags, int* fd);
uint32_t librpip_dt_file_get_str(uint32_t moduleid, uint32_t fileid, char* value, uint32_t len);
uint32_t librpip_dt_file_get_raw(uint32_t moduleid, uint32_t fileid, uint8_t* buf, uint32_t len, uint32_t* rlen);

#endif