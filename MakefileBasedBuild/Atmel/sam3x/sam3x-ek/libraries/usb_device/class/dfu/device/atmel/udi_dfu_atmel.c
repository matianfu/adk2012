/**
 * \file
 *
 * \brief USB Device Firmware Upgrade (DFU) interface definitions.
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

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_dfu.h"
#include "usb_atmel_dfu.h"
#include "udd.h"
#include "udc.h"
#include "udi_dfu_atmel.h"
#include "isp.h"
#include "string.h"


#ifndef UDI_DFU_ATMEL_PROTOCOL_VERSION
#  error UDI_DFU_ATMEL_PROTOCOL_VERSION must be define in conf_usb.h
#endif
#if ((UDI_DFU_ATMEL_PROTOCOL_VERSION != DFU_ATMEL_PROTOCOL_VERSION_2) \
    &&  (UDI_DFU_ATMEL_PROTOCOL_VERSION != DFU_ATMEL_PROTOCOL_VERSION_1))
#  error Bootloader protocol not supported (UDI_DFU_ATMEL_PROTOCOL_VERSION)
#endif


/**
 * \addtogroup udi_dfu_atmel_group
 * @{
 */

/**
 * \name Interface for UDC
 */
//@{

bool udi_dfu_atmel_enable(void);
void udi_dfu_atmel_disable(void);
bool udi_dfu_atmel_setup(void);
uint8_t udi_dfu_atmel_getsetting(void);

//! Global structure which contains standard UDI API for UDC
UDC_DESC_STORAGE udi_api_t udi_api_dfu_atmel = {
	.enable = udi_dfu_atmel_enable,
	.disable = udi_dfu_atmel_disable,
	.setup = udi_dfu_atmel_setup,
	.getsetting = udi_dfu_atmel_getsetting,
};

//@}


/**
 * \name Internal routines to manage DFU requests
 */
//@{
static void udi_dfu_atmel_reset_protocol(void);
static void udi_dfu_atmel_reset_cpu(void);

static bool udi_dfu_atmel_cmd_decode(void);
static void udi_dfu_atmel_sel_mem( uint8_t mem_num );
static bool udi_dfu_atmel_mem_protected(void);
static bool udi_dfu_atmel_mem_getaddr(uint8_t * arg);
static bool udi_dfu_atmel_mem_read(void);
static void udi_dfu_atmel_mem_check(void);
static bool udi_dfu_atmel_mem_send_last_add(void);
static bool udi_dfu_atmel_mem_write(void);

#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)
static bool udi_dfu_atmel_program(void);
static bool udi_dfu_atmel_read(void);
static bool udi_dfu_atmel_blankcheck(void);
static bool udi_dfu_atmel_erase_chip(void);
static void udi_dfu_atmel_start(void);
static bool udi_dfu_atmel_select_memory(void);
#else // V1
static bool udi_dfu_atmel_progstart(uint8_t mem);
static bool udi_dfu_atmel_read(uint8_t mem, bool b_check);
static void udi_dfu_atmel_read_id(uint8_t mem, uint8_t addr);
static bool udi_dfu_atmel_chip_erase(void);
static void udi_dfu_atmel_start_app(uint8_t mode);
#ifndef ISP_SMALL_MEMORY_SIZE
static bool udi_dfu_atmel_cmd_decode_changeaddr(void);
#endif
#endif
//@}

#define CAT_CMD(val1,val2) (((uint16_t)val1<<8)|(val2<<0))


/**
 * \name Internal variables to manage DFU requests
 */
//@{

static dfu_status_t udi_dfu_atmel_status;
#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)
#  ifdef UDI_DFU_ATMEL_PROTOCOL_2_SPLIT_ERASE_CHIP
static bool udi_dfu_atmel_erase_running;
#  endif
#endif

//! Structure to store the command fields
#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)
static dfu_atmel_v2_cmd_t udi_dfu_atmel_cmd;
#else // V1
static dfu_atmel_v1_cmd_t udi_dfu_atmel_cmd;
#endif

