/**
 * \file
 *
 * \brief USB Device Audio 1.0 interface.
 *
 * Copyright (C) 2011 Atmel Corporation. All rights reserved.
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
#include "usb_protocol_audio.h"
#include "udd.h"
#include "udc.h"
#include "udi_audio.h"
#include <string.h>

#define UDI_AUDIO_NUM_BUFFERS 4

/**
 * \addtogroup udi_audio_group
 *
 * @{
 */

/**
 * \name Interface for UDC
 */
//@{

bool udi_audio_control_enable(void);
void udi_audio_control_disable(void);
bool udi_audio_control_setup(void);
bool udi_audio_stream_enable(void);
void udi_audio_stream_disable(void);
bool udi_audio_stream_setup(void);
uint8_t udi_audio_control_getsetting(void);
uint8_t udi_audio_stream_getsetting(void);
static void udi_audio_stream_feature_changed(void);
#ifdef UDI_AUDIO_ENABLE_AUDIO_OUT
static bool udi_audio_stream_out_start(void);
static void udi_audio_stream_out_stop(void);
static void udi_audio_stream_samples_received(udd_ep_status_t status, iram_size_t n);
#endif
#ifdef UDI_AUDIO_ENABLE_AUDIO_IN
static bool udi_audio_stream_in_start(void);
static void udi_audio_stream_in_stop(void);
static void udi_audio_stream_samples_sent(udd_ep_status_t status, iram_size_t n);
#endif

static uint8_t udi_audio_stream_buf_state[UDI_AUDIO_NUM_BUFFERS];
static uint8_t udi_audio_stream_buf[UDI_AUDIO_NUM_BUFFERS][UDI_AUDIO_EPS_SIZE];
static uint16_t udi_audio_stream_buf_len[UDI_AUDIO_NUM_BUFFERS];
static uint8_t udi_audio_buf_write_index = 0;
static uint8_t udi_audio_buf_read_index = 0;
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
static int16_t udi_audio_cur_volume = 0x0000;
static int16_t udi_audio_min_volume = 0x0000;
static int16_t udi_audio_max_volume = 0x7FFF;
static int16_t udi_audio_res_volume = 0x0001;
#endif
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_MUTE
static uint8_t udi_audio_cur_mute = false;
#endif

//! Global structure which contains standard UDI API for UDC
UDC_DESC_STORAGE udi_api_t udi_api_audio_control = {
	.enable = udi_audio_control_enable,
	.disable = udi_audio_control_disable,
	.setup = udi_audio_control_setup,
	.getsetting = udi_audio_control_getsetting,
};

UDC_DESC_STORAGE udi_api_t udi_api_audio_data = {
	.enable = udi_audio_stream_enable,
	.disable = udi_audio_stream_disable,
	.setup = udi_audio_stream_setup,
	.getsetting = udi_audio_stream_getsetting,
};

//@}

static uint8_t udi_audio_data_alt_setting = 0;

/**
 * \name Internal routines
 */
//@{

bool udi_audio_control_enable(void)
{
	return true;
}

void udi_audio_control_disable(void)
{
	// Do nothing - control interface has no endpoints to manage
}

