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

#ifndef LIBRPIP_PWM_H
#define LIBRPIP_PWM_H

#define LIBRPIP_PWM_DUTYCYCLE	0
#define LIBRPIP_PWM_ENABLE		1
#define LIBRPIP_PWM_PERIOD		2
#define LIBRPIP_PWM_POLARITY		3


#define LIBRPIP_PWM_STATUS_OFF				0x0
#define LIBRPIP_PWM_STATUS_ON				0x1

#define LIBRPIP_PWM_FLAG_POLARITY_NORMAL	0x01
#define LIBRPIP_PWM_FLAG_POLARITY_INVERTED	0x02

#define LIBRPIP_PWM_FLAG_MASK				(LIBRPIP_PWM_FLAG_POLARITY_INVERTED | LIBRPIP_PWM_FLAG_POLARITY_NORMAL)


#define LIBRPIP_SERVO_RANGE		0
#define LIBRPIP_SERVO_PMIN		1
#define LIBRPIP_SERVO_PMAX		2

#define LIBRPIP_SERVO_RANGE_DEFAULT		180
#define LIBRPIP_SERVO_PMIN_DEFAULT		1000 //1000us == 1ms
#define LIBRPIP_SERVO_PMAX_DEFAULT		2000 //2000us == 2ms

uint32_t librpipPwmConfigRead(uint32_t id, uint32_t* pin, uint32_t* period, uint32_t* duty_cycle, uint32_t* flags);
uint32_t librpipPwmConfigWrite(uint32_t id, uint32_t period, uint32_t duty_cycle, uint32_t flags);
uint32_t librpipPwmStatusRead(uint32_t id, uint32_t* status);
uint32_t librpipPwmStatusWrite(uint32_t id, uint32_t status);
uint32_t librpipPwmDutyPercentWrite(uint32_t id, float duty_cycle);

uint32_t librpipServoConfigRead(uint32_t id, uint32_t* range, uint32_t* pmin, uint32_t* pmax);
uint32_t librpipServoConfigWrite(uint32_t id, uint32_t range, uint32_t pmin, uint32_t pmax);
uint32_t librpipServoPositionWrite(uint32_t id, float angle);

//private functions
uint32_t librpip_pwm_init(uint32_t id);
uint32_t librpip_pwm_validate_id(uint32_t id);
uint32_t librpip_pwm_discover_pin_usage(uint32_t id);
uint32_t librpip_pwm_get_sysfs_id(uint32_t id);
uint32_t librpip_pwm_dutycycle_get(uint32_t id, uint32_t* duty_cycle);
uint32_t librpip_pwm_period_get(uint32_t id, uint32_t* period);
uint32_t librpip_pwm_status_get(uint32_t id, uint32_t* status);
uint32_t librpip_pwm_flags_get(uint32_t id, uint32_t* flags);
uint32_t librpip_pwm_dutycycle_set(uint32_t id, uint32_t duty_cycle);
uint32_t librpip_pwm_period_set(uint32_t id, uint32_t period);
uint32_t librpip_pwm_status_set(uint32_t id, uint32_t status);
uint32_t librpip_pwm_flags_set(uint32_t id, uint32_t flags);

#endif