/**
 * \file
 *
 * \brief Part-specific system clock management
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
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

#include "preprocessor.h"
#include "compiler.h"
#include "board.h"
#include "pm.h"
#if UC3C
#include "ast.h"
#else
#include "rtc.h"
#endif
#include "cycle_counter.h"
#include "flashc.h"
#include "freq_detect.h"

/*!
 *  Detects extern OSC frequency and enable USB clock
 */
void sysclk_init(void)
{
	int mul;

	// Switch to OSC ISP
	// Set max startup time to make sure any crystal will be supported
	// We cannot use a TC to measure this OSC frequency
	// because the master clock must be faster than the clock selected by the TC
	pm_switch_to_osc0(&AVR32_PM, 16000000,
			AVR32_PM_OSCCTRL0_STARTUP_16384_RCOSC);
	// Initialize the RTC with the internal RC oscillator
	// RTC will count at the frequency of 115KHz/2
	rtc_init(&AVR32_RTC, RTC_OSC_RC, 0);
	rtc_enable(&AVR32_RTC);

	// Detect the frequency
	// mul = (((96000000 / freq_detect_start())/2)-1)
	switch (freq_detect_start()) {
	case 8000000:
		mul = 5;
		break;
	case 16000000:
		mul = 2;
		break;
	case 12000000:
	default:
		mul = 3;
		break;
	}

	// Set PLL0 VCO @ 96 MHz
	pm_pll_setup(&AVR32_PM, 0,	// pll
			mul,	// mul
			0,	// div
			0,	// osc
			63);	// lockcount

	// Set PLL0 @ 48 MHz
	pm_pll_set_option(&AVR32_PM, 0,	// pll
			1,	// pll_freq
			1,	// pll_div2
			0);	// pll_wbwdisable

	// Enable PLL0
	pm_pll_enable(&AVR32_PM, 0);

	// Wait for PLL0 locked with a 10-ms time-out
	pm_wait_for_pll0_locked(&AVR32_PM);

	// Use 1 flash wait state
	flashc_set_wait_state(1);

	// Switch the main clock to PLL0
	pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);

	// fPBA: 12 MHz
	// fPBB: 12 MHz
	// fHSB: 12 MHz
	pm_cksel(&AVR32_PM, 1,	// pbadiv
			1,	// pbasel
			1,	// pbbdiv
			1,	// pbbsel
			1,	// hsbdiv
			1);	// hsbsel

	// Use 0 flash wait state
	flashc_set_wait_state(0);
}


/*!
 *  Reset the generation of system clocks and switch to RCOsc
 */
void sysclk_reset(void)
{
	flashc_set_wait_state(1);
	pm_cksel(&AVR32_PM, 0, 0, 0, 0, 0, 0);
	pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_SLOW);
	flashc_set_wait_state(0);
	pm_pll_disable(&AVR32_PM, 0);
	pm_pll_set_option(&AVR32_PM, 0, 0, 0, 0);
	pm_pll_setup(&AVR32_PM, 0, 0, 0, 0, 0);
	pm_enable_clk0_no_wait(&AVR32_PM, AVR32_PM_OSCCTRL0_STARTUP_0_RCOSC);
	pm_disable_clk0(&AVR32_PM);
	pm_enable_osc0_ext_clock(&AVR32_PM);
}


void sysclk_enable_usb(void)
{
	// Setup USB GCLK
	pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_USBB,	// gc
			1,	// osc_or_pll: use Osc (if 0) or PLL (if 1)
			0,	// pll_osc: select Osc0/PLL0 or Osc1/PLL1
#if UC3A3
			1,	// diven
			1);	// div
#else
			0,	// diven
			0);	// div
#endif

	// Enable USB GCLK
	pm_gc_enable(&AVR32_PM, AVR32_PM_GCLK_USBB);
}


void sysclk_disable_usb(void)
{
	pm_gc_disable(&AVR32_PM, AVR32_PM_GCLK_USBB);
	pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_USBB, 0, 0, 0, 0);
}


void freq_detect_iface_ref_cnt_reset(void)
{
	rtc_set_value(&AVR32_RTC, 0);
}

int freq_detect_iface_ref_cnt_value(void)
{
	return rtc_get_value(&AVR32_RTC) * 2;
}

void freq_detect_iface_target_cnt_reset(void)
{
	Set_sys_count(0);
}

int freq_detetc_iface_target_cnt_value(void)
{
	return Get_sys_count();
}