bool udi_audio_control_setup(void)
{
	if (Udd_setup_is_in()) {
		// GET Interface Requests 
		if (Udd_setup_type() == USB_REQ_TYPE_CLASS) {
			// Requests Class Interface Get
			switch (udd_g_ctrlreq.req.bRequest) {
			case USB_REQ_AUDIO_GET_MIN:
				switch (udd_g_ctrlreq.req.wValue >> 8) {
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
				case AUDIO_CONTROL_VOLUME:
					udd_g_ctrlreq.payload =
							(uint8_t *) &
							udi_audio_min_volume;
					udd_g_ctrlreq.payload_size =
							sizeof(udi_audio_min_volume);
					return true;
#endif
				}
				break;
			case USB_REQ_AUDIO_GET_MAX:
				switch (udd_g_ctrlreq.req.wValue >> 8) {
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
				case AUDIO_CONTROL_VOLUME:
					udd_g_ctrlreq.payload =
							(uint8_t *) &
							udi_audio_max_volume;
					udd_g_ctrlreq.payload_size =
							sizeof(udi_audio_max_volume);
					return true;
#endif
				}
				break;
			case USB_REQ_AUDIO_GET_CUR:
				switch (udd_g_ctrlreq.req.wValue >> 8) {
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
				case AUDIO_CONTROL_VOLUME:
					udd_g_ctrlreq.payload =
							(uint8_t *) &
							udi_audio_cur_volume;
					udd_g_ctrlreq.payload_size =
							sizeof(udi_audio_cur_volume);
					return true;
#endif
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_MUTE
				case AUDIO_CONTROL_MUTE:
					udd_g_ctrlreq.payload =
							(uint8_t *) &
							udi_audio_cur_mute;
					udd_g_ctrlreq.payload_size =
							sizeof(udi_audio_cur_mute);
					return true;
#endif
				}
				break;
			case USB_REQ_AUDIO_GET_RES:
				switch (udd_g_ctrlreq.req.wValue >> 8) {
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
				case AUDIO_CONTROL_VOLUME:
					udd_g_ctrlreq.payload =
							(uint8_t *) &
							udi_audio_res_volume;
					udd_g_ctrlreq.payload_size =
							sizeof(udi_audio_res_volume);
					return true;
#endif
				}
				break;
			}
		}
	}
	if (Udd_setup_is_out()) {
		// SET Interface Requests  
		if (Udd_setup_type() == USB_REQ_TYPE_CLASS) {
			// Requests Class Interface Set
			switch (udd_g_ctrlreq.req.bRequest) {
			case USB_REQ_AUDIO_SET_CUR:
				switch (udd_g_ctrlreq.req.wValue >> 8) {
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
				case AUDIO_CONTROL_VOLUME:
					udd_g_ctrlreq.payload =
							(uint8_t *) &
							udi_audio_cur_volume;
					udd_g_ctrlreq.payload_size =
							sizeof(udi_audio_cur_volume);
					udd_g_ctrlreq.callback = udi_audio_stream_feature_changed;
					return true;
#endif
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_MUTE
				case AUDIO_CONTROL_MUTE:
					udd_g_ctrlreq.payload =
							(uint8_t *) &
							udi_audio_cur_mute;
					udd_g_ctrlreq.payload_size =
							sizeof(udi_audio_cur_mute);
					udd_g_ctrlreq.callback = udi_audio_stream_feature_changed;
					return true;
#endif				
				}
				break;
			}
		}
	}

	return false;
}

bool udi_audio_stream_enable(void)
{
	usb_iface_desc_t *current_interface_selected;
	current_interface_selected = udc_get_interface_desc();

	udi_audio_data_alt_setting = current_interface_selected->bAlternateSetting;

	udi_audio_buf_read_index = 0;
	udi_audio_buf_write_index = 0;

#ifdef UDI_AUDIO_ENABLE_AUDIO_IN
	if (udi_audio_data_alt_setting) {
		if (!udi_audio_stream_in_start())
			return false;

		UDI_AUDIO_ENABLE_EXT();
	}
	else {
		udi_audio_stream_in_stop();

		UDI_AUDIO_DISABLE_EXT();
	}
#endif
	
#ifdef UDI_AUDIO_ENABLE_AUDIO_OUT
	if (udi_audio_data_alt_setting) {
		if (!udi_audio_stream_out_start())
			return false;

		UDI_AUDIO_ENABLE_EXT();
	}
	else {
		udi_audio_stream_out_stop();

		UDI_AUDIO_DISABLE_EXT();
	}
#endif

	return true;
}