#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)
#define DFU_ATMEL_BUF_TRANS_SIZE     DFU_ATMEL_V2_BUF_TRANS_SIZE
#else // V1
#define DFU_ATMEL_BUF_TRANS_SIZE     DFU_ATMEL_V1_BUF_TRANS_SIZE
#endif


//! Buffer to receive or send data
COMPILER_WORD_ALIGNED
		static uint8_t
		udi_dfu_atmel_buf_trans[DFU_ATMEL_BUF_TRANS_SIZE];

//! Callback to use when an upload request is received
static bool(*udi_dfu_atmel_upload_callback) (void);

//! Notify a reset request to start
static void (*udi_dfu_atmel_reset_callback) (void);

//! Store the current security level
static bool udi_dfu_atmel_security;

/**
 * \name To manage memories
 */
//@{

/**
 * \name To manage memory transfers
 */
//@{
#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)
static bool udi_dfu_atmel_mem_b_protected;
#endif
static isp_addr_t udi_dfu_atmel_mem_add;
static isp_addr_t udi_dfu_atmel_mem_nb_data;
static isp_mem_t udi_dfu_atmel_mem_sel;
//@}



//@}

//@}


bool udi_dfu_atmel_enable(void)
{
	udi_dfu_atmel_reset_protocol();
	// Load chip information
	isp_init();
	udi_dfu_atmel_security = isp_is_security();
	return UDI_DFU_ENABLE_EXT();
}


void udi_dfu_atmel_disable(void)
{
	UDI_DFU_DISABLE_EXT();
}


bool udi_dfu_atmel_setup(void)
{
	//** Interface requests
	if (Udd_setup_type() != USB_REQ_TYPE_CLASS) {
		return false;	// Only class request decoded
	}

	if (Udd_setup_is_in()) {
		// Requests Class Interface Get
		switch (udd_g_ctrlreq.req.bRequest) {
		case USB_REQ_DFU_GETSTATUS:
#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)
#  ifdef UDI_DFU_ATMEL_PROTOCOL_2_SPLIT_ERASE_CHIP
			if (udi_dfu_atmel_erase_running) {
				udi_dfu_atmel_erase_chip();
			}
#  endif
#endif
			Assert(udd_g_ctrlreq.req.wValue==0);
			Assert(sizeof(udi_dfu_atmel_status)==udd_g_ctrlreq.req.wLength);
			udd_set_setup_payload(
					(uint8_t *) & udi_dfu_atmel_status,
					sizeof(udi_dfu_atmel_status));
			return true;

			// Used to send data to the host
			// when the previous Atmel command (DNLOAD) request data
		case USB_REQ_DFU_UPLOAD:
#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)
#  ifdef UDI_DFU_ATMEL_PROTOCOL_2_SPLIT_ERASE_CHIP
			Assert( !udi_dfu_atmel_erase_running );
#  endif
#endif
			Assert(DFU_STATE_DFUERROR != udi_dfu_atmel_status.bState);
			if (NULL != udi_dfu_atmel_upload_callback) {
				return udi_dfu_atmel_upload_callback();
			}
		}
	}
	
#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)
#  ifdef UDI_DFU_ATMEL_PROTOCOL_2_SPLIT_ERASE_CHIP
	Assert( !udi_dfu_atmel_erase_running );
#  endif
#endif
	
	if (Udd_setup_is_out()) {
		// Requests Class Interface Set
		switch (udd_g_ctrlreq.req.bRequest) {
		case USB_REQ_DFU_CLRSTATUS:
			Assert(udd_g_ctrlreq.req.wValue==0);
			Assert(udd_g_ctrlreq.req.wLength==0);
			udi_dfu_atmel_reset_protocol();
			return true;

			// DNLOAD request including Atmel command fields
			// and data for write operation.
			// For read operation, the data are sent in the next UPLOAD request
		case USB_REQ_DFU_DNLOAD:
			// Check if a reset has been requested
			if (NULL != udi_dfu_atmel_reset_callback) {
				// Remove force ISP before a reset CPU to start Application
				isp_force_isp(false);
				// Then this DNLOAD DFU request must be empty (0==wLength)
				// and valid the reset application command.
				// Valid SETUP request and reset application via request callback.
				udd_g_ctrlreq.callback = udi_dfu_atmel_reset_cpu;
				return true;
			}

			Assert(DFU_STATE_DFUERROR != udi_dfu_atmel_status.bState);
			Assert(udd_g_ctrlreq.req.wLength!=0);

			// The first packet contains the command
			// after this packet the over_under_run callback can be called 
			// if the Host want to send more data to device
			udd_set_setup_payload(
				(uint8_t *) & udi_dfu_atmel_cmd,
				sizeof(udi_dfu_atmel_cmd)),
			// Called when the first packet is received
			// before continuing DATA phase or start ZLP phase
			udd_g_ctrlreq.over_under_run = udi_dfu_atmel_cmd_decode;
			// Note udd_g_ctrlreq.callback is updated 
			// by udi_dfu_atmel_cmd_decode() before ZLP phase
			return true;
		}
	}
	// Unknown request
	udi_dfu_atmel_status.bStatus = DFU_STATUS_ERRSTALLEDPK;
	udi_dfu_atmel_status.bState = DFU_STATE_DFUERROR;
	return false;
}


