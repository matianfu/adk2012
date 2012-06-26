/**
 * \file
 *
 * \brief USB Device Audio 1.0 Class interface definitions.
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

#ifndef _UDI_AUDIO_H_
#define _UDI_AUDIO_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_audio.h"
#include "udd.h"
#include "udc_desc.h"
#include "udi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup udi_group
 * \defgroup udi_msc_group UDI for Audio 1.0 Class
 *
 * @{
 */

//! Internal Audio driver buffer states
enum udi_audio_buff_states {
	AUDIO_BUFFER_IDLE,
	AUDIO_BUFFER_BUSY,
	AUDIO_BUFFER_FULL
};

/**
 * \name Interface Descriptor
 *
 * The following structures provide the interface descriptor.
 * It must be implemented in USB configuration descriptor.
 */
//@{

//! Interface descriptor struture for Audio control
typedef struct {
	usb_iface_desc_t iface;
	usb_audio_ifacecspc_desc_t iface_spc;
#ifdef UDI_AUDIO_ENABLE_AUDIO_IN
	usb_audio_input_terminal_desc_t interm_audio_in;
	usb_audio_feature_desc_t feature_unit;
	usb_audio_output_terminal_desc_t outterm_audio_in;
#endif
#ifdef UDI_AUDIO_ENABLE_AUDIO_OUT
	usb_audio_input_terminal_desc_t interm_audio_out;
	usb_audio_feature_desc_t feature_unit;
	usb_audio_output_terminal_desc_t outterm_audio_out;
#endif
} udi_audio_control_desc_t;

//! Interface descriptor struture for Audio streaming data
typedef struct {
	usb_iface_desc_t iface_alt0;
	usb_iface_desc_t iface_alt1;
	usb_audio_ifacedspc_desc_t iface_spc;
	usb_audio_format_desc_t audioformat;
#if defined(UDI_AUDIO_ENABLE_AUDIO_IN)
	usb_audio_ep_desc_t ep_audio_in;
	usb_audio_epspc_desc_t ep_audio_in_spc;
#endif
#if defined(UDI_AUDIO_ENABLE_AUDIO_OUT)
	usb_audio_ep_desc_t ep_audio_out;
	usb_audio_epspc_desc_t ep_audio_out_spc;
#endif
} udi_audio_data_desc_t;

//! By default no string associated to this interface
#ifndef UDI_AUDIO_STRING_ID
#define UDI_AUDIO_STRING_ID             0
#endif

#define  UDI_AUDIO_ENDPOINTS            1

//! Audio Streaming enpoints size for full speed
#define UDI_AUDIO_EPS_SIZE_FS           256
//! Audio enpoints size for high speed
#define UDI_AUDIO_EPS_SIZE_HS           256

#ifdef USB_DEVICE_HS_SUPPORT
	#define UDI_AUDIO_EPS_SIZE UDI_AUDIO_EPS_SIZE_HS
#else
	#define UDI_AUDIO_EPS_SIZE UDI_AUDIO_EPS_SIZE_FS
#endif

#if defined(UDI_AUDIO_ENABLE_AUDIO_IN) && defined(UDI_AUDIO_ENABLE_AUDIO_OUT)
	#error Audio driver must be configured with either UDI_AUDIO_ENABLE_AUDIO_IN or UDI_AUDIO_ENABLE_AUDIO_OUT, not both.
#else
	#define AUDIO_IN_TERMINAL_IN_ID     1
	#define AUDIO_IN_FEATURE_UNIT_ID    2
	#define AUDIO_IN_TERMINAL_OUT_ID    3
	#define AUDIO_OUT_TERMINAL_IN_ID    1
	#define AUDIO_OUT_FEATURE_UNIT_ID   2
	#define AUDIO_OUT_TERMINAL_OUT_ID   3
#endif


