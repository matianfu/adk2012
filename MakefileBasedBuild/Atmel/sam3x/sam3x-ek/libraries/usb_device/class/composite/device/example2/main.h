/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Declaration of main function used by Composite example 1
 *
 * - Compiler:           IAR EWAVR and GNU GCC for AVR
 * - Supported devices:  All AVR devices with a USB module can be used.
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
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _MAIN_H_
#define _MAIN_H_

/*! \brief Called by MSC interface
 * Callback running when USB Host enable MSC interface
 *
 * \retval true if MSC startup is ok
 */
bool main_msc_enable(void);

/*! \brief Called by MSC interface
 * Callback running when USB Host disable MSC interface
 */
void main_msc_disable(void);

/*! \brief Called by HID interface
 * Callback running when USB Host enable mouse interface
 *
 * \retval true if mouse startup is ok
 */
bool main_mouse_enable(void);

/*! \brief Called by HID interface
 * Callback running when USB Host disable mouse interface
 */
void main_mouse_disable(void);

/*! \brief Called by HID interface
 * Callback running when USB Host enable keyboard interface
 *
 * \retval true if keyboard startup is ok
 */
bool main_kbd_enable(void);

/*! \brief Called by HID interface
 * Callback running when USB Host disable keyboard interface
 */
void main_kbd_disable(void);

/*! \brief Called by CDC interface
 * Callback running when USB Host enable CDC interface
 *
 * \retval true if cdc startup is successfully done
 */
bool main_cdc_enable(void);

/*! \brief Called by CDC interface
 * Callback running when USB Host disable cdc interface
 */
void main_cdc_disable(void);

/*! \brief Called when Vbus line state change
 */
void main_vbus_action(bool b_high);

/*! \brief Called when a start of frame is received on USB line
 */
void main_sof_action(void);

/*! \brief Called by UDD when a suspend is received
 * Callback running when USB Host set USB line in suspend state
 */
void main_suspend_action(void);

/*! \brief Called by UDD when the USB line exit of suspend state
 */
void main_resume_action(void);

/*! \brief Called by UDC when USB Host request to enable remote wakeup
 */
void main_remotewakeup_enable(void);

/*! \brief Called by UDC when USB Host request to disable remote wakeup
 */
void main_remotewakeup_disable(void);

/*! \brief Called by UDC when USB Host request a extra string different
 * of this specified in USB device descriptor
 */
bool main_extra_string(void);

/*! \brief Called by CDC interface to config port
 */
void main_cdc_config_uart(usb_cdc_line_coding_t * cfg);

/*! \brief Called by CDC interface to open/close port communication
 */
void main_cdc_set_dtr(bool b_enable);

/*! \brief Called by UART interrupt when a data is received
 */
void main_uart_rx_occur(bool b_error, uint8_t value_rx);

/*! \brief Called by UART interrupt when it is ready to send next data
 */
bool main_uart_tx_free(uint8_t * value_rx);

#endif // _MAIN_H_