uint8_t udi_dfu_atmel_getsetting(void)
{
	return 0;
}


static void udi_dfu_atmel_reset_protocol(void)
{
	// Reset DFU status
	udi_dfu_atmel_status.bStatus = DFU_STATUS_OK;
	udi_dfu_atmel_status.bState = DFU_STATE_DFUIDLE;
	// These fields are not used and always set to zero:
	// bwPollTimeout[3]
	// iString

	// Reset all callbacks
	udd_g_ctrlreq.over_under_run = NULL;
	udd_g_ctrlreq.callback = NULL;
	udi_dfu_atmel_reset_callback = NULL;
}

static void udi_dfu_atmel_reset_cpu(void)
{
	udi_dfu_atmel_reset_callback();
}


#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)

//! Called by over_under_run when the first data packet
//! from a DNLOAD DFU request is received
static bool udi_dfu_atmel_cmd_decode(void)
{
	// By default no callback initialized
	// By default request states are success and finish
	udi_dfu_atmel_reset_protocol();
	udi_dfu_atmel_upload_callback = NULL;

	// To restart ISP in case of USB cable is unplug during program load
	isp_force_isp(true);

	// Decode Atmel command ID
	switch (CAT_CMD(udi_dfu_atmel_cmd.group, udi_dfu_atmel_cmd.cmd_id)) {

	case CAT_CMD(DFU_ATMEL_V2_CMD_GRP_DNLOAD,
			DFU_ATMEL_V2_CMD_PROGRAM_START):
		return udi_dfu_atmel_program();

	case CAT_CMD(DFU_ATMEL_V2_CMD_GRP_UPLOAD,
					 DFU_ATMEL_V2_CMD_READ_MEMORY):
		return udi_dfu_atmel_read();
	case CAT_CMD(DFU_ATMEL_V2_CMD_GRP_UPLOAD,
					 DFU_ATMEL_V2_CMD_BLANK_CHECK):
		return udi_dfu_atmel_blankcheck();

	case CAT_CMD(DFU_ATMEL_V2_CMD_GRP_EXEC,
			DFU_ATMEL_V2_CMD_ERASE):
		return udi_dfu_atmel_erase_chip();
	case CAT_CMD(DFU_ATMEL_V2_CMD_GRP_EXEC,
			DFU_ATMEL_V2_CMD_START_APPLI):
		udi_dfu_atmel_start();
		return true;

	case CAT_CMD(DFU_ATMEL_V2_CMD_GRP_SELECT,
			DFU_ATMEL_V2_CMD_SELECT_MEMORY):
		return udi_dfu_atmel_select_memory();
	}
	return false;	// Unknow command
}

