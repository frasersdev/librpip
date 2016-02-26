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
 
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>	
#include <sys/types.h>	

#include "rpi.h"
#include "gpio.h"
#include "error.h"
#include "sysfs.h"
#include "dt.h"
#include "board.h"
#include "pins.h"
#include "pwm.h"


extern uint32_t librpip_feature_set;
extern uint32_t librpip_error_code;
extern uint32_t librpip_error_data;
extern uint32_t librpip_error_extra;
extern float 	librpip_error_extraf;
extern uint32_t librpip_board;
extern uint32_t librpip_flags;
extern uint32_t librpip_pins_used;

uint32_t librpip_pwm_pin[2];
uint32_t librpip_servo_config[2][3];

/* exposed functions */
uint32_t librpip_pwm_init(uint32_t id) {

	librpip_error_code=0;
	librpip_error_data=0;

	if(librpip_dt_module_enabled(LIBRPIP_DT_MODULE_PWM_ID)) {
		switch(id) {
			case 0:
				librpip_pwm_pin[0]=librpip_pins_getpins(LIBRPIP_FEATURE_PWM0);
				librpip_pins_used |= librpip_pwm_pin[0];
				if(librpip_sysfs_node_exists(LIBRPIP_SYSFS_PWM0_ID)) {
					if(librpip_flags & LIBRPIP_FLAG_SKIP_PWM0) { //we have been asked not not use PWM0
						librpip_error_code=0x407;					
						librpip_error_data=id;
					} else {
						if(librpip_sysfs_node_file_validate(LIBRPIP_SYSFS_PWM0_ID)) {
							librpip_feature_set |= LIBRPIP_FEATURE_PWM0;  //sysfs files all exist exists and we can RW them
						} else {
							librpip_error_code=0x401;					//device exists but we cant RW it
							librpip_error_data=id;
						}
					}
				} else {
					librpip_error_code=0x400;						//module is running, pins are is use
					librpip_error_data=id;
				}
				break;
			case 1:
				librpip_pwm_pin[1]=librpip_pins_getpins(LIBRPIP_FEATURE_PWM1);
				librpip_pins_used |= librpip_pwm_pin[1];
				if(librpip_sysfs_node_exists(LIBRPIP_SYSFS_PWM1_ID)) {
					if(librpip_flags & LIBRPIP_FLAG_SKIP_PWM1) { //we have been asked not not use PWM1
						librpip_error_code=0x407;					
						librpip_error_data=id;
					} else {
						if(librpip_sysfs_node_file_validate(LIBRPIP_SYSFS_PWM1_ID)) {
							librpip_feature_set |= LIBRPIP_FEATURE_PWM1;  //sysfs files all exist exists and we can RW them
						} else {
							librpip_error_code=0x401;				//device exists but we cant RW it
							librpip_error_data=id;
						}
					}
				} else {
					librpip_error_code=0x400;						//module is running, pins are is use
					librpip_error_data=id;
				}
				break;
			default:
				librpip_error_code=0x402;
				librpip_error_data=id;
				break;
		}
	} else {
		librpip_error_code=0x408;	//module is not running, pins are not in use
		librpip_error_data=id;
	}

	if(!librpip_error_code) {
		if(!(librpip_flags & LIBRPIP_FLAG_NO_RESET)) {
			librpip_pwm_status_set(id, LIBRPIP_PWM_STATUS_OFF);		//disabled
			librpip_pwm_dutycycle_set(id, 0);				//no pulse
			librpip_pwm_period_set(id, 20000);   				//20ms
			librpip_pwm_flags_set(id, LIBRPIP_PWM_FLAG_POLARITY_NORMAL);
		}
		//configure the servo in all cases so using without a config first will work. 
		librpip_servo_config[id][LIBRPIP_SERVO_RANGE]=LIBRPIP_SERVO_RANGE_DEFAULT;
		librpip_servo_config[id][LIBRPIP_SERVO_PMIN]=LIBRPIP_SERVO_PMIN_DEFAULT;
		librpip_servo_config[id][LIBRPIP_SERVO_PMAX]=LIBRPIP_SERVO_PMAX_DEFAULT;
	}
	
	return librpip_error_code ? 0 : 1;
}

//exposed functions

