/**
 * \file
 *
 * \brief User Interface from EVK1100
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
#include "main.h"

#define  USART               USARTC0

void uart_config(usb_cdc_line_coding_t * cfg)
{
	uint8_t reg_ctrlc;

	reg_ctrlc = USART_CMODE_ASYNCHRONOUS_gc;

	switch (cfg->bCharFormat) {
	case CDC_STOP_BITS_2:
		reg_ctrlc |= USART_SBMODE_bm;
		break;
	case CDC_STOP_BITS_1:
	default:
		// Default stop bit = 1 stop bit
		break;
	}

	switch (cfg->bParityType) {
	case CDC_PAR_EVEN:
		reg_ctrlc |= USART_PMODE_EVEN_gc;
		break;
	case CDC_PAR_ODD:
		reg_ctrlc |= USART_PMODE_ODD_gc;
		break;
	default:
	case CDC_PAR_NONE:
		reg_ctrlc |= USART_PMODE_DISABLED_gc;
		break;
	}

	switch (cfg->bDataBits) {
	case 5:
		reg_ctrlc |= USART_CHSIZE_5BIT_gc;
		break;
	case 6:
		reg_ctrlc |= USART_CHSIZE_6BIT_gc;
		break;
	case 7:
		reg_ctrlc |= USART_CHSIZE_7BIT_gc;
		break;
	default:
	case 8:
		reg_ctrlc |= USART_CHSIZE_8BIT_gc;
		break;
	case 9:
		reg_ctrlc |= USART_CHSIZE_9BIT_gc;
		break;
	}

	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_USART0);
	// Set configuration
	USART.CTRLC = reg_ctrlc;
	// Update baudrate
	USART.BAUDCTRLA = (uint16_t) (((((((uint32_t) sysclk_get_cpu_hz()) << 1)
									/
									((uint32_t) le32_to_cpu(cfg->dwDTERate) * 8)) + 1) >> 1) - 1);
}

void uart_open(void)
{
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_USART0);
	// Open UART communication
	PORTC.DIRSET = 0x08;	// PC3 (TXC0) as output.
	PORTC.DIRCLR = 0x04;	// PC2 (RXC0) as input.
	// Enable both RX and TX
	USART.CTRLB = USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm;
	// Enable interrupt with priority higher than USB
	USART.CTRLA = (register8_t) USART_RXCINTLVL_HI_gc | (register8_t)
			USART_DREINTLVL_OFF_gc;
}

void uart_close(void)
{
	sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_USART0);
	// Disable interrupts
	USART.CTRLA = 0;
	// Close RS232 communication
	USART.CTRLB = 0;
}

void uart_enable_tx(void)
{
	// Enable UART TX interrupt to send values
	USART.CTRLA = (register8_t) USART_RXCINTLVL_HI_gc | (register8_t)
			USART_DREINTLVL_HI_gc;
}


ISR(USARTC0_RXC_vect)
{
	bool b_error = (0 !=
			(USART.STATUS & (USART_FERR_bm | USART_BUFOVF_bm)));
	main_uart_rx_occur(b_error, USART.DATA);
}

ISR(USARTC0_DRE_vect)
{
	uint8_t value_tx;
	if (main_uart_tx_free(&value_tx)) {
		// Transmit next data
		USART.DATA = value_tx;
	} else {
		// Fifo empty then Stop UART transmission
		USART.CTRLA = (register8_t) USART_RXCINTLVL_HI_gc |
				(register8_t) USART_DREINTLVL_OFF_gc;
	}
}
