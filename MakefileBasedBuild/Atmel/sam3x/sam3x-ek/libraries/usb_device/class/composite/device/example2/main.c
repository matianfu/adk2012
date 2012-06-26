/**
 * \file
 *
 * \brief Main functions for USB composite example
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
#include "udi_msc.h"
#include "udi_hid.h"
#include "udi_cdc.h"
#include "ui.h"
#include "conf_access.h"
#include "at45dbx.h"
#include "uart.h"
#include "fifo.h"
#include <stdio.h>	// printf redirection

static bool main_b_mouse_enable = false;
static bool main_b_msc_enable = false;
static bool main_b_kbd_enable = false;
static bool main_b_cdc_enable = false;


#if UC3A3
//! \brief Init HMatrix
static void init_hmatrix(void);
#endif
#if ((defined SD_MMC_MCI_0_MEM) && (SD_MMC_MCI_0_MEM == ENABLE)) \
	|| ((defined SD_MMC_MCI_1_MEM) && (SD_MMC_MCI_1_MEM == ENABLE))
//! \brief Initializes SD/MMC resources: GPIO, MCI and SD/MMC.
static void sd_mmc_mci_resources_init(void);
#endif

/*! \brief Function for CDC interface
 */
#define  MAIN_CDC_FIFO_SIZE    32	// Must be a power of 2
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

#if UC3A3
	// Init Hmatrix bus   
	sysclk_enable_pbb_module(SYSCLK_HMATRIX);
	init_hmatrix();
#endif
#if (defined AT45DBX_MEM) && (AT45DBX_MEM == ENABLE)
	at45dbx_init();
#endif
#if ((defined SD_MMC_MCI_0_MEM) && (SD_MMC_MCI_0_MEM == ENABLE)) \
	|| ((defined SD_MMC_MCI_1_MEM) && (SD_MMC_MCI_1_MEM == ENABLE))
	// Initialize SD/MMC with MCI PB clock.
	sysclk_enable_pbb_module(SYSCLK_MCI);
	sysclk_enable_hsb_module(SYSCLK_DMACA);
	sd_mmc_mci_resources_init();
#endif

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
		if (main_b_msc_enable) {
   		udi_msc_process_trans();
      }

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
	if ((!main_b_mouse_enable) ||
		(!main_b_msc_enable) ||
		(!main_b_kbd_enable) ||
		(!main_b_cdc_enable))
		return;
	ui_process(udd_get_frame_number());
}

void main_remotewakeup_enable(void)
{
	ui_wakeup_enable();
}

void main_remotewakeup_disable(void)
{
	ui_wakeup_disable();
}

bool main_extra_string(void)
{
   static uint8_t udi_cdc_name[] = "CDC interface";
   static uint8_t udi_msc_name[] = "MSC interface";
   static uint8_t udi_hid_mouse_name[] = "HID mouse interface";
   static uint8_t udi_hid_kbd_name[] = "HID keyboard interface";

   struct extra_strings_desc_t{
	   usb_str_desc_t header;
	   le16_t string[Max(Max(Max( \
        sizeof(udi_cdc_name)-1, sizeof(udi_msc_name)-1),\
        sizeof(udi_hid_mouse_name)-1), sizeof(udi_hid_kbd_name)-1)];
   };
   static UDC_DESC_STORAGE struct extra_strings_desc_t extra_strings_desc = {
	   .header.bDescriptorType = USB_DT_STRING
   };

   uint8_t i;
	uint8_t *str;
	uint8_t str_lgt=0;

	// Link payload pointer to the string corresponding at request
	switch (udd_g_ctrlreq.req.wValue & 0xff) {
	case UDI_CDC_IAD_STRING_ID:
		str_lgt = sizeof(udi_cdc_name)-1;
		str = udi_cdc_name;
		break;
	case UDI_MSC_STRING_ID:
		str_lgt = sizeof(udi_msc_name)-1;
		str = udi_msc_name;
		break;
	case UDI_HID_MOUSE_STRING_ID:
		str_lgt = sizeof(udi_hid_mouse_name)-1;
		str = udi_hid_mouse_name;
		break;
	case UDI_HID_KBD_STRING_ID:
		str_lgt = sizeof(udi_hid_kbd_name)-1;
		str = udi_hid_kbd_name;
		break;
	default:
		return false;
	}

	if (str_lgt!=0) {
		for( i=0; i<str_lgt; i++) {
			extra_strings_desc.string[i] = cpu_to_le16((le16_t)str[i]);
		}
		extra_strings_desc.header.bLength = 2+ (str_lgt)*2;
		udd_g_ctrlreq.payload_size = extra_strings_desc.header.bLength;
		udd_g_ctrlreq.payload = (uint8_t *) &extra_strings_desc;
	}

	// if the string is larger than request length, then cut it
	if (udd_g_ctrlreq.payload_size > udd_g_ctrlreq.req.wLength) {
		udd_g_ctrlreq.payload_size = udd_g_ctrlreq.req.wLength;
	}
	return true;
}