uint32_t librpipPwmConfigRead(uint32_t id, uint32_t* pin, uint32_t* period, uint32_t* duty_cycle, uint32_t* flags) {

	librpip_error_code=0;
	librpip_error_data=0;
	
	*pin=0;
	*period=0;
	*duty_cycle=0;
	*flags=0;

	if(librpip_pwm_validate_id(id)) {
		switch(id) {
			case 0:
			case 1:
				*pin=librpip_pwm_pin[id];
				librpip_pwm_period_get(id, period);
				librpip_pwm_dutycycle_get(id, duty_cycle);
				librpip_pwm_flags_get(id, flags);
				break;
			default:
				librpip_error_code=0x402;
				librpip_error_code=id;
		}
	} else {
		librpip_error_code=0x406;
		librpip_error_code=id;
	}
	
	if(librpip_error_code) {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();	
		return 0;
	} else { 
		return 1;
	}
}


uint32_t librpipPwmConfigWrite(uint32_t id, uint32_t period, uint32_t duty_cycle, uint32_t flags) {
	
	librpip_error_code=0;
	librpip_error_data=0;
	
	if(librpip_pwm_validate_id(id)) {
		switch(id) {
			case 0:
			case 1:

				if(period < 109 || period > 2000000000) {
					librpip_error_code=0x420;
					librpip_error_data=id;
					librpip_error_extra=period;
				} 
				if(duty_cycle > period) {
					librpip_error_code=0x421;
					librpip_error_data=id;
					librpip_error_extra=duty_cycle;
				} 
				if(flags != (flags & LIBRPIP_PWM_FLAG_MASK) ) {
					librpip_error_code=0x422;
					librpip_error_data=id;
					librpip_error_extra=flags;
				} 
				if((flags & LIBRPIP_PWM_FLAG_MASK) == (LIBRPIP_PWM_FLAG_POLARITY_INVERTED | LIBRPIP_PWM_FLAG_POLARITY_NORMAL)) {
					librpip_error_code=0x423;
					librpip_error_data=id;
				} 
				break;
			default:
				librpip_error_code=0x402;
				librpip_error_data=id;
				break;
			}
			
	} else {
		librpip_error_code=0x406;
		librpip_error_data=id;
	}

	if(!librpip_error_code) {
		librpip_pwm_period_set(id, period);
		librpip_pwm_dutycycle_set(id, duty_cycle);
		if(flags) librpip_pwm_flags_set(id, flags);
	}

	if(librpip_error_code) {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();	
		return 0;
	} else { 
		return 1;
	}
}

uint32_t librpipPwmStatusRead(uint32_t id, uint32_t* status) {
	librpip_error_code=0;
	librpip_error_data=0;
	
	*status=0;

	if(librpip_pwm_validate_id(id)) {
		switch(id) {
			case 0:
			case 1:
				librpip_pwm_status_get(id, status);
				break;
			default:
				librpip_error_code=0x402;
				librpip_error_data=id;
				break;
		}
	} else {
		librpip_error_code=0x406;
		librpip_error_data=id;
	}
	
	if(librpip_error_code) {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();	
		return 0;
	} else { 
		return 1;
	}
}

uint32_t librpipPwmStatusWrite(uint32_t id, uint32_t status) {

	librpip_error_code=0;
	librpip_error_data=0;
	
	if(librpip_pwm_validate_id(id)) {
		switch(id) {
			case 0:
			case 1:
				if(status==LIBRPIP_PWM_STATUS_OFF) librpip_pwm_status_set(id,LIBRPIP_PWM_STATUS_OFF);
				if(status==LIBRPIP_PWM_STATUS_ON) librpip_pwm_status_set(id,LIBRPIP_PWM_STATUS_ON);
				break;
			default:
				librpip_error_code=0x402;
				librpip_error_data=id;
		}
	} else {
		librpip_error_code=0x406;
		librpip_error_data=id;
	}

	if(librpip_error_code) {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();	
		return 0;
	} else { 
		return 1;
	}
}

uint32_t librpipPwmDutyPercentWrite(uint32_t id, float duty_cycle) {

	librpip_error_code=0;
	librpip_error_data=0;
	
	uint32_t period,dc;
	
	if(librpip_pwm_validate_id(id)) {
		switch(id) {
			case 0:
			case 1:

				if(duty_cycle < 0.0 || duty_cycle > 100.0) {
					librpip_error_code=0x424;
					librpip_error_data=id;
					librpip_error_extraf=duty_cycle;
				} 
				break;
			default:
				librpip_error_code=0x402;
				librpip_error_data=id;
				break;
			}
			
	} else {
		librpip_error_code=0x406;
		librpip_error_data=id;
	}

	if(!librpip_error_code) {
		librpip_pwm_period_get(id, &period);
		dc = (uint32_t)((float)period * (duty_cycle / 100.0));
		librpip_pwm_dutycycle_set(id, dc);
	}

	if(librpip_error_code) {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();	
		return 0;
	} else { 
		return 1;
	}
}


