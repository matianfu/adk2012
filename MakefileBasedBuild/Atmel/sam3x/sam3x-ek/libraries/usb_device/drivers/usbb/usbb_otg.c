/**
 * \file
 *
 * \brief USB Otg drivers
 * Compliance with common driver OTG
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

#include "conf_usb.h"
#include "sysclk.h"
#include "udd.h"
#include "usbb_otg.h"
#include <string.h>

#ifndef UDD_USB_INT_LEVEL
#  define UDD_USB_INT_LEVEL 0	// By default USB interrupt have low priority
#endif

#ifdef OTG

/**
 * \ingroup usb_group
 * \defgroup otg_group USB OTG Driver (OTG)
 * @{
 */

//! Check that multiplexed pin used for USB_ID is defined
#ifndef OTG_ID
#error YOU MUST define in your board header file the multiplexed pin used for OTG_ID as AVR32_USBB_USB_ID_x_x
#endif

/**
 * \internal
 * \brief The USBB ISR is managed by OTG layer which call the USB mode actived (device/host).
 */
ISR(otg_interrupt, AVR32_USBB_IRQ_GROUP, UDD_USB_INT_LEVEL)
{
	if (Is_otg_id_transition() && Is_otg_id_interrupt_enabled()) {
		// UID pin change
		otg_ack_id_transition();
		USB_UID_EVENT(Is_otg_id_device());
		return;
	}
	// Call sub interrrupt routine correesponding at current mode
	if (Is_otg_id_device())
		udd_interrupt();
	else
		uhd_interrupt();

	otg_data_memory_barrier();
}

//@}

void otg_enable(void)
{
	// Link the USBB interrupt on otg_interrupt()
	irq_register_handler(otg_interrupt, AVR32_USBB_IRQ, UDD_USB_INT_LEVEL);
	// Enable UID control
	otg_input_id_pin();
	otg_enable_id_pin();
	otg_raise_id_transition();	// Check no ID transition has been missed during initialization
	otg_enable_id_interrupt();
}

#endif // define OTG

//@}
