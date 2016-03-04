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
#include "error.h"
#include "device.h"


extern uint32_t librpip_error_code;
extern uint32_t librpip_error_data;
extern uint32_t librpip_error_extra;
extern float 	librpip_error_extraf;

uint32_t librpipErrorGetCode(void) {
	return librpip_error_code;
}

void librpipErrorPrint(void) {

	char desc[300]={0};
	if(librpipErrorGetDescription(&desc[0], sizeof(desc))) {
		switch(librpipErrorGetSeverity()) {
			case LIBRPIP_ERROR_MSG_INFO:
				fprintf(stderr,"[librpip INFO   ] %s\n",desc);
				break;
			case LIBRPIP_ERROR_MSG_WARNING:
				fprintf(stderr,"[librpip WARNING] %s\n",desc);
				break;	
			case LIBRPIP_ERROR_MSG_ERROR:
				fprintf(stderr,"[librpip ERROR  ] %s\n",desc);
				break;					
		}
	}

}

uint32_t librpipErrorGetSeverity(void) {
	
	switch(librpip_error_code) {
		case 0x105:
		case 0x205:
		case 0x207:
		case 0x304:
		case 0x305:
		case 0x407:
		case 0x408:
		case 0x603:
		case 0x604:
		case 0x608:						
			return LIBRPIP_ERROR_MSG_INFO;			
			break;
		case 0x100:
		case 0x104:
		case 0x110:
		case 0x111:
		case 0x112:
		case 0x113:
		case 0x120:
		case 0x200:
		case 0x204:
		case 0x206:
		case 0x208:
		case 0x225:
		case 0x227:
		case 0x228:
		case 0x300:
		case 0x303:
		case 0x320:
		case 0x321:
		case 0x322:
		case 0x323:
		case 0x324:
		case 0x400:
		case 0x404:
		case 0x405:
		case 0x406:
		case 0x420:
		case 0x421:
		case 0x422:
		case 0x423:
		case 0x424:	
		case 0x425:
		case 0x426:	
		case 0x427:	
		case 0x428:
		case 0x429:
		case 0x500:
		case 0x501:
		case 0x510:
		case 0x511:
		case 0x512:
		case 0x513:
		case 0x514:
		case 0x515:
		case 0x516:
		case 0x517:
		case 0x520:
		case 0x521:
		case 0x530:
		case 0x531:
		case 0x532:
		case 0x533:
		case 0x540:
		case 0x600:
		case 0x601:
		case 0x605:
		case 0x606:
		case 0x607:
		case 0x620:
		case 0x621:
		case 0x622:
		case 0x623:	
		case 0x624:
		case 0x631:
		case 0x632:
		case 0x641:																
			return LIBRPIP_ERROR_MSG_WARNING;					
			break;
		case 0x001:
		case 0x010:
		case 0x011:
		case 0x012:
		case 0x101:
		case 0x102:
		case 0x103:
		case 0x114:
		case 0x201:
		case 0x202:
		case 0x203:
		case 0x210:
		case 0x211:
		case 0x212:
		case 0x213:
		case 0x214:
		case 0x220:
		case 0x221:
		case 0x222:
		case 0x223:
		case 0x224:
		case 0x230:
		case 0x301:
		case 0x302:
		case 0x311:
		case 0x330:
		case 0x340:
		case 0x341:	
		case 0x342:
		case 0x350:
		case 0x401:
		case 0x402:
		case 0x403:
		case 0x410:
		case 0x411:
		case 0x412:
		case 0x413:
		case 0x430:
		case 0x431:
		case 0x432:
		case 0x433:
		case 0x602:
		case 0x610:
		case 0x611:	
		case 0x612:
		case 0x630:	
		case 0x640:		
		default:										
			return LIBRPIP_ERROR_MSG_ERROR;			
			break;
			
	}
}

