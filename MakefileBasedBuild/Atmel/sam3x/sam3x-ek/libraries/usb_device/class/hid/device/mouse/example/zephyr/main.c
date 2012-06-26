/**
 * \file
 *
 * \brief Main functions
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "gpio.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "conf_usb.h"
#include "udc.h"
#include "udd.h"
#include "ui.h"
#include "stdio_serial.h"

static bool main_b_mouse_enable = false;


/*! \brief Main function. Execution starts here.
 */
#if 0
*(unsigned int *)0xfffe05C8	/* UPCON2 */
		* (unsigned int *)0xfffe0508	/* UPCFG2 */
		* (unsigned int *)0xfffe0538	/* UPSTA2 */
		* (unsigned int *)0xfffe0830	/* UDESC */
		* (unsigned int *)0xfffe0004	/* UDINT */
		* (unsigned int *)0xfffe0010	/* UDINTE */
		* (unsigned int *)0xfffe0800	/* USBCON */
		* (unsigned int *)0xfffe0804	/* USBSTA */
		* (unsigned int *)0xfffe0404	/* UHINT */
		* (unsigned int *)0xfffe0134	/* USBSTA1 */
fpga: V: \Users \ PM \ USB_Optimized \ I7256 \ source \ rtl \ synt \ systemAce 
\ rev_USBC_220.a doc: V:\Users \ PM \ USB_Optimized \ I7553 \ version \ 200 \
		documentation \ I7553 - 200 - 0001 -
		C_USBC
#endif
int main(void)
{
	irq_initialize_vectors();
	cpu_irq_enable();

	// USART options.
	static usart_serial_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};

	// Clear all leds
	LED_Off(LED0);
	LED_Off(LED1);
	LED_Off(LED2);
	LED_Off(LED3);

	// Initialize the sleep manager
	sleepmgr_init();

	sysclk_init();
	board_init();
	ui_init();
	ui_powerdown();

	// Initialize Serial Interface using Stdio Library
	stdio_serial_init(USART_SERIAL_EXAMPLE, &USART_SERIAL_OPTIONS);

	// Specify that stdout should not be buffered.
#if (defined(__GNUC__) && defined(__AVR32__))
	setbuf(stdout, NULL);
#else // __ICCAVR32__
	// Already the case in IAR's Normal DLIB default configuration: printf()
	// emits one character at a time.
#endif

	// Print welcome message
	printf("USBC Device (HID Mouse) validation %s\r\n", __TIME__);
	printf("----------------------------------\r\n");

	// Start USB stack to authorize VBus monitoring
	udc_start();

	if (!udc_include_vbus_monitoring()) {
		// VBUS monitoring is not available on this product
		// thereby VBUS has to be considered as present
		main_vbus_action(true);
	}

	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	while (true) {

		sleepmgr_enter_sleep();
		
#ifdef   USB_DEVICE_LOW_SPEED
		// No USB "Keep a live" interrupt available in low speed 
		// to scan mouse interface then use main loop 
		if (main_b_mouse_enable) {
			static uint16_t virtual_sof_sub = 0;
			static uint16_t virtual_sof = 0;
			if (700 == virtual_sof_sub++) {
				virtual_sof_sub = 0;
				ui_process(virtual_sof++);
			}
		}
#endif

	}
}

void main_vbus_action(bool b_high)
{
	if (b_high) {
		// Attach USB Device
		udc_attach();
	} else {
		// VBUS not present
		udc_detach();
	}
}

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if (!main_b_mouse_enable)
		return;
	ui_process(udd_get_frame_number());
}

void main_remotewakeup_enable(void)
{
	ui_wakeup_enable();
}

void main_remotewakeup_disable(void)
{
	ui_wakeup_disable();
}

bool main_mouse_enable(void)
{
	main_b_mouse_enable = true;
	return true;
}

void main_mouse_disable(void)
{
	main_b_mouse_enable = false;
}


/**
 * \mainpage ASF USB Device HID Mouse
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device HID Mouse
 * on AVR products with USB module.
 * The application note AVR4903 provides information about this implementation.
 *
 * \section startup Startup
 * The example uses the buttons or sensors available on the board 
 * to simulate a standard mouse.
 * After loading firmware, connect hardware board (EVKxx,XPlain,...) to the USB Host.
 * When connected to a USB host system this application provides a mouse application
 * in the Unix/Mac/Windows operating systems.
 * This example uses the native HID driver for these operating systems.
 *
 * \copydoc UI 
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and HID modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/hid/
 *   <br>services/usb/class/hid/mouse/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>\subpage power_management
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 */