static bool udi_dfu_atmel_program(void)
{
	// Check list before start memory programmation
	if (udi_dfu_atmel_security && udi_dfu_atmel_mem_b_protected) {
		// Security enabled
		return udi_dfu_atmel_mem_protected();
	}
	if (!udi_dfu_atmel_mem_sel.fnct_write) {
		udi_dfu_atmel_status.bStatus = DFU_STATUS_ERRWRITE;
		udi_dfu_atmel_status.bState = DFU_STATE_DFUERROR;
		return false;	// Write memory not available
	}
	if (!udi_dfu_atmel_mem_getaddr(&udi_dfu_atmel_cmd.arg[0]))
		return false;	// Bad Range
	
	// Init buffer to fill during next DATA phase of request
	udd_set_setup_payload(
		udi_dfu_atmel_buf_trans,
		DFU_ATMEL_BUF_TRANS_SIZE);
	// Init callback called after buffer filled
	udd_g_ctrlreq.over_under_run = udi_dfu_atmel_mem_write;
	return true;
}

static bool udi_dfu_atmel_read(void)
{
	// Check before decoding the command
	if (!udi_dfu_atmel_mem_getaddr(&udi_dfu_atmel_cmd.arg[0]))
		return false;	// Range bad

	if (!udi_dfu_atmel_mem_sel.fnct_read) {
		udi_dfu_atmel_status.bStatus = DFU_STATUS_ERRWRITE;
		udi_dfu_atmel_status.bState = DFU_STATE_DFUERROR;
		return false;	// Read memory not available
	}

	if (udi_dfu_atmel_security && udi_dfu_atmel_mem_b_protected) {
		// Memory security then remove read callback by protected callback
		udi_dfu_atmel_upload_callback = udi_dfu_atmel_mem_protected;
	}else{
		udi_dfu_atmel_upload_callback = udi_dfu_atmel_mem_read;
	}
	return true;
}

static bool udi_dfu_atmel_blankcheck(void)
{
	// Check before decoding the command
	if (!udi_dfu_atmel_mem_getaddr(&udi_dfu_atmel_cmd.arg[0]))
		return false;	// Range bad

	if (!udi_dfu_atmel_mem_sel.fnct_read) {
		udi_dfu_atmel_status.bStatus = DFU_STATUS_ERRWRITE;
		udi_dfu_atmel_status.bState = DFU_STATE_DFUERROR;
		return false;	// Read memory not available
	}
	udi_dfu_atmel_mem_check();
  	return true;
}

static bool udi_dfu_atmel_erase_chip(void)
{
	Assert(udi_dfu_atmel_cmd.arg[0]==DFU_ATMEL_V2_CMD_ERASE_ARG_CHIP);

#ifdef UDI_DFU_ATMEL_PROTOCOL_2_SPLIT_ERASE_CHIP
	if (isp_erase_chip()) {
		// Erase finish
		udi_dfu_atmel_security = false;
		udi_dfu_atmel_erase_running = false;
		udi_dfu_atmel_status.bStatus = DFU_STATUS_OK;
		udi_dfu_atmel_status.bState = DFU_STATE_DFUIDLE;
	}else{
		// Erase on-going
		udi_dfu_atmel_erase_running = true;
		udi_dfu_atmel_status.bStatus = DFU_STATUS_ERRNOTDONE;
		udi_dfu_atmel_status.bState = DFU_STATE_DFUDNBUSY;
	}
#else
	if (!isp_erase_chip()) {
		return false;
	}
	// Erase finish
	udi_dfu_atmel_security = false;
#endif		
	return true;
}

