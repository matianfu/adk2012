/**
 * \file
 *
 * \brief Main functions for Audio example
 *
 * Copyright (C) 2011 Atmel Corporation. All rights reserved.
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
#include "preprocessor.h"
#include "board.h"
#include "gpio.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "conf_usb.h"
#include "conf_example.h"
#include "udd.h"
#include "udc.h"
#include "udi_audio.h"
#include "ui.h"
#include "sample_processor.h"

static bool main_b_audio_enable = false;


/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();

	sysclk_init();
	board_init();
	ui_init();
	ui_powerdown();
	
	// Initialize the sample processor module ready to receive samples
	sample_processor_init();

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
		if (main_b_audio_enable
		&& udi_audio_stream_get_feature_cur(AUDIO_FEATURE_MUTE) == false) {
			udi_audio_process();
		}
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
	if (!main_b_audio_enable)
		return;
	ui_process(udd_get_frame_number());
}

bool main_audio_enable(void)
{
	main_b_audio_enable = true;
	ui_stream_enabled();
	return true;
}

void main_audio_disable(void)
{
	main_b_audio_enable = false;
	ui_stream_disabled();
}

void main_audio_out_process(uint8_t *data, uint16_t count)
{
	// Samples are encoded as 16-bit signed values in the datastream
	uint16_t *samples = (uint16_t*)data;
	
	// Since each sample is 16-bit, need to halve length to get sample count
	uint8_t   sample_count = count / 2;
	
	// Hand off received samples to the board sample processor
	sample_processor_output_samples(samples, sample_count);	
}

/**
 * \mainpage ASF USB Device Audio 1.0
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device Audio 1.0
 * on AVR products with USB module.
 *
 * \section startup Startup
 * The example uses the speaker and amplifier available on the board.
 * After loading firmware, connect hardware board to the USB Host.
 * When connected to a USB host system this application provides an audio output
 * device in the Unix/Mac/Windows operating systems.
 *
 * \note
 * This example uses the native Audio driver on Unix/Mac/Windows OS.
 *
 * \copydoc UI 
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and Audio modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/audio/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>\subpage power_management
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (leds)
 */