#ifdef UDI_AUDIO_ENABLE_AUDIO_IN
	#define UDI_AUDIO_CONTROL_IN_TERMINAL_DESC \
		.interm_audio_in.bLength            = sizeof(usb_audio_input_terminal_desc_t),\
		.interm_audio_in.bDescriptorType    = AUDIO_CS_INTERFACE,\
		.interm_audio_in.bDescriptorSubtype = AUDIO_CS_INTERFACE_TERMINAL_IN,\
		.interm_audio_in.bTerminalID        = AUDIO_IN_TERMINAL_IN_ID,\
		.interm_audio_in.wTerminalType      = LE16(AUDIO_TERMINAL_STREAMING),\
		.interm_audio_in.bAssocTerminal     = 0,\
		.interm_audio_in.bNrChannels        = UDI_AUDIO_SAMPLE_CHANNELS,\
		.interm_audio_in.wChannelConfig     = LE16(AUDIO_CHANNEL_CENTER_FRONT),\
		.interm_audio_in.iChannelNames      = 0,\
		.interm_audio_in.iTerminal          = 0,\
		.feature_unit.bLength               = sizeof(usb_audio_feature_desc_t),\
		.feature_unit.bDescriptorType       = AUDIO_CS_INTERFACE,\
		.feature_unit.bDescriptorSubtype    = AUDIO_CS_INTERFACE_FEATURE,\
		.feature_unit.bUnitID               = AUDIO_IN_FEATURE_UNIT_ID,\
		.feature_unit.bSourceID             = AUDIO_IN_TERMINAL_IN_ID,\
		.feature_unit.bControlSize          = 2,\
		.feature_unit.bmaControls           = {LE16(UDI_AUDIO_FEATURES)},\
		.feature_unit.iFeature              = 0,\
		.outterm_audio_in.bLength           = sizeof(usb_audio_output_terminal_desc_t),\
		.outterm_audio_in.bDescriptorType   = AUDIO_CS_INTERFACE,\
		.outterm_audio_in.bDescriptorSubtype = AUDIO_CS_INTERFACE_TERMINAL_OUT,\
		.outterm_audio_in.bTerminalID       = AUDIO_IN_TERMINAL_OUT_ID,\
		.outterm_audio_in.wTerminalType     = LE16(AUDIO_TERMINAL_IN_MIC),\
		.outterm_audio_in.bAssocTerminal    = 0,\
		.outterm_audio_in.bSourceID         = AUDIO_IN_TERMINAL_IN_ID,\
		.outterm_audio_in.iTerminal         = 0,

	#define UDI_AUDIO_CONTROL_IN_EP_DESC(interval) \
		.ep_audio_in.ep.bLength             = sizeof(usb_audio_ep_desc_t),\
		.ep_audio_in.ep.bDescriptorType     = USB_DT_ENDPOINT,\
		.ep_audio_in.ep.bEndpointAddress    = UDI_AUDIO_EP_IN,\
		.ep_audio_in.ep.bmAttributes        = (USB_EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_SYNCHRONOUS),\
		.ep_audio_in.ep.bInterval           = interval,\
		.ep_audio_in.bRefresh               = 0,\
		.ep_audio_in.bSynchAddress          = 0,\
		.ep_audio_in_spc.bLength            = sizeof(usb_audio_epspc_desc_t),\
		.ep_audio_in_spc.bDescriptorType    = AUDIO_CS_ENDPOINT,\
		.ep_audio_in_spc.bDescriptorSubtype = AUDIO_CS_INTERFACE_GENERAL,\
		.ep_audio_in_spc.bmAttributes       = 0,\
		.ep_audio_in_spc.bLockDelayUnits    = 0,\
		.ep_audio_in_spc.wLockDelay         = LE16(0),

	#define UDI_AUDIO_CONTROL_IN_EP_FS_DESC \
		UDI_AUDIO_CONTROL_IN_EP_DESC(1)\
		.ep_audio_in.ep.wMaxPacketSize = LE16(UDI_AUDIO_EPS_SIZE_FS),

	#define UDI_AUDIO_CONTROL_IN_EP_HS_DESC \
		UDI_AUDIO_CONTROL_IN_EP_DESC(4)\
		.ep_audio_in.ep.wMaxPacketSize = LE16(UDI_AUDIO_EPS_SIZE_HS),
#else
	#define UDI_AUDIO_CONTROL_IN_TERMINAL_DESC
	#define UDI_AUDIO_CONTROL_IN_EP_FS_DESC
	#define UDI_AUDIO_CONTROL_IN_EP_HS_DESC
#endif

