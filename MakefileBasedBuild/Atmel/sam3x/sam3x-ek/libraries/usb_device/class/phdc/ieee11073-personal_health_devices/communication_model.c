/**
 * \file
 *
 * \brief Sample of IEE11073 Communication Model
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
#include "udi_phdc.h"
#include "communication_model.h"

//! @defgroup IEEE11073_COM_MODEL_PHDC Structure used by Communication Model
//! to connect a USB PHDC Device
//! @{
#define  IEEE11073_LGT_OPAQUEDATA_IN   10
#define  IEEE11073_LGT_OPAQUEDATA_OUT  10
#define  IEEE11073_LGT_METADATA_IN     80
#define  IEEE11073_LGT_METADATA_OUT    80
udi_phdc_metadata_t ieee11073_g_phdc_metadata_in;
udi_phdc_metadata_t ieee11073_g_phdc_metadata_out;
uint8_t ieee11073_g_opaquedata_in[IEEE11073_LGT_OPAQUEDATA_IN];
uint8_t ieee11073_g_opaquedata_out[IEEE11073_LGT_OPAQUEDATA_OUT];
uint8_t ieee11073_g_metadata_in[IEEE11073_LGT_METADATA_IN];
uint8_t ieee11073_g_metadata_out[IEEE11073_LGT_METADATA_OUT];
//! @}


//! @defgroup IEEE11073_COM_MODEL_STATE Communication model processus states
//! @{
#define  IEEE11073_STATE_DISABLE       0
#define  IEEE11073_STATE_WAIT_ASSOC    1
#define  IEEE11073_STATE_RUNNING       2
uint8_t ieee11073_g_state = IEEE11073_STATE_DISABLE;
//! @}


//! @defgroup IEEE11073_COM_MODEL_EVENT Internal events
//! for communication model processus
//! @{
#define  IEEE11073_EVENT_NONE          0
#define  IEEE11073_EVENT_ENABLE        1
#define  IEEE11073_EVENT_RECEIVED        3
#define  IEEE11073_EVENT_RETRY_RECEP   4
#define  IEEE11073_EVENT_SENDING       5
#define  IEEE11073_EVENT_SEND_ABORT    6
#define  IEEE11073_EVENT_MEASURE_1      7
#define  IEEE11073_EVENT_MEASURE_2      8
uint8_t ieee11073_g_event;
//! @}


//! @defgroup PHD_DATA Array with IEEE11073 data
//! @{                                                  
//! Association Data Request
uint8_t phd_assoc_req[] = {
	0xE2, 0x00, 0x00, 0x32, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x2A,
	0x50, 0x79,
	0x00, 0x26, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x08,
	0x4C, 0x4E, 0x49, 0x41, 0x47, 0x45, 0x4E, 0x54, 0x40, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

//! Sample of measure
uint8_t phd_measure[] = {
	0xE7, 0x00, 0x00, 0x5A, 0x00, 0x58, 0x12, 0x36, 0x01, 0x01, 0x00, 0x52,
	0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0D, 0x1D, 0x00, 0x48, 0xF0, 0x00, 0x00, 0x00,
	0x00, 0x04,
	0x00, 0x40, 0x00, 0x01, 0x00, 0x0C, 0xFF, 0x00, 0x02, 0xFA, 0x20, 0x07,
	0x12, 0x06,
	0x12, 0x10, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0C, 0xFF, 0x00, 0x00, 0xF3,
	0x20, 0x09, 0x06, 0x12,
	0x12, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0C, 0xFF, 0x00, 0x02, 0xF8,
	0x20, 0x09, 0x06, 0x12,
	0x20, 0x05, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0C, 0xFF, 0x00, 0x00, 0xF2,
	0x20, 0x09, 0x06, 0x12,
	0x20, 0x05, 0x00, 0x00
};

//! Device Attributes
uint8_t phd_attr[] = {
	0xE7, 0x00, 0x00, 0x6E, 0x00, 0x6C, 0x00, 0x02, 0x02, 0x03, 0x00, 0x66,
	0x00, 0x00, 0x00, 0x06,
	0x00, 0x60, 0x0A, 0x5A, 0x00, 0x08, 0x00, 0x01, 0x00, 0x04, 0x10, 0x0F,
	0x00, 0x01, 0x09, 0x28,
	0x00, 0x1A, 0x00, 0x0A, 0x46, 0x72, 0x65, 0x65, 0x73, 0x63, 0x61, 0x6C,
	0x65, 0x20, 0x00, 0x0C, 0x4D, 0x65,
	0x64, 0x69, 0x63, 0x61, 0x6C, 0x20, 0x43, 0x46, 0x56, 0x31, 0x09, 0x84,
	0x00, 0x0A,
	0x00, 0x08, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x0a, 0x44,
	0x00, 0x02, 0x40, 0x00,
	0x09, 0x2D, 0x00, 0x12, 0x00, 0x01, 0x00, 0x0E, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x08, 0x44, 0x45,
	0x31, 0x32, 0x34, 0x35, 0x36, 0x37, 0x09, 0x87, 0x00, 0x08, 0x20, 0x09,
	0x06, 0x12,
	0x12, 0x05, 0x00, 0x00
};

//! @}


//! Internal routines
static bool ieee11073_enable_reception(void);
static bool ieee11073_decode_metadata(void);
void ieee11073_received(uint16_t nb_received);
void ieee11073_sent(uint16_t nb_send);


/*! \brief Enable the communication model processus
 *
 * \retval TRUE
 */
