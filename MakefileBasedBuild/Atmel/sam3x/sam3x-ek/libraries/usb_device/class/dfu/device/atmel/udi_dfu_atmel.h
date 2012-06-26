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

#ifndef _UDI_DFU_ATMEL_H_
#define _UDI_DFU_ATMEL_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_dfu.h"
#include "usb_atmel_dfu.h"
#include "udd.h"
#include "udc_desc.h"
#include "udi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup udi_group
 * \defgroup udi_dfu_atmel_group UDI for Device Firmware Upgrade Atmel specific
 *
 * The DFU from Atmel is based on DFU specification,
 * but does not implement all features.
 * @{
 */

/**
 * \name Interface Descriptor
 *
 * The following structures provide the interface descriptor.
 * It must be implemented in USB configuration descriptor.
 */
//@{

//! Interface descriptor structure for DFU Atmel
typedef struct {
	usb_iface_desc_t iface;
} udi_dfu_atmel_desc_t;

//! By default no string associated to this interface
#ifndef UDI_DFU_ATMEL_STRING_ID
#define UDI_DFU_ATMEL_STRING_ID     0
#endif

//! Content of DFU interface descriptor for all speeds
#define UDI_DFU_ATMEL_DESC      {\
   .iface.bLength             = sizeof(usb_iface_desc_t),\
   .iface.bDescriptorType     = USB_DT_INTERFACE,\
   .iface.bInterfaceNumber    = UDI_DFU_ATMEL_IFACE_NUMBER,\
   .iface.bAlternateSetting   = 0,\
   .iface.bNumEndpoints       = 0,\
   .iface.bInterfaceClass     = NO_CLASS,\
   .iface.bInterfaceSubClass  = NO_SUBCLASS,\
   .iface.bInterfaceProtocol  = NO_PROTOCOL,\
   .iface.iInterface          = UDI_DFU_ATMEL_STRING_ID,\
   }
//@}


//! Global structure which contains standard UDI API for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_dfu_atmel;

//@}

#ifdef __cplusplus
}
#endif
#endif // _UDI_DFU_ATMEL_H_
