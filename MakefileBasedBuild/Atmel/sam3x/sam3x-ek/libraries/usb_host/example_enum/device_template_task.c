/* This source file is part of the AVR Software Framework 2.0.0 release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Management of the USB high-level applicative device task.
 *
 * This file manages the USB high-level applicative device task.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USB module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ***************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

//_____  I N C L U D E S ___________________________________________________

#include "conf_usb.h"


#if USB_DEVICE_FEATURE == ENABLED

#include "board.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "device_template_task.h"


//_____ M A C R O S ________________________________________________________
#define BOARD             99

#define EVK1100           1   //!< AT32UC3A EVK1100 board.
#define EVK1101           2   //!< AT32UC3B EVK1101 board.
#define UC3C_EK           3   //!< AT32UC3C UC3C_EK board.
#define EVK1104           4   //!< AT32UC3A3 EVK1104 board.
#define EVK1105           5   //!< AT32UC3A EVK1105 board.
#define STK600_RCUC3L0    6   //!< STK600 RCUC3L0 board.
#define UC3L_EK           7   //!< AT32UC3L-EK board.
#define XPLAIN            8   //!< ATxmega128A1 Xplain board
#define XPLAIN_A1         9   //!< ATxmega128A1U Xplain-A1 board
#define STK600_RC064X     10  //!< ATxmega256A3 STK600 board
#define STK600_RC100X     11  //!< ATxmega128A1 STK600 board

#define USER_BOARD        99  //!< User-reserved board (if any).



//_____ D E F I N I T I O N S ______________________________________________

#if BOARD == EVK1100
#  define LED_APPLI_0   LED_BI0_GREEN
#  define LED_APPLI_1   LED_BI0_RED
#elif BOARD == EVK1101 || BOARD == EVK1104 || BOARD == UC3C_EK || BOARD == EVK1105
#  define LED_APPLI_0   LED2
#  define LED_APPLI_1   LED3
#endif

#if !defined(LED_APPLI_0) || \
    !defined(LED_APPLI_1)
#warning The LED configuration to use in this example is missing.
#endif


//_____ D E C L A R A T I O N S ____________________________________________

static U16  sof_cnt;
static U8   data_length;


//!
//! @brief This function initializes the hardware/software resources required for device applicative task.
//!
void device_template_task_init(void)
{
  sof_cnt = 0;
  data_length = 0;
#ifndef FREERTOS_USED
  #if USB_HOST_FEATURE == ENABLED
  // If both device and host features are enabled, check if device mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  if (Is_usb_device())
  #endif  // USB_HOST_FEATURE == ENABLED
    Usb_enable_sof_interrupt();
#endif  // FREERTOS_USED

#ifdef FREERTOS_USED
  if ( xTaskCreate( device_template_task, configTSK_USB_DTP_NAME, configTSK_USB_DTP_STACK_SIZE, NULL, configTSK_USB_DTP_PRIORITY, NULL ) != pdPASS )
  {
    printf( "Failed to create device_template_task\r\n" ) ;
  }
#endif  // FREERTOS_USED
}


//!
//! @brief Entry point of the device applicative task management
//!
//! This function links the device application to the USB bus.
//!
#ifdef FREERTOS_USED
void device_template_task(void *pvParameters)
#else
void device_template_task(void)
#endif
{
  static U8 buf[EP_SIZE_TEMP2];

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (TRUE)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_DTP_PERIOD);

    // First, check the device enumeration state
    if (!Is_device_enumerated()) continue;
#else
    // First, check the device enumeration state
    if (!Is_device_enumerated()) return;
#endif  // FREERTOS_USED

    // HERE STARTS THE USB DEVICE APPLICATIVE CODE
    // The example below just performs a loopback transmission/reception.
    // All data received with the OUT endpoint is stored in a RAM buffer and
    // sent back to the IN endpoint.

#if BOARD == EVK1100
    // For example, display Start-of-Frame counter on LEDs
    LED_Display_Field(LED_MONO0_GREEN |
                      LED_MONO1_GREEN |
                      LED_MONO2_GREEN |
                      LED_MONO3_GREEN,
                      sof_cnt >> 5);
#elif BOARD == EVK1101 || BOARD == UC3C_EK || BOARD == EVK1104 || BOARD == EVK1105
    // For example, display Start-of-Frame counter on LEDs
    LED_Display_Field(LED0 |
                      LED1,
                      sof_cnt >> 5);
#else
  #warning The display of the SOFs must be defined here.
#endif

    // If we receive something in the OUT endpoint, just store it in the RAM buffer
    if (Is_usb_out_received(EP_TEMP_OUT))
    {
#if BOARD != 99
        LED_On(LED_APPLI_1);
#endif
      Usb_reset_endpoint_fifo_access(EP_TEMP_OUT);
      data_length = Usb_byte_count(EP_TEMP_OUT);
      usb_read_ep_rxpacket(EP_TEMP_OUT, buf, data_length, NULL);
      Usb_ack_out_received_free(EP_TEMP_OUT);
#if BOARD != 99
      LED_Off(LED_APPLI_1);
#endif
    }

    // Load the IN endpoint with the contents of the RAM buffer
    if (data_length && Is_usb_in_ready(EP_TEMP_IN))
    {
#if BOARD != 99
      LED_On(LED_APPLI_0);
#endif
      Usb_reset_endpoint_fifo_access(EP_TEMP_IN);
      usb_write_ep_txpacket(EP_TEMP_IN, buf, data_length, NULL);
      data_length = 0;
      Usb_ack_in_ready_send(EP_TEMP_IN);
#if BOARD != 99
      LED_Off(LED_APPLI_0);
#endif
    }
#ifdef FREERTOS_USED
  }
#endif
}


//!
//! @brief usb_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void usb_sof_action(void)
{
  sof_cnt++;
}


#endif  // USB_DEVICE_FEATURE == ENABLED
