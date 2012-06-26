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
#include "pm_uc3c.h"
#include "scif_uc3c.h"
#include "ast.h"
#include "cycle_counter.h"
#include "flashc.h"


// Use Internal RC8MHz, wait characterization results
// If it works use this configuration as default.
//#define INTERNAL_RC8MHZ_USED

#ifndef INTERNAL_RC8MHZ_USED
#include "freq_detect.h"
#endif

/*!
 *  Detects extern OSC frequency and enable USB clock
 */
void sysclk_init(void)
{

#ifdef INTERNAL_RC8MHZ_USED
	// Start the 8Mhz Oscillator
	scif_start_rc8M();
	// Set the main clock source as being RC8MHz.
	pm_set_mclk_source(PM_CLK_SRC_RC8M);

	scif_pll_opt_t opt;
	opt.osc = 2;
	opt.lockcount = 63;
	opt.div = 0;
	opt.mul = 5;
	opt.pll_div2 = 1;
	opt.pll_wbwdisable = 0;
	opt.pll_freq = 1;

	// lockcount in main clock for the PLL wait lock
	scif_pll_setup(SCIF_PLL0, opt);

	/* Enable PLL0 */
	scif_pll_enable(SCIF_PLL0);

	/* Wait for PLL0 locked */
	scif_wait_for_pll_locked(SCIF_PLL0);
#else
	int mul;

	// Switch to OSC ISP
	// Set max startup time to make sure any crystal will be supported
	// We cannot use a TC to measure this OSC frequency
	// because the master clock must be faster than the clock selected by the TC

	// Configure OSC0 in crystal mode, external crystal 
	// with a fcrystal Hz frequency.
	scif_configure_osc_crystalmode(SCIF_OSC0, 16000000);
	// Enable the OSC0
	scif_enable_osc(SCIF_OSC0, AVR32_SCIF_OSCCTRL0_STARTUP_16384_RCOSC,
			true);
	flashc_set_flash_waitstate_and_readmode(16000000);
	pm_set_mclk_source(PM_CLK_SRC_OSC0);

	// Initialize the AST with the internal RC oscillator
	// AST will count at the frequency of 115KHz/2
	if (!ast_init_counter(&AVR32_AST, AST_OSC_RC, 0, 0)) {
		while (1);
	}
	// Enable the AST
	ast_enable(&AVR32_AST);

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

	scif_pll_opt_t opt;

	// Set PLL0 VCO @ 96 MHz
	// Set PLL0 @ 48 MHz
	opt.osc = SCIF_OSC0;
	opt.lockcount = 63;
	opt.div = 0;
	opt.mul = mul;
	opt.pll_div2 = 1;
	opt.pll_wbwdisable = 0;
	opt.pll_freq = 1;

	// lockcount in main clock for the PLL wait lock
	scif_pll_setup(SCIF_PLL0, opt);

	/* Enable PLL0 */
	scif_pll_enable(SCIF_PLL0);

	/* Wait for PLL0 locked */
	scif_wait_for_pll_locked(SCIF_PLL0);

	// Use 1 flash wait state  
	flashc_set_wait_state(1);

	// Switch the main clock to PLL0
	pm_set_mclk_source(PM_CLK_SRC_PLL0);

	// fPBA: 12 MHz
	// fPBB: 12 MHz
	// fHSB: 12 MHz  
	pm_set_clk_domain_div(PM_CLK_DOMAIN_0, PM_CKSEL_DIVRATIO_4);	// CPU
	pm_set_clk_domain_div(PM_CLK_DOMAIN_1, PM_CKSEL_DIVRATIO_4);	// HSB
	pm_set_clk_domain_div(PM_CLK_DOMAIN_3, PM_CKSEL_DIVRATIO_4);	// PBB
	pm_set_clk_domain_div(PM_CLK_DOMAIN_2, PM_CKSEL_DIVRATIO_4);	// PBA
	pm_set_clk_domain_div(PM_CLK_DOMAIN_4, PM_CKSEL_DIVRATIO_4);	// PBC

	// Use 0 flash wait state
	flashc_set_wait_state(0);
#endif
}


/*!
 *  Reset the generation of system clocks and switch to RCOsc
 */
void sysclk_reset(void)
{
	flashc_set_wait_state(1);
	pm_set_clk_domain_div(PM_CLK_DOMAIN_0, PM_CKSEL_DIVRATIO_2);	// CPU
	pm_set_clk_domain_div(PM_CLK_DOMAIN_1, PM_CKSEL_DIVRATIO_2);	// HSB
	pm_set_clk_domain_div(PM_CLK_DOMAIN_3, PM_CKSEL_DIVRATIO_2);	// PBB
	pm_set_clk_domain_div(PM_CLK_DOMAIN_2, PM_CKSEL_DIVRATIO_2);	// PBA
	pm_set_clk_domain_div(PM_CLK_DOMAIN_4, PM_CKSEL_DIVRATIO_2);	// PBC  

	pm_set_mclk_source(PM_CLK_SRC_SLOW);

	flashc_set_wait_state(0);
	scif_pll_disable(SCIF_PLL0);

	scif_pll_opt_t opt;
	opt.osc = 0;
	opt.lockcount = 0;
	opt.div = 0;
	opt.mul = 0;
	opt.pll_div2 = 0;
	opt.pll_wbwdisable = 0;
	opt.pll_freq = 0;
	scif_pll_setup(SCIF_PLL0, opt);	// lockcount in main clock for the PLL wait lock
}


void sysclk_enable_usb(void)
{
	// Setup USB GCLK
	scif_gc_setup(AVR32_SCIF_GCLK_USBC,
			SCIF_GCCTRL_PLL0, AVR32_SCIF_GC_NO_DIV_CLOCK, 0);
	// Enable USB GCLK
	scif_gc_enable(AVR32_SCIF_GCLK_USBC);
}


void sysclk_disable_usb(void)
{
	AVR32_SCIF.gcctrl[AVR32_SCIF_GCLK_USBC] = 0;
	scif_gc_setup(AVR32_SCIF_GCLK_USBC,
			SCIF_GCCTRL_PLL0, AVR32_SCIF_GC_NO_DIV_CLOCK, 0);
}


void freq_detect_iface_ref_cnt_reset(void)
{
	ast_set_counter_value(&AVR32_AST, 0);
}

int freq_detect_iface_ref_cnt_value(void)
{
	return ast_get_counter_value(&AVR32_AST) * 2;
}

void freq_detect_iface_target_cnt_reset(void)
{
	Set_sys_count(0);
}

int freq_detetc_iface_target_cnt_value(void)
{
	return Get_sys_count();
}
