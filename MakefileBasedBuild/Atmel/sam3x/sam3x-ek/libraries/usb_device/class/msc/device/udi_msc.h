/**
 * \file
 *
 * \brief USB Device Mass Storage Class (MSC) interface definitions.
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

#ifndef _UDI_MSC_H_
#define _UDI_MSC_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_msc.h"
#include "udd.h"
#include "udc_desc.h"
#include "udi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup udi_group
 * \defgroup udi_msc_group UDI for Mass Storage Class
 *
 * @{
 */

/**
 * \name Interface Descriptor
 *
 * The following structures provide the interface descriptor.
 * It must be implemented in USB configuration descriptor.
 */
//@{

//! Interface descriptor struture for MSC
typedef struct {
	usb_iface_desc_t iface;
	usb_ep_desc_t ep_in;
	usb_ep_desc_t ep_out;
} udi_msc_desc_t;

//! By default no string associated to this interface
#ifndef UDI_MSC_STRING_ID
#define UDI_MSC_STRING_ID     0
#endif

//! MSC enpoints size for full speed
#define UDI_MSC_EPS_SIZE_FS   64
//! MSC enpoints size for high speed
#define UDI_MSC_EPS_SIZE_HS   512

//! Content of MSC interface descriptor for all speeds
#define UDI_MSC_DESC      \
   .iface.bLength             = sizeof(usb_iface_desc_t),\
   .iface.bDescriptorType     = USB_DT_INTERFACE,\
   .iface.bInterfaceNumber    = UDI_MSC_IFACE_NUMBER,\
   .iface.bAlternateSetting   = 0,\
   .iface.bNumEndpoints       = 2,\
   .iface.bInterfaceClass     = MSC_CLASS,\
   .iface.bInterfaceSubClass  = MSC_SUBCLASS_TRANSPARENT,\
   .iface.bInterfaceProtocol  = MSC_PROTOCOL_BULK,\
   .iface.iInterface          = UDI_MSC_STRING_ID,\
   .ep_in.bLength             = sizeof(usb_ep_desc_t),\
   .ep_in.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.bEndpointAddress    = UDI_MSC_EP_IN,\
   .ep_in.bmAttributes        = USB_EP_TYPE_BULK,\
   .ep_in.bInterval           = 0,\
   .ep_out.bLength            = sizeof(usb_ep_desc_t),\
   .ep_out.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.bEndpointAddress   = UDI_MSC_EP_OUT,\
   .ep_out.bmAttributes       = USB_EP_TYPE_BULK,\
   .ep_out.bInterval          = 0,

//! Content of MSC interface descriptor for full speed only
#define UDI_MSC_DESC_FS   {\
   UDI_MSC_DESC \
   .ep_in.wMaxPacketSize      = LE16(UDI_MSC_EPS_SIZE_FS),\
   .ep_out.wMaxPacketSize     = LE16(UDI_MSC_EPS_SIZE_FS),\
   }

//! Content of MSC interface descriptor for high speed only
#define UDI_MSC_DESC_HS   {\
   UDI_MSC_DESC \
   .ep_in.wMaxPacketSize      = LE16(UDI_MSC_EPS_SIZE_HS),\
   .ep_out.wMaxPacketSize     = LE16(UDI_MSC_EPS_SIZE_HS),\
   }
//@}


//! Global struture which contains standard UDI interface for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_msc;

/**
 * \name Interface for application
 *
 * These routines are used by memory to transfer its data
 * to/from USB MSC endpoints.
 */
//@{

/**
 * \brief Process the background read/write commands
 *
 * Routine called by the main loop
 */
void udi_msc_process_trans(void);

/**
 * \brief Transfers data to/from USB MSC endpoints
 *
 *
 * \param b_read        Memory to USB, if true
 * \param block         Buffer on Internal RAM to send or fill
 * \param block_size    Buffer size to send or fill
 * \param callback      Function to call at the end of transfer.
 *                      If NULL then the routine exit when transfer is finish.
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_msc_trans_block(bool b_read, uint8_t * block, iram_size_t block_size,
		void (*callback) (udd_ep_status_t status, iram_size_t n));

//@}

//@}

#ifdef __cplusplus
}
#endif
#endif // _UDI_MSC_H_