static void udi_dfu_atmel_start(void)
{
	// Start application reset after next DNLOAD request
	if (udi_dfu_atmel_cmd.arg[0] == DFU_ATMEL_V2_CMD_START_APPLI_ARG_RESET) {
		udi_dfu_atmel_reset_callback = isp_start_appli_rst;
	}else{
		Assert(app == DFU_ATMEL_V2_CMD_START_APPLI_ARG_NO_RESET);
		udi_dfu_atmel_reset_callback = isp_start_appli_norst;
	}	
}

static bool udi_dfu_atmel_select_memory(void)
{
	switch (udi_dfu_atmel_cmd.arg[0]) {
	case DFU_ATMEL_V2_CMD_SELECT_MEMORY_ARG_UNIT:
		if (DFU_ATMEL_V2_MEM_COUNT <= udi_dfu_atmel_cmd.arg[1]) {
			udi_dfu_atmel_status.bStatus = DFU_STATUS_ERRADDRESS;
			udi_dfu_atmel_status.bState = DFU_STATE_DFUERROR;
			return false;	// Memory id error
		}
		udi_dfu_atmel_sel_mem(udi_dfu_atmel_cmd.arg[1]);
		udi_dfu_atmel_mem_add = 0;
		break;

#ifndef ISP_SMALL_MEMORY_SIZE
	case DFU_ATMEL_V2_CMD_SELECT_MEMORY_ARG_PAGE:
		{
			uint32_t tmp = 0;
			MSB0W(tmp) = udi_dfu_atmel_cmd.arg[1];
			MSB1W(tmp) = udi_dfu_atmel_cmd.arg[2];
			if (tmp >= udi_dfu_atmel_mem_sel.size) {
				udi_dfu_atmel_status.bStatus = DFU_STATUS_ERRADDRESS;
				udi_dfu_atmel_status.bState = DFU_STATE_DFUERROR;
				return false;	// Address error
			}
			udi_dfu_atmel_mem_add = tmp;
		}
		break;
#endif
		
	default:
		Assert(false);	// Bad command
		break;
	}

	return true;
}


#else // V1