#ifdef UDI_AUDIO_ENABLE_AUDIO_OUT
	#define UDI_AUDIO_CONTROL_OUT_TERMINAL_DESC \
		.interm_audio_out.bLength          = sizeof(usb_audio_input_terminal_desc_t),\
		.interm_audio_out.bDescriptorType  = AUDIO_CS_INTERFACE,\
		.interm_audio_out.bDescriptorSubtype = AUDIO_CS_INTERFACE_TERMINAL_IN,\
		.interm_audio_out.bTerminalID      = AUDIO_OUT_TERMINAL_IN_ID,\
		.interm_audio_out.wTerminalType    = LE16(AUDIO_TERMINAL_STREAMING),\
		.interm_audio_out.bAssocTerminal   = 0,\
		.interm_audio_out.bNrChannels      = UDI_AUDIO_SAMPLE_CHANNELS,\
		.interm_audio_out.wChannelConfig   = LE16(AUDIO_CHANNEL_LEFT_FRONT | AUDIO_CHANNEL_RIGHT_FRONT),\
		.interm_audio_out.iChannelNames    = 0,\
		.interm_audio_out.iTerminal        = 0,\
		.feature_unit.bLength              = sizeof(usb_audio_feature_desc_t),\
		.feature_unit.bDescriptorType      = AUDIO_CS_INTERFACE,\
		.feature_unit.bDescriptorSubtype   = AUDIO_CS_INTERFACE_FEATURE,\
		.feature_unit.bUnitID              = AUDIO_OUT_FEATURE_UNIT_ID,\
		.feature_unit.bSourceID            = AUDIO_OUT_TERMINAL_IN_ID,\
		.feature_unit.bControlSize         = 2,\
		.feature_unit.bmaControls          = {LE16(UDI_AUDIO_FEATURES)},\
		.feature_unit.iFeature             = 0,\
		.outterm_audio_out.bLength         = sizeof(usb_audio_output_terminal_desc_t),\
		.outterm_audio_out.bDescriptorType = AUDIO_CS_INTERFACE,\
		.outterm_audio_out.bDescriptorSubtype = AUDIO_CS_INTERFACE_TERMINAL_OUT,\
		.outterm_audio_out.bTerminalID     = AUDIO_OUT_TERMINAL_OUT_ID,\
		.outterm_audio_out.wTerminalType   = LE16(AUDIO_TERMINAL_OUT_SPEAKER),\
		.outterm_audio_out.bAssocTerminal  = 0,\
		.outterm_audio_out.bSourceID       = AUDIO_OUT_FEATURE_UNIT_ID,\
		.outterm_audio_out.iTerminal       = 0,

	#define UDI_AUDIO_CONTROL_OUT_EP_DESC(interval) \
		.ep_audio_out.ep.bLength           = sizeof(usb_audio_ep_desc_t),\
		.ep_audio_out.ep.bDescriptorType   = USB_DT_ENDPOINT,\
		.ep_audio_out.ep.bEndpointAddress  = UDI_AUDIO_EP_OUT,\
		.ep_audio_out.ep.bmAttributes      = (USB_EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_SYNCHRONOUS),\
		.ep_audio_out.ep.bInterval         = interval,\
		.ep_audio_out.bRefresh             = 0,\
		.ep_audio_out.bSynchAddress        = 0,\
		.ep_audio_out_spc.bLength          = sizeof(usb_audio_epspc_desc_t),\
		.ep_audio_out_spc.bDescriptorType  = AUDIO_CS_ENDPOINT,\
		.ep_audio_out_spc.bDescriptorSubtype = AUDIO_CS_INTERFACE_GENERAL,\
		.ep_audio_out_spc.bmAttributes     = 0,\
		.ep_audio_out_spc.bLockDelayUnits  = 0,\
		.ep_audio_out_spc.wLockDelay       = LE16(0),

	#define UDI_AUDIO_CONTROL_OUT_EP_FS_DESC \
		UDI_AUDIO_CONTROL_OUT_EP_DESC(1) \
		.ep_audio_out.ep.wMaxPacketSize = LE16(UDI_AUDIO_EPS_SIZE_FS),

	#define UDI_AUDIO_CONTROL_OUT_EP_HS_DESC \
		UDI_AUDIO_CONTROL_OUT_EP_DESC(4) \
		.ep_audio_out.ep.wMaxPacketSize = LE16(UDI_AUDIO_EPS_SIZE_HS),
#else
	#define UDI_AUDIO_CONTROL_OUT_TERMINAL_DESC
	#define UDI_AUDIO_CONTROL_OUT_EP_FS_DESC
	#define UDI_AUDIO_CONTROL_OUT_EP_HS_DESC
#endif

