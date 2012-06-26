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
 * Implementation of media layer for the NAND flash.
 *
 */

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/

#include "memories.h"

#include <string.h>
#include <assert.h>

/*------------------------------------------------------------------------------
 *         Definitions
 *------------------------------------------------------------------------------*/
/// Casts
#define MODEL(interface)        ((struct NandFlashModel *) interface)
#define TRANSLATED(interface)   ((struct TranslatedNandFlash *) interface)

/*----------------------------------------------------------------------------
 *        Local macros
 *----------------------------------------------------------------------------*/
#define min( a, b ) (((a) < (b)) ? (a) : (b))

/*-----------------------------------------------------------------------------
 *         Internal variables
 *------------------------------------------------------------------------------*/
static uint8_t pageWriteBuffer[NandCommon_MAXPAGEDATASIZE];
static int16_t currentWriteBlock;
static int16_t currentWritePage;

static uint8_t pageReadBuffer[NandCommon_MAXPAGEDATASIZE];
static int16_t currentReadBlock;
static int16_t currentReadPage;
/*------------------------------------------------------------------------------
 *         Internal functions
 *------------------------------------------------------------------------------*/
/**
 * \brief Writes the current page of data on the NandFlash.
 * Returns 0 if successful; otherwise returns 1.
 * \param tnf  Pointer to a TranslatedNandFlash instance.
*/
static uint8_t FlushCurrentPage( Media *media )
{
    // Make sure there is a page to flush
    if ( currentWritePage == -1 )
    {
        return 0 ;
    }

    TRACE_DEBUG("FlushCurrentPage(B#%d:P#%d)\n\r",
              currentWriteBlock, currentWritePage);

    // Write page
    if (TranslatedNandFlash_WritePage(TRANSLATED(media->interface),
                                      currentWriteBlock,
                                      currentWritePage,
                                      pageWriteBuffer,
                                      0)) {

        TRACE_ERROR("FlushCurrentPage: Failed to write page.\n\r");
        return 1;
    }

    // No current write page & block
    currentWriteBlock = -1;
    currentWritePage = -1;

    return 0;
}

/**
 * \brief Writes data at an unaligned (page-wise) address and size. The address is
 * provided as the block & page number plus an offset. The data to write MUST
 * NOT span more than one page.
 * Returns 0 if the data has been written; 1 otherwise.
 * \param media  Pointer to a nandflash Media instance.
 * \param block  Number of the block to write.
 * \param page  Number of the page to write.
 * \param offset  Write offset.
 * \param buffer  Data buffer.
 * \param size  Number of bytes to write.
*/
static uint8_t UnalignedWritePage(
    Media *media,
    uint16_t block,
    uint16_t page,
    uint16_t offset,
    uint8_t *buffer,
    uint32_t size)
{
    uint8_t error;
    uint16_t pageDataSize = NandFlashModel_GetPageDataSize(MODEL(media->interface));
    uint8_t writePage = ((size + offset) == pageDataSize);

    TRACE_DEBUG( "UnalignedWritePage(B%d:P%d@%d, %d)\n\r", (int)block, (int)page, (int)offset, (int)size ) ;
    assert( (size + offset) <= pageDataSize ) ; /* "UnalignedWrite: Write size and offset exceed page data size\n\r" */

    // If size is 0, return immediately
    if ( size == 0 )
    {
        return 0 ;
    }

    // If this is not the current page, flush the previous one
    if ((currentWriteBlock != block) || (currentWritePage != page))
    {
        // Flush and make page the new current page
        FlushCurrentPage(media);
        TRACE_DEBUG("Current write page: B#%d:P#%d\n\r", block, page);
        currentWriteBlock = block;
        currentWritePage = page;

        // Read existing page data in a temporary buffer if the page is not
        // entirely written
        if (size != pageDataSize) {

            error = TranslatedNandFlash_ReadPage(TRANSLATED(media->interface),
                                                 block,
                                                 page,
                                                 pageWriteBuffer,
                                                 0);
            if (error) {

                TRACE_ERROR(
                          "UnalignedWrite: Could not read existing page data\n\r");
                return 1;
            }
        }
    }

    // Copy data in temporary buffer
    memcpy(&(pageWriteBuffer[offset]), buffer, size);
    // Update read buffer if necessary
    if ((currentReadPage == currentWritePage)
        && (currentReadBlock == currentWriteBlock)) {

        TRACE_DEBUG("Updating current read buffer\n\r");
        memcpy(&(pageReadBuffer[offset]), buffer, size);
    }

    // Flush page if it is complete
    if (writePage) {

        FlushCurrentPage(media);
    }

    return 0;
}