// Called when a DNLOAD DFU request is received
// The DFU Atmel command are sending in DNLOAD request
//
// Note: An Atmel DFU commands can be stalled in following cases:
// memory security, bad address or blank check fail
static bool udi_dfu_atmel_cmd_decode(void)
{
	// By default no callback initialized
	// By default request states are success and finish
	udi_dfu_atmel_reset_protocol();
	udi_dfu_atmel_upload_callback = NULL;
	
	// Decode Atmel command ID
	switch (CAT_CMD(udi_dfu_atmel_cmd.cmd_id,
								 udi_dfu_atmel_cmd.arg[0])) {
		
#ifndef ISP_SMALL_MEMORY_SIZE
	// Command to change high address
	case CAT_CMD(DFU_ATMEL_V1_CMD_CHANGE_BASE_ADDR,
					 DFU_ATMEL_V1_CMD_CHANGE_BASE_ADDR_ARG0):
		return udi_dfu_atmel_cmd_decode_changeaddr();
#endif
		
	// Commands to program a memory
	case CAT_CMD(DFU_ATMEL_V1_CMD_PROG_START,
					 DFU_ATMEL_V1_CMD_PROG_START_ARG_FLASH):
		return udi_dfu_atmel_progstart(DFU_ATMEL_V1_MEM_FLASH);
	case CAT_CMD(DFU_ATMEL_V1_CMD_PROG_START,
					 DFU_ATMEL_V1_CMD_PROG_START_ARG_EEPROM):
		return udi_dfu_atmel_progstart(DFU_ATMEL_V1_MEM_EEPROM);
	case CAT_CMD(DFU_ATMEL_V1_CMD_PROG_START,
					 DFU_ATMEL_V1_CMD_PROG_START_ARG_CUSTOM):
		return udi_dfu_atmel_progstart(DFU_ATMEL_V1_MEM_CUSTOM);
		
	// Commands to read a memory
	case CAT_CMD(DFU_ATMEL_V1_CMD_READ,
					 DFU_ATMEL_V1_CMD_READ_ARG_FLASH):
		return udi_dfu_atmel_read(DFU_ATMEL_V1_MEM_FLASH,false);
	case CAT_CMD(DFU_ATMEL_V1_CMD_READ,
					 DFU_ATMEL_V1_CMD_READ_ARG_EEPROM):
		return udi_dfu_atmel_read(DFU_ATMEL_V1_MEM_EEPROM,false);
	case CAT_CMD(DFU_ATMEL_V1_CMD_READ,
					 DFU_ATMEL_V1_CMD_READ_ARG_CUSTOM):
		return udi_dfu_atmel_read(DFU_ATMEL_V1_MEM_CUSTOM,false);

	// Commands to blank check a memory
	case CAT_CMD(DFU_ATMEL_V1_CMD_READ,
					 DFU_ATMEL_V1_CMD_READ_ARG_FLASHCHECK):
		return udi_dfu_atmel_read(DFU_ATMEL_V1_MEM_FLASH,true);
	}
	
	switch (CAT_CMD(udi_dfu_atmel_cmd.cmd_id,
						 udi_dfu_atmel_cmd.arg[0])) {

	// Commands to erase chip
	case CAT_CMD(DFU_ATMEL_V1_CMD_WRITE,
					 DFU_ATMEL_V1_CMD_WRITE_ARG_ERASE):
		return udi_dfu_atmel_chip_erase();

	// Commands to start application
	case CAT_CMD(DFU_ATMEL_V1_CMD_WRITE,
					 DFU_ATMEL_V1_CMD_WRITE_ARG_RST):
		udi_dfu_atmel_start_app(udi_dfu_atmel_cmd.arg[1]);
		return true;

	// Commands to read Bootloader version
	case CAT_CMD(DFU_ATMEL_V1_CMD_READ_ID,
					 DFU_ATMEL_V1_CMD_READ_ID_ARG_BOOTLOADER):
		udi_dfu_atmel_read_id(DFU_ATMEL_V1_MEM_BOOTLOADER,
									 udi_dfu_atmel_cmd.arg[1]);
		return true;

	// Commands to read Chip indentification
	case CAT_CMD(DFU_ATMEL_V1_CMD_READ_ID,
					 DFU_ATMEL_V1_CMD_READ_ID_ARG_SIGNATURE):
		switch (udi_dfu_atmel_cmd.arg[1]) {
		case DFU_ATMEL_V1_CMD_READ_ID_SIGNATURE_ARG_MANUF:
			udi_dfu_atmel_read_id(DFU_ATMEL_V1_MEM_SIGNATURE,0);
			break;
		case DFU_ATMEL_V1_CMD_READ_ID_SIGNATURE_ARG_FAMILY:
			udi_dfu_atmel_read_id(DFU_ATMEL_V1_MEM_SIGNATURE,1);
			break;
		case DFU_ATMEL_V1_CMD_READ_ID_SIGNATURE_ARG_PRODUCT:
			udi_dfu_atmel_read_id(DFU_ATMEL_V1_MEM_SIGNATURE,2);
			break;
		case DFU_ATMEL_V1_CMD_READ_ID_SIGNATURE_ARG_REVISION:
			udi_dfu_atmel_read_id(DFU_ATMEL_V1_MEM_SIGNATURE,3);
			break;
		}
		return true;
	}
	
	return false;	// Unknow command
}

static bool udi_dfu_atmel_progstart(uint8_t mem)
{
	udi_dfu_atmel_sel_mem(mem);
	
	if (udi_dfu_atmel_security) {
		return udi_dfu_atmel_mem_protected();
	}

	if (!udi_dfu_atmel_mem_getaddr(&udi_dfu_atmel_cmd.arg[1])) {
		return false;	// Bad Range
	}

	// Init buffer to fill during next DATA phase of request
	udd_set_setup_payload(
		udi_dfu_atmel_buf_trans,
		DFU_ATMEL_BUF_TRANS_SIZE);
	// Init callback called after buffer filled
	udd_g_ctrlreq.over_under_run = udi_dfu_atmel_mem_write;
	return true;
}

