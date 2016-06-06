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

#ifndef LIBRPIP_H
#define LIBRPIP_H

//some English
#define LIBRPIP_OFF				0
#define LIBRPIP_ON				1


/*
 *   Library Defines
 */
//supported boards
#define LIBRPIP_BOARD_DETECT			0
#define LIBRPIP_BOARD_PI_A			1
#define LIBRPIP_BOARD_PI_A_PLUS			2
#define LIBRPIP_BOARD_PI_B_R1			3
#define LIBRPIP_BOARD_PI_B_R2			4
#define LIBRPIP_BOARD_PI_B_PLUS			5
#define LIBRPIP_BOARD_PI2_B			6
#define LIBRPIP_BOARD_PI_COMPUTE		7
#define LIBRPIP_BOARD_PI_ZERO			8

//init flags
#define LIBRPIP_FLAG_DEBUG_ON			0x001
#define LIBRPIP_FLAG_NO_RESET			0x002
#define LIBRPIP_FLAG_SKIP_GPIO			0x004
#define LIBRPIP_FLAG_SKIP_UART0			0x008
#define LIBRPIP_FLAG_SKIP_UART1			0x010
#define LIBRPIP_FLAG_SKIP_PWM0			0x020
#define LIBRPIP_FLAG_SKIP_PWM1			0x040
#define LIBRPIP_FLAG_SKIP_I2C0			0x080
#define LIBRPIP_FLAG_SKIP_I2C1			0x100
#define LIBRPIP_FLAG_SKIP_SPI0			0x200
#define LIBRPIP_FLAG_SKIP_SPI1			0x400

#define LIBRPIP_FLAG_SKIP_UART			(LIBRPIP_FLAG_SKIP_UART0 | LIBRPIP_FLAG_SKIP_UART1)
#define LIBRPIP_FLAG_SKIP_I2C			(LIBRPIP_FLAG_SKIP_I2C0 | LIBRPIP_FLAG_SKIP_I2C1)
#define LIBRPIP_FLAG_SKIP_SPI			(LIBRPIP_FLAG_SKIP_SPI0 | LIBRPIP_FLAG_SKIP_SPI1)
#define LIBRPIP_FLAG_SKIP_PWM			(LIBRPIP_FLAG_SKIP_PWM0 | LIBRPIP_FLAG_SKIP_PWM1)

//feature set 
#define LIBRPIP_FEATURE_GPIO			0x001
#define LIBRPIP_FEATURE_UART0			0x002	
#define LIBRPIP_FEATURE_UART1			0x004
#define LIBRPIP_FEATURE_I2C0			0x008	
#define LIBRPIP_FEATURE_I2C1			0x010	
#define LIBRPIP_FEATURE_SPI0			0x020	
#define LIBRPIP_FEATURE_SPI1			0x040
#define LIBRPIP_FEATURE_PWM0			0x080	
#define LIBRPIP_FEATURE_PWM1			0x100


//Error Severity
#define LIBRPIP_ERROR_MSG_INFO			1
#define LIBRPIP_ERROR_MSG_WARNING		2
#define LIBRPIP_ERROR_MSG_ERROR			3


/*
 *   GPIO Defines
 */
#define LIBRPIP_GPIO_FLAG_FNC_IN		0x001
#define LIBRPIP_GPIO_FLAG_FNC_OUT		0x002
#define LIBRPIP_GPIO_FLAG_PUD_OFF		0x004
#define LIBRPIP_GPIO_FLAG_PUD_DOWN		0x008
#define LIBRPIP_GPIO_FLAG_PUD_UP		0x010 
#define LIBRPIP_GPIO_FLAG_ED_OFF		0x020
#define LIBRPIP_GPIO_FLAG_ED_RISE		0x040
#define LIBRPIP_GPIO_FLAG_ED_FALL		0x080
#define LIBRPIP_GPIO_FLAG_ED_HIGH		0x100 
#define LIBRPIP_GPIO_FLAG_ED_LOW		0x200	
#define LIBRPIP_GPIO_FLAG_ED_ARISE		0x400
#define LIBRPIP_GPIO_FLAG_ED_AFALL		0x800

/*
 *   PWM Defines
 */
#define LIBRPIP_PWM_STATUS_OFF			0x0
#define LIBRPIP_PWM_STATUS_ON			0x1

#define LIBRPIP_PWM_FLAG_POLARITY_NORMAL	0x01
#define LIBRPIP_PWM_FLAG_POLARITY_INVERTED	0x02


