/**
 * \file
 *
 * \brief USB configuration file
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

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"
#include "nvm.h"

/* Redefine USB interrupt vectors base address to safe code space */
# if defined(__GNUC__)
#   error GCC no supported when interrupt vectors space is optimized
/*
#   undef USB_BUSEVENT_vect
#   undef USB_TRNCOMPL_vect
#   undef USB_BUSEVENT_vect_num
#   undef USB_TRNCOMPL_vect_num
#   define USB_BUSEVENT_vect_num  1
#   define USB_BUSEVENT_vect      _VECTOR(1)
#   define USB_TRNCOMPL_vect_num  2
#   define USB_TRNCOMPL_vect      _VECTOR(2)
*/
# elif defined(__ICCAVR__)
#   undef USB_BUSEVENT_vect
#   undef USB_TRNCOMPL_vect
#   define USB_BUSEVENT_vect 0x0
#   define USB_TRNCOMPL_vect 0x4
# endif


/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#if part_is_defined(ATxmega128A1U)
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_DFU_ATXMEGA128A1U
#if (FLASH_SIZE>0x10000) // Optimize code space (Boot = 4KB) Temporary to support 64A1U
#define  USB_DEVICE_PRODUCT_NAME          "DFU ATXMEGA128A1U"
#endif
#elif part_is_defined(ATxmega641U)
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_DFU_ATXMEGA64A1U
//#define  USB_DEVICE_PRODUCT_NAME          "DFU ATXMEGA64A1U" // To safe code space
#elif part_is_defined(ATxmega128B1)
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_DFU_ATXMEGA128B1
#define  USB_DEVICE_PRODUCT_NAME          "DFU ATXMEGA128B1"
#elif part_is_defined(ATxmega256A3U) || part_is_defined(ATxmega256A3BU)
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_DFU_ATXMEGA256A3_U_BU
#define  USB_DEVICE_PRODUCT_NAME          "DFU ATXMEGA256A3_U_BU"
#else
#error Unknow DFU PID for this AVR part
#endif

#define  USB_DEVICE_MAJOR_VERSION         (BOOTLOADER_VERSION>>4)
#define  USB_DEVICE_MINOR_VERSION         (BOOTLOADER_VERSION&0x0F)
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#if (FLASH_SIZE>0x10000) // Optimize code space (Boot = 4KB)
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL"
#endif
#define  USB_DEVICE_ATTR                  USB_CONFIG_ATTR_SELF_POWERED

/**
 * Device speeds support
 * Only Full speed is authorized on DFU ATMEL
 * Because FLIP supports only full speed
 * Also:
 * - low speed is too slow (endpoint 8 Bytes)
 * - high speed does not increase flash program speed
 * - high speed is not supported on custom board with bad USB layout
 * @{
 */
// No speed change possible
//@}

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)      // NO VBUS EVENT on XMEGA
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */
/**
 * Configuration of DFU interface
 * @{
 */
//! Interface callback definition
#define  UDI_DFU_ENABLE_EXT()             true
#define  UDI_DFU_DISABLE_EXT()

//! Version of ATMEL protocol used
#define  UDI_DFU_ATMEL_PROTOCOL_VERSION   DFU_ATMEL_PROTOCOL_VERSION_2
//@}
//@}


/**
 * USB Device Driver Configuration    
 * @{
 */
#define UDD_NO_SLEEP_MGR
//@}

//! The includes of classes and other headers must be done at the end of this file to avoid compile error
#include "udi_dfu_atmel_conf.h"
#include "conf_isp.h"   // Need to put bootloader version at USB device version

#endif // _CONF_USB_H_
