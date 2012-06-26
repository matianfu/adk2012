/**
 * \file
 *
 * \brief In system programming to control security, memories and fuses
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

#include "conf_isp.h"
#include "board.h"
#include "isp.h"
#include "boot.h"
#include "string.h"
#include "flash_api.h"
#include "udc.h"

#if UC3C || UC3D
// These defines are missing from or wrong 
// in the toolchain header file ip_xxx.h or part.h
#ifndef AVR32_WDT_KEY_VALUE
  #define AVR32_WDT_KEY_VALUE        0x00000055
#endif
// These defines are missing from or wrong
// in the toolchain header file ip_xxx.h or part.h
#ifndef AVR32_SCIF_RCOSC_FREQUENCY
  #define AVR32_SCIF_RCOSC_FREQUENCY 115200
#endif
#endif

extern void sysclk_reset(void);

/**
 * \name Memory APIs
 */
//@{

//! Memory signature which store information about device
static isp_mem_signature_t mem_signature;

//! Memory bootloader which store information about bootloader
static isp_mem_bootloader_t mem_bootloader = {
	.version = BOOTLOADER_VERSION,
	.id1 = 0,
	.id2 = 0,
};


void mem_flash_read(void *dst, uint32_t src, uint16_t nbytes)
{
	memcpy(dst, FLASH_API_BASE_ADDRESS + src, nbytes);
}

void mem_flash_write(uint32_t dst, const void *src, uint16_t nbytes)
{
	flash_api_memcpy(FLASH_API_BASE_ADDRESS + dst, src, nbytes, FALSE);
}

void mem_security_read(void *dst, uint32_t src, uint16_t nbytes)
{
	if (nbytes) {
		*(U8 *) dst = flash_api_is_security_bit_active();
	}
}

void mem_security_write(uint32_t dst, const void *src, uint16_t nbytes)
{
	if (nbytes && *(U8 *) src) {
		flash_api_activate_security_bit();
	}
}

void mem_configuration_read(void *dst, uint32_t src, uint16_t nbytes)
{
	U8 *dest = dst;
	while (nbytes--) {
		*dest++ = flash_api_read_gp_fuse_bit(src++);
	}
}

void mem_configuration_write(uint32_t dst, const void *src, uint16_t nbytes)
{
	const U8 *source = src;
	while (nbytes--) {
		flash_api_set_gp_fuse_bit(dst++, *source++);
	}
}

static void mem_bootloader_read(void *dst, uint32_t src, uint16_t nbytes)
{
	memcpy(dst, &mem_bootloader + src, nbytes);
}

void mem_signature_read(void *dst, uint32_t src, uint16_t nbytes)
{
	memcpy(dst, &mem_signature + src, nbytes);
}

void mem_user_read(void *dst, uint32_t src, uint16_t nbytes)
{
	memcpy(dst, (U8 *) FLASH_API_USER_PAGE + src, nbytes);
}


void mem_user_write(uint32_t dst, const void *src, uint16_t nbytes)
{
	flash_api_memcpy(FLASH_API_USER_PAGE + dst, src, nbytes, TRUE);
}

//! Interface for memory flash
const isp_mem_t isp_flash = {
	.size        = FLASH_API_SIZE,
	.fnct_read   = mem_flash_read,
	.fnct_write  = mem_flash_write,
};

//! Interface for bit security
const isp_mem_t isp_security = {
	.size        = 1,
	.fnct_read   = mem_security_read,
	.fnct_write  = mem_security_write,
};

//! Interface for memory configuration
const isp_mem_t isp_conf = {
	.size        = FLASH_API_GPF_NUM,
	.fnct_read   = mem_configuration_read,
	.fnct_write  = mem_configuration_write,
};

//! Interface for memory bootloader
const isp_mem_t isp_bootloader = {
	.size        = sizeof(mem_bootloader),
	.fnct_read   = mem_bootloader_read,
	.fnct_write  = NULL,
};

//! Interface for memory signature
const isp_mem_t isp_signature = {
	.size        = sizeof(mem_signature),
	.fnct_read   = mem_signature_read,
	.fnct_write  = NULL,
};

//! Interface for memory user
const isp_mem_t isp_user = {
	.size        = FLASH_API_USER_PAGE_SIZE,
	.fnct_read   = mem_user_read,
	.fnct_write  = mem_user_write,
};

//! Interface for memory no available
const isp_mem_t isp_no_available = {
	.size        = 0,
	.fnct_read   = NULL,
	.fnct_write  = NULL,
};