void udi_audio_stream_disable(void)
{
	udi_audio_data_alt_setting = 0;
	UDI_AUDIO_DISABLE_EXT();
}

bool udi_audio_stream_setup(void)
{
	return false;
}

uint8_t udi_audio_control_getsetting(void)
{
	// No alternative setting for the Audio control interface
	return 0;
}

uint8_t udi_audio_stream_getsetting(void)
{
	// Return if the audio stream is enabled or not (enabled when alt setting != 0)
	return udi_audio_data_alt_setting;
}

void udi_audio_process(void)
{
#ifdef UDI_AUDIO_ENABLE_AUDIO_OUT
	if (udi_audio_stream_buf_state[udi_audio_buf_write_index] == AUDIO_BUFFER_IDLE) {
		udi_audio_stream_out_start();
	}

	if (udi_audio_stream_buf_state[udi_audio_buf_read_index] == AUDIO_BUFFER_FULL) {
		UDI_AUDIO_PROCESS_EXT(udi_audio_stream_buf[udi_audio_buf_read_index],
		                      udi_audio_stream_buf_len[udi_audio_buf_read_index]);

		udi_audio_stream_buf_state[udi_audio_buf_read_index] = AUDIO_BUFFER_IDLE;
		udi_audio_stream_buf_len[udi_audio_buf_read_index] = 0;
		udi_audio_buf_read_index = (udi_audio_buf_read_index + 1) % UDI_AUDIO_NUM_BUFFERS;
	}
#endif

#ifdef UDI_AUDIO_ENABLE_AUDIO_IN
	if (udi_audio_stream_buf_state[udi_audio_buf_read_index] == AUDIO_BUFFER_FULL) {
		udi_audio_stream_in_start();
	}
#endif
}

void udi_audio_stream_set_feature_min(uint16_t feature, uint16_t value)
{
	switch (feature)
	{
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
		case AUDIO_CONTROL_VOLUME:
			udi_audio_min_volume = value;
			break;
#endif
	}
	
	udi_audio_stream_feature_changed();
}

void udi_audio_stream_set_feature_max(uint16_t feature, uint16_t value)
{
	switch (feature)
	{
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
		case AUDIO_CONTROL_VOLUME:
			udi_audio_max_volume = value;
			break;
#endif
	}
	
	udi_audio_stream_feature_changed();
}

void udi_audio_stream_set_feature_res(uint16_t feature, uint16_t value)
{
	switch (feature)
	{
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
		case AUDIO_CONTROL_VOLUME:
			udi_audio_res_volume = value;
			break;
#endif
	}
	
	udi_audio_stream_feature_changed();
}

void udi_audio_stream_set_feature_cur(uint16_t feature, uint16_t value)
{
	switch (feature)
	{
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
		case AUDIO_CONTROL_VOLUME:
			udi_audio_cur_volume = value;
			break;
#endif
	}
	
	udi_audio_stream_feature_changed();
}

uint16_t udi_audio_stream_get_feature_cur(uint16_t feature)
{
	switch (feature)
	{
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
		case AUDIO_CONTROL_VOLUME:
			return udi_audio_cur_volume;
#endif
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_MUTE
		case AUDIO_CONTROL_MUTE:
			return udi_audio_cur_mute;
#endif
	}
	
	return 0;
}

#ifdef UDI_AUDIO_ENABLE_AUDIO_IN
bool udi_audio_stream_samples_send(uint8_t* buffer, uint8_t length)
{
	while (!(udi_audio_stream_buf_state[udi_audio_buf_write_index] == AUDIO_BUFFER_IDLE)) {};

	memcpy(udi_audio_stream_buf[udi_audio_buf_write_index], buffer, length);

	udi_audio_stream_buf_len[udi_audio_buf_write_index] = length;
	udi_audio_stream_buf_state[udi_audio_buf_write_index] = AUDIO_BUFFER_FULL;
	udi_audio_buf_write_index = (udi_audio_buf_write_index + 1) % UDI_AUDIO_NUM_BUFFERS;
	
	if (udi_audio_stream_buf_state[udi_audio_buf_read_index] == AUDIO_BUFFER_FULL) {
		udi_audio_stream_in_start();
	}
	
	return true;
}
#endif