//! Content of Audio Control interface descriptor for all speeds
#define UDI_AUDIO_CONTROL_DESC  {\
   .iface.bLength                  = sizeof(usb_iface_desc_t),\
   .iface.bDescriptorType          = USB_DT_INTERFACE,\
   .iface.bInterfaceNumber         = UDI_AUDIO_IFACE_CONTROL_NUMBER,\
   .iface.bAlternateSetting        = 0,\
   .iface.bNumEndpoints            = 0,\
   .iface.bInterfaceClass          = AUDIO_CLASS,\
   .iface.bInterfaceSubClass       = AUDIO_SUBCLASS_CONTROL,\
   .iface.bInterfaceProtocol       = 0,\
   .iface.iInterface               = UDI_AUDIO_STRING_ID,\
   .iface_spc.bLength              = sizeof(usb_audio_ifacecspc_desc_t),\
   .iface_spc.bDescriptorType      = AUDIO_CS_INTERFACE,\
   .iface_spc.bDescriptorSubtype   = AUDIO_CS_INTERFACE_HEADER,\
   .iface_spc.bcdADC               = LE16(0x0100),\
   .iface_spc.wTotalLength         = LE16(sizeof(udi_audio_control_desc_t) - sizeof(usb_iface_desc_t)),\
   .iface_spc.bInCollection        = 1,\
   .iface_spc.bInterfaceNumbers    = UDI_AUDIO_IFACE_DATA_NUMBER,\
   UDI_AUDIO_CONTROL_IN_TERMINAL_DESC \
   UDI_AUDIO_CONTROL_OUT_TERMINAL_DESC \
   }

//! Content of Audio Control interface descriptor for full speed
#define UDI_AUDIO_DATA_DESC_FS {\
   .iface_alt0.bLength             = sizeof(usb_iface_desc_t),\
   .iface_alt0.bDescriptorType     = USB_DT_INTERFACE,\
   .iface_alt0.bInterfaceNumber    = UDI_AUDIO_IFACE_DATA_NUMBER,\
   .iface_alt0.bAlternateSetting   = 0,\
   .iface_alt0.bNumEndpoints       = 0,\
   .iface_alt0.bInterfaceClass     = AUDIO_CLASS,\
   .iface_alt0.bInterfaceSubClass  = AUDIO_SUBCLASS_STREAMING,\
   .iface_alt0.bInterfaceProtocol  = 0,\
   .iface_alt0.iInterface          = 0,\
   .iface_alt1.bLength             = sizeof(usb_iface_desc_t),\
   .iface_alt1.bDescriptorType     = USB_DT_INTERFACE,\
   .iface_alt1.bInterfaceNumber    = UDI_AUDIO_IFACE_DATA_NUMBER,\
   .iface_alt1.bAlternateSetting   = 1,\
   .iface_alt1.bNumEndpoints       = UDI_AUDIO_ENDPOINTS,\
   .iface_alt1.bInterfaceClass     = AUDIO_CLASS,\
   .iface_alt1.bInterfaceSubClass  = AUDIO_SUBCLASS_STREAMING,\
   .iface_alt1.bInterfaceProtocol  = 0,\
   .iface_alt1.iInterface          = 0,\
   .iface_spc.bLength              = sizeof(usb_audio_ifacedspc_desc_t),\
   .iface_spc.bDescriptorType      = AUDIO_CS_INTERFACE,\
   .iface_spc.bDescriptorSubtype   = AUDIO_CS_INTERFACE_GENERAL,\
   .iface_spc.bTerminalLink        = 0x01,\
   .iface_spc.bDelay               = 1,\
   .iface_spc.wFormatTag           = LE16(0x0001),\
   .audioformat.bLength            = sizeof(usb_audio_format_desc_t),\
   .audioformat.bDescriptorType    = AUDIO_CS_INTERFACE,\
   .audioformat.bDescriptorSubtype = AUDIO_CS_INTERFACE_FORMAT,\
   .audioformat.bFormatType        = 0x01,\
   .audioformat.bNrChannels        = UDI_AUDIO_SAMPLE_CHANNELS,\
   .audioformat.bSubFrameSize      = UDI_AUDIO_SAMPLE_SIZE_BYTES,\
   .audioformat.bBitResolution     = UDI_AUDIO_SAMPLE_BITS,\
   .audioformat.bSampleFrequencyType = 1,\
   .audioformat.SampleFrequencies  = {UDI_AUDIO_SAMPLE_RATE & 0xFF, UDI_AUDIO_SAMPLE_RATE >> 8, UDI_AUDIO_SAMPLE_RATE >> 16},\
   UDI_AUDIO_CONTROL_IN_EP_FS_DESC \
   UDI_AUDIO_CONTROL_OUT_EP_FS_DESC \
   }

