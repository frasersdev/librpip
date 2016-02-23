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
	if(librpipErrorGetDescription(&desc[0], sizeof(desc))) 
		fprintf(stderr,"%s",desc);

}

uint32_t librpipErrorGetDescription(char* desc, int len) {
	
	switch(librpip_error_code) {
	
// init
		case 0x001:
			snprintf(desc, len, "[librpip ERROR  ] Aborting due to elevated privileges. The whole point of librpip is that elevated privileges are not required. Try again using a standard user account.\n");
			break;
		case 0x010:
			snprintf(desc, len, "[librpip ERROR  ] Cannot find /proc/cpuinfo to detect board model/revision. Board must be set manually.\n");
			break;
		case 0x011:
			snprintf(desc, len, "[librpip ERROR  ] Cannot read /proc/cpuinfo to detect board model/revision. Board must be set manually.\n");
			break;
		case 0x012:
			snprintf(desc, len, "[librpip ERROR  ] Unknown board revision 0x%x. (Got a top secret prototype?). Board must be set manually.\n", librpip_error_data);
			break;

//GPIO	
		case 0x100:
			snprintf(desc, len, "[librpip WARNING] No GPIO functions available. /dev/gpiomem does not exist (kernel module not loaded?).\n");
			break;
		case 0x101:
			snprintf(desc, len, "[librpip ERROR  ] No GPIO functions available. Unable to open /dev/gpiomem for writing (user permissions?).\n");
			break;
		case 0x102:
			snprintf(desc, len, "[librpip ERROR  ] No GPIO functions available. Unable to map to GPIO memory registers.\n");
			break;
		case 0x103:
			snprintf(desc, len, "[librpip ERROR  ] No GPIO functions available. Unable to map GPIO pins for board %u.\n", librpip_error_data);
			break;
		case 0x104:
			snprintf(desc, len, "[librpip WARNING] The pins 0x%x are configured for an unknown alternate function, removing them from valid pin list.\n", librpip_error_data);
			break;
		case 0x105:
			snprintf(desc, len, "[librpip INFO   ] No GPIO functions available. Skipping initialisation as instructed.\n");
			break;
		case 0x110:
			snprintf(desc, len, "[librpip WARNING] Ignoring request. No GPIO functions available as it was not initialised.\n");
			break;
		case 0x111:
			snprintf(desc, len, "[librpip WARNING] Ignoring request. %u is not a valid pin. Only pins from bank 0 (0-31) are valid.\n", librpip_error_data);
			break;
		case 0x112:
			snprintf(desc, len, "[librpip WARNING] Ignoring request. Pin %u is not available to the GPIO.\n", librpip_error_data);
			break;	
		case 0x113:
			snprintf(desc, len, "[librpip WARNING] Ignoring request. Pin %u is not configured for output.\n", librpip_error_data);
			break;	
		case 0x114:
			snprintf(desc, len, "[librpip ERROR  ] Pin #%u is configured for an unexpected alternate function 0x%x!\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x120:
			snprintf(desc, len, "[librpip WARNING] Timeout waiting for event on Pin #%u.\n", librpip_error_data);
			break;				

			
// SPI			
		case 0x200:
			snprintf(desc, len, "[librpip INFO   ] No SPI%u functions available. /dev/spidev%u.x does not exist (kernel module not loaded?). Pins are available to GPIO\n", librpip_error_data, librpip_error_data);
			break;	
		case 0x201:
			snprintf(desc, len, "[librpip ERROR  ] No SPI%u functions available. Unable to open /dev/spidev%u.x for writing (user permissions?). Pins NOT available to GPIO\n", librpip_error_data, librpip_error_data);
			break;
		case 0x202:
			snprintf(desc, len, "[librpip ERROR  ] Invalid SPI device number %u.\n", librpip_error_data);
			break;	
		case 0x203:
			snprintf(desc, len, "[librpip ERROR  ] Invalid CS number %u for SPI%u.\n", librpip_error_extra, librpip_error_data);
			break;	
		case 0x204:
			snprintf(desc, len, "[librpip WARNING] Ignoring request. No SPI%u functions available as it was not initialised.\n", librpip_error_data);
			break;
		case 0x205:
			snprintf(desc, len, "[librpip INFO   ] No SPI%u functions available. Skipping initialisation as instructed. Pins NOT available to GPIO\n", librpip_error_data);
			break;		
		case 0x206:
			snprintf(desc, len, "[librpip WARNING] Ignoring request. 3Wire mode is not supported on SPI%u.\n", librpip_error_data);
			break;			
		case 0x210:
			snprintf(desc, len, "[librpip ERROR  ] Unable to read SPI%u Mode (SPI_IOC_RD_MODE). Error 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra) );
			break;				
		case 0x211:
			snprintf(desc, len, "[librpip ERROR  ] Unable to read SPI%u LSB First (SPI_IOC_RD_LSB_FIRST). Error 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x212:
			snprintf(desc, len, "[librpip ERROR  ] Unable to read SPI%u Bits per Word (SPI_IOC_RD_BITS_PER_WORD). Error 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x213:
			snprintf(desc, len, "[librpip ERROR  ] Unable to read SPI%u Max Speed (SPI_IOC_RD_MAX_SPEED_HZ). Error 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x214:
			snprintf(desc, len, "[librpip ERROR  ] Unable to read SPI%u Mode Flags (SPI_IOC_RD_MODE32). Error 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x220:
			snprintf(desc, len, "[librpip ERROR  ] Unable to write SPI%u Mode (SPI_IOC_WR_MODE). Error 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;				
		case 0x221:
			snprintf(desc, len, "[librpip ERROR  ] Unable to write SPI%u LSB First (SPI_IOC_WR_LSB_FIRST). Error 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x222:
			snprintf(desc, len, "[librpip ERROR  ] Unable to write SPI%u Bits per Word (SPI_IOC_WR_BITS_PER_WORD). Error 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x223:
			snprintf(desc, len, "[librpip ERROR  ] Unable to write SPI%u Max Speed (SPI_IOC_WR_MAX_SPEED_HZ). Error 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x224:
			snprintf(desc, len, "[librpip ERROR  ] Unable to write SPI%u Mode Flags (SPI_IOC_WR_MODE32). Error 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x225:
			snprintf(desc, len, "[librpip WARNING] Ignoring SPI%u config request. Some flags submitted (0x%x) are unsupported by the kernel module. Supported flags are: SPI_CS_HIGH, SPI_NO_CS.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x227:
			snprintf(desc, len, "[librpip WARNING] Ignoring SPI%u config request. Invalid mode requested 0x%x.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x228:
			snprintf(desc, len, "[librpip WARNING] Ignoring SPI%u config request. Invalid Speed requested %u. Supported speeds are 8kHz to 125mHz.\n", librpip_error_data, librpip_error_extra);
			break;		
		case 0x230:
			snprintf(desc, len, "[librpip ERROR  ] Failure transmitting on SPI%u. Reported Error was 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	

//I2C			
		case 0x300:
			snprintf(desc, len, "[librpip WARNING] No I2C%u functions available. /dev/i2c-%u does not exist (i2c-dev not loaded?). Pins NOT available to GPIO\n", librpip_error_data, librpip_error_data);
			break;	
		case 0x301:
			snprintf(desc, len, "[librpip ERROR  ] No I2C%u functions available. Unable to open /dev/i2c-%u for writing (user permissions?). Pins NOT available to GPIO\n", librpip_error_data, librpip_error_data);
			break;
		case 0x302:
			snprintf(desc, len, "[librpip ERROR  ] Invalid I2C device number %u.\n", librpip_error_data);
			break;	
		case 0x303:
			snprintf(desc, len, "[librpip WARNING] Ignoring request. No I2C%u functions available as it was not initialised.\n", librpip_error_data);
			break;
		case 0x304:
			snprintf(desc, len, "[librpip INFO   ] No I2C%u functions available. Skipping initialisation as instructed. Pins NOT available to GPIO\n", librpip_error_data);
			break;	
		case 0x305:
			snprintf(desc, len, "[librpip INFO   ] No I2C%u functions available. I2C%u module is not enabled. Consult /boot/config.txt. Pins are available to GPIO\n", librpip_error_data , librpip_error_data);
			break;			
		case 0x311:
			snprintf(desc, len, "[librpip ERROR  ] No I2C%u functions available. Device /dev/i2c-%u does not support I2C Functions. Pins NOT available to GPIO\n", librpip_error_data, librpip_error_data);
			break;	
		case 0x320:
			snprintf(desc, len, "[librpip WARNING] Ignoring request on /dev/i2c-%u. Some flags submitted (0x%x) are not valid.\n", librpip_error_data, librpip_error_extra);
			break;
		case 0x321:
			snprintf(desc, len, "[librpip WARNING] Ignoring request on /dev/i2c-%u. PEC is not supported on this device.\n", librpip_error_data);
			break;
		case 0x322:
			snprintf(desc, len, "[librpip WARNING] Ignoring request on /dev/i2c-%u. 10bit is not supported on this device\n", librpip_error_data);
			break;
		case 0x323:
			snprintf(desc, len, "[librpip WARNING] Ignoring request on /dev/i2c-%u. Slaveid %u is not a valid 10bit id.\n", librpip_error_data, librpip_error_extra);
			break;
		case 0x324:
			snprintf(desc, len, "[librpip WARNING] Ignoring request on /dev/i2c-%u. Slaveid %u is not a valid 7bit id.\n", librpip_error_data, librpip_error_extra);
			break;
		case 0x330:
			snprintf(desc, len, "[librpip ERROR  ] No I2C%u functions available. Unable to read device functionality. Pins NOT available to GPI. Error was 0x%x (%s).\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x340:
			snprintf(desc, len, "[librpip ERROR  ] Unable to set slave ID on /dev/i2c-%u. Error was 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x341:
			snprintf(desc, len, "[librpip ERROR  ] Unable to set 10Bit flag on /dev/i2c-%u. Error was 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x342:
			snprintf(desc, len, "[librpip ERROR  ] Unable to set PEC flag on /dev/i2c-%u. Error was 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;		
		case 0x350:
			snprintf(desc, len, "[librpip ERROR  ] Unable to send transaction on /dev/i2c-%u. Error was 0x%x (%s)\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
			
			
//PWM			
		case 0x400:
			snprintf(desc, len, "[librpip WARNING] No PWM%u functions available. /sys/class/pwm/pwmchip0/pwm%u/* does not exist (PWM init script not run?). Pins NOT available to GPIO\n", librpip_error_data, librpip_error_data);
			break;	
		case 0x401:
			snprintf(desc, len, "[librpip ERROR  ] No PWM%u functions available. Unable to open /sys/class/pwm/pwmchip0/pwm%u/* for writing (user permissions?). Pins NOT available to GPIO\n", librpip_error_data, librpip_error_data);
			break;
		case 0x402:
			snprintf(desc, len, "[librpip ERROR  ] Invalid PWM device number %u.\n", librpip_error_data);
			break;	
		case 0x403:
			snprintf(desc, len, "[librpip ERROR  ] No PWM%u functions available. Unable to map GPIO pins for board. Pins NOT available to GPIO\n", librpip_error_data);
			break;	
		case 0x404:
			snprintf(desc, len, "[librpip WARNING] No possible pins available to perform discovery on for PWM%u. Please consult config.txt for Pin Allocation.\n", librpip_error_data);
			break;	
		case 0x405:
			snprintf(desc, len, "[librpip WARNING] Discovery did not locate an assigned pin for PWM%u. Please consult config.txt for Pin Allocation.\n", librpip_error_data);
			break;	
		case 0x406:
			snprintf(desc, len, "[librpip WARNING] Ignoring request. No PWM%u functions available as it was not initialised.\n", librpip_error_data);
			break;
		case 0x407:
			snprintf(desc, len, "[librpip INFO   ] No PWM%u functions available. Skipping initialisation as instructed. Pins NOT available to GPIO\n", librpip_error_data);
			break;
		case 0x408:
			snprintf(desc, len, "[librpip INFO   ] No PWM%u functions available. PWM module is not enabled. Consult /boot/config.txt. Pins are available to GPIO\n", librpip_error_data);
			break;
		case 0x410:
			snprintf(desc, len, "[librpip ERROR  ] Error reading config for PWM%u. Opening file /sys/class/pwm/pwmchip0/pwm%u/duty_cycle caused errno 0x%x (%s).\n", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x411:
			snprintf(desc, len, "[librpip ERROR  ] Error reading config for PWM%u. Opening file /sys/class/pwm/pwmchip0/pwm%u/period caused errno 0x%x (%s).\n", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x412:
			snprintf(desc, len, "[librpip ERROR  ] Error reading config for PWM%u. Opening file /sys/class/pwm/pwmchip0/pwm%u/enable caused errno 0x%x (%s).\n", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x413:
			snprintf(desc, len, "[librpip ERROR  ] Error reading config for PWM%u. Opening file /sys/class/pwm/pwmchip0/pwm%u/polarity caused errno 0x%x (%s).\n", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x420:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to configure PWM%u. The supplied period (%u) was not in the expected range of 109ns to 2s.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x421:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to configure PWM%u. The supplied duty cycle (%u) was greater than the period.\n", librpip_error_data, librpip_error_extra);
			break;
		case 0x422:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to configure PWM%u. The supplied flags (0x%x) contained unknown flags.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x423:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to configure PWM%u. It is not possible to have the polarity normal and inverted at the same time.\n", librpip_error_data);
			break;	
		case 0x424:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to configure PWM%u. The supplied duty cycle %f was not a percentage between 0 and 100.\n", librpip_error_data, librpip_error_extraf);
			break;	
		case 0x425:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to set a servo on PWM%u. The supplied angle %f was outside of the configured range %u.\n", librpip_error_data, librpip_error_extraf, librpip_error_extra);
			break;	
		case 0x426:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to set a servo on PWM%u. The supplied range (%u) is outside of the 0-720deg range.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x427:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to set a servo on PWM%u. The supplied min pulse (%u) is outside of the 0-20ms range.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x428:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to set a servo on PWM%u. The supplied max pulse (%u) is outside of the 0-20ms range.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x429:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to set a servo on PWM%u. The supplied min pulse (%u) is greater than the max pulse (%.0f).\n", librpip_error_data, librpip_error_extra, librpip_error_extraf);
			break;
		case 0x430:
			snprintf(desc, len, "[librpip ERROR  ] Error writing config for PWM%u. Accessing file /sys/class/pwm/pwmchip0/pwm%u/duty_cycle caused errno 0x%x (%s).\n", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x431:
			snprintf(desc, len, "[librpip ERROR  ] Error writing config for PWM%u. Accessing file /sys/class/pwm/pwmchip0/pwm%u/period caused errno 0x%x (%s).\n", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x432:
			snprintf(desc, len, "[librpip ERROR  ] Error writing config for PWM%u. Accessing file /sys/class/pwm/pwmchip0/pwm%u/enable caused errno 0x%x (%s).\n", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x433:
			snprintf(desc, len, "[librpip ERROR  ] Error writing config for PWM%u. Accessing file /sys/class/pwm/pwmchip0/pwm%u/polarity caused errno 0x%x (%s).\n", librpip_error_data, librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	

//transactions			
		case 0x500:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to create a transaction. Mode of %u is not valid.\n", librpip_error_data);
			break;	
		case 0x501:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to create a transaction. Bits per word of %u is not valid.\n", librpip_error_data);
			break;	
		case 0x510:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to add a message segment. Direction of %u is not valid.\n", librpip_error_data);
			break;	
		case 0x511:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to add a message segment. Length of %u is not valid.\n", librpip_error_data);
			break;	
		case 0x512:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to add a message segment, this transaction has already been transmitted.\n");
			break;	
		case 0x513:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to add a message segment, combined read/write messages are only valid on SPI.\n");
			break;	
		case 0x514:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to add a message segment, SPI only supports combined read/write messages.\n");
			break;	
		case 0x515:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to add a message segment, I2C max message length (%u) reached..\n", librpip_error_data);
			break;
		case 0x516:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to set message variable '%c'. Length of %u is not valid.\n", (char)librpip_error_extra, librpip_error_data);
			break;	
		case 0x517:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to  add a variable message segment. Variable name is not valid.\n");
			break;
		case 0x520:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to transmit a transaction that has no messages.\n");
			break;	
		case 0x521:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to transmit a transaction that has unset variable segments.\n");
			break;
		case 0x530:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to read from a transaction that has not been initialised.\n");
			break;
		case 0x531:
			snprintf(desc, len, "[librpip WARNING] No data to return. Could not find another message segment to read from in the transaction.\n");
			break;
		case 0x532:
			snprintf(desc, len, "[librpip WARNING] No data to return. Could not find a message segment to read from in the transaction.\n");
			break;
		case 0x533:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to read from a transaction that has not been sent. Status is %u\n", librpip_error_data);
			break;
		case 0x540:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to destroy transaction. It has not been initialised.\n");
			break;

//Uarts			
		case 0x600:
			snprintf(desc, len, "[librpip WARNING] No UART%u functions available. /dev/ttyAMA0 does not exist.\n", librpip_error_data);
			break;	
		case 0x601:
			snprintf(desc, len, "[librpip WARNING] No UART%u functions available. Unable to open /dev/ttyAMA0 for writing (user permissions or in use by console?). Pins NOT available to GPIO\n", librpip_error_data);
			break;
		case 0x602:
			snprintf(desc, len, "[librpip ERROR  ] Invalid UART device number %u.\n", librpip_error_data);
			break;	
		case 0x603:
			snprintf(desc, len, "[librpip INFO   ] No UART%u functions available. Skipping initialisation as instructed. Pins NOT available to GPIO\n", librpip_error_data);
			break;	
		case 0x604:
			snprintf(desc, len, "[librpip INFO   ] Neither UART device node exists. Pins are available to GPIO\n");
			break;	
		case 0x605:
			snprintf(desc, len, "[librpip WARNING] Ignoring request. No UART%u functions available as it was not initialised.\n", librpip_error_data);
			break;
		case 0x606:
			snprintf(desc, len, "[librpip WARNING] No UART%u functions available. /dev/ttyS0 does not exist.\n", librpip_error_data);
			break;	
		case 0x607:
			snprintf(desc, len, "[librpip WARNING] No UART%u functions available. Unable to open /dev/ttyS0 for writing (user permissions or in use by console?). Pins NOT available to GPIO\n", librpip_error_data);
			break;
		case 0x608:
			snprintf(desc, len, "[librpip INFO   ] No UART%u functions available. UART%u module is not enabled. Consult /boot/config.txt.\n", librpip_error_data, librpip_error_data);
			break;							
		case 0x610:
			snprintf(desc, len, "[librpip ERROR  ] Unable to read configuration of UART%u. The Error was 0x%x (%s).\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;
		case 0x611:
			snprintf(desc, len, "[librpip ERROR  ] Unable to flush UART%u.The Error was 0x%x (%s).\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x612:
			snprintf(desc, len, "[librpip ERROR  ] Unable to set configuration of UART%u. The Error was 0x%x (%s).\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;		
		case 0x620:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to configure UART%u. The supplied baud %u is not valid.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x621:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to configure UART%u. The supplied mode %u is not valid.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x622:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to configure UART%u. The supplied character bits size %u is not valid.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x623:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to configure UART%u. The supplied parity %u is not valid.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x624:
			snprintf(desc, len, "[librpip WARNING] Ignoring request to configure UART%u. The supplied stopbits %u is not valid.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x630:
			snprintf(desc, len, "[librpip ERROR  ] Error writing to UART%u. The error was 0x%x (%s).\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x631:
			snprintf(desc, len, "[librpip WARNING] Incomplete transfer on UART%u. The returned length transmitted %u is less than the length attempted.\n", librpip_error_data, librpip_error_extra);
			break;	
		case 0x632:
			snprintf(desc, len, "[librpip WARNING] Ignoring transfer on UART%u. The length %u is is not valid.\n", librpip_error_data, librpip_error_extra);
			break;
		case 0x640:
			snprintf(desc, len, "[librpip ERROR  ] Error reading from UART%u. The error was 0x%x (%s).\n", librpip_error_data, librpip_error_extra, strerror(librpip_error_extra));
			break;	
		case 0x641:
			snprintf(desc, len, "[librpip WARNING] Timeout waiting for data from UART%u.\n", librpip_error_data);
			break;	
			
		default:
			snprintf(desc, len, "[librpip WARNING] Unknown Error 0x%x [0x%x 0x%x %f].\n", librpip_error_code, librpip_error_data, librpip_error_extra, librpip_error_extraf);			
			
	}
	
	return strlen(desc);
}



