/* This source file is part of the AVR Software Framework 2.0.0 release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Management of the USB high-level applicative host task.
 *
 * This file manages the USB high-level applicative host task.
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


#if USB_HOST_FEATURE == ENABLED

#include "board.h"
#ifdef FREERTOS_USED
#include "libfreertos.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOSTask.h"
#endif
#include "usb_drv.h"
#include "usb_host_enum.h"
#include "usb_host_task.h"
#include "host_template_task.h"


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
#elif BOARD == EVK1101 || BOARD == UC3C_EK || BOARD == EVK1104 || BOARD == EVK1105
#  define LED_APPLI_0   LED2
#  define LED_APPLI_1   LED3
#endif

#if !defined(LED_APPLI_0) || \
    !defined(LED_APPLI_1)
#warning The LED configuration to use in this example is missing.
#endif


//_____ D E C L A R A T I O N S ____________________________________________

static            U16 sof_cnt;
static            U8  pipe_in;
static            U8  pipe_out;
static            U8  buf[64];
#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
static  volatile  U8  busy;
#endif

volatile Bool tpl_new_device_connected;


//!
//! @brief This function initializes the high-level host application.
//! Here initialize specific hardware resources requirements.
//!
void host_template_task_init(void)
{
  U8 i;

  sof_cnt = 0;
  for (i = 0; i < sizeof(buf); i++) buf[i] = i;
#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
  busy = FALSE;
#endif
  tpl_new_device_connected = FALSE;

#ifdef FREERTOS_USED
  if ( xTaskCreate( host_template_task, configTSK_USB_HTP_NAME, configTSK_USB_HTP_STACK_SIZE, NULL, configTSK_USB_HTP_PRIORITY, NULL ) != pdPASS )
  {
    printf( "Failed to create host_template_task\r\n" ) ;
  }
#endif  // FREERTOS_USED
}


//!
//! @brief High-level host applicative task entry point
//! Put here the code of your host application.
//! The sample code just sends and receives 64 bytes from
//! IN and OUT pipes.
//!
#ifdef FREERTOS_USED
void host_template_task(void *pvParameters)
#else
void host_template_task(void)
#endif
{
#if USB_HOST_PIPE_INTERRUPT_TRANSFER == DISABLE
  Status_t sta;
  U16 nb;
#endif
  U8 i;

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (TRUE)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_HTP_PERIOD);

#endif  // FREERTOS_USED
    // First, check the host controller is in full operating mode with the
    // B-device attached and enumerated
    if (Is_host_ready())
    {
      // Put here the code to execute in host mode

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

      // New device connection (executed only once after device connection)
      if (tpl_new_device_connected)
      {
        tpl_new_device_connected = FALSE;

#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
        // No more pipe interrupt transfer pending
        busy = FALSE;
#endif

        // For all supported interfaces
        for (i = 0; i < Get_nb_supported_interface(); i++)
        {
          // If vendor-specific class
          if (Get_class(i) == VENDOR_CLASS)
          {
            // Get correct physical pipes associated with IN/OUT endpoints
            if (Is_ep_in(i, 0))
            { // Yes, associate it with the IN pipe
              pipe_in = Get_ep_pipe(i, 0);
              pipe_out = Get_ep_pipe(i, 1);
            }
            else
            { // No, invert...
              pipe_in = Get_ep_pipe(i, 1);
              pipe_out = Get_ep_pipe(i, 0);
            }
            break;
          }
        }
      }

#if USB_HOST_PIPE_INTERRUPT_TRANSFER == DISABLE
      // The sample task sends 64 bytes through OUT pipe
#if BOARD != 99
      LED_On(LED_APPLI_0);
#endif
      sta = host_send_data(pipe_out, sizeof(buf), buf);
#if BOARD != 99
      LED_Off(LED_APPLI_0);
#endif

      // And receives 64 bytes from IN pipe
      nb = sizeof(buf);
#if BOARD != 99
      LED_On(LED_APPLI_1);
#endif
      sta = host_get_data(pipe_in, &nb, buf);
#if BOARD != 99
      LED_Off(LED_APPLI_1);
#endif
#else
      // Similar applicative task under interrupt mode...
      if (!busy)
      {
        busy = TRUE;
        LED_On(LED_APPLI_0);
        host_send_data_interrupt(pipe_out, sizeof(buf), buf, host_send_data_callback);
        LED_Off(LED_APPLI_0);
      }
#endif

      // Here is an example of an applicative request to go to USB suspend ...
      if (FALSE/* applicative conditions */)
      {
        host_set_feature_remote_wakeup();
        Host_request_suspend();
      }
    }

    // Here an applicative example of resume request...
    if (Is_host_suspended()/* && applicative conditions */)
    {
      Host_request_resume();
    }
#ifdef FREERTOS_USED
  }
#endif
}


#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE

void host_send_data_callback(Status_t status, U16 nb_byte)
{
  if (status == PIPE_GOOD)
  {
    LED_On(LED_APPLI_1);
    host_get_data_interrupt(pipe_in, sizeof(buf), buf, host_get_data_callback);
    LED_Off(LED_APPLI_1);
  }
}


void host_get_data_callback(Status_t status, U16 nb_byte)
{
  if (status == PIPE_GOOD)
  {
    busy = FALSE;
  }
}

#endif  // USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE


//!
//! @brief host_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void host_sof_action(void)
{
  sof_cnt++;
}


#endif  // USB_HOST_FEATURE == ENABLED
