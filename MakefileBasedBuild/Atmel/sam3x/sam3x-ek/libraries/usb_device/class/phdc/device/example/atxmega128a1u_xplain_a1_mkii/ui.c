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
#include "board.h"
#include "led.h"
#include "communication_model.h"
#include "ui.h"

void ui_init(void)
{
	LED_On(LEDUSB_GPIO);
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
}

void ui_powerdown(void)
{
	LED_Off(LEDUSB_GPIO);
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);
}

void ui_wakeup(void)
{
	LED_On(LEDUSB_GPIO);
}

void ui_association(bool b_on)
{
	if (b_on) {
		LED_On(LED2_GPIO);
	} else {
		LED_Off(LED2_GPIO);
	}
}

void ui_process(uint16_t framenumber)
{
	bool b_btn_state;
	static bool btn0_last_state = false;
	static bool btn1_last_state = false;
	static uint8_t cpt_sof = 0;

	if (0 == framenumber) {
		LED_On(LED0_GPIO);
	}
	if (1000 == framenumber) {
		LED_Off(LED0_GPIO);
	}

	cpt_sof++;
	if (20 > cpt_sof)
		return;
	cpt_sof = 0;

	// Scan buttons on switch 0 (left), 1 (middle), 2 (right)
	b_btn_state = gpio_pin_is_low(GPIO_PUSH_BUTTON_0);
	if (b_btn_state != btn0_last_state) {
		btn0_last_state = b_btn_state;
		if (b_btn_state) {
			ieee11073_com_model_send_measure_1();
		}
	}
	b_btn_state = gpio_pin_is_low(GPIO_PUSH_BUTTON_1);
	if (b_btn_state != btn1_last_state) {
		btn1_last_state = b_btn_state;
		if (b_btn_state) {
			ieee11073_com_model_send_measure_2();
		}
	}
}

/**
 * \defgroup UI User Iinterface
 *
 * Human interface on Xplain-A1:
 * - RED led close to USB connector is always on after firmware startup
 * - Led 0 is on when USB line is in IDLE mode, and off in SUSPEND mode
 * - Led 1 blinks when USB Host have checked and enabled PHDC interface
 * - Led 2 is on when PHDC has validated association
 */
