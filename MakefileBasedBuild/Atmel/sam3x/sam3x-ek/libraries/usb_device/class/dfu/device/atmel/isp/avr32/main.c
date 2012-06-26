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

#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "sysclk.h"
#include "conf_usb.h"
#include "udd.h"
#include "udc.h"
#include "udi_dfu_atmel.h"
#include "isp.h"
#if UC3D
#include "gpio.h"
#endif

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
#if UC3D
	static const gpio_map_t USB_GPIO_MAP =
	{
		{AVR32_USBC_DP_PIN, AVR32_USBC_DP_FUNCTION},
		{AVR32_USBC_DM_PIN, AVR32_USBC_DM_FUNCTION},
		{AVR32_USBC_VBUS_PIN, AVR32_USBC_VBUS_FUNCTION}
	};
  
	// Assign GPIO pins to USB.
	gpio_enable_module(USB_GPIO_MAP,
            sizeof(USB_GPIO_MAP) / sizeof(USB_GPIO_MAP[0]));
#endif  
	cpu_irq_enable();

	sysclk_init();

	// Start USB stack to authorize VBus monitoring
	udc_start();

	if (!udc_include_vbus_monitoring()) {
		// VBUS monitoring is not available on this product
		// thereby VBUS has to be considered as present
		main_vbus_action(true);
	}

	while (true) {
	}
}

void main_vbus_action(bool b_high)
{
	if (b_high) {
		// Attach USB Device
		udc_attach();
	}else{
		// Detach USB Device
		udc_detach();
	}
}


/**
 * \mainpage ASF USB Device DFU
 *
 * TODO
 */