static bool udi_dfu_atmel_read(uint8_t mem, bool b_check)
{
	udi_dfu_atmel_sel_mem( mem );

	if (!udi_dfu_atmel_mem_getaddr(&udi_dfu_atmel_cmd.arg[1]))
		return false;	// Bad Range

	if ((!udi_dfu_atmel_mem_sel.fnct_read) || udi_dfu_atmel_security) {
		// Read memory not available OR memory protected
		// then accept request but stall next UPLOAD DFU request
		udi_dfu_atmel_upload_callback = udi_dfu_atmel_mem_protected;
		return true;
	}
   
	if (b_check) {
		// It is not a read operation then it is a blanc check, thus do it now
		udi_dfu_atmel_mem_check();
	}else{
		udi_dfu_atmel_upload_callback = udi_dfu_atmel_mem_read;
	}
	return true;
}


static bool udi_dfu_atmel_chip_erase(void)
{
	Assert(udi_dfu_atmel_cmd.arg[1]==DFU_ATMEL_V1_CMD_WRITE_ARG_ERASE_CHIP);
	if (!isp_erase_chip()) {
		return false;
	}
	udi_dfu_atmel_security = false;
	return true;
}


static void udi_dfu_atmel_start_app(uint8_t mode)
{
	// Start application reset after next DNLOAD request
	if (mode == DFU_ATMEL_V1_CMD_WRITE_ARG_RST_HW) {
		udi_dfu_atmel_reset_callback = isp_start_appli_rst;
	}else{
		Assert(app == DFU_ATMEL_V1_CMD_WRITE_ARG_RST_SF);
		udi_dfu_atmel_reset_callback = isp_start_appli_norst;
	}
}

static void udi_dfu_atmel_read_id(uint8_t mem, uint8_t addr)
{
	udi_dfu_atmel_sel_mem( mem );
	udi_dfu_atmel_mem_add = addr;
	udi_dfu_atmel_mem_nb_data = 1;
	udi_dfu_atmel_upload_callback = udi_dfu_atmel_mem_read;
}

#ifndef ISP_SMALL_MEMORY_SIZE
static bool udi_dfu_atmel_cmd_decode_changeaddr(void)
{
	udi_dfu_atmel_mem_add = ((uint32_t)udi_dfu_atmel_cmd.arg[2])<<16;
	return true;
}
#endif

#endif // Protocol V1 or V2


static void udi_dfu_atmel_sel_mem( uint8_t mem_num )
{
#if (UDI_DFU_ATMEL_PROTOCOL_VERSION == DFU_ATMEL_PROTOCOL_VERSION_2)
	if((mem_num!=DFU_ATMEL_V2_MEM_CONFIGURATION)
	&&(mem_num!=DFU_ATMEL_V2_MEM_SECURITY)
	&&(mem_num!=DFU_ATMEL_V2_MEM_BOOTLOADER)
	&&(mem_num!=DFU_ATMEL_V2_MEM_SIGNATURE)) {
		udi_dfu_atmel_mem_b_protected = true;
	}else{
		udi_dfu_atmel_mem_b_protected = false;
	}
#endif
	udi_dfu_atmel_mem_sel = *isp_memories.mem[mem_num];
}

static bool udi_dfu_atmel_mem_protected(void)
{
	udi_dfu_atmel_status.bStatus = DFU_STATUS_ERRWRITE;
	udi_dfu_atmel_status.bState = DFU_STATE_DFUIDLE;
	return false;
}