/*
 *   Transaction Defines
 */

#define LIBRPIP_TX_NOBUF			0x0	

#define LIBRPIP_TX_STATUS_NEW			0x1	
#define LIBRPIP_TX_STATUS_MSG			0x2	
#define LIBRPIP_TX_STATUS_SENTOK		0x3	
#define LIBRPIP_TX_STATUS_FAILED		0x4	

#define LIBRPIP_TX_MODE_I2C			0x1	
#define LIBRPIP_TX_MODE_SPI			0x2	
#define LIBRPIP_TX_MODE_SPI_3W			0x3	/* not valid for SPI1 */
#define LIBRPIP_TX_MODE_UART			0x4	

#define LIBRPIP_TX_MSG_TX			0x1	
#define LIBRPIP_TX_MSG_RX			0x2	
#define LIBRPIP_TX_MSG_TXRX			(LIBRPIP_TX_MSG_TX | LIBRPIP_TX_MSG_RX)	



/*
 *   UART Defines
 */
#define LIBRPIP_UART_BAUD_1200 			1200
#define LIBRPIP_UART_BAUD_2400 			2400
#define LIBRPIP_UART_BAUD_4800 			4800
#define LIBRPIP_UART_BAUD_9600 			9600
#define LIBRPIP_UART_BAUD_19200 		19200
#define LIBRPIP_UART_BAUD_38400 		38400
#define LIBRPIP_UART_BAUD_57600 		57600
#define LIBRPIP_UART_BAUD_115200 		115200
#define LIBRPIP_UART_BAUD_230400 		230400
#define LIBRPIP_UART_BAUD_460800 		460800
#define LIBRPIP_UART_BAUD_500000 		500000
#define LIBRPIP_UART_BAUD_576000 		576000
#define LIBRPIP_UART_BAUD_921600 		921600
#define LIBRPIP_UART_BAUD_1000000 		1000000
#define LIBRPIP_UART_BAUD_1152000 		1152000
#define LIBRPIP_UART_BAUD_1500000 		1500000
#define LIBRPIP_UART_BAUD_2000000 		2000000
#define LIBRPIP_UART_BAUD_2500000 		2500000
#define LIBRPIP_UART_BAUD_3000000 		3000000
#define LIBRPIP_UART_BAUD_3500000 		3500000
#define LIBRPIP_UART_BAUD_4000000 		4000000

#define LIBRPIP_UART_SIZE_5			5  /* not valid for uart1 */
#define LIBRPIP_UART_SIZE_6			6  /* not valid for uart1 */
#define LIBRPIP_UART_SIZE_7			7
#define LIBRPIP_UART_SIZE_8			8

#define LIBRPIP_UART_PARITY_OFF			0
#define LIBRPIP_UART_PARITY_ODD			1  /* not valid for uart1 */
#define LIBRPIP_UART_PARITY_EVEN		2  /* not valid for uart1 */

#define LIBRPIP_UART_STOPBITS_1			1
#define LIBRPIP_UART_STOPBITS_2			2  /* not valid for uart1 */

#define LIBRPIP_UART_MODE_BINARY		0
#define LIBRPIP_UART_MODE_ASCII			1


/*
 *   SPI Defines
 */
#define LIBRPIP_SPI_MODE_0			0x0    //0        | 0
#define LIBRPIP_SPI_MODE_1			0x1    //0        | SPI_CPHA
#define LIBRPIP_SPI_MODE_2			0x2    //SPI_CPOL | 0
#define LIBRPIP_SPI_MODE_3			0x3    //SPI_CPOL | SPI_CPHA

#define LIBRPIP_SPI_FLAG_CS_HIGH		0x04
#define LIBRPIP_SPI_FLAG_NO_CS			0x40

/*
 *   I2C Defines
 */
#define LIBRPIP_I2C_FLAG_PEC			0x01
 