//-------------------------------------------------
//------- Internal routines to process data transfer

static void udi_audio_stream_feature_changed(void)
{
#if UDI_AUDIO_FEATURES & AUDIO_FEATURE_VOLUME
	if (udi_audio_curr_volume < udi_audio_min_volume) {
		udi_audio_curr_volume = udi_audio_min_volume;
	}
	else if (udi_audio_curr_volume > udi_audio_max_volume) {
		udi_audio_curr_volume = udi_audio_max_volume;
	}
#endif
}

#ifdef UDI_AUDIO_ENABLE_AUDIO_IN
static void udi_audio_stream_samples_sent(udd_ep_status_t status, iram_size_t n)
{
	if (UDD_EP_TRANSFER_OK != status) {
		// Abort reception
		udi_audio_stream_buf_state[udi_audio_buf_read_index] = AUDIO_BUFFER_IDLE;
		return;
	}
	
	udi_audio_stream_buf_state[udi_audio_buf_read_index] = AUDIO_BUFFER_IDLE;
	udi_audio_stream_buf_len[udi_audio_buf_read_index] = n;
	udi_audio_buf_read_index = (udi_audio_buf_read_index + 1) % UDI_AUDIO_NUM_BUFFERS;

	if (udi_audio_stream_buf_state[udi_audio_buf_read_index] == AUDIO_BUFFER_FULL) {
		udi_audio_stream_in_start();
	}
}

static bool udi_audio_stream_in_start(void)
{
	if (!udd_ep_run(UDI_AUDIO_EP_IN,
					true,
					udi_audio_stream_buf[udi_audio_buf_read_index],
					udi_audio_stream_buf_len[udi_audio_buf_read_index],
					udi_audio_stream_samples_sent)) {
		return false;
	}
	
	udi_audio_stream_buf_state[udi_audio_buf_read_index] = AUDIO_BUFFER_BUSY;
	return true;
}

static void udi_audio_stream_in_stop(void)
{
	udd_ep_abort(UDI_AUDIO_EP_IN);
}
#endif

#ifdef UDI_AUDIO_ENABLE_AUDIO_OUT
static void udi_audio_stream_samples_received(udd_ep_status_t status, iram_size_t n)
{
	if (UDD_EP_TRANSFER_OK != status) {
		// Abort reception
		udi_audio_stream_buf_state[udi_audio_buf_write_index] = AUDIO_BUFFER_IDLE;
		return;
	}
	
	udi_audio_stream_buf_state[udi_audio_buf_write_index] = AUDIO_BUFFER_FULL;
	udi_audio_stream_buf_len[udi_audio_buf_write_index] = n;
	udi_audio_buf_write_index = (udi_audio_buf_write_index + 1) % UDI_AUDIO_NUM_BUFFERS;

	if (udi_audio_stream_buf_state[udi_audio_buf_write_index] == AUDIO_BUFFER_IDLE) {
		udi_audio_stream_out_start();
	}
}

static bool udi_audio_stream_out_start(void)
{
	if (!udd_ep_run(UDI_AUDIO_EP_OUT,
					true,
					udi_audio_stream_buf[udi_audio_buf_write_index],
					UDI_AUDIO_EPS_SIZE,
					udi_audio_stream_samples_received)) {
		return false;
	}
	
	udi_audio_stream_buf_state[udi_audio_buf_write_index] = AUDIO_BUFFER_BUSY;
	return true;
}

static void udi_audio_stream_out_stop(void)
{
	udd_ep_abort(UDI_AUDIO_EP_OUT);
}
#endif

//@}
