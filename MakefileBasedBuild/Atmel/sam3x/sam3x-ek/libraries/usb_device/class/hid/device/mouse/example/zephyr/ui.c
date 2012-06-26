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
#include "udi_hid_mouse.h"
#include "board.h"
#include "gpio.h"
#include "sysclk.h"
#include "cycle_counter.h"
#include "ui.h"

#define MOUSE_MOVE_RANGE      3
#define TIMER_UI           100

static t_cpu_time ui_timer;
static uint8_t state_mouse = 0;

void ui_init(void)
{
	LED_On(LED0);
	LED_Off(LED1);
	cpu_set_timeout(cpu_ms_2_cy(TIMER_UI, sysclk_get_cpu_hz()), &ui_timer);
	state_mouse = 0;
}

void ui_powerdown(void)
{
	LED_Off(LED0);
	LED_Off(LED1);
}

void ui_wakeup_enable(void)
{
}

void ui_wakeup_disable(void)
{
}

void ui_wakeup(void)
{
	LED_On(LED0);
}

void ui_process(uint16_t framenumber)
{
	bool b_btn_state;
	static bool btn_left_last_state = HID_MOUSE_BTN_UP;
	static bool btn_right_last_state = HID_MOUSE_BTN_UP;
	static bool btn_middle_last_state = HID_MOUSE_BTN_UP;
	static uint8_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED1);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED1);
	}
	// Scan process running each 2ms
	cpt_sof++;
	if (2 > cpt_sof)
		return;
	cpt_sof = 0;

	// Scan buttons on switch 0 (left), 1 (middle), 2 (right)
	//b_btn_state = (!gpio_get_pin_value(GPIO_PUSH_BUTTON_0))? HID_MOUSE_BTN_DOWN : HID_MOUSE_BTN_UP;
	b_btn_state = HID_MOUSE_BTN_RELEASED;
	if (b_btn_state != btn_left_last_state) {
		udi_hid_mouse_btnleft(b_btn_state);
		btn_left_last_state = b_btn_state;
	}
	//b_btn_state = (!gpio_get_pin_value(GPIO_PUSH_BUTTON_1))? HID_MOUSE_BTN_DOWN : HID_MOUSE_BTN_UP;
	b_btn_state = HID_MOUSE_BTN_RELEASED;
	if (b_btn_state != btn_middle_last_state) {
		udi_hid_mouse_btnmiddle(b_btn_state);
		btn_middle_last_state = b_btn_state;
	}
	//b_btn_state = (!gpio_get_pin_value(GPIO_PUSH_BUTTON_2))? HID_MOUSE_BTN_DOWN : HID_MOUSE_BTN_UP;
	b_btn_state = HID_MOUSE_BTN_RELEASED;
	if (b_btn_state != btn_right_last_state) {
		udi_hid_mouse_btnright(b_btn_state);
		btn_right_last_state = b_btn_state;
	}
	// Scan move on switch 4 (right), 5 (left), 6 (down), 7 (up)
	if (state_mouse == 0)
		udi_hid_mouse_moveX(MOUSE_MOVE_RANGE);
	else if (state_mouse == 2)
		udi_hid_mouse_moveX(-MOUSE_MOVE_RANGE);
	else if (state_mouse == 1)
		udi_hid_mouse_moveY(-MOUSE_MOVE_RANGE);
	else if (state_mouse == 3)
		udi_hid_mouse_moveY(MOUSE_MOVE_RANGE);

	if (cpu_is_timeout(&ui_timer)) {
		cpu_set_timeout(cpu_ms_2_cy(TIMER_UI, sysclk_get_cpu_hz()),
				&ui_timer);
		state_mouse = (state_mouse + 1) % 4;
	}
}
