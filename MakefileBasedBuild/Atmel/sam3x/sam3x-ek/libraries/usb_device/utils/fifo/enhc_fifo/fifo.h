/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief This file controls the software FIFO management.
 *
 * These functions manages FIFOs thanks to simple a API. The FIFO can
 * be 100% full thanks to a double-index range implementation. For example,
 * a FIFO of 4 elements can be implemented: the FIFO can really hold up to 4
 * elements.
 * This is particurly well suited for any kind of application needing a lot of 
 * small FIFO.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ***************************************************************************/

/* Copyright (c) 2010 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#ifndef _FIFO_H_
#define _FIFO_H_

//! Error codes used by FIFO driver.
enum {
	FIFO_OK = 0,          //!< Normal operation.
	FIFO_ERROR_OVERFLOW,  //!< Attempt to push something in a FIFO that is full.
	FIFO_ERROR_UNDERFLOW, //!< Attempt to pull something from a FIFO that is empty
	FIFO_ERROR
//!< Error (malloc failed, ...)
};

//! FIFO descriptor used by FIFO driver.
typedef struct {
	volatile UnionVPtr buffer;
	volatile uint16_t rd_id;
	volatile uint16_t wr_id;
	uint16_t size;
} fifo_desc_t;

/**
 *  @brief This function initializes a new software FIFO for a certain 'size'.
 *  Both fifo descriptor and buffer must be allocated by the caller before calling
 *  this function.
 *
 *  @param fifo_desc  Pointer on the FIFO descriptor.
 *  @param buffer     Pointer on the FIFO buffer.
 *  @param size       Size of the buffer (unit is in number of 'element').
 *                    It must be a 2-power.
 *
 *  @return Status
 *    @retval FIFO_OK when no error occured.
 *    @retval FIFO_ERROR when the size is not a 2-power.
 */
int fifo_init(fifo_desc_t *fifo_desc, void *buffer, uint16_t size);

/**
 *  @brief This function returns the number of elements in the FIFO.
 *
 *  @param fifo_desc  The FIFO descriptor.
 *
 *  @return The number of used elements.
 */
static inline uint16_t fifo_get_used_size(fifo_desc_t *fifo_desc)
{
	uint16_t val;
	val = fifo_desc->wr_id + 2 * fifo_desc->size;
	val -= fifo_desc->rd_id;
	return val & ((2 * fifo_desc->size) - 1);
}

/**
 *  @brief This function returns the remaining free spaces of the FIFO (in number of elements).
 *
 *  @param fifo_desc  The FIFO descriptor.
 *
 *  @return The number of free elements.
 */
static inline uint16_t fifo_get_free_size(fifo_desc_t *fifo_desc)
{
	return fifo_desc->size - fifo_get_used_size(fifo_desc);
}

/**
 *  @brief This function tests if a FIFO is empty or not.
 *
 *  @param fifo_desc  The FIFO descriptor.
 *
 *  @return Status
 *    @retval true when the FIFO is empty.
 *    @retval false when the FIFO is not empty.
 */
static inline bool fifo_is_empty(fifo_desc_t *fifo_desc)
{
    uint16_t wr_id = fifo_desc->wr_id;
	return (wr_id == fifo_desc->rd_id);
}