uint32_t librpipServoConfigRead(uint32_t id, uint32_t* range, uint32_t* pmin, uint32_t* pmax) {
	librpip_error_code=0;
	librpip_error_data=0;
	
	if(librpip_pwm_validate_id(id)) {
		switch(id) {
			case 0:
			case 1:
				*range=librpip_servo_config[id][LIBRPIP_SERVO_RANGE];
				*pmin=librpip_servo_config[id][LIBRPIP_SERVO_PMIN];
				*pmax=librpip_servo_config[id][LIBRPIP_SERVO_PMAX];
				break;
			default:
				librpip_error_code=0x402;
				librpip_error_data=id;
		}
	} else {
		librpip_error_code=0x406;
		librpip_error_data=id;
	}
	if(librpip_error_code) {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();	
		return 0;
	} else { 
		return 1;
	}
}

uint32_t librpipServoConfigWrite(uint32_t id, uint32_t range, uint32_t pmin, uint32_t pmax) {
	librpip_error_code=0;
	librpip_error_data=0;
	
	if(librpip_pwm_validate_id(id)) {
		switch(id) {
			case 0:
			case 1:
				if(range < 0 || range > 720) {  
					librpip_error_code=0x426;
					librpip_error_data=id;
					librpip_error_extra=range;
				} 
				if(pmin < 0 || pmin > 20000) {
					librpip_error_code=0x427;
					librpip_error_data=id;
					librpip_error_extra=pmin;
				} 
				if(pmax < 0 || pmax > 20000) {
					librpip_error_code=0x428;
					librpip_error_data=id;
					librpip_error_extra=pmax;
				} 
				if(pmin > pmax) {
					librpip_error_code=0x429;
					librpip_error_data=id;
					librpip_error_extra=pmin;
					librpip_error_extraf=(float)pmax;
				} 
				break;
			default:
				librpip_error_code=0x402;
				librpip_error_data=id;
		}
	} else {
		librpip_error_code=0x406;
		librpip_error_data=id;
	}
	if(!librpip_error_code) {
		librpip_servo_config[id][LIBRPIP_SERVO_RANGE]=range;
		librpip_servo_config[id][LIBRPIP_SERVO_PMIN]=pmin;
		librpip_servo_config[id][LIBRPIP_SERVO_PMAX]=pmax;
	}
	if(librpip_error_code) {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();	
		return 0;
	} else { 
		return 1;
	}
}


uint32_t librpipServoPositionWrite(uint32_t id, float angle) {
	librpip_error_code=0;
	librpip_error_data=0;
	
	uint32_t ns;
	float us_per_degree, us, a, aa;
	
	a=(float)librpip_servo_config[id][LIBRPIP_SERVO_RANGE]/2.0;
	
	if(librpip_pwm_validate_id(id)) {
		switch(id) {
			case 0:
			case 1:
				if(angle < -a || angle > a) {
					librpip_error_code=0x425;
					librpip_error_data=id;
					librpip_error_extra=librpip_servo_config[id][LIBRPIP_SERVO_RANGE];
					librpip_error_extraf=angle;
				} 
				break;
			default:
				librpip_error_code=0x402;
				librpip_error_data=id;
				break;
			}
			
	} else {
		librpip_error_code=0x406;
		librpip_error_data=id;
	}

	if(!librpip_error_code) {

		us_per_degree = ((float)librpip_servo_config[id][LIBRPIP_SERVO_PMAX] - (float)librpip_servo_config[id][LIBRPIP_SERVO_PMIN]) / (float)librpip_servo_config[id][LIBRPIP_SERVO_RANGE];
		aa = angle+a; //shift the angle so zero degrees is pmin instead of centre
		us = (aa*us_per_degree) + (float)librpip_servo_config[id][LIBRPIP_SERVO_PMIN]; // calculate us as a float so we can scale to ps without losing digits - seriously does any servo actually respond to 4 sig figs?
		ns = (uint32_t)(us*1000); //convert to ns then unit32.

		librpip_pwm_period_set(id, 20000000); //20ms	
		librpip_pwm_dutycycle_set(id, ns);
	}

	if(librpip_error_code) {
		if(librpip_flags & LIBRPIP_FLAG_DEBUG_ON) librpipErrorPrint();	
		return 0;
	} else { 
		return 1;
	}
}



// internal functions


uint32_t librpip_pwm_validate_id(uint32_t id) {
	switch(id) {
		case 0:
			if(!(librpip_feature_set & LIBRPIP_FEATURE_PWM0)) {
				librpip_error_code=0x406;
				librpip_error_data=id;
			}
			break;
		case 1:
			if(!(librpip_feature_set & LIBRPIP_FEATURE_PWM1)) {
				librpip_error_code=0x406;
				librpip_error_data=id;
			}		
			break; 
		default:
			librpip_error_code=0x402;
			librpip_error_data=id;
		break;
	}
	return librpip_error_code ? 0 : 1;
}

