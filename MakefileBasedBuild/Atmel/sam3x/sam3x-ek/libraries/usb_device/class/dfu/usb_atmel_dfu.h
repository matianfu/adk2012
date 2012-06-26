/**
 * \file
 *
 * \brief USB Device Firmware Upgrade (DFU) Atmel definitions.
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

#ifndef _USB_ATMEL_DFU_H_
#define _USB_ATMEL_DFU_H_

/**
 * \ingroup usb_atmel_ids_group
 * \defgroup usb_dfu_atmel USB Device Firmware Upgrade (DFU) Atmel definitions
 *
 * @{
 */

//! \defgroup Atmel DFU Protocol versions
//@{
#define  DFU_ATMEL_PROTOCOL_VERSION_2     2	// Used by UC3 parts
#define  DFU_ATMEL_PROTOCOL_VERSION_1     1	// Used by Mega and Xmega parts
#define  DFU_ATMEL_PROTOCOL_VERSION_0     0	// Used by C51 parts
//@}

/**
 * \defgroup usb_dfu_atmel_pv1 Definitions for Atmel DFU Protocol version 1
 *
 * Note: Often used by mega parts.
 *
 * @{
 */

COMPILER_PACK_SET(1);
/**
 * \brief DFU Atmel AVR 8-bit command fields
 */
typedef struct {
	uint8_t cmd_id;
	uint8_t arg[5];
	//! To do a structure size = control endpoint size = 32
	uint8_t reserved[32 - 6];
} dfu_atmel_v1_cmd_t;
COMPILER_PACK_RESET();

//! Size of buffer used by FLIP to receive or send data
#define  DFU_ATMEL_V1_BUF_TRANS_SIZE  1024

//! \name USB DFU Atmel AVR 8-bit Commands
//@{
#define  DFU_ATMEL_V1_CMD_PROG_START            0x01
#define  DFU_ATMEL_V1_CMD_READ                  0x03
#define  DFU_ATMEL_V1_CMD_WRITE                 0x04
#define  DFU_ATMEL_V1_CMD_READ_ID               0x05
#define  DFU_ATMEL_V1_CMD_CHANGE_BASE_ADDR      0x06

//! \name PROG_START arguments
//@{
#define DFU_ATMEL_V1_CMD_PROG_START_ARG_FLASH   0x00
#define DFU_ATMEL_V1_CMD_PROG_START_ARG_EEPROM  0x01
#define DFU_ATMEL_V1_CMD_PROG_START_ARG_CUSTOM  0x08
//@}

//! \name READ arguments
//@{
#define DFU_ATMEL_V1_CMD_READ_ARG_FLASH      0x00
#define DFU_ATMEL_V1_CMD_READ_ARG_FLASHCHECK 0x01
#define DFU_ATMEL_V1_CMD_READ_ARG_EEPROM     0x02
#define DFU_ATMEL_V1_CMD_READ_ARG_CUSTOM     0x03
//@}

//! \name DFU_ATMEL_V1_CMD_WRITE arguments
//@{
#define DFU_ATMEL_V1_CMD_WRITE_ARG_ERASE     0x00
#define DFU_ATMEL_V1_CMD_WRITE_ARG_RST       0x03
//! \name DFU_ATMEL_V1_CMD_WRITE_ARG_ERASE arguments
//@{
#define DFU_ATMEL_V1_CMD_WRITE_ARG_ERASE_CHIP 0xFF
//@}
//! \name DFU_ATMEL_V1_CMD_WRITE_ARG_RST arguments
//@{
#define DFU_ATMEL_V1_CMD_WRITE_ARG_RST_HW    0x00
#define DFU_ATMEL_V1_CMD_WRITE_ARG_RST_SF    0x01
//@}
//@}

//! \name DFU_ATMEL_V1_CMD_READID_ arguments
//@{
#define DFU_ATMEL_V1_CMD_READ_ID_ARG_BOOTLOADER 0x00
#define DFU_ATMEL_V1_CMD_READ_ID_ARG_SIGNATURE  0x01
//! \name DFU_ATMEL_V1_CMD_READ_ID_ARG_SIGNATURE arguments
//@{
#define DFU_ATMEL_V1_CMD_READ_ID_SIGNATURE_ARG_MANUF              0x30
#define DFU_ATMEL_V1_CMD_READ_ID_SIGNATURE_ARG_FAMILY             0x31
#define DFU_ATMEL_V1_CMD_READ_ID_SIGNATURE_ARG_PRODUCT            0x60
#define DFU_ATMEL_V1_CMD_READ_ID_SIGNATURE_ARG_REVISION           0x61
//@}
//@}

//! \name DFU_ATMEL_V1_CMD_CHANGE_BASE_ADDR arguments
//@{
#define DFU_ATMEL_V1_CMD_CHANGE_BASE_ADDR_ARG0   0x03
#define DFU_ATMEL_V1_CMD_CHANGE_BASE_ADDR_ARG1   0x00
//@}