/**
 *  \brief This function gets a new 8-bits element from the FIFO.
 *
 *  @param fifo_desc  The FIFO descriptor.
 *  @param item       extracted element.
 *
 *  @return Status
 *    @retval FIFO_OK when no error occured.
 *    @retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int fifo_push_byte(fifo_desc_t *fifo_desc, uint32_t item){
	uint8_t wr_id;
	if (fifo_get_free_size(fifo_desc) == 0)
		return FIFO_ERROR_OVERFLOW;

	wr_id = fifo_desc->wr_id;
	fifo_desc->buffer.u8ptr[wr_id & (fifo_desc->size - 1)] = item;

	// Must be the last thing to do.
	barrier();
	fifo_desc->wr_id = (wr_id + 1) & ((2 * fifo_desc->size) - 1);
	return FIFO_OK;
}

/**
 *  \brief This function gets a new 16-bits element from the FIFO.
 *
 *  @param fifo_desc  The FIFO descriptor.
 *  @param item       extracted element.
 *
 *  @return Status
 *    @retval FIFO_OK when no error occured.
 *    @retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int fifo_push_halfword(fifo_desc_t *fifo_desc, uint32_t item){
	uint8_t wr_id;
	if (fifo_get_free_size(fifo_desc) == 0)
		return FIFO_ERROR_OVERFLOW;

	wr_id = fifo_desc->wr_id;
	fifo_desc->buffer.u16ptr[wr_id & (fifo_desc->size - 1)] = item;

	// Must be the last thing to do.
	barrier();
	fifo_desc->wr_id = (wr_id + 1) & ((2 * fifo_desc->size) - 1);
	return FIFO_OK;
}

/**
 *  \brief This function gets a new 32-bits element from the FIFO.
 *
 *  @param fifo_desc  The FIFO descriptor.
 *  @param item       extracted element.
 *
 *  @return Status
 *    @retval FIFO_OK when no error occured.
 *    @retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int fifo_push_word(fifo_desc_t *fifo_desc, uint32_t item){
	uint8_t wr_id;
	if (fifo_get_free_size(fifo_desc) == 0)
		return FIFO_ERROR_OVERFLOW;

	wr_id = fifo_desc->wr_id;
	fifo_desc->buffer.u32ptr[wr_id & (fifo_desc->size - 1)] = item;

	// Must be the last thing to do.
	barrier();
	fifo_desc->wr_id = (wr_id + 1) & ((2 * fifo_desc->size) - 1);
	return FIFO_OK;
}

/**
 *  \brief This function gets a new 8-bits element from the FIFO.
 *
 *  @param fifo_desc  The FIFO descriptor.
 *  @param item       extracted element.
 *
 *  @return Status
 *    @retval FIFO_OK when no error occured.
 *    @retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int fifo_pull_byte(fifo_desc_t *fifo_desc, uint8_t *item)
{
	uint8_t rd_id;
	if (fifo_is_empty(fifo_desc))
		return FIFO_ERROR_UNDERFLOW;

	rd_id = fifo_desc->rd_id;
	*item = fifo_desc->buffer.u8ptr[rd_id & (fifo_desc->size - 1)];

	// Must be the last thing to do.
	barrier();
	fifo_desc->rd_id = (rd_id + 1) & ((2 * fifo_desc->size) - 1);
	return FIFO_OK;
}

/**
 *  \brief This function gets a new 16-bits element from the FIFO.
 *
 *  @param fifo_desc  The FIFO descriptor.
 *  @param item       extracted element.
 *
 *  @return Status
 *    @retval FIFO_OK when no error occured.
 *    @retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int fifo_pull_halfword(fifo_desc_t *fifo_desc, uint16_t *item)
{
	uint8_t rd_id;
	if (fifo_is_empty(fifo_desc))
		return FIFO_ERROR_UNDERFLOW;

	rd_id = fifo_desc->rd_id;
	*item = fifo_desc->buffer.u16ptr[rd_id & (fifo_desc->size - 1)];

	// Must be the last thing to do.
	barrier();
	fifo_desc->rd_id = (rd_id + 1) & ((2 * fifo_desc->size) - 1);
	return FIFO_OK;
}

/**
 *  \brief This function gets a new 32-bits element from the FIFO.
 *
 *  @param fifo_desc  The FIFO descriptor.
 *  @param item       extracted element.
 *
 *  @return Status
 *    @retval FIFO_OK when no error occured.
 *    @retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int fifo_pull_word(fifo_desc_t *fifo_desc, uint32_t *item)
{
	uint8_t rd_id;
	if (fifo_is_empty(fifo_desc))
		return FIFO_ERROR_UNDERFLOW;

	rd_id = fifo_desc->rd_id;
	*item = fifo_desc->buffer.u32ptr[rd_id & (fifo_desc->size - 1)];

	// Must be the last thing to do.
	barrier();
	fifo_desc->rd_id = (rd_id + 1) & ((2 * fifo_desc->size) - 1);
	return FIFO_OK;
}

/**
 *  \brief This function resets a software FIFO.
 *
 *  @param fifo_desc  The FIFO descriptor.
 */
static inline void fifo_reset(fifo_desc_t *fifo_desc)
{
	// Fifo starts empty.
	fifo_desc->rd_id = fifo_desc->wr_id = 0;
}

#endif  // _FIFO_H_
