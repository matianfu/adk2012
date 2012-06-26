/**
 * \file
 *
 * \brief UART functions
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
#include "conf_usb.h"
#include "usb_protocol_cdc.h"
#include "sysclk.h"
#include "uart.h"
#include "usart.h"
#include "gpio.h"
#include "main.h"

static usart_options_t usart_options;

ISR(usart_interrupt, AVR32_USART1_IRQ_GROUP, 3)
{
	if (AVR32_USART1.csr & AVR32_USART_CSR_RXRDY_MASK) {
		// Data received
		int value;
		bool b_error = (USART_SUCCESS != usart_read_char(&AVR32_USART1,
						&value));
		if (b_error) {
			usart_reset_status(&AVR32_USART1);
		}
		main_uart_rx_occur(b_error, value);
		return;
	}

	if (AVR32_USART1.csr & AVR32_USART_CSR_TXRDY_MASK) {
		uint8_t value_tx;
		if (main_uart_tx_free(&value_tx)) {
			// Transmit next data
			usart_write_char(&AVR32_USART1, value_tx);
		} else {
			// Fifo empty then Stop UART transmission
			AVR32_USART1.idr = AVR32_USART_IER_TXRDY_MASK;
		}
	}
}


void uart_config(usb_cdc_line_coding_t * cfg)
{
	uint32_t stopbits, parity;
	uint32_t ier;

	switch (cfg->bCharFormat) {
	case CDC_STOP_BITS_2:
		stopbits = USART_2_STOPBITS;
		break;
	case CDC_STOP_BITS_1_5:
		stopbits = USART_1_5_STOPBITS;
		break;
	case CDC_STOP_BITS_1:
	default:
		// Default stop bit = 1 stop bit
		stopbits = USART_1_STOPBIT;
		break;
	}

	switch (cfg->bParityType) {
	case CDC_PAR_EVEN:
		parity = USART_EVEN_PARITY;
		break;
	case CDC_PAR_ODD:
		parity = USART_ODD_PARITY;
		break;
	case CDC_PAR_MARK:
		parity = USART_MARK_PARITY;
		break;
	case CDC_PAR_SPACE:
		parity = USART_SPACE_PARITY;
		break;
	default:
	case CDC_PAR_NONE:
		parity = USART_NO_PARITY;
		break;
	}

	// Options for USART.
	usart_options.baudrate = LE32_TO_CPU(cfg->dwDTERate);
	usart_options.charlength = cfg->bDataBits;
	usart_options.paritytype = parity;
	usart_options.stopbits = stopbits;
	usart_options.channelmode = USART_NORMAL_CHMODE;
	ier = AVR32_USART1.ier ;
	usart_init_rs232(&AVR32_USART1, &usart_options, sysclk_get_pba_hz());
	// Restore both RX and TX
	AVR32_USART1.ier = ier;
}

void uart_open(void)
{
	static const gpio_map_t USART_GPIO_MAP = {
		{AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION},
		{AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION}
	};

	// Setup GPIO for USART.
	gpio_enable_module(USART_GPIO_MAP,
			sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));
	// Enable interrupt with priority higher than USB
	irq_register_handler(usart_interrupt, AVR32_USART1_IRQ, 3);

	// Initialize it in RS232 mode.
	sysclk_enable_pba_module(SYSCLK_USART1);
	if (USART_SUCCESS != usart_init_rs232(&AVR32_USART1, &usart_options,
					sysclk_get_pba_hz())) {
		return;
	}
	// Enable both RX and TX
	AVR32_USART1.ier =
			AVR32_USART_IER_TXRDY_MASK | AVR32_USART_IER_RXRDY_MASK;
}

void uart_close(void)
{
	// Disable interrupts
	AVR32_USART1.ier = 0;
	// Close RS232 communication
}

void uart_enable_tx(void)
{
	// Enable UART TX interrupt to send values
	AVR32_USART1.ier = AVR32_USART_IER_TXRDY_MASK;
}
