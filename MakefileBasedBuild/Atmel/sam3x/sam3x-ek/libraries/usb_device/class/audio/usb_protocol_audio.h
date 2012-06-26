/**
 * \file
 *
 * \brief USB Audio Class 1.0 protocol definitions.
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

#ifndef _USB_PROTOCOL_AUDIO_H_
#define _USB_PROTOCOL_AUDIO_H_

/**
 * \ingroup usb_protocol_group
 * \defgroup usb_audio_protocol USB Audio Class protocol definitions
 *
 * @{
 */

/**
 * \name Possible Class value
 */
//@{
#define AUDIO_CLASS                       0x01
//@}

//! \name USB Audio Subclass IDs
//@{
#define AUDIO_SUBCLASS_CONTROL            0x01
#define AUDIO_SUBCLASS_STREAMING          0x02
//@}

//! \name USB Audio Functional Descriptor Types
//@{
#define AUDIO_CS_INTERFACE                0x24	//!< Interface Functional Descriptor
#define AUDIO_CS_ENDPOINT                 0x25	//!< Endpoint Functional Descriptor
//@}

//! \name USB Audio Control Functional Descriptor Subtypes
//@{
#define AUDIO_CS_INTERFACE_HEADER         0x01
#define AUDIO_CS_INTERFACE_TERMINAL_IN    0x02
#define AUDIO_CS_INTERFACE_TERMINAL_OUT   0x03
#define AUDIO_CS_INTERFACE_MIXER          0x04
#define AUDIO_CS_INTERFACE_SELECTER       0x05
#define AUDIO_CS_INTERFACE_FEATURE        0x06
#define AUDIO_CS_INTERFACE_PROCESSING     0x07
#define AUDIO_CS_INTERFACE_EXTENSION      0x08
#define AUDIO_CS_INTERFACE_FORMAT         0x02
//@}

//! \name USB Audio Streaming Functional Descriptor Subtypes
//@{
#define AUDIO_CS_INTERFACE_GENERAL        0x01
#define AUDIO_CS_INTERFACE_FORMATSPECIFIC 0x03
//@}

//! \name USB Audio Terminal Descriptor Types
//@{
#define AUDIO_TERMINAL_UNDEFINED           0x0100
#define AUDIO_TERMINAL_STREAMING           0x0101
#define AUDIO_TERMINAL_VENDOR              0x01FF
#define AUDIO_TERMINAL_IN_UNDEFINED        0x0200
#define AUDIO_TERMINAL_IN_MIC              0x0201
#define AUDIO_TERMINAL_IN_DESKTOP_MIC      0x0202
#define AUDIO_TERMINAL_IN_PERSONAL_MIC     0x0203
#define AUDIO_TERMINAL_IN_OMNIDIR_MIC      0x0204
#define AUDIO_TERMINAL_IN_MIC_ARRAY        0x0205
#define AUDIO_TERMINAL_IN_PROCESSING_MIC   0x0206
#define AUDIO_TERMINAL_IN_OUT_UNDEFINED    0x0300
#define AUDIO_TERMINAL_OUT_SPEAKER         0x0301
#define AUDIO_TERMINAL_OUT_HEADPHONES      0x0302
#define AUDIO_TERMINAL_OUT_HEAD_MOUNTED    0x0303
#define AUDIO_TERMINAL_OUT_DESKTOP         0x0304
#define AUDIO_TERMINAL_OUT_ROOM            0x0305
#define AUDIO_TERMINAL_OUT_COMMUNICATION   0x0306
#define AUDIO_TERMINAL_OUT_LOWFREQ         0x0307
//@}

//! \name USB Audio Channel Types
//@{
#define AUDIO_CHANNEL_LEFT_FRONT           (1 << 0)
#define AUDIO_CHANNEL_RIGHT_FRONT          (1 << 1)
#define AUDIO_CHANNEL_CENTER_FRONT         (1 << 2)
#define AUDIO_CHANNEL_LOW_FREQ_ENHANCE     (1 << 3)
#define AUDIO_CHANNEL_LEFT_SURROUND        (1 << 4)
#define AUDIO_CHANNEL_RIGHT_SURROUND       (1 << 5)
#define AUDIO_CHANNEL_LEFT_OF_CENTER       (1 << 6)
#define AUDIO_CHANNEL_RIGHT_OF_CENTER      (1 << 7)
#define AUDIO_CHANNEL_SURROUND             (1 << 8)
#define AUDIO_CHANNEL_SIDE_LEFT            (1 << 9)
#define AUDIO_CHANNEL_SIDE_RIGHT           (1 << 10)
#define AUDIO_CHANNEL_TOP                  (1 << 11)
//@}

//! \name Audio Feature Unit Control Indexes
//@{
#define AUDIO_CONTROL_MUTE                 1
#define AUDIO_CONTROL_VOLUME               2
#define AUDIO_CONTROL_BASS                 3
#define AUDIO_CONTROL_MID                  4
#define AUDIO_CONTROL_TREBLE               5
#define AUDIO_CONTROL_GRAPHIC_EQ           6
#define AUDIO_CONTROL_AUTO_GAIN            7
#define AUDIO_CONTROL_DELAY                8
#define AUDIO_CONTROL_BASS_BOOST           9
#define AUDIO_CONTROL_LOUDNESS             A
//@}