bool ieee11073_com_model_enable(void)
{
	ieee11073_g_event = IEEE11073_EVENT_ENABLE;
	return TRUE;
}

/*! \brief Disable the communication model processus
 */
void ieee11073_com_model_disable(void)
{
	ieee11073_g_event = IEEE11073_EVENT_NONE;
	ieee11073_g_state = IEEE11073_STATE_DISABLE;
}

/*! \brief Send a measure on communication line
 */
void ieee11073_com_model_send_measure_1(void)
{
	if (IEEE11073_EVENT_NONE != ieee11073_g_event)
		return;
	ieee11073_g_event = IEEE11073_EVENT_MEASURE_1;
}

/*! \brief Send a measure on communication line
 */
void ieee11073_com_model_send_measure_2(void)
{
	if (IEEE11073_EVENT_NONE != ieee11073_g_event)
		return;
	ieee11073_g_event = IEEE11073_EVENT_MEASURE_2;
}

/*! \brief Callback called when a data is received
 *
 * \param nb_received  Number of data received
 */
void ieee11073_received(uint16_t nb_received)
{
	ieee11073_g_phdc_metadata_out.metadata_size = nb_received;
	ieee11073_g_event = IEEE11073_EVENT_RECEIVED;
}

/*! \brief Callback called when a data is sent
 *
 * \param nb_send  Number of data sent
 */
void ieee11073_sent(uint16_t nb_send)
{
	if (0 == nb_send) {
		ieee11073_g_event = IEEE11073_EVENT_SEND_ABORT;
		return;
	}
	ieee11073_g_phdc_metadata_in.metadata_size = nb_send;
	ieee11073_g_event = IEEE11073_EVENT_SENDING;
}

/*! \brief Function to process communication model
 * Must be scheduled by other process
 */
bool ieee11073_com_model_process(void)
{
	if (IEEE11073_EVENT_NONE == ieee11073_g_event)
		goto ieee11073_com_model_process_end;

	if (IEEE11073_EVENT_RETRY_RECEP == ieee11073_g_event) {
		ieee11073_g_event = IEEE11073_EVENT_NONE;
		ieee11073_enable_reception();
		goto ieee11073_com_model_process_end;
	}

	switch (ieee11073_g_state) {

	case IEEE11073_STATE_DISABLE:
		if (IEEE11073_EVENT_ENABLE != ieee11073_g_event)
			break;
		ieee11073_g_phdc_metadata_in.qos = USB_PHDC_QOS_MEDIUM_BEST;
		ieee11073_g_phdc_metadata_in.opaque_size = 0;
		ieee11073_g_phdc_metadata_in.metadata = phd_assoc_req;
		ieee11073_g_phdc_metadata_in.metadata_size =
				sizeof(phd_assoc_req);
		if (!udi_phdc_senddata(&ieee11073_g_phdc_metadata_in,
						ieee11073_sent)) {
			break;
		}
		ieee11073_g_state = IEEE11073_STATE_WAIT_ASSOC;
		break;


	case IEEE11073_STATE_WAIT_ASSOC:
		switch (ieee11073_g_event) {
		case IEEE11073_EVENT_SEND_ABORT:
			// Descriptor abort then resend it
			ieee11073_g_phdc_metadata_in.qos =
					USB_PHDC_QOS_MEDIUM_BEST;
			ieee11073_g_phdc_metadata_in.opaque_size = 0;
			ieee11073_g_phdc_metadata_in.metadata = phd_assoc_req;
			ieee11073_g_phdc_metadata_in.metadata_size =
					sizeof(phd_assoc_req);
			if (!udi_phdc_senddata(&ieee11073_g_phdc_metadata_in,
							ieee11073_sent)) {
				break;
			}
			break;
		case IEEE11073_EVENT_SENDING:
			// Descriptor sending, OK
			// Now wait for association to be accepted
			if (!ieee11073_enable_reception())
				goto ieee11073_com_model_process_end;
			break;
		case IEEE11073_EVENT_RECEIVED:
			ieee11073_decode_metadata();
			if (!ieee11073_enable_reception())
				goto ieee11073_com_model_process_end;
			break;
		}
		break;


	case IEEE11073_STATE_RUNNING:
		switch (ieee11073_g_event) {
		case IEEE11073_EVENT_RECEIVED:
			ieee11073_decode_metadata();
			// Restart recption
			if (!ieee11073_enable_reception())
				goto ieee11073_com_model_process_end;
			break;

		case IEEE11073_EVENT_MEASURE_1:
		case IEEE11073_EVENT_MEASURE_2:
			ieee11073_g_phdc_metadata_in.opaque_size = 0;
			ieee11073_g_phdc_metadata_in.metadata = phd_measure;
			ieee11073_g_phdc_metadata_in.metadata_size =
					sizeof(phd_measure);
			if (IEEE11073_EVENT_MEASURE_1 == ieee11073_g_event)
				ieee11073_g_phdc_metadata_in.qos =
						USB_PHDC_QOS_LOW_GOOD;
			else
				ieee11073_g_phdc_metadata_in.qos =
						USB_PHDC_QOS_MEDIUM_BETTER;
			if (!udi_phdc_senddata(&ieee11073_g_phdc_metadata_in,
							ieee11073_sent))
				break;
		}
	}

	ieee11073_g_event = IEEE11073_EVENT_NONE;

ieee11073_com_model_process_end:
	return (ieee11073_g_state == IEEE11073_STATE_RUNNING);
}

