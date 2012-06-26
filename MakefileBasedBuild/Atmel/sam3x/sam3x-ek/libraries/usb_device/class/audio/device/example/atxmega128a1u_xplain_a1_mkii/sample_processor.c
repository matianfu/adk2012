/**
 * \file
 *
 * \brief Audio sample processing functions
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
#include "conf_example.h"
#include "conf_usb.h"
#include "gpio.h"
#include "sample_processor.h"
#include "dac.h"

void sample_processor_init(void)
{
	/* Create DAC configuration:
	 * - 1V from bandgap as reference, left adjusted channel value
	 * - one active DAC channel, no internal output
	 * - conversions triggered by event channel 0
	 * - 1 us conversion intervals
	 */
	struct dac_config dac_conf;
	dac_set_conversion_parameters(&dac_conf, DAC_REF_BANDGAP, DAC_ADJ_LEFT);
	dac_set_active_channel(&dac_conf, SPEAKER_DAC_CHANNEL, 0);
	dac_set_conversion_trigger(&dac_conf, SPEAKER_DAC_CHANNEL, 0);
	dac_set_conversion_interval(&dac_conf, 1);

	dac_write_configuration(&SPEAKER_DAC, &dac_conf);
	dac_enable(&SPEAKER_DAC);

	// Configure timer/counter to generate events at sample rate.
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC0);
	TCC0.PER = (sysclk_get_per_hz() / UDI_AUDIO_SAMPLE_RATE) - 1;

	// Configure event channel 0 to generate events upon T/C overflow.
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;

	// Start the timer/counter.
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;
}

void sample_processor_output_samples(uint16_t *samples, uint8_t count)
{
	// Process each sample in the datastream
	for (uint8_t i = 0; i < count; i += 2)
	{
		// Two channels (left and right) are encoded together to form a sample pair
		int16_t sample_left  = le16_to_cpu(samples[i]);
		int16_t sample_right = le16_to_cpu(samples[i + 1]);

		// Perform simple mono mixing of the two channels
		int16_t sample_mono = ((sample_left >> 1) + (sample_right >> 1));
		
		// Wait for the DAC to become ready to accept the next sample
		dac_wait_for_channel_ready(&SPEAKER_DAC, SPEAKER_DAC_CHANNEL);
		
		// Output mixed sample - output should be unsigned, so we have to offset half the
		// total range (flip highest bit) to get the correct sample value for the DAC
		dac_set_channel_value(&SPEAKER_DAC, SPEAKER_DAC_CHANNEL, sample_mono ^ (1 << 15));
	}
}