uint32_t librpip_pwm_get_sysfs_id(uint32_t id) {
	switch(id) {
		case 0:
			return LIBRPIP_SYSFS_PWM0_ID;
		case 1:
			return LIBRPIP_SYSFS_PWM1_ID;		
	}
	return 0;
}

uint32_t librpip_pwm_dutycycle_get(uint32_t id, uint32_t* duty_cycle) {

	int temp;

	if(librpip_sysfs_node_file_get_int(librpip_pwm_get_sysfs_id(id), LIBRPIP_SYSFS_PWM_DUTYCYCLE_ID, &temp)) {
		*duty_cycle=temp;
	} else {
		librpip_error_code=0x410;
		librpip_error_data=id;
		librpip_error_extra=errno;
	}
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_pwm_period_get(uint32_t id, uint32_t* period) {

	int temp;

	if(librpip_sysfs_node_file_get_int(librpip_pwm_get_sysfs_id(id), LIBRPIP_SYSFS_PWM_PERIOD_ID, &temp)) {
		*period=temp;
	} else {
		librpip_error_code=0x411;
		librpip_error_data=id;
		librpip_error_extra=errno;
	}
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_pwm_status_get(uint32_t id, uint32_t* status) {

	int temp;

	if(librpip_sysfs_node_file_get_int(librpip_pwm_get_sysfs_id(id), LIBRPIP_SYSFS_PWM_ENABLE_ID, &temp)) {
		*status=temp;
	} else {
		librpip_error_code=0x412;
		librpip_error_data=id;
		librpip_error_extra=errno;
	}
	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_pwm_flags_get(uint32_t id, uint32_t* flags) {

	char buf[20];
	
	if(librpip_sysfs_node_file_get_str(librpip_pwm_get_sysfs_id(id), LIBRPIP_SYSFS_PWM_POLARITY_ID, buf, sizeof(buf))) {
		if(!strncasecmp(buf,"normal",6)) *flags |= LIBRPIP_PWM_FLAG_POLARITY_NORMAL;
		if(!strncasecmp(buf,"inverted",8)) *flags |= LIBRPIP_PWM_FLAG_POLARITY_INVERTED;
	} else {
		librpip_error_code=0x413;
		librpip_error_data=id;
		librpip_error_extra=errno;
	}
	return librpip_error_code  ? 0 : 1;

}

uint32_t librpip_pwm_dutycycle_set(uint32_t id, uint32_t duty_cycle) {

	if(!librpip_sysfs_node_file_set_int(librpip_pwm_get_sysfs_id(id), LIBRPIP_SYSFS_PWM_DUTYCYCLE_ID, duty_cycle)) {
		librpip_error_code=0x430;
		librpip_error_data=id;
		librpip_error_extra=errno;
	}

	return librpip_error_code  ? 0 : 1;

}

uint32_t librpip_pwm_period_set(uint32_t id, uint32_t period) {

	if(!librpip_sysfs_node_file_set_int(librpip_pwm_get_sysfs_id(id), LIBRPIP_SYSFS_PWM_PERIOD_ID, period)) {
		librpip_error_code=0x431;
		librpip_error_data=id;
		librpip_error_extra=errno;
	}

	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_pwm_status_set(uint32_t id, uint32_t status) {

	if(!librpip_sysfs_node_file_set_int(librpip_pwm_get_sysfs_id(id), LIBRPIP_SYSFS_PWM_ENABLE_ID, status)) {
		librpip_error_code=0x432;
		librpip_error_data=id;
		librpip_error_extra=errno;
	}

	return librpip_error_code  ? 0 : 1;
}

uint32_t librpip_pwm_flags_set(uint32_t id, uint32_t flags) {

	char buf[20];
	if(flags & LIBRPIP_PWM_FLAG_POLARITY_NORMAL) sprintf(buf,"normal");
	if(flags & LIBRPIP_PWM_FLAG_POLARITY_INVERTED) sprintf(buf,"inverted");
	
	if(!librpip_sysfs_node_file_set_str(librpip_pwm_get_sysfs_id(id), LIBRPIP_SYSFS_PWM_POLARITY_ID, buf, sizeof(buf))) {
		librpip_error_code=0x432;
		librpip_error_data=id;
		librpip_error_extra=errno;
	}

	return librpip_error_code  ? 0 : 1;
}


