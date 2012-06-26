/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */
/**
 * \file
 *
 * Implementation of media layer for the Sdram.
 *
 */

/*------------------------------------------------------------------------------
 *       Headers
 *------------------------------------------------------------------------------*/

#include "memories.h"

#if defined(AT91C_EBI_SDRAM)

/*------------------------------------------------------------------------------
 *      Types
 *------------------------------------------------------------------------------*/

/** Do copy and modify pointer */
typedef void copyFunction(uint8_t **, uint8_t **, uint32_t);

/*------------------------------------------------------------------------------
 *      Internal Functions
 *------------------------------------------------------------------------------*/

/**
 * Do copy for 8-byte aligned data
 */
static void AlignedCopy(uint8_t * *src,
                        uint8_t * *dst,
                        uint32_t len)
{
    uint32_t *src32, *dst32;
    src32 = (uint32_t*)*src;
    dst32 = (uint32_t*)*dst;
    for (;len > 0; len -= 8) {
        *dst32 ++ = *src32 ++;
        *dst32 ++ = *src32 ++;
    }
    *src = (uint8_t*)src32;
    *dst = (uint8_t*)dst32;
}

/**
 * Do copy for byte-aligned data
 */
static void UnalignedCopy(uint8_t * *src,
                          uint8_t * *dst,
                          uint32_t  len)
{
    for (;len > 0; len --) {
        *(*dst) ++ = *(*src) ++;
    }
}

/**
 * \brief  Reads a specified amount of data from a SDRAM memory
 * \param  media    Pointer to a Media instance
 * \param  address  Address of the data to read
 * \param  data     Pointer to the buffer in which to store the retrieved
 *                   data
 * \param  length   Length of the buffer
 * \param  callback Optional pointer to a callback function to invoke when
 *                   the operation is finished
 * \param  argument Optional pointer to an argument for the callback
 * \return Operation result code
*/
static uint8_t MEDSdram_Read(Media         *media,
                                   uint32_t  address,
                                   void          *data,
                                   uint32_t  length,
                                   MediaCallback callback,
                                   void          *argument)
{
    uint8_t *source;
    uint8_t *dest;
    copyFunction  *pCpy;

    // Check that the media is ready
    if (media->state != MED_STATE_READY) {

        TRACE_INFO("SDRAM busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the data to read is not too big
    if ((length + address) > media->size) {

        TRACE_WARNING("SdRamD_Read: Data too big: %u, 0x%08X\n\r",
                      length, address);
        return MED_STATUS_ERROR;
    }

    // Enter Busy state
    media->state = MED_STATE_BUSY;

    // Source & Destination
    source = (uint8_t *)(media->blockSize
                                    * (address + media->baseAddress));
    dest   = (uint8_t *)data;

    // Align/Unaligned copy
    pCpy   = (((uint32_t)source%4) == 0 && (media->blockSize%8) == 0)
                        ? AlignedCopy : UnalignedCopy;

    for (; length > 0; length --) {
        pCpy(&source, &dest, media->blockSize);
    }

    // Leave the Busy state
    media->state = MED_STATE_READY;

    // Invoke callback
    if (callback != 0) {

        callback(argument, MED_STATUS_SUCCESS, 0, 0);
    }

    return MED_STATUS_SUCCESS;
}

/**
 * \brief  Writes data on a SDRAM media
 * \param  media    Pointer to a Media instance
 * \param  address  Address at which to write
 * \param  data     Pointer to the data to write
 * \param  length   Size of the data buffer
 * \param  callback Optional pointer to a callback function to invoke when
 *                   the write operation terminates
 * \param  argument Optional argument for the callback function
 * \return Operation result code
 * \see    Media
 * \see    MediaCallback
 */
static uint8_t MEDSdram_Write(Media         *media,
                                    uint32_t  address,
                                    void          *data,
                                    uint32_t  length,
                                    MediaCallback callback,
                                    void          *argument)
{
    uint8_t *source;
    uint8_t *dest;
    copyFunction  *pCpy;

    //TRACE_DEBUG("SdRamD_Write\n\r");

    // Check that the media if ready
    if (media->state != MED_STATE_READY) {

        TRACE_WARNING("SdRamD_Write: Media is busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the data to write is not too big
    if ((length + address) > media->size) {

        TRACE_WARNING("SdRamD_Write: Data too big\n\r");
        return MED_STATUS_ERROR;
    }

    // Compute function parameters
    source = (uint8_t *) data;
    dest = (uint8_t *) (media->blockSize *
                                    (media->baseAddress + address));

    // Align/Unaligned copy
    pCpy   = (((uint32_t)source%4) == 0 && (media->blockSize%8) == 0)
                        ? AlignedCopy : UnalignedCopy;

    for (; length > 0; length --) {
        pCpy(&source, &dest, media->blockSize);
    }

    // Leave the Busy state
    media->state = MED_STATE_READY;

    // Invoke the callback if it exists
    if (callback != 0) {

        callback(argument, MED_STATUS_SUCCESS, 0, 0);
    }

    return MED_STATUS_SUCCESS;
}

/*------------------------------------------------------------------------------
 *      Exported Functions
 *------------------------------------------------------------------------------*/
/**
 * \brief  Initializes a Media instance and the associated physical interface
 * \param  media Pointer to the Media instance to initialize
 * \see    Media
 */
void MEDSdram_Initialize(Media *media,
                         uint32_t blockSize,
                         uint32_t baseAddress,
                         uint32_t size)
{
    uint32_t value;

    TRACE_INFO("MEDSdram init\n\r");

    // Initialize SDRAM if not already done
    value = *((volatile uint32_t *) AT91C_EBI_SDRAM);
    *((volatile uint32_t *) AT91C_EBI_SDRAM) = 0xDEADBEEF;

    if (*((volatile uint32_t *) AT91C_EBI_SDRAM) == 0xDEADBEEF) {

        *((volatile uint32_t *) AT91C_EBI_SDRAM) = value;
        TRACE_INFO("SDRAM already initialized\n\r");
    }
    else {

        TRACE_INFO("Initializing the SDRAM ...\n\r");
        BOARD_ConfigureSdram(BOARD_SDRAM_BUSWIDTH);
    }

    // Initialize media fields
    media->write = MEDSdram_Write;
    media->read = MEDSdram_Read;
    media->lock = 0;
    media->unlock = 0;
    media->handler = 0;
    media->flush = 0;

    media->blockSize = blockSize;
    media->baseAddress = baseAddress;
    media->size = size;

    media->mappedRD  = 1;
    media->mappedWR  = 1;
    media->protected = 0;
    media->removable = 0;
    media->state = MED_STATE_READY;

    media->transfer.data = 0;
    media->transfer.address = 0;
    media->transfer.length = 0;
    media->transfer.callback = 0;
    media->transfer.argument = 0;
}
#endif //#if defined(AT91C_EBI_SDRAM)

