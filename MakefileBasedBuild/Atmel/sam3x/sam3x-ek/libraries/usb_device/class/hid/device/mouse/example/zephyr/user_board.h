/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief User board definitions.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USART module can be used.
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#ifndef _USER_BOARD_H_
#define _USER_BOARD_H_

#define LED0  0x01
#define LED1  0x02
#define LED2  0x04
#define LED3  0x08
#define LED4  0x10
#define LED5  0x20
#define LED6  0x40
#define LED7  0x80

extern volatile avr32_fpga_debug_t *debug;

#if (defined __GNUC__)
__attribute__ ((__always_inline__))
#endif
static inline void LED_On(unsigned int led)
{
	debug->led |= led;
}

#if (defined __GNUC__)
__attribute__ ((__always_inline__))
#endif
static inline void LED_Off(unsigned int led)
{
	debug->led &= ~led;
}

#if (defined __GNUC__)
__attribute__ ((__always_inline__))
#endif
static inline void LED_Toggle(unsigned int led)
{
	debug->led ^= led;
}

/*! \name Configuration
 */
//! @{
#define USART_SERIAL_EXAMPLE             &AVR32_USART0
#define USART_SERIAL_EXAMPLE_BAUDRATE    57600
#define USART_SERIAL_CHAR_LENGTH         8
#define USART_SERIAL_PARITY              USART_NO_PARITY
#define USART_SERIAL_STOP_BIT            USART_1_STOPBIT
//! @}

/* These are documented in services/basic/clock/uc3a0_a1/osc.h */
#define BOARD_OSC0_HZ           FCPU_HZ_ZEPHYR
#define BOARD_OSC0_STARTUP_US   17000
#define BOARD_OSC0_IS_XTAL      true
//#define BOARD_OSC1_HZ           11289600
//#define BOARD_OSC1_STARTUP_US   17000
//#define BOARD_OSC1_IS_XTAL      true
//#define BOARD_OSC32_HZ          32768
//#define BOARD_OSC32_STARTUP_US  71000
//#define BOARD_OSC32_IS_XTAL     true

#endif // _USER_BOARD_H_