bool main_msc_enable(void)
{
	main_b_msc_enable = true;
	return true;
}

void main_msc_disable(void)
{
	main_b_msc_enable = false;
}

bool main_mouse_enable(void)
{
	main_b_mouse_enable = true;
	return true;
}

void main_mouse_disable(void)
{
	main_b_mouse_enable = false;
}

bool main_kbd_enable(void)
{
	main_b_kbd_enable = true;
	return true;
}

void main_kbd_disable(void)
{
	main_b_kbd_enable = false;
}

bool main_cdc_enable(void)
{
	main_b_cdc_enable = true;
	return true;
}

void main_cdc_disable(void)
{
	main_b_cdc_enable = false;
	// Close communication
	main_cdc_open(false);
}

#if UC3A3
/*! \name Hmatrix bus configuration
 */
static void init_hmatrix(void)
{
	union {
		unsigned long scfg;
		avr32_hmatrix_scfg_t SCFG;
	} u_avr32_hmatrix_scfg;

	// For the internal-flash HMATRIX slave, use last master as default.
	u_avr32_hmatrix_scfg.scfg =
			AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH];
	u_avr32_hmatrix_scfg.SCFG.defmstr_type =
			AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] =
			u_avr32_hmatrix_scfg.scfg;
}
#endif


#if ((defined SD_MMC_MCI_0_MEM) && (SD_MMC_MCI_0_MEM == ENABLE)) \
	|| ((defined SD_MMC_MCI_1_MEM) && (SD_MMC_MCI_1_MEM == ENABLE))
#include "mci.h"
#include "conf_sd_mmc_mci.h"

/*! \brief Initializes SD/MMC resources: GPIO, MCI and SD/MMC.
 */
