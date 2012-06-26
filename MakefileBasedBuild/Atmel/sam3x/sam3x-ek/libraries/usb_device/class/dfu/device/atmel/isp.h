/**
 * \file
 *
 * \brief In system programming API to manage security, memories and fuses
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

#ifndef _ISP_H_
#define _ISP_H_

#include "conf_isp.h"
#include "conf_usb.h"
#include "usb_atmel_dfu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Data type for holding flash memory addresses.
 */
#ifdef ISP_SMALL_MEMORY_SIZE
typedef uint16_t isp_addr_t;
#else
typedef uint32_t isp_addr_t;
#endif

/**
 * \name In system programming API to manage security, memories and fuses
 */
//@{

/**
 * \name Interfaces to control chip
 */
//@{
extern void isp_init(void);
extern bool isp_is_security(void);
extern void isp_force_isp(bool force);
extern bool isp_erase_chip(void);
#ifdef UDI_DFU_ATMEL_FIX_ERASE_CHIP_PROTOCOL
extern bool isp_start_erase_chip(void);
#endif
extern void isp_start_appli_rst(void);
extern void isp_start_appli_norst(void);
//@}

//! Memory API definition
typedef struct {
	uint32_t size;
	void (*fnct_read) (void *dst, isp_addr_t src, uint16_t nbytes);
	void (*fnct_write) (isp_addr_t dst, const void *src, uint16_t nbytes);
} isp_mem_t;

//! \name Memories list
#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_1)
typedef union {
	isp_mem_t const *mem[DFU_ATMEL_V1_MEM_COUNT];
	struct {
		isp_mem_t const *flash;
		isp_mem_t const *eeprom;
		isp_mem_t const *bootloader;
		isp_mem_t const *signature;
		isp_mem_t const *user;
	};
} isp_mems_t;
#endif

#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)
typedef union {
	isp_mem_t const *mem[DFU_ATMEL_V2_MEM_COUNT];
	struct {
		isp_mem_t const *flash;
		isp_mem_t const *eeprom;
		isp_mem_t const *security;
		isp_mem_t const *conf;
		isp_mem_t const *bootloader;
		isp_mem_t const *signature;
		isp_mem_t const *user;
		isp_mem_t const *int_ram;
		isp_mem_t const *ext_mem_cs0;
		isp_mem_t const *ext_mem_cs1;
		isp_mem_t const *ext_mem_cs2;
		isp_mem_t const *ext_mem_cs3;
		isp_mem_t const *ext_mem_cs4;
		isp_mem_t const *ext_mem_cs5;
		isp_mem_t const *ext_mem_cs6;
		isp_mem_t const *ext_mem_cs7;
		isp_mem_t const *ext_mem_df;
	};
} isp_mems_t;
#endif

extern const isp_mems_t isp_memories;



/*
 * alignment requested to simulate a memory
 */
COMPILER_PACK_SET(1);

//! \name Memory signature structure
typedef union {
	uint8_t mem[4];
	struct {
		uint8_t manufacture;
		uint8_t product_number_msb;
		uint8_t product_number_lsb;
		uint8_t product_revision;
	};
} isp_mem_signature_t;


//! \name Memory bootloader structure
// It is used to store medium and minor bootloader versions
// Example: Version 0x00 give 1.0.0 on batchisp log
// Example: Version 0x03 give 1.0.3 on batchisp log
// Example: Version 0x25 give 1.2.5 on batchisp log
// id1 & id2 is not used and must be always at 0.
typedef struct {
   uint8_t version;
   uint8_t id1;
   uint8_t id2;
} isp_mem_bootloader_t;

COMPILER_PACK_RESET();


//@}

#ifdef __cplusplus
}
#endif
#endif // _ISP_H_
