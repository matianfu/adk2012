/**
 * \file
 *
 * \brief Default Audio 1.0 configuration for a USB Device
 * with a single interface MSC
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

#ifndef _UDI_AUDIO_CONF_H_
#define _UDI_AUDIO_CONF_H_

/**
 * \ingroup udi_audio_group
 * \defgroup udi_audio_group_conf Default Audio 1.0 configuration for a USB Device
 *
 * @{
 */

//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE          64

//! Audio stream direction configuration
//#define  UDI_AUDIO_ENABLE_AUDIO_IN
#define  UDI_AUDIO_ENABLE_AUDIO_OUT

//! Audio stream format configuration
//@{
#define UDI_AUDIO_SAMPLE_RATE             48000
#define UDI_AUDIO_SAMPLE_BITS             16
#define UDI_AUDIO_SAMPLE_SIZE_BYTES       2
#define UDI_AUDIO_SAMPLE_CHANNELS         2
//@}

//! Audio stream feature configuration
#define UDI_AUDIO_FEATURES                AUDIO_FEATURE_VOLUME | AUDIO_FEATURE_MUTE
#define UDI_AUDIO_MUTE_FEATURE_DEFAULT    false
#define UDI_AUDIO_VOLUME_FEATURE_MIN      0x0000
#define UDI_AUDIO_VOLUME_FEATURE_MAX      0x7FFF
#define UDI_AUDIO_VOLUME_FEATURE_DEFAULT  0x7FFF

//! Endpoint numbers used by Audio interface
#define  UDI_AUDIO_EP_IN                  (5 | USB_EP_DIR_IN)
#define  UDI_AUDIO_EP_OUT                 (6 | USB_EP_DIR_OUT)

#define  UDI_AUDIO_IFACE_CONTROL_NUMBER   0
#define  UDI_AUDIO_IFACE_DATA_NUMBER      1

/**
 * \name UDD Configuration
 */
//@{
#define USB_DEVICE_MAX_EP                 6
//@}

//@}

#include "udi_audio.h"

#endif // _UDI_AUDIO_CONF_H_