static void sd_mmc_mci_resources_init(void)
{
	static const gpio_map_t SD_MMC_MCI_GPIO_MAP = {
		{SD_SLOT_8BITS_CLK_PIN, SD_SLOT_8BITS_CLK_FUNCTION},	// SD CLK.
		{SD_SLOT_8BITS_CMD_PIN, SD_SLOT_8BITS_CMD_FUNCTION},	// SD CMD.
		{SD_SLOT_8BITS_DATA0_PIN, SD_SLOT_8BITS_DATA0_FUNCTION},	// SD DAT[0].
		{SD_SLOT_8BITS_DATA1_PIN, SD_SLOT_8BITS_DATA1_FUNCTION},	// DATA Pin.
		{SD_SLOT_8BITS_DATA2_PIN, SD_SLOT_8BITS_DATA2_FUNCTION},	// DATA Pin.
		{SD_SLOT_8BITS_DATA3_PIN, SD_SLOT_8BITS_DATA3_FUNCTION},	// DATA Pin.
		{SD_SLOT_8BITS_DATA4_PIN, SD_SLOT_8BITS_DATA4_FUNCTION},	// DATA Pin.
		{SD_SLOT_8BITS_DATA5_PIN, SD_SLOT_8BITS_DATA5_FUNCTION},	// DATA Pin.
		{SD_SLOT_8BITS_DATA6_PIN, SD_SLOT_8BITS_DATA6_FUNCTION},	// DATA Pin.
		{SD_SLOT_8BITS_DATA7_PIN, SD_SLOT_8BITS_DATA7_FUNCTION}	// DATA Pin.
	};

	// MCI options.
	static const mci_options_t MCI_OPTIONS = {
		.card_speed = 400000,
		.card_slot = SD_SLOT_8BITS,	// Default card initialization.
	};

	// Assign I/Os to MCI.
	gpio_enable_module(SD_MMC_MCI_GPIO_MAP,
			sizeof(SD_MMC_MCI_GPIO_MAP) /
			sizeof(SD_MMC_MCI_GPIO_MAP[0]));

	// Enable pull-up for Card Detect.
	gpio_enable_pin_pull_up(SD_SLOT_8BITS_CARD_DETECT);

	// Enable pull-up for Write Protect.
	gpio_enable_pin_pull_up(SD_SLOT_8BITS_WRITE_PROTECT);

	sd_mmc_mci_init(&MCI_OPTIONS, sysclk_get_pbb_hz(), sysclk_get_cpu_hz());
}
#endif // SD_MMC_MCI_0_MEM == ENABLE || SD_MMC_MCI_1_MEM == ENABLE


void main_cdc_config_uart(usb_cdc_line_coding_t * cfg)
{
	uart_config(cfg);
}


void main_cdc_set_dtr(bool b_enable)
{
	main_cdc_open(b_enable);
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
 * \mainpage ASF USB Device Composite
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device Composite with HID mouse, keyboard, CDC and 
 * Mass Storage interfaces on AVR products with USB module.
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
 *
 * \section startup Startup
 * The example uses all memories available on the board and connects these to
 * USB Device Mass Storage stack. 
 * Also, the example uses the buttons or sensors available on the board 
 * to simulate a standard mouse, keyboard.
 * After loading firmware, connect hardware board (EVKxx,XPlain,...) to the USB Host.
 * When connected to a USB host system this application allows to display 
 * all available memories as a removable disks and provides a mouse in
 * the Unix/Mac/Windows operating systems.
 *
 * For USB to UART bridge:
 * - Connect the USART peripheral to the USART interface of the board.
 * - Connect the application to a USB host (e.g. a PC) 
 *   with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as a virtual COM (see Windows Hardware Manager).
 * - Open a hyperterminal on both COM ports (RS232 and Virtual COM)
 * - Select the same configuration for both COM ports up to 115200 baud.
 * - Type a character in one hyperterminal and see it in the other.
 *
 * \note
 * This example uses the native MSC and HID drivers on Unix/Mac/Windows OS, except for Win98.
 *
 * \copydoc UI 
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and HID, CDC and MSC modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/msc/
 *   <br>services/usb/class/cdc/
 *   <br>services/usb/class/hid/
 *   <br>services/usb/class/hid/mouse/
 *   <br>services/usb/class/hid/keyboard/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>\subpage power_management
 *      <br>manages UI
 *    - udi_composite_desc.c,udi_composite_conf.h,
 *      <br>USB Device composite definition
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 *       - uart.c       implement of RS232 bridge
 *
 * <SUP>1</SUP> The memory data transfers are done outside USB interrupt routine. 
 * This is done in the MSC process ("udi_msc_process_trans()") called by main loop.
 * <SUP>1</SUP>Simple FIFOs are implemented between UART and CDC interfaces.
 * The CDC I/O routines are called directly in UART RX/TX interrupts
 * to avoid data being lost.
 * In this case, the CDC I/O routines are called in the main loop to fill 
 * or read FIFOs. The redirection of printf to CDC is supported for GCC only.
 */
