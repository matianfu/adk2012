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

#warning You must refill the following definitions with a correct values

/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             0x0000
#define  USB_DEVICE_PRODUCT_ID            0x0000
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  \
	(USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
// #define  USB_DEVICE_MANUFACTURE_NAME      "Manufacture name"
// #define  USB_DEVICE_PRODUCT_NAME          "Product name"
// #define  USB_DEVICE_SERIAL_NAME           "12...EF"	// Disk SN for MSC

/**
 * Device speeds support
 * @{
 */
//! To define a Low speed device
//#define  USB_DEVICE_LOW_SPEED

//! To authorize the High speed
#if (UC3A3||UC3A4)
//#define  USB_DEVICE_HS_SUPPORT
#endif
//@}

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
// #define  UDC_VBUS_EVENT(b_vbus_high)      user_callback_vbus_action(b_vbus_high)
// #define  UDC_SOF_EVENT()                  user_callback_sof_action()
// #define  UDC_SUSPEND_EVENT()              user_callback_suspend_action()
// #define  UDC_RESUME_EVENT()               user_callback_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()        user_callback_remotewakeup_enable()
// #define  UDC_REMOTEWAKEUP_DISABLE()       user_callback_remotewakeup_disable()
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()
//@}

/**
 * USB Device low level configuration
 * When only one interface is used, these configurations are defined by the class module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Number of interfaces for this device
#define  USB_DEVICE_NB_INTERFACE       1	// 1 or more

//! Total endpoint used by all interfaces
//! Note:
//! It is possible to define an IN and OUT endpoints with the same number on xmega product only
//! E.g. MSC class can be have IN endpoint 0x81 and OUT endpoint 0x01
#define  USB_DEVICE_MAX_EP             0	// 0 to max endpoint requested by interfaces
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of CDC interface (if used)
 * @{
 */
//! Interface callback definition
#define  UDI_CDC_ENABLE_EXT()             true
#define  UDI_CDC_DISABLE_EXT()            
#define  UDI_CDC_SET_CODING_EXT(cfg)      
#define  UDI_CDC_SET_DTR_EXT(set)         
#define  UDI_CDC_SET_RTS_EXT(set)

//! Default configuration of communication port
#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8

/**
 * USB CDC low level configuration
 * In standalone these configurations are defined by the CDC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_CDC_DATA_EP_IN            (1 | USB_EP_DIR_IN)	// TX
#define  UDI_CDC_DATA_EP_OUT           (2 | USB_EP_DIR_OUT)	// RX
#define  UDI_CDC_COMM_EP               (3 | USB_EP_DIR_IN)	// Notify endpoint

//! Interface numbers
#define  UDI_CDC_COMM_IFACE_NUMBER     0
#define  UDI_CDC_DATA_IFACE_NUMBER     1
//@}
//@}


/**
 * Configuration of MSC interface (if used)
 * @{
 */
//! Vendor name and Product version of MSC interface 
#define UDI_MSC_GLOBAL_VENDOR_ID            \
   'A', 'T', 'M', 'E', 'L', ' ', ' ', ' '
#define UDI_MSC_GLOBAL_PRODUCT_VERSION            \
   '1', '.', '0', '0'

//! Interface callback definition
#define  UDI_MSC_ENABLE_EXT()          true
#define  UDI_MSC_DISABLE_EXT()         

/**
 * USB MSC low level configuration
 * In standalone these configurations are defined by the MSC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_MSC_EP_IN                 (1 | USB_EP_DIR_IN)
#define  UDI_MSC_EP_OUT                (2 | USB_EP_DIR_OUT)

//! Interface number
#define  UDI_MSC_IFACE_NUMBER          0
//@}
//@}


/**
 * Configuration of HID Mouse interface (if used)
 * @{
 */
//! Interface callback definition
#define  UDI_HID_MOUSE_ENABLE_EXT()       true
#define  UDI_HID_MOUSE_DISABLE_EXT()      

/**
 * USB HID Mouse low level configuration
 * In standalone these configurations are defined by the HID Mouse module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_HID_MOUSE_EP_IN           (1 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_HID_MOUSE_IFACE_NUMBER    0
//@}
//@}


/**
 * Configuration of HID Keyboard interface (if used)
 * @{
 */
//! Interface callback definition
#define  UDI_HID_KBD_ENABLE_EXT()       true
#define  UDI_HID_KBD_DISABLE_EXT()      
#define  UDI_HID_KBD_CHANGE_LED(value)  

/**
 * USB HID Keyboard low level configuration
 * In standalone these configurations are defined by the HID Keyboard module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_HID_KBD_EP_IN           (1 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_HID_KBD_IFACE_NUMBER    0
//@}
//@}


/**
 * Configuration of HID Generic interface (if used)
 * @{
 */
//! Interface callback definition
#define  UDI_HID_GENERIC_ENABLE_EXT()       true
#define  UDI_HID_GENERIC_DISABLE_EXT()      
#define  UDI_HID_GENERIC_REPORT_OUT(ptr)    
#define  UDI_HID_GENERIC_SET_FEATURE(f)     

#define  UDI_HID_REPORT_IN_SIZE             64
#define  UDI_HID_REPORT_OUT_SIZE            64
#define  UDI_HID_REPORT_FEATURE_SIZE        4
#define  UDI_HID_GENERIC_EP_SIZE            64

/**
 * USB HID Generic low level configuration
 * In standalone these configurations are defined by the HID generic module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_HID_GENERIC_EP_OUT   (2 | USB_EP_DIR_OUT)
#define  UDI_HID_GENERIC_EP_IN    (1 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_HID_GENERIC_IFACE_NUMBER    0
//@}
//@}


/**
 * Configuration of PHDC interface (if used)
 * @{
 */

//! Interface callback definition
#define  UDI_PHDC_ENABLE_EXT()          true
#define  UDI_PHDC_DISABLE_EXT()         

//! Select data format of application 
//! USB_PHDC_DATAMSG_FORMAT_VENDOR or USB_PHDC_DATAMSG_FORMAT_11073_20601
#define  UDI_PHDC_DATAMSG_FORMAT       USB_PHDC_DATAMSG_FORMAT_11073_20601
//! If USB_PHDC_DATAMSG_FORMAT_11073_20601 then define specialization(s)
#define  UDI_PHDC_SPECIALIZATION       {0x2345}	// Define in 11073_20601

//! Defines the QOS need for each transfer direction
#define  UDI_PHDC_QOS_OUT              \
	(USB_PHDC_QOS_MEDIUM_BETTER|USB_PHDC_QOS_HIGH_BEST)
#define  UDI_PHDC_QOS_IN               \
	(USB_PHDC_QOS_LOW_GOOD|USB_PHDC_QOS_MEDIUM_BETTER|USB_PHDC_QOS_MEDIUM_BEST)

//! The following METADATAs can be send during USB enumeration
//! for each endpoints (optional).
//! It can define the Opaque data format of each endpoints
//! These must be up to 253 and recommended <30
#define  UDI_PHDC_METADATA_DESC_BULK_IN   {0x01,0x02,0x03}
#define  UDI_PHDC_METADATA_DESC_BULK_OUT  {0x01,0x02,0x03}
#define  UDI_PHDC_METADATA_DESC_INT_IN    {0x01,0x02,0x03}

/**
 * USB PHDC low level configuration
 * In standalone these configurations are defined by the PHDC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_PHDC_EP_BULK_OUT          (1 | USB_EP_DIR_OUT)
#define  UDI_PHDC_EP_BULK_IN           (2 | USB_EP_DIR_IN)
#define  UDI_PHDC_EP_INTERRUPT_IN      (3 | USB_EP_DIR_IN)	// Only if UDI_PHDC_QOS_IN include USB_PHDC_QOS_LOW_GOOD

//! Interface number
#define  UDI_PHDC_IFACE_NUMBER          0
//@}
//@}

//... Eventually add other Interface Configuration

//@}


/**
 * Description of Composite Device
 * @{
 */
//! USB Interfaces descriptor structure 
#define	UDI_COMPOSITE_DESC_T
	
//! USB Interfaces descriptor value for Full Speed 
#define	UDI_COMPOSITE_DESC_FS

//! USB Interfaces descriptor value for High Speed 
#define	UDI_COMPOSITE_DESC_HS

//! USB Interface APIs
#define	UDI_COMPOSITE_API	

/* Example for device with cdc, msc and hid mouse interface
#define	UDI_COMPOSITE_DESC_T				\
	usb_iad_desc_t udi_cdc_iad;			\
	udi_cdc_comm_desc_t udi_cdc_comm;	\
	udi_cdc_data_desc_t udi_cdc_data;	\
	udi_msc_desc_t udi_msc;					\
	udi_hid_mouse_desc_t udi_hid_mouse

//! USB Interfaces descriptor value for Full Speed 
#define	UDI_COMPOSITE_DESC_FS			\
	.udi_cdc_iad               = UDI_CDC_IAD_DESC, \
	.udi_cdc_comm              = UDI_CDC_COMM_DESC, \
	.udi_cdc_data              = UDI_CDC_DATA_DESC, \
	.udi_msc                   = UDI_MSC_DESC_FS,	\
	.udi_hid_mouse             = UDI_HID_MOUSE_DESC

//! USB Interfaces descriptor value for High Speed 
#define	UDI_COMPOSITE_DESC_HS			\
	.udi_cdc_iad               = UDI_CDC_IAD_DESC, \
	.udi_cdc_comm              = UDI_CDC_COMM_DESC, \
	.udi_cdc_data              = UDI_CDC_DATA_DESC, \
	.udi_msc                   = UDI_MSC_DESC_HS,	\
	.udi_hid_mouse             = UDI_HID_MOUSE_DESC

//! USB Interface APIs
#define	UDI_COMPOSITE_API  \
	&udi_api_cdc_comm,       \
	&udi_api_cdc_data,       \
	&udi_api_msc,            \
	&udi_api_hid_mouse
*/

//@}


/**
 * USB Device Driver Configuration    
 * @{
 */
//@}

//! The includes of classes and other headers must be done at the end of this file to avoid compile error

/* Example of include for interface
#include "udi_msc.h"
#include "udi_hid_kbd.h"
#include "udi_hid_mouse.h"
#include "udi_cdc_conf.h"
#include "udi_phdc.h"
*/
/* Declaration of callbacks used by USB
#include "callback_def.h"
*/

#endif // _CONF_USB_H_