//! \name Audio Feature Unit Control Masks
//@{
#define AUDIO_FEATURE_MUTE                 (1 << 0)
#define AUDIO_FEATURE_VOLUME               (1 << 1)
#define AUDIO_FEATURE_BASS                 (1 << 2)
#define AUDIO_FEATURE_MID                  (1 << 3)
#define AUDIO_FEATURE_TREBLE               (1 << 4)
#define AUDIO_FEATURE_GRAPHIC_EQ           (1 << 5)
#define AUDIO_FEATURE_AUTO_GAIN            (1 << 6)
#define AUDIO_FEATURE_DELAY                (1 << 7)
#define AUDIO_FEATURE_BASS_BOOST           (1 << 8)
#define AUDIO_FEATURE_LOUDNESS             (1 << 9)
//@}

//! \name Standard Endpoint Descriptor Types
//@{
#define ENDPOINT_ATTR_NO_SYNC              (0x00 << 2)
#define ENDPOINT_ATTR_ASYNCHRONOUS         (0x01 << 2)
#define ENDPOINT_ATTR_ADAPTIVE             (0x02 << 2)
#define ENDPOINT_ATTR_SYNCHRONOUS          (0x03 << 2)
//@}

//! \name USB CDC Request IDs
//@{
#define USB_REQ_AUDIO_SET_CUR              0x01
#define USB_REQ_AUDIO_SET_MIN              0x02
#define USB_REQ_AUDIO_SET_MAX              0x03
#define USB_REQ_AUDIO_SET_RES              0x04
#define USB_REQ_AUDIO_SET_MEM              0x05
#define USB_REQ_AUDIO_GET_CUR              0x81
#define USB_REQ_AUDIO_GET_MIN              0x82
#define USB_REQ_AUDIO_GET_MAX              0x83
#define USB_REQ_AUDIO_GET_RES              0x84
#define USB_REQ_AUDIO_GET_MEM              0x85
//@}


/*
 * Need to pack structures tightly, or the compiler might insert padding
 * and violate the spec-mandated layout.
 */
COMPILER_PACK_SET(1);

//! \name USB CDC Descriptors
//@{

//! Audio Header Class Specific Control Interface Descriptor
typedef struct {
	uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
	uint16_t bcdADC;
	uint16_t wTotalLength;
	uint8_t  bInCollection;
	uint8_t  bInterfaceNumbers;
} usb_audio_ifacecspc_desc_t;

//! Audio Header Class Specific Feature Unit Descriptor
typedef struct {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint8_t  bDescriptorSubtype;
	uint8_t  bUnitID;
	uint8_t  bSourceID;
	uint8_t  bControlSize;
	uint16_t bmaControls[1];
	uint8_t  iFeature;
} usb_audio_feature_desc_t;

//! Audio Header Class Specific Input Terminal Descriptor
typedef struct {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint8_t  bDescriptorSubtype;
	uint8_t  bTerminalID;
	uint16_t wTerminalType;
	uint8_t  bAssocTerminal;
	uint8_t  bNrChannels;
	uint16_t wChannelConfig;
	uint8_t  iChannelNames;
	uint8_t  iTerminal;
} usb_audio_input_terminal_desc_t;

//! Audio Header Class Specific Output Terminal Descriptor
typedef struct {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint8_t  bDescriptorSubtype;
	uint8_t  bTerminalID;
	uint16_t wTerminalType;
	uint8_t  bAssocTerminal;
	uint8_t  bSourceID;
	uint8_t  iTerminal;
} usb_audio_output_terminal_desc_t;

//! Audio Header Class Specific Streaming Interface Descriptor
typedef struct {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint8_t  bDescriptorSubtype;
	uint8_t  bTerminalLink;
	uint8_t  bDelay;
	uint16_t wFormatTag;
} usb_audio_ifacedspc_desc_t;

//! Audio Header Class Specific Audio Format Descriptor
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bFormatType;
	uint8_t bNrChannels;
	uint8_t bSubFrameSize;
	uint8_t bBitResolution;
	uint8_t bSampleFrequencyType;
	uint8_t SampleFrequencies[1 * 3];
} usb_audio_format_desc_t;

//! Audio Header Class Specific Extended Endpoint Descriptor
typedef struct {
	usb_ep_desc_t ep;
	uint8_t bRefresh;
	uint8_t bSynchAddress;
} usb_audio_ep_desc_t;

//! Audio Header Class Specific Endpoint Descriptor
typedef struct {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint8_t  bDescriptorSubtype;
	uint8_t  bmAttributes;
	uint8_t  bLockDelayUnits;
	uint16_t wLockDelay;
} usb_audio_epspc_desc_t;

//@}

COMPILER_PACK_RESET();

//@}

#endif // _USB_PROTOCOL_AUDIO_H_