static bool udi_dfu_atmel_mem_getaddr(uint8_t * arg)
{
	isp_addr_t addr_end;

	// Get address for request argument
	udi_dfu_atmel_mem_add = 
		(udi_dfu_atmel_mem_add&0xFFFF0000) + ((uint16_t)arg[0]<<8) + (arg[1]<<0);
	addr_end = 
		(udi_dfu_atmel_mem_add&0xFFFF0000) + ((uint16_t)arg[2]<<8) + (arg[3]<<0);

	Assert(addr_end >= udi_dfu_atmel_mem_add);

	// Check address
	if (addr_end >= udi_dfu_atmel_mem_sel.size) {
		udi_dfu_atmel_status.bStatus = DFU_STATUS_ERRADDRESS;
		udi_dfu_atmel_status.bState = DFU_STATE_DFUERROR;
		return false;
	}

	// Compute the number of data to transfer
	udi_dfu_atmel_mem_nb_data = addr_end - udi_dfu_atmel_mem_add + 1;
	return true;
}


static bool udi_dfu_atmel_mem_read(void)
{
	Assert(udi_dfu_atmel_mem_nb_data <= DFU_ATMEL_BUF_TRANS_SIZE);
	udi_dfu_atmel_mem_sel.fnct_read(udi_dfu_atmel_buf_trans,
		udi_dfu_atmel_mem_add, udi_dfu_atmel_mem_nb_data);

	// Init buffer to transfer
	udd_set_setup_payload( udi_dfu_atmel_buf_trans, udi_dfu_atmel_mem_nb_data);
	return true;
}


static void udi_dfu_atmel_mem_check(void)
{
	uint8_t *ptr_buf;
	uint16_t packet_size;
	
	while (udi_dfu_atmel_mem_nb_data) {
		// Compute buffer to read
		packet_size = min(udi_dfu_atmel_mem_nb_data,
			DFU_ATMEL_BUF_TRANS_SIZE);
		udi_dfu_atmel_mem_nb_data -= packet_size;

		// Fill buffer from memory
		udi_dfu_atmel_mem_sel.fnct_read(udi_dfu_atmel_buf_trans,
			udi_dfu_atmel_mem_add, packet_size);

		// Check buffer content
		ptr_buf = udi_dfu_atmel_buf_trans;
		while (packet_size--) {
			if (*ptr_buf++ != 0xFF) {
				// Error, don't stall request but:
				// Update DFU status
				udi_dfu_atmel_status.bStatus = DFU_STATUS_ERRCHECK_ERASED;
				// Send last address checked in next Upload command
				udi_dfu_atmel_upload_callback = udi_dfu_atmel_mem_send_last_add;
				return;
			}
			udi_dfu_atmel_mem_add++;
		}
	}
}


static bool udi_dfu_atmel_mem_send_last_add(void)
{
	// Send last checked address 
	udi_dfu_atmel_buf_trans[0] = (uint8_t)(udi_dfu_atmel_mem_add>>8);
	udi_dfu_atmel_buf_trans[1] = (uint8_t)udi_dfu_atmel_mem_add;
	// Init buffer to transfer
	udd_set_setup_payload( udi_dfu_atmel_buf_trans, 2);
	return true;
}


static bool udi_dfu_atmel_mem_write(void)
{
	uint8_t padding_prefix;

	Assert(udi_dfu_atmel_mem_nb_data <= DFU_ATMEL_BUF_TRANS_SIZE);
	Assert(udi_dfu_atmel_mem_nb_data == udd_g_ctrlreq.payload_size);
	
	// In order to be in accordance with the memory write entity (page size),
	// X non-significant bytes may be added before the first byte to program.
	// The X number is calculated to align the beginning of the firmware 
	// with the memory write entity.
	padding_prefix = Get_align(udi_dfu_atmel_mem_add,
			USB_DEVICE_EP_CTRL_SIZE);

		// Program data in memory
		udi_dfu_atmel_mem_sel.fnct_write
				(udi_dfu_atmel_mem_add,
				udi_dfu_atmel_buf_trans + padding_prefix,
				udi_dfu_atmel_mem_nb_data);

	// Init callback called after buffer filled
	udd_g_ctrlreq.over_under_run = NULL;
	return true;
}


//@}