/**
 * \brief Writes a data buffer at the specified address on a NandFlash media. An
 * optional callback can be triggered after the transfer is completed.
 * Returns MED_STATUS_SUCCESS if the transfer has been started successfully;
 * otherwise returns MED_STATUS_ERROR.
 * \param media  Pointer to the NandFlash Media instance.
 * \param address  Address where the data shall be written.
 * \param data  Data buffer.
 * \param length  Number of bytes to write.
 * \param callback  Optional callback to call when the write is finished.
 * \param argument  Optional argument to the callback function.
*/
static uint8_t MEDNandFlash_Write(
    Media *media,
    uint32_t address,
    void *data,
    uint32_t length,
    MediaCallback callback,
    void *argument)
{
    uint16_t pageDataSize =
                NandFlashModel_GetPageDataSize(MODEL(media->interface));
    uint16_t blockSize =
                NandFlashModel_GetBlockSizeInPages(MODEL(media->interface));
    uint16_t block, page, offset;
    uint32_t writeSize;
    uint8_t *buffer = (uint8_t *) data;
    uint32_t remainingLength;
    uint8_t status;

    TRACE_INFO("MEDNandFlash_Write(0x%08X, %d)\n\r", address, (int)length);

    // Translate access
    if (NandFlashModel_TranslateAccess(MODEL(media->interface),
                                       address,
                                       length,
                                       &block,
                                       &page,
                                       &offset)) {

        TRACE_ERROR("MEDNandFlash_Write: Could not start write.\n\r");
        return MED_STATUS_ERROR;
    }

    TRACE_DEBUG("MEDNandFlash_Write(B#%d:P#%d@%d, %d)\n\r",
              block, page, offset, (int)length);

    // Write pages
    remainingLength = length;
    status = MED_STATUS_SUCCESS;
    while ((status == MED_STATUS_SUCCESS) && (remainingLength > 0)) {

        // Write one page
        writeSize = min((uint32_t)(pageDataSize-offset), remainingLength);
        if (UnalignedWritePage(media, block, page, offset, buffer, writeSize)) {

            TRACE_ERROR("MEDNandFlash_Write: Failed to write page\n\r");
            status = MED_STATUS_ERROR;
        }
        else {

            // Update addresses
            remainingLength -= writeSize;
            buffer += writeSize;
            offset = 0;
            page++;
            if (page == blockSize) {

                page = 0;
                block++;
            }
        }
    }

    // Trigger callback
    if (callback) {

        callback(argument, status, length - remainingLength, remainingLength);
    }

    return status;

}

/**
 * \brief Reads data at an unaligned address and/or size. The address is provided as
 * the block & page numbers plus an offset.
 * Returns 0 if the data has been read; otherwise returns 1.
 * \param media  Pointer to a nandflash Media instance.
 * \param block  Number of the block to read.
 * \param page  Number of the page to read.
 * \param offset  Read offset.
 * \param buffer  Buffer for storing data.
 * \param size  Number of bytes to read.
*/
static uint8_t UnalignedReadPage(
    Media *media,
    uint16_t block,
    uint16_t page,
    uint16_t offset,
    uint8_t *buffer,
    uint32_t size)
{
    uint8_t error;
    uint16_t pageDataSize = NandFlashModel_GetPageDataSize(MODEL(media->interface));

    TRACE_DEBUG("UnalignedReadPage(B%d:P%d@%d, %d)\n\r", (int)block, (int)page, (int)offset, (int)size);

    // Check that one page is read at most
    if( (size + offset) > pageDataSize )
    {
      TRACE_ERROR("UnalignedReadPage: Read size & offset exceed page data size\n\r");
      return 1;
    }
    // Check if this is not the current read page
    if ((block != currentReadBlock) || (page != currentReadPage))
    {
        TRACE_DEBUG("Current read page: B#%d:P#%d\n\r", block, page);
        currentReadBlock = block;
        currentReadPage = page;

        // Check if this is the current write page
        if ((currentReadBlock == currentWriteBlock)
            && (currentReadPage == currentWritePage)) {

            TRACE_DEBUG("Reading current write page\n\r");
            memcpy(pageReadBuffer, pageWriteBuffer, NandCommon_MAXPAGEDATASIZE);
        }
        else {

            // Read whole page into a temporary buffer
            error = TranslatedNandFlash_ReadPage(TRANSLATED(media->interface),
                                                 block,
                                                 page,
                                                 pageReadBuffer,
                                                 0);
            if (error) {

                TRACE_ERROR("UnalignedRead: Could not read page\n\r");
                return 1;
            }
        }
    }

    // Copy data into buffer
    memcpy(buffer, &(pageReadBuffer[offset]), size);

    return 0;
}

