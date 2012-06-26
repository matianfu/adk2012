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
 * Implementation of media layer for the SdCard.
 *
 */

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/
#include "board.h"
#include "memories.h"
#include <assert.h>
#include <string.h>
/*------------------------------------------------------------------------------
 *         Constants
 *------------------------------------------------------------------------------*/

/** Number of SD Slots */
#define NUM_SD_SLOTS            1
/** Default block size for SD/MMC card access */
#define SD_BLOCK_SIZE       512
/**
 * \brief  Reads a specified amount of data from a SDCARD memory
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
static uint8_t MEDSdcard_Read(Media         *media,
                                    uint32_t  address,
                                    void          *data,
                                    uint32_t  length,
                                    MediaCallback callback,
                                    void          *argument)
{
    uint8_t error;

    // Check that the media is ready
    if (media->state != MED_STATE_READY) {

        TRACE_INFO("Media busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the data to read is not too big
    if ((length + address) > media->size) {

        TRACE_WARNING("MEDSdcard_Read: Data too big: %d, %d\n\r",
                      (int)length, (int)address);
        return MED_STATUS_ERROR;
    }

    // Enter Busy state
    media->state = MED_STATE_BUSY;

    //error = SD_Read((sSdCard*)media->interface, address, data,length,NULL,NULL);
    error = SD_ReadBlocks((sSdCard*)media->interface, address,data,length);

    // Leave the Busy state
    media->state = MED_STATE_READY;

    // Invoke callback
    if (callback != 0) {

        callback(argument, error, 0, 0);
    }

    return error;
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
static uint8_t MEDSdcard_Write(Media         *media,
                                    uint32_t  address,
                                    void          *data,
                                    uint32_t  length,
                                    MediaCallback callback,
                                    void          *argument)
{
    uint8_t error;

    // Check that the media if ready
    if (media->state != MED_STATE_READY) {

        TRACE_WARNING("MEDSdcard_Write: Media is busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the data to write is not too big
    if ((length + address) > media->size) {

        TRACE_WARNING("MEDSdcard_Write: Data too big\n\r");
        return MED_STATUS_ERROR;
    }

    // Put the media in Busy state
    media->state = MED_STATE_BUSY;

    //error = SD_Write((sSdCard*)media->interface, address,data,length,NULL,NULL);
    error = SD_WriteBlocks((sSdCard*)media->interface, address,data,length);

    // Leave the Busy state
    media->state = MED_STATE_READY;

    // Invoke the callback if it exists
    if (callback != 0) {

        callback(argument, error, 0, 0);
    }

    return error;
}
#if 0
//------------------------------------------------------------------------------
 * \brief Callback invoked when SD/MMC transfer done
//------------------------------------------------------------------------------
static void SdMmcCallback( uint32_t status, Media *pMed )
{
    MEDTransfer * pXfr = &pMed->transfer;

    TRACE_INFO_WP("SDCb ");

    if (status == SDMMC_ERROR_BUSY) {
        status = MED_STATUS_BUSY;
    }
    else if (status) {
        status = MED_STATUS_ERROR;
    }

    pMed->state = MED_STATE_READY;
    if (pXfr->callback) {
        pXfr->callback(pXfr->argument,
                       status,
                       pXfr->length * pMed->blockSize,
                       0);
    }
}
#endif
/**
 * \brief  Reads a specified amount of data from a SDCARD memory
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
static uint8_t MEDSdusb_Read(Media         *media,
                             uint32_t       address,
                             void          *data,
                             uint32_t       length,
                             MediaCallback  callback,
                             void          *argument)
{
    uint8_t error;

    TRACE_INFO_WP("SDuRd(%d,%d) ", (int)address, (int)length);

    // Check that the media is ready
    if (media->state != MED_STATE_READY) {
        TRACE_INFO("MEDSdusb_Read: Busy\n\r");
        return MED_STATUS_BUSY;
    }
    // Check that the data to read is not too big
    if ((length + address) > media->size) {
        TRACE_WARNING("MEDSdusb_Read: Data too big: %d, %d\n\r",
                      (int)length, (int)address);
        return MED_STATUS_ERROR;
    }
    // Enter Busy state
    media->state = MED_STATE_BUSY;
  #if 1
    error = SD_Read((sSdCard*)media->interface,
                     address,data,length,
                     NULL,NULL);
    error = (error ? MED_STATUS_ERROR : MED_STATUS_SUCCESS);
    media->state = MED_STATE_READY;
    if (callback) callback(argument, error, 0, 0);
    return error;
  #else
    MEDTransfer * pXfr;
    // Start media transfer
    pXfr = &media->transfer;
    pXfr->data     = data;
    pXfr->address  = address;
    pXfr->length   = length;
    pXfr->callback = callback;
    pXfr->argument = argument;

    error = SD_Read((sSdCard*)media->interface,
                     address,
                     data,
                     length,
                     (fSdmmcCallback)SdMmcCallback,
                     media);
    return (error ? MED_STATUS_ERROR : MED_STATUS_SUCCESS);
  #endif
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
static uint8_t MEDSdusb_Write(Media         *media,
                              uint32_t       address,
                              void          *data,
                              uint32_t       length,
                              MediaCallback  callback,
                              void          *argument)
{
    uint8_t error;
    TRACE_INFO_WP("SDuWr(%d,%d) ", (int)address, (int)length);

    // Check that the media if ready
    if (media->state != MED_STATE_READY)
    {
        TRACE_INFO("MEDSdusb_Write: Busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the data to write is not too big
    if ((length + address) > media->size)
    {
        TRACE_WARNING("MEDSdcard_Write: Data too big\n\r");
        return MED_STATUS_ERROR;
    }
    // Put the media in Busy state
    media->state = MED_STATE_BUSY;

  #if 1
    error = SD_Write((sSdCard*)media->interface,
                     address,data,length,
                     NULL,NULL);
    error = (error ? MED_STATUS_ERROR : MED_STATUS_SUCCESS);
    media->state = MED_STATE_READY;
    if (callback) callback(argument, error, 0, 0);
    return error;
  #else
    MEDTransfer * pXfr;
    // Start media transfer
    pXfr = &media->transfer;
    pXfr->data = data;
    pXfr->address = address;
    pXfr->length = length;
    pXfr->callback = callback;
    pXfr->argument = argument;

    error = SD_Write((sSdCard*)media->interface,
                      address,
                      data,
                      length,
                      (fSdmmcCallback)SdMmcCallback,
                      media);
    return (error ? MED_STATUS_ERROR : MED_STATUS_SUCCESS);
  #endif
}

/**
 * \brief  Initializes a Media instance 
 * \param  media Pointer to the Media instance to initialize
 * \return 1 if success.
 */
