/**
 * \file
 *
 * \brief CDC Application Main functions
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

#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "gpio.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "conf_usb.h"
#include "udd.h"
#include "udc.h"
#include "udi_cdc.h"
#include "ui.h"
#include "uart.h"
#include "fifo.h"
#include <stdio.h>	// printf redirection


#define  MAIN_CDC_FIFO_SIZE    32	// Must be a power of 2

static bool main_b_cdc_enable = false;
static bool main_b_com_startup = false;
static bool main_b_com_open = false;

static uint8_t fifo_rx[MAIN_CDC_FIFO_SIZE];
static uint8_t fifo_tx[MAIN_CDC_FIFO_SIZE];

static fifo_desc_t fifo_desc_rx;
static fifo_desc_t fifo_desc_tx;

const char main_msg_welcome[] = "\x0C"
		"--------------------------\r\n"
		"    ATMEL CDC UART bridge \r\n"
		"--------------------------\r\n";

static void main_cdc_open(bool b_enable);

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();

	sysclk_init();
	board_init();
	ui_init();
	ui_powerdown();

	// Initialize the FIFOs
	fifo_init(&fifo_desc_rx, &fifo_rx, MAIN_CDC_FIFO_SIZE);
	fifo_init(&fifo_desc_tx, &fifo_tx, MAIN_CDC_FIFO_SIZE);

	// Start USB stack to authorize VBus monitoring
	udc_start();

	if (!udc_include_vbus_monitoring()) {
		// VBUS monitoring is not available on this product
		// thereby VBUS has to be considered as present
		main_vbus_action(true);
	}
	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	while (true) {

		sleepmgr_enter_sleep();

		if (main_b_com_open) {
			// To display a message when the port is open
			if (main_b_com_startup) {
				udi_cdc_ctrl_signal_dsr(true);
				udi_cdc_ctrl_signal_dsr(false);
				main_b_com_startup = false;
			}
			//** The transfer fifo<->CDC is done in the main loop
			// to reduce process time in TX/RX UART interrupts
			// Transfer CDC RX to UART TX fifo
			while (udi_cdc_is_rx_ready()) {
				if (fifo_get_free_size(&fifo_desc_tx) == 0)
					break;	// Fifo full then transmission
				ui_com_rx_start();
				fifo_push_byte(&fifo_desc_tx, udi_cdc_getc());
				uart_enable_tx();
			}
			// Transfer UART RX fifo to CDC TX
			while (udi_cdc_is_tx_ready()) {
				uint8_t value_pull;
				if (fifo_pull_byte(&fifo_desc_rx, &value_pull) !=
						FIFO_OK) {
					// No data then end of loop
					ui_com_tx_stop();
					break;
				}
				udi_cdc_putc(value_pull);
			}
		}
	}
}

void main_vbus_action(bool b_high)
{
	if (b_high) {
		// Attach USB Device
		udc_attach();
	} else {
		// VBUS not present
		udc_detach();
	}
}

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if (!main_b_cdc_enable)
		return;
	ui_process(udd_get_frame_number());
}

bool main_cdc_enable(void)
{
	main_b_cdc_enable = true;

#ifdef UDI_CDC_DONT_USE_DTR_SIGNAL_TO_OPEN_COM
	// Open port
	main_cdc_open(true);
#else
	// Nothing, wait open port with DTR signal
#endif
	return true;
}

void main_cdc_disable(void)
{
	main_b_cdc_enable = false;
	// Close communication
	main_cdc_open(false);
}

void main_cdc_config_uart(usb_cdc_line_coding_t * cfg)
{
	uart_config(cfg);
}


void main_cdc_set_dtr(bool b_enable)
{
#ifndef UDI_CDC_DONT_USE_DTR_SIGNAL_TO_OPEN_COM
	main_cdc_open(b_enable);
#endif
}

static void main_cdc_open(bool b_enable)
{
	if (b_enable) {
		// Open communication
		fifo_flush(&fifo_desc_rx);
		fifo_flush(&fifo_desc_tx);
		uart_open();
		ui_com_open();
		main_b_com_startup = true;
		main_b_com_open = true;
	} else {
		// Close communication
		uart_close();
		ui_com_close();
		main_b_com_startup = false;
		main_b_com_open = false;
	}
}

void main_uart_rx_occur(bool b_error, uint8_t value_rx)
{
	if (b_error) {
		udi_cdc_signal_framing_error();
		ui_com_error();
	}
	if (fifo_push_byte(&fifo_desc_rx, value_rx) != FIFO_OK) {
		// Fifo full
		udi_cdc_signal_overrun();
		ui_com_overflow();
	}
	ui_com_tx_start();
}

bool main_uart_tx_free(uint8_t * value_rx)
{
	if (fifo_pull_byte(&fifo_desc_tx, value_rx) == FIFO_OK) {
		return true;
	}
	// Fifo empty then Stop UART transmission
	ui_com_rx_stop();
	return false;
}


/**
 * \mainpage ASF USB Device CDC
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device CDC
 * on AVR products with USB module.
 * The application note AVR4907 provides more information
 * about this implementation.
 *
 * \section desc Description of the Communication Device Class (CDC)
 * The Communication Device Class (CDC) is a general-purpose way to enable all
 * types of communications on the Universal Serial Bus (USB).
 * This class makes it possible to connect communication devices such as 
 * digital telephones or analog modems, as well as networking devices 
 * like ADSL or Cable modems.
 * While a CDC device enables the implementation of quite complex devices,
 * it can also be used as a very simple method for communication on the USB.
 * For example, a CDC device can appear as a virtual COM port, which greatly 
 * simplifies application development on the host side.
 *
 * \section startup Startup
 * The example is a bridge between a USART from the main MCU
 * and the USB CDC interface.
 *
 * For our example, we will only use a PC as a host for the demonstration:
 * it will connect to the USB and to the USART board connector.
 * - Connect the USART peripheral to the USART interface of the board.
 * - Connect the application to a USB host (e.g. a PC) 
 *   with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as a virtual COM (see Windows Hardware Manager).
 * - Open a hyperterminal on both COM ports (RS232 and Virtual COM)
 * - Select the same configuration for both COM ports up to 115200 baud.
 * - Type a character in one hyperterminal and see it in the other.
 *
 * \note
 * On the first connection of the board on the PC,
 * the operating system will detect a new peripheral:
 * - This will open a new hardware installation window.
 * - Choose "No, not this time" to connect to Windows Update for this installation
 * - click "Next"
 * - At the request to search the INF file, give the "./cdc/example/" folder
 * - click "Next"
 *
 * \copydoc UI 
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and CDC modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/cdc/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>\subpage power_management
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (leds,buttons...)
 *       - uart.c       implement of RS232 bridge
 *
 * <SUP>1</SUP>Simple FIFOs are implemented between UART and CDC interfaces.
 * The CDC I/O routines are called directly in UART RX/TX interrupts
 * to avoid data being lost.
 * In this case, the CDC I/O routines are called in the main loop to fill 
 * or read FIFOs. The redirection of printf to CDC is supported for GCC only.
 */