uint32_t librpipErrorGetDescription(char* desc, int len) {
	
	switch(librpip_error_code) {
	
// init
		case 0x001:
			snprintf(desc, len, "Aborting due to elevated privileges. The whole point of librpip is that elevated privileges are not required. Try again using a standard user account.");
			break;
		case 0x010:
			snprintf(desc, len, "Cannot find /proc/cpuinfo to detect board model/revision. Board must be set manually.");
			break;
		case 0x011:
			snprintf(desc, len, "Cannot read /proc/cpuinfo to detect board model/revision. Board must be set manually.");
			break;
		case 0x012:
			snprintf(desc, len, "Unknown board revision 0x%x. (Got a top secret prototype?). Board must be set manually.", librpip_error_data);
			break;

//GPIO	
		case 0x100:
			snprintf(desc, len, "No GPIO functions available. /dev/gpiomem does not exist (kernel module not loaded?).");
			break;
		case 0x101:
			snprintf(desc, len, "No GPIO functions available. Unable to open /dev/gpiomem for writing (user permissions?).");
			break;
		case 0x102:
			snprintf(desc, len, "No GPIO functions available. Unable to map to GPIO memory registers.");
			break;
		case 0x103:
			snprintf(desc, len, "No GPIO functions available. Unable to map GPIO pins for board %u.", librpip_error_data);
			break;
		case 0x104:
			snprintf(desc, len, "The pins 0x%x are configured for an unknown alternate function, removing them from valid pin list.", librpip_error_data);
			break;
		case 0x105:
			snprintf(desc, len, "No GPIO functions available. Skipping initialisation as instructed.");
			break;
		case 0x110:
			snprintf(desc, len, "Ignoring request. No GPIO functions available as it was not initialised.");
			break;
		case 0x111:
			snprintf(desc, len, "Ignoring request. %u is not a valid pin. Only pins from bank 0 (0-31) are valid.", librpip_error_data);
			break;
		case 0x112:
			snprintf(desc, len, "Ignoring request. Pin %u is not available to the GPIO.", librpip_error_data);
			break;	
		case 0x113:
			snprintf(desc, len, "Ignoring request. Pin %u is not configured for output.", librpip_error_data);
			break;	
		case 0x114:
			snprintf(desc, len, "Pin #%u is configured for an unexpected alternate function 0x%x!", librpip_error_data, librpip_error_extra);
			break;	
		case 0x120:
			snprintf(desc, len, "Timeout waiting for event on Pin #%u.", librpip_error_data);
			break;				

			
// SPI			
		case 0x200:
			snprintf(desc, len, "No SPI%u functions available. /dev/spidev%u.x does not exist (spidev blacklisted?). Pins NOT available to GPIO", librpip_error_data, librpip_error_data);
			break;	
		case 0x201:
			snprintf(desc, len, "No SPI%u functions available. Unable to open /dev/spidev%u.x for writing (user permissions?). Pins NOT available to GPIO", librpip_error_data, librpip_error_data);
			break;
		case 0x202:
			snprintf(desc, len, "Invalid SPI device number %u.", librpip_error_data);
			break;	
		case 0x203:
			snprintf(desc, len, "Invalid CS number %u for SPI%u.", librpip_error_extra, librpip_error_data);
			break;	
		case 0x204:
			snprintf(desc, len, "Ignoring request. No SPI%u functions available as it was not initialised.", librpip_error_data);
			break;
		case 0x205:
			snprintf(desc, len, "No SPI%u functions available. Skipping initialisation as instructed. Pins NOT available to GPIO", librpip_error_data);
			break;		
		case 0x206:
			snprintf(desc, len, "Ignoring request. 3Wire mode is not supported on SPI%u.", librpip_error_data);
			break;	
		case 0x207:
			snprintf(desc, len, "No SPI%u functions available. SPI%u module is not enabled. Consult /boot/overlays/README. Pins are available to GPIO", librpip_error_data , librpip_error_data);
			break;
		case 0x208:
			snprintf(desc, len, "Ignoring request. No SPI%u functions available on CS%u as it was not initialised.", librpip_error_data, librpip_error_extra);
			break;									
		case 0x210:
			snprintf(desc, len, "Unable to read SPI%u Mode (SPI_IOC_RD_MODE). Error 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra) );
			break;				
		case 0x211:
			snprintf(desc, len, "Unable to read SPI%u LSB First (SPI_IOC_RD_LSB_FIRST). Error 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x212:
			snprintf(desc, len, "Unable to read SPI%u Bits per Word (SPI_IOC_RD_BITS_PER_WORD). Error 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x213:
			snprintf(desc, len, "Unable to read SPI%u Max Speed (SPI_IOC_RD_MAX_SPEED_HZ). Error 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x214:
			snprintf(desc, len, "Unable to read SPI%u Mode Flags (SPI_IOC_RD_MODE32). Error 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x220:
			snprintf(desc, len, "Unable to write SPI%u Mode (SPI_IOC_WR_MODE). Error 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;				
		case 0x221:
			snprintf(desc, len, "Unable to write SPI%u LSB First (SPI_IOC_WR_LSB_FIRST). Error 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x222:
			snprintf(desc, len, "Unable to write SPI%u Bits per Word (SPI_IOC_WR_BITS_PER_WORD). Error 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x223:
			snprintf(desc, len, "Unable to write SPI%u Max Speed (SPI_IOC_WR_MAX_SPEED_HZ). Error 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x224:
			snprintf(desc, len, "Unable to write SPI%u Mode Flags (SPI_IOC_WR_MODE32). Error 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x225:
			snprintf(desc, len, "Ignoring SPI%u config request. Some flags submitted (0x%x) are unsupported by the kernel module. Supported flags are: SPI_CS_HIGH, SPI_NO_CS.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x227:
			snprintf(desc, len, "Ignoring SPI%u config request. Invalid mode requested 0x%x.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x228:
			snprintf(desc, len, "Ignoring SPI%u config request. Invalid Speed requested %u. Supported speeds are 8kHz to 125mHz.", librpip_error_data, librpip_error_extra);
			break;		
		case 0x230:
			snprintf(desc, len, "Failure transmitting on SPI%u. Reported Error was 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	

//I2C			
		case 0x300:
			snprintf(desc, len, "No I2C%u functions available. /dev/i2c-%u does not exist (i2c-dev not loaded?). Pins NOT available to GPIO", librpip_error_data, librpip_error_data);
			break;	
		case 0x301:
			snprintf(desc, len, "No I2C%u functions available. Unable to open /dev/i2c-%u for writing (user permissions?). Pins NOT available to GPIO", librpip_error_data, librpip_error_data);
			break;
		case 0x302:
			snprintf(desc, len, "Invalid I2C device number %u.", librpip_error_data);
			break;	
		case 0x303:
			snprintf(desc, len, "Ignoring request. No I2C%u functions available as it was not initialised.", librpip_error_data);
			break;
		case 0x304:
			snprintf(desc, len, "No I2C%u functions available. Skipping initialisation as instructed. Pins NOT available to GPIO", librpip_error_data);
			break;	
		case 0x305:
			snprintf(desc, len, "No I2C%u functions available. I2C%u module is not enabled. Consult /boot/overlays/README. Pins are available to GPIO", librpip_error_data , librpip_error_data);
			break;			
		case 0x311:
			snprintf(desc, len, "No I2C%u functions available. Device /dev/i2c-%u does not support I2C Functions. Pins NOT available to GPIO", librpip_error_data, librpip_error_data);
			break;	
		case 0x320:
			snprintf(desc, len, "Ignoring request on /dev/i2c-%u. Some flags submitted (0x%x) are not valid.", librpip_error_data, librpip_error_extra);
			break;
		case 0x321:
			snprintf(desc, len, "Ignoring request on /dev/i2c-%u. PEC is not supported on this device.", librpip_error_data);
			break;
		case 0x322:
			snprintf(desc, len, "Ignoring request on /dev/i2c-%u. 10bit is not supported on this device", librpip_error_data);
			break;
		case 0x323:
			snprintf(desc, len, "Ignoring request on /dev/i2c-%u. Slaveid %u is not a valid 10bit id.", librpip_error_data, librpip_error_extra);
			break;
		case 0x324:
			snprintf(desc, len, "Ignoring request on /dev/i2c-%u. Slaveid %u is not a valid 7bit id.", librpip_error_data, librpip_error_extra);
			break;
		case 0x330:
			snprintf(desc, len, "No I2C%u functions available. Unable to read device functionality. Pins NOT available to GPI. Error was 0x%x (%s).", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x340:
			snprintf(desc, len, "Unable to set slave ID on /dev/i2c-%u. Error was 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x341:
			snprintf(desc, len, "Unable to set 10Bit flag on /dev/i2c-%u. Error was 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x342:
			snprintf(desc, len, "Unable to set PEC flag on /dev/i2c-%u. Error was 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;		
		case 0x350:
			snprintf(desc, len, "Unable to send transaction on /dev/i2c-%u. Error was 0x%x (%s)", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
			
			
//PWM			
		case 0x400:
			snprintf(desc, len, "No PWM%u functions available. /sys/class/pwm/pwmchip0/pwm%u/* does not exist (PWM init script not run?). Pins NOT available to GPIO", librpip_error_data, librpip_error_data);
			break;	
		case 0x401:
			snprintf(desc, len, "No PWM%u functions available. Unable to open /sys/class/pwm/pwmchip0/pwm%u/* for writing (user permissions?). Pins NOT available to GPIO", librpip_error_data, librpip_error_data);
			break;
		case 0x402:
			snprintf(desc, len, "Invalid PWM device number %u.", librpip_error_data);
			break;	
		case 0x403:
			snprintf(desc, len, "No PWM%u functions available. Unable to map GPIO pins for board. Pins NOT available to GPIO", librpip_error_data);
			break;	
		case 0x404:
			snprintf(desc, len, "No possible pins available to perform discovery on for PWM%u. Please consult config.txt for Pin Allocation.", librpip_error_data);
			break;	
		case 0x405:
			snprintf(desc, len, "Discovery did not locate an assigned pin for PWM%u. Please consult config.txt for Pin Allocation.", librpip_error_data);
			break;	
		case 0x406:
			snprintf(desc, len, "Ignoring request. No PWM%u functions available as it was not initialised.", librpip_error_data);
			break;
		case 0x407:
			snprintf(desc, len, "No PWM%u functions available. Skipping initialisation as instructed. Pins NOT available to GPIO", librpip_error_data);
			break;
		case 0x408:
			snprintf(desc, len, "No PWM%u functions available. PWM module is not enabled. Consult /boot/overlays/README. Pins are available to GPIO", librpip_error_data);
			break;
		case 0x410:
			snprintf(desc, len, "Error reading config for PWM%u. Opening file /sys/class/pwm/pwmchip0/pwm%u/duty_cycle caused errno 0x%x (%s).", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x411:
			snprintf(desc, len, "Error reading config for PWM%u. Opening file /sys/class/pwm/pwmchip0/pwm%u/period caused errno 0x%x (%s).", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x412:
			snprintf(desc, len, "Error reading config for PWM%u. Opening file /sys/class/pwm/pwmchip0/pwm%u/enable caused errno 0x%x (%s).", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x413:
			snprintf(desc, len, "Error reading config for PWM%u. Opening file /sys/class/pwm/pwmchip0/pwm%u/polarity caused errno 0x%x (%s).", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x420:
			snprintf(desc, len, "Ignoring request to configure PWM%u. The supplied period (%u) was not in the expected range of 109ns to 2s.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x421:
			snprintf(desc, len, "Ignoring request to configure PWM%u. The supplied duty cycle (%u) was greater than the period.", librpip_error_data, librpip_error_extra);
			break;
		case 0x422:
			snprintf(desc, len, "Ignoring request to configure PWM%u. The supplied flags (0x%x) contained unknown flags.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x423:
			snprintf(desc, len, "Ignoring request to configure PWM%u. It is not possible to have the polarity normal and inverted at the same time.", librpip_error_data);
			break;	
		case 0x424:
			snprintf(desc, len, "Ignoring request to configure PWM%u. The supplied duty cycle %f was not a percentage between 0 and 100.", librpip_error_data, librpip_error_extraf);
			break;	
		case 0x425:
			snprintf(desc, len, "Ignoring request to set a servo on PWM%u. The supplied angle %f was outside of the configured range %u.", librpip_error_data, librpip_error_extraf, librpip_error_extra);
			break;	
		case 0x426:
			snprintf(desc, len, "Ignoring request to set a servo on PWM%u. The supplied range (%u) is outside of the 0-720deg range.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x427:
			snprintf(desc, len, "Ignoring request to set a servo on PWM%u. The supplied min pulse (%u) is outside of the 0-20ms range.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x428:
			snprintf(desc, len, "Ignoring request to set a servo on PWM%u. The supplied max pulse (%u) is outside of the 0-20ms range.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x429:
			snprintf(desc, len, "Ignoring request to set a servo on PWM%u. The supplied min pulse (%u) is greater than the max pulse (%.0f).", librpip_error_data, librpip_error_extra, librpip_error_extraf);
			break;
		case 0x430:
			snprintf(desc, len, "Error writing config for PWM%u. Accessing file /sys/class/pwm/pwmchip0/pwm%u/duty_cycle caused errno 0x%x (%s).", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x431:
			snprintf(desc, len, "Error writing config for PWM%u. Accessing file /sys/class/pwm/pwmchip0/pwm%u/period caused errno 0x%x (%s).", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x432:
			snprintf(desc, len, "Error writing config for PWM%u. Accessing file /sys/class/pwm/pwmchip0/pwm%u/enable caused errno 0x%x (%s).", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x433:
			snprintf(desc, len, "Error writing config for PWM%u. Accessing file /sys/class/pwm/pwmchip0/pwm%u/polarity caused errno 0x%x (%s).", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	

//transactions			
		case 0x500:
			snprintf(desc, len, "Ignoring request to create a transaction. Mode of %u is not valid.", librpip_error_data);
			break;	
		case 0x501:
			snprintf(desc, len, "Ignoring request to create a transaction. Bits per word of %u is not valid.", librpip_error_data);
			break;	
		case 0x510:
			snprintf(desc, len, "Ignoring request to add a message segment. Direction of %u is not valid.", librpip_error_data);
			break;	
		case 0x511:
			snprintf(desc, len, "Ignoring request to add a message segment. Length of %u is not valid.", librpip_error_data);
			break;	
		case 0x512:
			snprintf(desc, len, "Ignoring request to add a message segment, this transaction has already been transmitted.");
			break;	
		case 0x513:
			snprintf(desc, len, "Ignoring request to add a message segment, combined read/write messages are only valid on SPI.");
			break;	
		case 0x514:
			snprintf(desc, len, "Ignoring request to add a message segment, SPI only supports combined read/write messages.");
			break;	
		case 0x515:
			snprintf(desc, len, "Ignoring request to add a message segment, I2C max message length (%u) reached..", librpip_error_data);
			break;
		case 0x516:
			snprintf(desc, len, "Ignoring request to set message variable '%c'. Length of %u is not valid.", (char)librpip_error_extra, librpip_error_data);
			break;	
		case 0x517:
			snprintf(desc, len, "Ignoring request to  add a variable message segment. Variable name is not valid.");
			break;
		case 0x520:
			snprintf(desc, len, "Ignoring request to transmit a transaction that has no messages.");
			break;	
		case 0x521:
			snprintf(desc, len, "Ignoring request to transmit a transaction that has unset variable segments.");
			break;
		case 0x530:
			snprintf(desc, len, "Ignoring request to read from a transaction that has not been initialised.");
			break;
		case 0x531:
			snprintf(desc, len, "No data to return. Could not find another message segment to read from in the transaction.");
			break;
		case 0x532:
			snprintf(desc, len, "No data to return. Could not find a message segment to read from in the transaction.");
			break;
		case 0x533:
			snprintf(desc, len, "Ignoring request to read from a transaction that has not been sent. Status is %u", librpip_error_data);
			break;
		case 0x540:
			snprintf(desc, len, "Ignoring request to destroy transaction. It has not been initialised.");
			break;

//Uarts			
		case 0x600:
			snprintf(desc, len, "No UART%u functions available. /dev/ttyAMA0 does not exist.", librpip_error_data);
			break;	
		case 0x601:
			snprintf(desc, len, "No UART%u functions available. Unable to open /dev/ttyAMA0 for writing (user permissions or in use by console?). Pins NOT available to GPIO", librpip_error_data);
			break;
		case 0x602:
			snprintf(desc, len, "Invalid UART device number %u.", librpip_error_data);
			break;	
		case 0x603:
			snprintf(desc, len, "No UART%u functions available. Skipping initialisation as instructed. Pins NOT available to GPIO", librpip_error_data);
			break;	
		case 0x604:
			snprintf(desc, len, "Neither UART device node exists. Pins are available to GPIO");
			break;	
		case 0x605:
			snprintf(desc, len, "Ignoring request. No UART%u functions available as it was not initialised.", librpip_error_data);
			break;
		case 0x606:
			snprintf(desc, len, "No UART%u functions available. /dev/ttyS0 does not exist.", librpip_error_data);
			break;	
		case 0x607:
			snprintf(desc, len, "No UART%u functions available. Unable to open /dev/ttyS0 for writing (user permissions or in use by console?). Pins NOT available to GPIO", librpip_error_data);
			break;
		case 0x608:
			snprintf(desc, len, "No UART%u functions available. UART%u module is not enabled. Consult /boot/overlays/README.", librpip_error_data, librpip_error_data);
			break;							
		case 0x610:
			snprintf(desc, len, "Unable to read configuration of UART%u. The Error was 0x%x (%s).", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x611:
			snprintf(desc, len, "Unable to flush UART%u.The Error was 0x%x (%s).", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x612:
			snprintf(desc, len, "Unable to set configuration of UART%u. The Error was 0x%x (%s).", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;		
		case 0x620:
			snprintf(desc, len, "Ignoring request to configure UART%u. The supplied baud %u is not valid.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x621:
			snprintf(desc, len, "Ignoring request to configure UART%u. The supplied mode %u is not valid.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x622:
			snprintf(desc, len, "Ignoring request to configure UART%u. The supplied character bits size %u is not valid.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x623:
			snprintf(desc, len, "Ignoring request to configure UART%u. The supplied parity %u is not valid.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x624:
			snprintf(desc, len, "Ignoring request to configure UART%u. The supplied stopbits %u is not valid.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x630:
			snprintf(desc, len, "Error writing to UART%u. The error was 0x%x (%s).", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x631:
			snprintf(desc, len, "Incomplete transfer on UART%u. The returned length transmitted %u is less than the length attempted.", librpip_error_data, librpip_error_extra);
			break;	
		case 0x632:
			snprintf(desc, len, "Ignoring transfer on UART%u. The length %u is is not valid.", librpip_error_data, librpip_error_extra);
			break;
		case 0x640:
			snprintf(desc, len, "Error reading from UART%u. The error was 0x%x (%s).", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x641:
			snprintf(desc, len, "Timeout waiting for data from UART%u.", librpip_error_data);
			break;	
			
		default:
			snprintf(desc, len, "Unknown Error 0x%x [0x%x 0x%x %f].", librpip_error_code, librpip_error_data, librpip_error_extra, librpip_error_extraf);			
			
	}
	
	return strlen(desc);
}



