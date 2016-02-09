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
 
#ifndef LIBRPIP_SYSFS_H
#define LIBRPIP_SYSFS_H



//sysfs files
#define LIBRPIP_SYSFS_PWM0				"/sys/class/pwm/pwmchip0/pwm0"
#define LIBRPIP_SYSFS_PWM1				"/sys/class/pwm/pwmchip0/pwm1"

//component files
#define LIBRPIP_SYSFS_PWM_DUTYCYCLE		"duty_cycle"
#define LIBRPIP_SYSFS_PWM_ENABLE		"enable"
#define LIBRPIP_SYSFS_PWM_PERIOD		"period"
#define LIBRPIP_SYSFS_PWM_POLARITY		"polarity"

//id's for accessing above files
#define LIBRPIP_SYSFS_PWM0_ID			1 
#define LIBRPIP_SYSFS_PWM1_ID			2

#define LIBRPIP_SYSFS_PWM_DUTYCYCLE_ID	1
#define LIBRPIP_SYSFS_PWM_ENABLE_ID		2
#define LIBRPIP_SYSFS_PWM_PERIOD_ID		3
#define LIBRPIP_SYSFS_PWM_POLARITY_ID	4


uint32_t librpip_sysfs_node_exists(uint32_t sysfsid);
uint32_t librpip_sysfs_node_file_validate(uint32_t sysfsid);
uint32_t librpip_sysfs_node_file_open(uint32_t sysfsid, uint32_t fileid, int flags, int* fd);
uint32_t librpip_sysfs_node_file_get_int(uint32_t sysfsid, uint32_t fileid, int* value);
uint32_t librpip_sysfs_node_file_get_str(uint32_t sysfsid, uint32_t fileid, char* value, uint32_t len);
uint32_t librpip_sysfs_node_file_set_int(uint32_t sysfsid, uint32_t fileid, int value);
uint32_t librpip_sysfs_node_file_set_str(uint32_t sysfsid, uint32_t fileid, char* value, uint32_t len);

#endif