//! Content of Audio Control interface descriptor for high speed
#define UDI_AUDIO_DATA_DESC_HS {\
   .iface_alt0.bLength             = sizeof(usb_iface_desc_t),\
   .iface_alt0.bDescriptorType     = USB_DT_INTERFACE,\
   .iface_alt0.bInterfaceNumber    = UDI_AUDIO_IFACE_DATA_NUMBER,\
   .iface_alt0.bAlternateSetting   = 0,\
   .iface_alt0.bNumEndpoints       = 0,\
   .iface_alt0.bInterfaceClass     = AUDIO_CLASS,\
   .iface_alt0.bInterfaceSubClass  = AUDIO_SUBCLASS_STREAMING,\
   .iface_alt0.bInterfaceProtocol  = 0,\
   .iface_alt0.iInterface          = 0,\
   .iface_alt1.bLength             = sizeof(usb_iface_desc_t),\
   .iface_alt1.bDescriptorType     = USB_DT_INTERFACE,\
   .iface_alt1.bInterfaceNumber    = UDI_AUDIO_IFACE_DATA_NUMBER,\
   .iface_alt1.bAlternateSetting   = 1,\
   .iface_alt1.bNumEndpoints       = UDI_AUDIO_ENDPOINTS,\
   .iface_alt1.bInterfaceClass     = AUDIO_CLASS,\
   .iface_alt1.bInterfaceSubClass  = AUDIO_SUBCLASS_STREAMING,\
   .iface_alt1.bInterfaceProtocol  = 0,\
   .iface_alt1.iInterface          = 0,\
   .iface_spc.bLength              = sizeof(usb_audio_ifacedspc_desc_t),\
   .iface_spc.bDescriptorType      = AUDIO_CS_INTERFACE,\
   .iface_spc.bDescriptorSubtype   = AUDIO_CS_INTERFACE_GENERAL,\
   .iface_spc.bTerminalLink        = 0x01,\
   .iface_spc.bDelay               = 1,\
   .iface_spc.wFormatTag           = LE16(0x0001),\
   .audioformat.bLength            = sizeof(usb_audio_format_desc_t),\
   .audioformat.bDescriptorType    = AUDIO_CS_INTERFACE,\
   .audioformat.bDescriptorSubtype = AUDIO_CS_INTERFACE_FORMAT,\
   .audioformat.bFormatType        = 0x01,\
   .audioformat.bNrChannels        = 2,\
   .audioformat.bSubFrameSize      = 2,\
   .audioformat.bBitResolution     = 16,\
   .audioformat.bSampleFrequencyType = 1,\
   .audioformat.SampleFrequencies  = {48000 & 0xFF, 48000 >> 8, 48000 >> 16},\
   UDI_AUDIO_CONTROL_IN_EP_HS_DESC \
   UDI_AUDIO_CONTROL_OUT_EP_HS_DESC \
   }

//! Global struture which contains standard UDI interface for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_audio_control;

//! Global struture which contains standard UDI interface for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_audio_data;

/**
 * \brief Transfers sample data to the USB Audio IN endpoint
 *
 *
 * \param buffer        Buffer of stored samples to send to the host
 * \param length        Length of the sample buffer to send
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_audio_stream_samples_send(uint8_t* buffer, uint8_t length);

/**
 * \brief Processes any pending audio sample buffers.
 * Processes pending sample buffers, sending/receiving data between the USB
 * host and the device, and executing the user callback for filled sample
 * buffers when in audio output mode.
 */
void udi_audio_process(void);

/**
 * \brief Sets the minimum value for a given feature.
 *
 * \param feature  Feature to alter, an AUDIO_FEATURE_* mask
 * \param value    Minuimum allowable value for the given feature
 */
void udi_audio_stream_set_feature_min(uint16_t feature, uint16_t value);

/**
 * \brief Sets the maximum value for a given feature.
 *
 * \param feature  Feature to alter, an AUDIO_FEATURE_* mask
 * \param value    Maximum allowable value for the given feature
 */
void udi_audio_stream_set_feature_max(uint16_t feature, uint16_t value);

/**
 * \brief Sets the resolution value for a given feature.
 *
 * \param feature  Feature to alter, an AUDIO_FEATURE_* mask
 * \param value    New resolution value for the given feature
 */
void udi_audio_stream_set_feature_res(uint16_t feature, uint16_t value);

/**
 * \brief Sets the current value for a given feature.
 *
 * \param feature  Feature to alter, an AUDIO_FEATURE_* mask
 * \param value    New current value for the given feature
 */
void udi_audio_stream_set_feature_cur(uint16_t feature, uint16_t value);

/**
 * \brief Gets the currently set value for a given feature.
 *
 * \param feature  Feature to retrieve, an AUDIO_FEATURE_* mask
 *
 * \retval Current value for the given feature
 */
uint16_t udi_audio_stream_get_feature_cur(uint16_t feature);

//@}

#ifdef __cplusplus
}
#endif
#endif // _UDI_AUDIO_H_