/**
 * \brief Reads data at the specified address of a NandFlash media. An optional
 * callback is invoked when the transfer completes.
 * Returns 1 if the transfer has been started; otherwise returns 0.
 * \param media  Pointer to the NandFlash Media to read.
 * \param address  Address at which the data shall be read.
 * \param data  Data buffer.
 * \param length  Number of bytes to read.
 * \param callback  Optional callback function.
 * \param argument  Optional argument to the callback function.
*/
static uint8_t MEDNandFlash_Read(
    Media *media,
    uint32_t address,
    void *data,
    uint32_t length,
    MediaCallback callback,
    void *argument)
{
    uint16_t block, page, offset;
    uint16_t pageDataSize = NandFlashModel_GetPageDataSize(MODEL(media->interface));
    uint16_t blockSizeInPages = NandFlashModel_GetBlockSizeInPages(MODEL(media->interface));
    uint32_t remainingLength;
    uint32_t readSize;
    uint8_t *buffer = (uint8_t *) data;
    uint8_t status;

    TRACE_INFO("MEDNandFlash_Read(0x%08X, %d)\n\r", address, (int)length);

    // Translate access into block, page and offset
    if (NandFlashModel_TranslateAccess(MODEL(media->interface),
                                       address,
                                       length,
                                       &block,
                                       &page,
                                       &offset)) {

        TRACE_ERROR("MEDNandFlash_Read: Cannot perform access\n\r");
        return MED_STATUS_ERROR;
    }

    // Read
    remainingLength = length;
    status = MED_STATUS_SUCCESS;
    while ((status == MED_STATUS_SUCCESS) && (remainingLength > 0)) {

        // Read page
        readSize = min((uint32_t)(pageDataSize-offset), remainingLength);
        if (UnalignedReadPage(media, block, page, offset, buffer, readSize)) {

            TRACE_ERROR("MEDNandFlash_Read: Could not read page\n\r");
            status = MED_STATUS_ERROR;
        }
        else {

            // Update values
            remainingLength -= readSize;
            buffer += readSize;
            offset = 0;
            page++;
            if (page == blockSizeInPages) {

                page = 0;
                block++;
            }
        }
    }

    // Trigger callback
    if (callback) {

        callback(argument, status, length - remainingLength, remainingLength);
    }

    return status;
}

/**
 * \brief Carries out all pending operations. Returns MED_STATUS_SUCCESS if
 * succesful; otherwise, returns MED_STATUS_ERROR.
 * \param media  Pointer to a NandFlash Media instance.
*/
static uint8_t MEDNandFlash_Flush(Media *media)
{
    TRACE_INFO("MEDNandFlash_Flush()\n\r");

    if (FlushCurrentPage(media)) {

        TRACE_ERROR("MEDNandFlash_Flush: Could not flush current page\n\r");
        return MED_STATUS_ERROR;
    }

    if (TranslatedNandFlash_Flush(TRANSLATED(media->interface))) {

        TRACE_ERROR("MEDNandFlash_Flush: Could not flush translated nand\n\r");
        return MED_STATUS_ERROR;
    }

    if (TranslatedNandFlash_SaveLogicalMapping(TRANSLATED(media->interface))) {

        TRACE_ERROR("MEDNandFlash_Flush: Could not save the logical mapping\n\r");
        return MED_STATUS_ERROR;
    }

    return MED_STATUS_SUCCESS;
}

/**
 * \brief Interrupt handler for the nandflash media. Triggered when the flush timer
 * expires, initiating a MEDNandFlash_Flush().
 * \param media  Pointer to a nandflash Media instance.
*/
static void MEDNandFlash_InterruptHandler(Media *media)
{
    //volatile uint32_t dummy;

    TRACE_DEBUG("Flush timer expired\n\r");
    MEDNandFlash_Flush(media);

    // Acknowledge interrupt
    //dummy = AT91C_BASE_NANDFLUSHTIMER->TC_SR;
}

/*------------------------------------------------------------------------------
 *         Exported functions
 *------------------------------------------------------------------------------*/
/**
 * \brief Initializes a media instance to operate on the given NandFlash device.
 * \param media  Pointer to a Media instance.
 * \param tnf  Pointer to the TranslatedNandFlash to use.
*/
void MEDNandFlash_Initialize( Media* pMedia, struct TranslatedNandFlash *translated )
{
    TRACE_INFO( "MEDNandFlash_Initialize()\n\r" ) ;

    pMedia->write = (Media_write)MEDNandFlash_Write;
    pMedia->read = (Media_read)MEDNandFlash_Read;
    pMedia->lock = 0;
    pMedia->unlock = 0;
    pMedia->flush = MEDNandFlash_Flush;
    pMedia->handler = MEDNandFlash_InterruptHandler;

    pMedia->interface = translated;

    pMedia->baseAddress = 0;
    pMedia->blockSize   = 1;
    pMedia->size = TranslatedNandFlash_GetDeviceSizeInBytes( translated ) ;

    TRACE_INFO( "NF Size: %d\n\r", (int)pMedia->size ) ;

    pMedia->mappedRD  = 0;
    pMedia->mappedWR  = 0;
    pMedia->protected = 0;
    pMedia->removable = 0;
    pMedia->state = MED_STATE_READY;

    currentWriteBlock = -1;
    currentWritePage = -1;
    currentReadBlock = -1;
    currentReadPage = -1;

}