uint8_t MEDSdcard_Initialize(Media *media, sSdCard *pSdDrv)
{
    TRACE_INFO("MEDSdcard init\n\r");
    // Initialize media fields
    media->interface = pSdDrv;
    #if !defined(OP_BOOTSTRAP_MCI_on)
    media->write = MEDSdcard_Write;
    #else
    media->write = 0;
    #endif
    media->read = MEDSdcard_Read;
    media->lock = 0;
    media->unlock = 0;
    media->handler = 0;
    media->flush = 0;

    media->blockSize = SD_BLOCK_SIZE;
    media->baseAddress = 0;
    media->size = pSdDrv->dwNbBlocks;

    media->mappedRD  = 0;
    media->mappedWR  = 0;
    media->removable = 1;

    media->state = MED_STATE_READY;

    media->transfer.data = 0;
    media->transfer.address = 0;
    media->transfer.length = 0;
    media->transfer.callback = 0;
    media->transfer.argument = 0;

    return 1;
}

/**
 * \brief  Initializes a Media instance
 * \param  media Pointer to the Media instance to initialize
 * \return 1 if success.
 */
uint8_t MEDSdusb_Initialize(Media *media, sSdCard *pSdDrv)
{
    TRACE_INFO("MEDSdusb init\n\r");

    // Initialize media fields
    media->interface = pSdDrv;
    media->write = MEDSdusb_Write;
    media->read = MEDSdusb_Read;
    media->lock = 0;
    media->unlock = 0;
    media->handler = 0;
    media->flush = 0;

    media->blockSize = SD_BLOCK_SIZE;
    media->baseAddress = 0;
    media->size = pSdDrv->dwNbBlocks;

    media->mappedRD  = 0;
    media->mappedWR  = 0;
    media->protected = 0;
    media->removable = 1;

    media->state = MED_STATE_READY;

    media->transfer.data = 0;
    media->transfer.address = 0;
    media->transfer.length = 0;
    media->transfer.callback = 0;
    media->transfer.argument = 0;

    return 1;
}

/**
 * \brief  erase all the Sdcard
 * \param  media Pointer to the Media instance to initialize
 */

void MEDSdcard_EraseAll(Media *media)
{
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t block;
    uint32_t multiBlock = 1; // change buffer size for multiblocks
    uint8_t error;

    TRACE_INFO("MEDSdcard Erase All ...\n\r");

    // Clear the block buffer
    memset(buffer, 0, media->blockSize * multiBlock);

    for (block=0;
         block < (media->size-multiBlock);
         block += multiBlock)
    {
        error = SD_WriteBlocks((sSdCard*)media->interface, block,buffer,multiBlock);
        if( error )
        {
          TRACE_ERROR("\n\r-F- Failed to erase block (%d) #%u\n\r", error, block);
          
          /* Wait for watchdog reset or freeze the program */
          while (1);
        }
    }
}

/**
 * \brief  erase block
 * \param  media Pointer to the Media instance to initialize
 * \param  block to erase
 */
void MEDSdcard_EraseBlock(Media *media, uint32_t block)
{
    uint8_t buffer[SD_BLOCK_SIZE];
    uint8_t error;

    // Clear the block buffer
    memset(buffer, 0, media->blockSize);

    error = SD_WriteBlocks((sSdCard*)media->interface, block,buffer,1);
    if( error )
    {
      TRACE_ERROR("\n\r-F- Failed to write block (%d) #%u\n\r", error, block);
      
      /* Wait for watchdog reset or freeze the program */
      while (1);
    }
}