/*! \brief Start the reception of data on communication line
 *
 * \retval TRUE   Reception started
 * \retval FALSE  Impossible to start reception
 */
static bool ieee11073_enable_reception(void)
{
	ieee11073_g_phdc_metadata_out.opaquedata = ieee11073_g_opaquedata_out;
	ieee11073_g_phdc_metadata_out.opaque_size =
			IEEE11073_LGT_OPAQUEDATA_OUT;
	ieee11073_g_phdc_metadata_out.metadata = ieee11073_g_metadata_out;
	ieee11073_g_phdc_metadata_out.metadata_size =
			IEEE11073_LGT_METADATA_OUT;
	if (!udi_phdc_waitdata(&ieee11073_g_phdc_metadata_out,
					ieee11073_received)) {
		ieee11073_g_event = IEEE11073_EVENT_RETRY_RECEP;
		return FALSE;
	}
	return TRUE;
}


/*! \brief Start the data  reception on communication line
 *
 * \retval TRUE   Metadata correct
 * \retval FALSE  Metadata unknow
 */
static bool ieee11073_decode_metadata(void)
{
	typedef struct {
		uint16_t choice;
		uint16_t length;
		uint8_t values[1];
	} Phd_t;

	Phd_t *phd = (Phd_t *) ieee11073_g_phdc_metadata_out.metadata;

	if (0 == ieee11073_g_phdc_metadata_out.metadata_size)
		return FALSE;	// Error reception
	if (ieee11073_g_phdc_metadata_out.metadata_size !=
			(4 + be16_to_cpu(phd->length)))
		return FALSE;	// Error on length

	switch (be16_to_cpu(phd->choice)) {

	case 0xE300:
		if (0 == be16_to_cpu(*((uint16_t *) & phd->values[0]))) {
			ieee11073_g_state = IEEE11073_STATE_RUNNING;
			return TRUE;
		}
		break;

	case 0xE700:
		{
			uint16_t invoke_id;
			uint16_t choice;

			if (ieee11073_g_phdc_metadata_out.metadata_size !=
					(6 + be16_to_cpu(*((uint16_t *) & phd->values[0]))))
				return FALSE;	// Error on length

			invoke_id = *((uint16_t *) & phd->values[2]);
			choice = *((uint16_t *) & phd->values[4]);

			if (be16_to_cpu(choice) == 0x0103) {
				invoke_id = be16_to_cpu(invoke_id);
				phd_attr[6] = (uint8_t) (invoke_id >> 8);
				phd_attr[7] = (uint8_t) (invoke_id & 0xFF);
				ieee11073_g_phdc_metadata_in.qos =
						USB_PHDC_QOS_MEDIUM_BEST;
				ieee11073_g_phdc_metadata_in.opaque_size = 0;
				ieee11073_g_phdc_metadata_in.metadata =
						phd_attr;
				ieee11073_g_phdc_metadata_in.metadata_size =
						sizeof(phd_attr);
				if (!udi_phdc_senddata
						(&ieee11073_g_phdc_metadata_in,
								ieee11073_sent))
				{
					break;
				}
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}