const isp_mems_t isp_memories = {
	.flash =      			&isp_flash,
	.eeprom =     			&isp_no_available,
	.security =   			&isp_security,
	.conf =       			&isp_conf,
	.bootloader = 			&isp_bootloader,
	.signature =  			&isp_signature,
	.user =       			&isp_user,
	.int_ram =    			&isp_no_available,
	.ext_mem_cs0 =			&isp_no_available,
	.ext_mem_cs1 =			&isp_no_available,
	.ext_mem_cs2 =			&isp_no_available,
	.ext_mem_cs3 =			&isp_no_available,
	.ext_mem_cs4 =			&isp_no_available,
	.ext_mem_cs5 =			&isp_no_available,
	.ext_mem_cs6 =			&isp_no_available,
	.ext_mem_cs7 =			&isp_no_available,
	.ext_mem_df =      	&isp_no_available,
};

//@}


void isp_init(void)
{
	uint32_t did_reg = Get_debug_register(AVR32_DID);
	mem_signature.manufacture = 
		Rd_bitfield(did_reg, AVR32_DID_MID_MASK);
	mem_signature.product_number_msb = 
		Rd_bitfield(did_reg, AVR32_DID_PN_MASK)>>8;
	mem_signature.product_number_lsb = 
		Rd_bitfield(did_reg, AVR32_DID_PN_MASK);
	mem_signature.product_revision = 
		Rd_bitfield(did_reg, AVR32_DID_RN_MASK);
}

bool isp_is_security(void)
{
	uint8_t value;
	isp_security.fnct_read(&value, 0, 1);
	return value;
}

#ifdef UDI_DFU_ATMEL_PROTOCOL_2_SPLIT_ERASE_CHIP
bool isp_erase_chip(void)
{
	static uint16_t isp_page_number=0;
	uint8_t isp_page_number_split;

	if (isp_page_number==0) {
		isp_page_number = flash_api_get_page_count();
		flash_api_lock_all_regions(FALSE);
	}
	isp_page_number_split = 128;
	while (isp_page_number && isp_page_number_split) {
		flash_api_erase_page(--isp_page_number, FALSE);
		isp_page_number_split--;
	}
	return (isp_page_number==0);
}
#else
bool isp_erase_chip(void)
{
	flash_api_lock_all_regions(FALSE);
	flash_api_erase_all_pages(FALSE);
	return true;
}
#endif

void isp_start_appli_rst(void)
{
	cpu_irq_disable();
	AVR32_WDT.ctrl = AVR32_WDT_CTRL_EN_MASK |
			(10 << AVR32_WDT_CTRL_PSEL_OFFSET) |
#if (UC3C || UC3D)
			AVR32_WDT_CTRL_CEN_MASK | AVR32_WDT_CTRL_DAR_MASK |
#endif
			(AVR32_WDT_KEY_VALUE << AVR32_WDT_CTRL_KEY_OFFSET);
	AVR32_WDT.ctrl = AVR32_WDT_CTRL_EN_MASK |
			(10 << AVR32_WDT_CTRL_PSEL_OFFSET) |
#if (UC3C || UC3D)
			AVR32_WDT_CTRL_CEN_MASK | AVR32_WDT_CTRL_DAR_MASK |
#endif
			((~AVR32_WDT_KEY_VALUE << AVR32_WDT_CTRL_KEY_OFFSET) &
			AVR32_WDT_CTRL_KEY_MASK);
	while (1);
}

void isp_start_appli_norst(void)
{
	udc_stop();	// Stop USB device
	sysclk_reset();	// Reset system clock

	boot_program();
}

/**
*   Calculates the CRC-8-CCITT.
*
*   CRC-8-CCITT is defined to be x^8 + x^2 + x + 1
*
*   To use this function use the following template:
*
*       crc = Crc8( crc, data );
*/
#define POLYNOMIAL    (ISP_CFG1_CRC8_POLYNOMIAL << 7)
static U8 Crc8(U8 inCrc, U8 inData)
{
	int i;
	U16 data;

	data = inCrc ^ inData;
	data <<= 8;

	for (i = 0; i < 8; i++) {
		if ((data & 0x8000) != 0) {
			data = data ^ POLYNOMIAL;
		}
		data = data << 1;
	}
	return (unsigned char)(data >> 8);
}


void isp_force_isp(bool force)
{
	uint32_t tempo;
	U8 crc8 = 0;
	int i;

	// 1) Read the config word1 and set the ISP_FORCE bit to force.
	tempo = (ISP_CFG1 & ~ISP_CFG1_FORCE_MASK)
		| ((force << ISP_CFG1_FORCE_OFFSET) & ISP_CFG1_FORCE_MASK);

	// 2) Compute the CRC8 and update the config word1
	for (i = 24; i; i -= 8)	// Compute the CRC8 on the 3 upper Bytes of the word.
		crc8 = Crc8(crc8, tempo >> i);
	tempo = (tempo & ~ISP_CFG1_CRC8_MASK)
		| ((crc8 << ISP_CFG1_CRC8_OFFSET) & ISP_CFG1_CRC8_MASK);

	// 3) Write the config word1
	mem_user_write(ISP_CFG1_OFFSET, &tempo, 4);
}