//! \name Memory units available
//@{
#define DFU_ATMEL_V1_MEM_FLASH                0x00
#define DFU_ATMEL_V1_MEM_EEPROM               0x01
#define DFU_ATMEL_V1_MEM_BOOTLOADER           0x02
#define DFU_ATMEL_V1_MEM_SIGNATURE            0x03
#define DFU_ATMEL_V1_MEM_CUSTOM               0x04
#define DFU_ATMEL_V1_MEM_COUNT                0x05	// Number of memory units
//@}

//@}


/**
 * \defgroup usb_dfu_atmel_pv2 Definitions for Atmel DFU Protocol version 2
 *
 * Note: Often used by uc3.parts
 *
 * @{
 */

COMPILER_PACK_SET(1);
/**
 * \brief DFU Atmel command fields
 */
typedef struct {
	uint8_t group;
	uint8_t cmd_id;
	uint8_t arg[4];
	// To do a structure size = control endpoint size = 64
	uint8_t reserved[64 - 6];
} dfu_atmel_v2_cmd_t;
COMPILER_PACK_RESET();

//! Size of buffer used by FLIP to receive or send data
//! Note:  Write uses 2K and Read uses 1KB
#define  DFU_ATMEL_V2_BUF_TRANS_SIZE  (2*1024)

//! \name Command Groups 
//@{
#define DFU_ATMEL_V2_CMD_GRP_DNLOAD                0x01
#define DFU_ATMEL_V2_CMD_GRP_UPLOAD                0x03
#define DFU_ATMEL_V2_CMD_GRP_EXEC                  0x04
#define DFU_ATMEL_V2_CMD_GRP_SELECT                0x06
//@}

//! \name DNLOAD commands
//@{
#define DFU_ATMEL_V2_CMD_PROGRAM_START             0x00
//@}


//! \name UPLOAD commands
//@{
#define DFU_ATMEL_V2_CMD_READ_MEMORY               0x00
#define DFU_ATMEL_V2_CMD_BLANK_CHECK               0x01
//@}

//! \name EXEC commands
//@{
#define DFU_ATMEL_V2_CMD_ERASE                     0x00
#define DFU_ATMEL_V2_CMD_START_APPLI               0x03
//! \name ERASE arguments
//@{
#define DFU_ATMEL_V2_CMD_ERASE_ARG_CHIP            0xFF
//@}
//! \name START_APPLI arguments
//@{
#define DFU_ATMEL_V2_CMD_START_APPLI_ARG_RESET     0x00
#define DFU_ATMEL_V2_CMD_START_APPLI_ARG_NO_RESET  0x01
//@}
//@}

//! \name SELECT commands
//@{
#define DFU_ATMEL_V2_CMD_SELECT_MEMORY             0x03
//! \name SELECT_MEMORY arguments
//@{
#define DFU_ATMEL_V2_CMD_SELECT_MEMORY_ARG_UNIT    0x00
#define DFU_ATMEL_V2_CMD_SELECT_MEMORY_ARG_PAGE    0x01
//! \name Memory units field
//@{
#define DFU_ATMEL_V2_MEM_FLASH                  0x00
#define DFU_ATMEL_V2_MEM_EEPROM                 0x01
#define DFU_ATMEL_V2_MEM_SECURITY               0x02
#define DFU_ATMEL_V2_MEM_CONFIGURATION          0x03
#define DFU_ATMEL_V2_MEM_BOOTLOADER             0x04
#define DFU_ATMEL_V2_MEM_SIGNATURE              0x05
#define DFU_ATMEL_V2_MEM_USER                   0x06
#define DFU_ATMEL_V2_MEM_INT_RAM                0x07
#define DFU_ATMEL_V2_MEM_EXT_MEM_CS0            0x08
#define DFU_ATMEL_V2_MEM_EXT_MEM_CS1            0x09
#define DFU_ATMEL_V2_MEM_EXT_MEM_CS2            0x0A
#define DFU_ATMEL_V2_MEM_EXT_MEM_CS3            0x0B
#define DFU_ATMEL_V2_MEM_EXT_MEM_CS4            0x0C
#define DFU_ATMEL_V2_MEM_EXT_MEM_CS5            0x0D
#define DFU_ATMEL_V2_MEM_EXT_MEM_CS6            0x0E
#define DFU_ATMEL_V2_MEM_EXT_MEM_CS7            0x0F
#define DFU_ATMEL_V2_MEM_EXT_MEM_DF             0x10
#define DFU_ATMEL_V2_MEM_COUNT                  0x11	// Number of memory units
//@}
//@}
//@}

//@}

//@}

#endif // _USB_ATMEL_DFU_H_
