/**
 * \file
 *
 * \brief USB Device Firmware Upgrade (DFU) interface definitions.
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
#include "usb_protocol.h"
#include "usb_protocol_dfu.h"
#include "udd.h"
#include "udc.h"
#include "udi_dfu.h"

/**
 * \addtogroup udi_dfu_group
 * @{
 */

/**
 * \name Interface for UDC
 */
//@{

bool udi_dfu_enable(void);
void udi_dfu_disable(void);
bool udi_dfu_setup(void);
uint8_t udi_dfu_getsetting(void);

//! Global structure which contains standard UDI API for UDC
UDC_DESC_STORAGE udi_api_t udi_api_dfu = {
	.enable = udi_dfu_enable,
	.disable = udi_dfu_disable,
	.setup = udi_dfu_setup,
	.getsetting = udi_dfu_getsetting,
};

//@}

/**
 * \name Variables to manage DFU status
 */
//@{
static dfu_status_t dfu_status;
//@}

//@}


bool udc_dfu_enable(void)
{
	return UDI_DFU_ENABLE_EXT();
}


void udc_dfu_disable(void)
{
	UDI_DFU_DISABLE_EXT();
}

bool udc_dfu_setup(void)
{

	//** Interface requests
	if (Udd_setup_type() != USB_REQ_TYPE_CLASS) {
		return false;	// Only class request decoded
	}
// TODO: use state of DFU specification
	if (Udd_setup_is_in()) {
		// Requests Class Interface Get
		switch (udd_g_ctrlreq.req.bRequest) {
		case USB_REQ_DFU_GETSTATUS:
			if (0 != udd_g_ctrlreq.req.wValue)
				break;
			if (sizeof(udi_dfu_status) !=
					udd_g_ctrlreq.req.wLength)
				break;
			udd_g_ctrlreq.payload =
					(uint8_t *) & udi_dfu_status;
			udd_g_ctrlreq.payload_size =
					sizeof(udi_dfu_status);
			return true;

		case USB_REQ_DFU_GETSTATE:
			if (0 != udd_g_ctrlreq.req.wValue)
				break;
			if (sizeof(udi_dfu_status.bState) !=
					udd_g_ctrlreq.req.wLength)
				break;
			udd_g_ctrlreq.payload =
					(uint8_t *) &
					(udi_dfu_status.bState);
			udd_g_ctrlreq.payload_size =
					sizeof(udi_dfu_status.bState);
			return true;

		case USB_REQ_DFU_UPLOAD:
			break;
		}
	}


	if (Udd_setup_is_out()) {
		// Requests Class Interface Set
		switch (udd_g_ctrlreq.req.bRequest) {
		case USB_REQ_DFU_ABORT:
			break;
		case USB_REQ_DFU_CLRSTATUS:
			if (0 != udd_g_ctrlreq.req.wValue)
				break;
			if (0 != udd_g_ctrlreq.req.wLength)
				break;
			udi_dfu_clear_status();
			return true;

		case USB_REQ_DFU_DNLOAD:
			break;
		}
	}
	// Unknown request
	udi_dfu_status.bStatus = DFU_STATUS_ERRSTALLEDPK;
	udi_dfu_status.bState = DFU_STATE_DFUERROR;
	return false;
}


uint8_t udi_dfu_getsetting(void)
{
	return 0;
}


static void udi_dfu_clear_status(void)
{
	// Reset DFU status
	udi_dfu_status.bStatus = DFU_STATUS_OK;
	udi_dfu_status.bState = DFU_STATE_DFUIDLE;
	// These fields are not used and always set to zero:
	// bwPollTimeout[3]
	// iString
}