#include <stdint.h>
#include <sys/types.h>

 
#ifdef __cplusplus 
extern "C" {
#endif


/*
 *   Core Functions
 */
uint32_t librpipInit(uint32_t board, uint32_t flags, uint32_t ignore_pins);
void	 librpipClose(void);
uint32_t librpipGetBoardID(void);
void 	 librpipVersionStr(char* version, int len);
void 	 librpipVersionInt(uint8_t* maj, uint8_t* min, uint8_t* sub);
uint32_t librpipBoardGetDescription(char* desc, int len);

/*
 *   GPIO Functions
 */
uint32_t librpipGpioConfigPinRead(uint32_t pin, uint32_t* flags);
uint32_t librpipGpioConfigPinWrite(uint32_t pin, uint32_t flags);
uint32_t librpipGpioPinRead(uint32_t pin, uint32_t* value);
uint32_t librpipGpioPinWrite(uint32_t pin, uint32_t value);
uint32_t librpipGpioPinToggle(uint32_t pin);
uint32_t librpipGpioPinPulse(uint32_t pin, uint32_t length);
uint32_t librpipGpioPinEvent(uint32_t pin, uint32_t* event);
uint32_t librpipGpioPinEventWait(uint32_t pin, uint32_t timeout);
uint32_t librpipGpioGetValidPins(void);

/*
 *   PWM Functions
 */
uint32_t librpipPwmConfigRead(uint32_t id, uint32_t* pin, uint32_t* period, uint32_t* duty_cycle, uint32_t* flags);
uint32_t librpipPwmConfigWrite(uint32_t id, uint32_t period, uint32_t duty_cycle, uint32_t flags);
uint32_t librpipPwmStatusRead(uint32_t id, uint32_t* status);
uint32_t librpipPwmStatusWrite(uint32_t id, uint32_t status);
uint32_t librpipPwmDutyPercentWrite(uint32_t id, float duty_cycle);

uint32_t librpipServoConfigRead(uint32_t id, uint32_t* range, uint32_t* pmin, uint32_t* pmax);
uint32_t librpipServoConfigWrite(uint32_t id, uint32_t range, uint32_t pmin, uint32_t pmax);
uint32_t librpipServoPositionWrite(uint32_t id, float angle);
 
/*
 *   Transaction Functions
 */
struct librpip_tx*	librpipTransactionCreate(uint8_t mode, uint8_t bpw);
void 			librpipTransactionConfigRead(struct librpip_tx* t, uint8_t* mode, uint8_t* bpw, uint8_t* status, uint16_t* len);
uint32_t 		librpipTransactionMsgAdd(struct librpip_tx* t, uint8_t dir, void* txbuf, uint16_t len);
uint32_t 		librpipTransactionMsgAddRegRead(struct librpip_tx* t, uint8_t reg, uint16_t len);
uint32_t 		librpipTransactionMsgAddRegWrite(struct librpip_tx* t, uint8_t reg, uint8_t value);
uint32_t 		librpipTransactionMsgVarAdd(struct librpip_tx* t, uint8_t dir, char var);
uint32_t 		librpipTransactionMsgVarSet(struct librpip_tx* t, char var,  void* txbuf, uint16_t len);
uint32_t 		librpipTransactionSend(struct librpip_tx* t, uint32_t device, uint32_t client);
uint32_t		librpipTransactionRead(struct librpip_tx* t, void* result, uint16_t len);
uint32_t 		librpipTransactionReadSize(struct librpip_tx* t, uint16_t* size) {
uint32_t 		librpipTransactionDestroy(struct librpip_tx* t); 
 

/*
 *   SPI Functions
 */
uint32_t librpipSpiConfigRead(uint32_t id, uint32_t cs, uint32_t* mode, uint32_t* lsb_first, uint32_t* bits_per_word, uint32_t* max_speed, uint32_t* flags);
uint32_t librpipSpiConfigWrite(uint32_t id, uint32_t cs, uint32_t mode, uint32_t max_speed, uint32_t flags);


/*
 *   I2C Functions
 */
uint32_t librpipI2cConfigWrite(uint32_t id, uint32_t flags);
uint32_t librpipI2cConfigRead(uint32_t id, uint32_t* flags);


/*
 *   UART Functions
 */
uint32_t librpipUartConfigWrite(uint32_t id, uint32_t baud, uint32_t csize, uint32_t parity, uint32_t stop, uint32_t mode);
uint32_t librpipUartConfigRead(uint32_t id, uint32_t* baud, uint32_t* csize, uint32_t* parity, uint32_t* stop, uint32_t* mode);
uint32_t librpipUartWrite(uint32_t id, void* buf, uint16_t len); 
uint32_t librpipUartRead(uint32_t id, void* buf, uint16_t len, uint32_t timeout); 


/*
 *   Error Functions
 */
uint32_t librpipErrorGetCode(void);
uint32_t librpipErrorGetSeverity(void);
uint32_t librpipErrorGetDescription(char* desc, int len);
void 	 librpipErrorPrint(void);

#ifdef __cplusplus 
}
#endif

#endif
