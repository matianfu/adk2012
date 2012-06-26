/**
 * \file
 *
 * \brief User Interface
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

#include "compiler.h"
#include "conf_usb.h"
#include "udc.h"
#include "udi_hid_generic.h"
#include "board.h"
#include "led.h"
#include "ui.h"

void ui_init(void)
{
}

void ui_powerdown(void)
{
}

void ui_wakeup_enable(void)
{
	PORT_t *port;
	// Configure pin change interrupt for asynch. wake-up on button pin.
	ioport_configure_pin(GPIO_PUSH_BUTTON_0,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	port = ioport_pin_to_port(GPIO_PUSH_BUTTON_0);
	port->INT0MASK = 0xFF;
	port->INTCTRL = PORT_INT0LVL_LO_gc;
}

void ui_wakeup_disable(void)
{
	PORT_t *port;
	port = ioport_pin_to_port(GPIO_PUSH_BUTTON_0);
	port->INT0MASK = 0x00;
}

// Interrupt on "pin change" from switch to do wakeup on USB
// Note:
// This interrupt is enable when the USB host enable remotewakeup feature
// This interrupt wakeup the CPU if this one is in idle mode
ISR(PORTE_INT0_vect)
{
	PORT_t *port;
	port = ioport_pin_to_port(GPIO_PUSH_BUTTON_0);
	port->INTFLAGS = 0x01;	// Ack interrupt
	// It is a wakeup then send wakeup USB
	udc_wakeup();
}

void ui_wakeup(void)
{
}

void ui_process(uint16_t framenumber)
{
}

void ui_led_on(uint8_t led_num)
{
	switch (led_num) {
	case 1:
		LED_On(LED0_GPIO);
		break;
	case 2:
		LED_On(LED1_GPIO);
		break;
	case 3:
		LED_On(LED2_GPIO);
		break;
	case 4:
		LED_On(LED3_GPIO);
		break;
	}
}

void ui_led_off(uint8_t led_num)
{
	switch (led_num) {
	case 1:
		LED_Off(LED0_GPIO);
		break;
	case 2:
		LED_Off(LED1_GPIO);
		break;
	case 3:
		LED_Off(LED2_GPIO);
		break;
	case 4:
		LED_Off(LED3_GPIO);
		break;
	}
}

bool ui_button(void)
{
	return gpio_pin_is_low(GPIO_PUSH_BUTTON_0);
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on XMEGA-B1 Xplain:
 * - RED led close to USB connector is always on after firmware startup
 * - Led 0 to 3 are controlled by USB Host application
 * - Switch 0 events are reported to USB Host application
 * - Switch 0 can be used to wakeup USB Host in remote wakeup mode.
 */
