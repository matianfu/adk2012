/* This header file is part of the AVR Software Framework 2.0.0 release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief FreeRTOS configuration file.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

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

#ifndef FREERTOS_TASK_H
#define FREERTOS_TASK_H

/* USB task definitions. */
#define configTSK_USB_NAME            ((const signed portCHAR *)"USB")
#define configTSK_USB_STACK_SIZE      256
#define configTSK_USB_PRIORITY        (tskIDLE_PRIORITY + 3)

/* USB device task definitions. */
#define configTSK_USB_DEV_NAME        ((const signed portCHAR *)"USB Device")
#define configTSK_USB_DEV_STACK_SIZE  256
#define configTSK_USB_DEV_PRIORITY    (tskIDLE_PRIORITY + 2)
#define configTSK_USB_DEV_PERIOD      20

/* USB host task definitions. */
#define configTSK_USB_HST_NAME        ((const signed portCHAR *)"USB Host")
#define configTSK_USB_HST_STACK_SIZE  256
#define configTSK_USB_HST_PRIORITY    (tskIDLE_PRIORITY + 2)
#define configTSK_USB_HST_PERIOD      20

/* USB device template task definitions. */
#define configTSK_USB_DTP_NAME        ((const signed portCHAR *)"USB Device Template")
#define configTSK_USB_DTP_STACK_SIZE  256
#define configTSK_USB_DTP_PRIORITY    (tskIDLE_PRIORITY + 1)
#define configTSK_USB_DTP_PERIOD      20

/* USB host template task definitions. */
#define configTSK_USB_HTP_NAME        ((const signed portCHAR *)"USB Host Template")
#define configTSK_USB_HTP_STACK_SIZE  256
#define configTSK_USB_HTP_PRIORITY    (tskIDLE_PRIORITY + 1)
#define configTSK_USB_HTP_PERIOD      20

#endif /* FREERTOS_TASK_H */
