/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Management of the media memory.
 *
 * This file manages the media memory.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USB module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ***************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
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

//_____  I N C L U D E S ___________________________________________________

#include "board.h"
#include "conf_access.h"
#include "conf_media_mem.h"


#if (MEDIA_0_MEM == ENABLE) || (MEDIA_1_MEM == ENABLE) || (MEDIA_2_MEM == ENABLE)

#include "media_mem.h"
#include <string.h>

//_____ M A C R O S ________________________________________________________

#ifndef media_access_signal_on
#define media_access_signal_on()
#endif
#ifndef media_access_signal_off
#define media_access_signal_off()
#endif

//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

// USB FIFO mode
//#define USB_FIFO_MODE
#ifdef USB_FIFO_MODE
#define MED_FIFO
#define USB_FIFO
#endif

//! buffer block size in number of sectors
#define FIFO_BLOCK_NB_SECTORS       8
//! Number of buffer blocks
#define FIFO_NB_BLOCKS              2

#define BLOCK_IDLE                  0
#define BLOCK_RD                    1
#define BLOCK_RD_WAIT               2
#define BLOCK_RD_NEXT               3
#define BLOCK_WR                    4
#define BLOCK_WR_WAIT               5
#define BLOCK_WR_NEXT               6

#define BLOCK_RX                    1
#define BLOCK_RX_WAIT               2
#define BLOCK_RX_NEXT               3
#define BLOCK_TX                    4
#define BLOCK_TX_WAIT               5
#define BLOCK_TX_NEXT               6

//! buffer block size in number of bytes
#define FIFO_BLOCK_NB_BYTES()       (media_mem_block_nb_sectors*MEDIA_SECTOR_SIZE)
//! Buffer size in number of sectors.
#define MEDIA_BUFFER_NB_SECTORS()   \
    (media_mem_buffer_nb_blocks*media_mem_block_nb_sectors)
//! Buffer size in number of bytes.
#define MEDIA_BUFFER_NB_BYTES()     (MEDIA_BUFFER_NB_SECTORS()*MEDIA_SECTOR_SIZE)

//_____ D E F I N I T I O N S ______________________________________________

//! Media Presence status per media.
static  U8   media_presence_status[MEDIA_NB_IDS] =
{MEDIA_INSERTED, MEDIA_INSERTED, MEDIA_INSERTED};

//! Default Media memory access buffer (8K bytes)
static U32 media_mem_buffer_space[FIFO_BLOCK_NB_SECTORS * FIFO_NB_BLOCKS * MEDIA_SECTOR_SIZE / 4];
//! Media memory access buffer pointer
static U8 *media_mem_buffer;
//! Media memory access buffer size in number of sectors
static U32 media_mem_buffer_nb_blocks;
//! Media memory access block size
static U16 media_mem_block_nb_sectors;

//_____ D E C L A R A T I O N S ____________________________________________

//! This function initialize media access buffer by default (2*8*512 bytes)
void media_access_init_default(void)
{
  media_mem_buffer = (U8*)media_mem_buffer_space;
  media_mem_buffer_nb_blocks = FIFO_NB_BLOCKS;
  media_mem_block_nb_sectors = FIFO_BLOCK_NB_SECTORS;
}

//! This function initialize media access buffer customized:
//! Total buffer size is (wNbParts*dwPartSiz*MEDIA_SECTOR_SIZE).
//! In FIFO access, the buffer is splited to parts to access.
//! @param pBuffer   Pointer to customized buffer space
//! @param wNbParts  Buffer size in number of blocks
//! @param dwPartSiz Block size in number of sectors
void media_access_init_customize(void* pBuffer, U16 wNbParts, U32 dwPartSiz)
{
  media_mem_buffer = (U8*)pBuffer;
  if (wNbParts > 0)
    media_mem_buffer_nb_blocks = wNbParts;
  else
    media_mem_buffer_nb_blocks = FIFO_NB_BLOCKS;
  if (dwPartSiz > 0)
    media_mem_block_nb_sectors = dwPartSiz;
  else
    media_mem_block_nb_sectors = FIFO_BLOCK_NB_SECTORS;
}

//! This function tests memory state, and starts memory initialization
//! @param media Media ID to access.
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_test_unit_ready(U8 media)
{
  if (media > MEDIA_LAST_ID) return CTRL_FAIL;

  media_access_signal_on();
  switch (media_presence_status[media])
  {
    case MEDIA_REMOVED:
      if (OK == media_mem_check(media))
      {
        media_presence_status[media] = MEDIA_INSERTED;
        media_access_signal_off();
        return CTRL_BUSY;
      }
      media_access_signal_off();
      return CTRL_NO_PRESENT;

    case MEDIA_INSERTED:
      if (OK != media_mem_check(media))
      {
        media_presence_status[media] = MEDIA_REMOVING;
        media_access_signal_off();
        return CTRL_BUSY;
      }
      media_access_signal_off();
      return CTRL_GOOD;

    case MEDIA_REMOVING:
      media_presence_status[media] = MEDIA_REMOVED;
      media_access_signal_off();
      return CTRL_NO_PRESENT;

    default:
      media_presence_status[media] = MEDIA_REMOVED;
      media_access_signal_off();
      return CTRL_BUSY;
  }
}

//! This function tests memory state, and starts memory initialization
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_test_unit_ready_0(void)
{
  return media_test_unit_ready(0);
}

//! This function tests memory state, and starts memory initialization
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_test_unit_ready_1(void)
{
  return media_test_unit_ready(1);
}

//! This function tests memory state, and starts memory initialization
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_test_unit_ready_2(void)
{
  return media_test_unit_ready(2);
}


//! This function returns the address of the last valid sector
//! @param media          Media ID to access.
//! @param u32_nb_sector  Pointer to number of sectors (sector = 512 bytes)
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_read_capacity(U8 media, U32 *u32_nb_sector)
{
  Media *pMed = &medias[media];
  media_access_signal_on();

  if ( !media_mem_check(media) )
  {
    media_access_signal_off();
    return CTRL_NO_PRESENT;
  }
  if (pMed->blockSize >= MEDIA_SECTOR_SIZE)
    *u32_nb_sector = pMed->size*(pMed->blockSize/MEDIA_SECTOR_SIZE) - 1;
  else
    *u32_nb_sector = pMed->size* pMed->blockSize/MEDIA_SECTOR_SIZE  - 1;
  media_access_signal_off();
  return CTRL_GOOD;
}

//! This function returns the address of the last valid sector
//! @param u32_nb_sector  Pointer to number of sectors (sector = 512 bytes)
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_read_capacity_0(U32 *u32_nb_sector)
{
  return media_read_capacity(0, u32_nb_sector);
}

//! This function returns the address of the last valid sector
//! @param u32_nb_sector  Pointer to number of sectors (sector = 512 bytes)
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_read_capacity_1(U32 *u32_nb_sector)
{
  return media_read_capacity(1, u32_nb_sector);
}

//! This function returns the address of the last valid sector
//! @param u32_nb_sector  Pointer to number of sectors (sector = 512 bytes)
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_read_capacity_2(U32 *u32_nb_sector)
{
  return media_read_capacity(2, u32_nb_sector);
}


//! This function returns the write-protected mode
//! Only used by memory removal with a HARDWARE-SPECIFIC write-protected detection
//! @param media          Media ID to access.
//! @warning The customer must unplug the memory to change this write-protected mode.
//! @return TRUE if the memory is protected
Bool media_wr_protect(U8 media)
{
  Media *pMed = &medias[media];
  return pMed->protected;
}

//! This function returns the write-protected mode
//! Only used by memory removal with a HARDWARE-SPECIFIC write-protected detection
//! @warning The customer must unplug the memory to change this write-protected mode.
//! @return TRUE if the memory is protected
Bool media_wr_protect_0(void)
{
  return media_wr_protect(0);
}

//! This function returns the write-protected mode
//! Only used by memory removal with a HARDWARE-SPECIFIC write-protected detection
//! @warning The customer must unplug the memory to change this write-protected mode.
//! @return TRUE if the memory is protected
Bool media_wr_protect_1(void)
{
  return media_wr_protect(1);
}

//! This function returns the write-protected mode
//! Only used by memory removal with a HARDWARE-SPECIFIC write-protected detection
//! @warning The customer must unplug the memory to change this write-protected mode.
//! @return TRUE if the memory is protected
Bool media_wr_protect_2(void)
{
  return media_wr_protect(2);
}


//! This function informs about the memory type
//! @param media          Media ID to access.
//! @return TRUE if the memory is removable
Bool media_removal(U8 media)
{
  Media *pMed = &medias[media];
  return pMed->removable;
}

//! This function informs about the memory type
//! @return TRUE if the memory is removable
Bool media_removal_0(void)
{
  return media_removal(0);
}

//! This function informs about the memory type
//! @return TRUE if the memory is removable
Bool media_removal_1(void)
{
  return media_removal(1);
}

//! This function informs about the memory type
//! @return TRUE if the memory is removable
Bool media_removal_2(void)
{
  return media_removal(2);
}


//------------ SPECIFIC FUNCTIONS FOR TRANSFER BY USB --------------------------

#if ACCESS_USB == ENABLED

#include "udi_msc.h"

#ifdef MED_FIFO

static Bool med_access_done[MEDIA_NB_IDS] =  {false, false, false};
static void med_trans_callback(Bool* pDone)
{
    *pDone = true;
}
#define MED_RD(pMed, medAddr, ioBuffer, rdSize, done) \
    MED_Read(pMed, medAddr, ioBuffer, rdSize, (MediaCallback)med_trans_callback, &done)
#define MED_WR(pMed, medAddr, ioBuffer, wrSize, done) \
    MED_Write(pMed, medAddr, ioBuffer, wrSize, (MediaCallback)med_trans_callback, &done)
#else

#define MED_RD(pMed, medAddr, ioBuffer, rdSize) \
    MED_Read(pMed, medAddr, ioBuffer, rdSize, NULL, NULL)

#define MED_WR(pMed, medAddr, ioBuffer, wrSize) \
    MED_Write(pMed, medAddr, ioBuffer, wrSize, NULL, NULL)

#endif
#ifdef USB_FIFO

static Bool udi_access_done[MEDIA_NB_IDS] = {false, false, false};
typedef void (*udi_trans_callback)(udd_ep_status_t status, iram_size_t n);
extern bool udi_msc_b_ack_trans;
extern bool udi_msc_b_abort_trans;
static void udi_trans_callback_0(udd_ep_status_t status)
{
	udi_msc_b_abort_trans = (UDD_EP_TRANSFER_OK != status) ? true : false;
    udi_msc_b_ack_trans = true;
    udi_access_done[0] = true;
}
static void udi_trans_callback_1(udd_ep_status_t status)
{
	udi_msc_b_abort_trans = (UDD_EP_TRANSFER_OK != status) ? true : false;
    udi_msc_b_ack_trans = true;
    udi_access_done[1] = true;
}
static void udi_trans_callback_2(udd_ep_status_t status)
{
	udi_msc_b_abort_trans = (UDD_EP_TRANSFER_OK != status) ? true : false;
    udi_msc_b_ack_trans = true;
    udi_access_done[2] = true;
}
static udi_trans_callback udi_trans_callbacks[] =
{
    (udi_trans_callback)udi_trans_callback_0,
    (udi_trans_callback)udi_trans_callback_1,
    (udi_trans_callback)udi_trans_callback_2
};
/// USB to memory transfer with callback
#define USB_RX(media, ioBuffer, rxSize) \
    udi_msc_trans_block( false, ioBuffer, rxSize, udi_trans_callbacks[media])
/// Memory to USB transfer with callback
#define USB_TX(media, ioBuffer, txSize) \
    udi_msc_trans_block( true, ioBuffer, txSize, udi_trans_callbacks[media])
#else

/// USB to memory transfer without callback
#define USB_RX(ioBuffer, rxSize)    \
    udi_msc_trans_block( false, ioBuffer, rxSize, NULL)
/// Memory to USB transfer without callback
#define USB_TX(ioBuffer, txSize)    \
    udi_msc_trans_block( true, ioBuffer, txSize, NULL)

#endif

//! This function transfers the memory data (programmed in sbc_read_10) directly to the USB interface
//! sector = 512 bytes
//! @param media        Media ID to read.
//! @param addr         Sector address to start read
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_usb_read_10(U8 media, U32 addr, U16 nb_sector)
{
  Media *pMed = &medias[media];
  U32 medAddr, medLen;
  //printf("M%dR.%x ", media, addr);
  if (pMed->blockSize < MEDIA_SECTOR_SIZE)
  {
    medAddr = addr      * (MEDIA_SECTOR_SIZE/pMed->blockSize);
    medLen  = nb_sector * (MEDIA_SECTOR_SIZE/pMed->blockSize);
  }
  else
  {
    medAddr = addr      * MEDIA_SECTOR_SIZE / pMed->blockSize;
    medLen  = nb_sector * MEDIA_SECTOR_SIZE / pMed->blockSize;
  }
  if ((medAddr > pMed->size)
  ||  (medAddr + medLen > pMed->size))
    return CTRL_FAIL;

  media_access_signal_on();

  if ( !media_mem_check(media) )
  {
    media_access_signal_off();
    return CTRL_NO_PRESENT;
  }
  /* Flush before read */
  MED_Flush(pMed);
  /* Read */
  if ( pMed->mappedRD )
  {
    U8 *ptr_cram = (U8*)((pMed->baseAddress + medAddr) * pMed->blockSize);
    if (!udi_msc_trans_block( true, ptr_cram, medLen * pMed->blockSize, NULL))
    {
      media_access_signal_off();
      return CTRL_FAIL;
    }
  }
  else
  {
#ifndef USB_FIFO_MODE
    while(medLen)
    {
      U32 rdSize = min(medLen, MEDIA_BUFFER_NB_BYTES()/pMed->blockSize);
      //printf("%d*%d ", rdSize, pMed->blockSize);
      //printf("m ", rdSize);
      /* Read media */
      if (MED_STATUS_SUCCESS != MED_Read(pMed, medAddr, media_mem_buffer, rdSize, NULL, NULL))
      {
        media_access_signal_off();
        return CTRL_FAIL; // transfer aborted
      }
      //printf("u ");
      /* Write to USB */
      if (!udi_msc_trans_block( true, (uint8_t*)media_mem_buffer, rdSize * pMed->blockSize, NULL))
      {
        media_access_signal_off();
        return CTRL_FAIL; // transfert aborted
      }
      //printf("n ");
      /* Next */
      medLen  -= rdSize;
      medAddr += rdSize;
    }
#else
    U32 medBlkSiz = FIFO_BLOCK_NB_BYTES()/pMed->blockSize;
    U8  medSta = BLOCK_RD, usbSta = BLOCK_IDLE;
    U8 *pBuffer = (U8*)media_mem_buffer, *ioBuffer = NULL;
    U32  medIndex = 0, usbIndex = 0; // buffer byte index
    U32  medTotal = 0, usbTotal = 0;
    U32  medRSize = 0, usbTSize = 0;
    Bool fifoFull = false, fifoNull = true;
    while(usbTotal < medLen)
    {
      /* Media reading */
      switch(medSta)
      {
        /* Start media reading */
        case BLOCK_RD:
            //printf("mRD ");
            ioBuffer = &pBuffer[medIndex];
            medRSize = min(medLen - medTotal, medBlkSiz);
          #ifdef MED_FIFO
            med_access_done[media] = false;
            if (MED_STATUS_SUCCESS != MED_RD(pMed, medAddr, ioBuffer, medRSize, med_access_done[media]))
            {
              media_access_signal_off();
              return CTRL_FAIL; // transfer aborted
            }
            medSta = BLOCK_RD_WAIT;
          #else
            if (MED_STATUS_SUCCESS != MED_RD(pMed, medAddr, ioBuffer, medRSize))
            {
              media_access_signal_off();
              return CTRL_FAIL; // transfer aborted
            }
            medSta = BLOCK_RD_NEXT;
          #endif
            break;

       #ifdef MED_FIFO
        case BLOCK_RD_WAIT:
            if (med_access_done[media])
            {
              //printf("mRN ");
              medSta = BLOCK_RD_NEXT;
            }
            break;
       #endif

        case BLOCK_RD_NEXT:
            fifoNull = false;
            medTotal += medRSize;
            medAddr  += medRSize;
            medIndex  = (medIndex + medRSize * pMed->blockSize) % MEDIA_BUFFER_NB_BYTES();
            /* Loading finished */
            if (medTotal >= medLen)
            {
              //printf("mRE ");
              medSta = BLOCK_IDLE;
            }
            /* FIFO full */
            else if (medIndex == usbIndex)
            {
              //printf("mRP ");
              fifoFull = true;
              medSta = BLOCK_IDLE;
            }
            else
            {
              //printf("mRC ");
              medSta = BLOCK_RD;
            }
            break;

        case BLOCK_IDLE:
            if (medTotal < medLen && !fifoFull)
            {
              //printf("mRS ");
              medSta = BLOCK_RD;
            }
        default:
            break;
      }
      /* USB transfer */
      switch(usbSta)
      {
        case BLOCK_TX:
            //printf("uTX ");
            ioBuffer = &pBuffer[usbIndex];
            usbTSize = min(medLen - usbTotal, medBlkSiz);
          #ifdef USB_FIFO
            udi_access_done[media] = false;
            if (!USB_TX(media, ioBuffer, usbTSize * pMed->blockSize))
            {
              media_access_signal_off();
              return CTRL_FAIL; // transfer aborted
            }
            usbSta = BLOCK_TX_WAIT;
          #else
            if (!USB_TX(ioBuffer, usbTSize * pMed->blockSize))
            {
              media_access_signal_off();
              return CTRL_FAIL; // transfer aborted
            }
            usbSta = BLOCK_TX_NEXT;
          #endif
            break;

       #ifdef USB_FIFO
        case BLOCK_TX_WAIT:
            if (udi_access_done[media])
            {
              //printf("uTN ");
              usbSta = BLOCK_TX_NEXT;
            }
            break;
       #endif

        case BLOCK_TX_NEXT:
            fifoFull = false;
            usbTotal += usbTSize;
            usbIndex  = (usbIndex + usbTSize * pMed->blockSize) % MEDIA_BUFFER_NB_BYTES();
            /* FIFO null */
            if (medIndex == usbIndex)
            {
              //printf("uTP ");
              fifoNull = true;
              usbSta = BLOCK_IDLE;
            }
            else
            {
              //printf("uTC ");
              usbSta = BLOCK_TX;
            }
            break;

        case BLOCK_IDLE:
            if (medTotal > usbTotal)
            {
              //printf("uTS ");
              usbSta = BLOCK_TX;
            }
        default:
            break;
      }
    }
    //printf("\n\r");
#endif
  }
  media_access_signal_off();
  //printf("r ");
  return CTRL_GOOD;
}

//! This function transfers the memory data (programmed in sbc_read_10) directly to the USB interface
//! sector = 512 bytes
//! @param addr         Sector address to start read
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_usb_read_10_0(U32 addr, U16 nb_sector)
{
  return media_usb_read_10(0, addr, nb_sector);
}

//! This function transfers the memory data (programmed in sbc_read_10) directly to the USB interface
//! sector = 512 bytes
//! @param addr         Sector address to start read
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_usb_read_10_1(U32 addr, U16 nb_sector)
{
  return media_usb_read_10(1, addr, nb_sector);
}

//! This function transfers the memory data (programmed in sbc_read_10) directly to the USB interface
//! sector = 512 bytes
//! @param addr         Sector address to start read
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_usb_read_10_2(U32 addr, U16 nb_sector)
{
  return media_usb_read_10(2, addr, nb_sector);
}


//! This function transfers the USB data (programmed in sbc_write_10) directly to the memory interface
//! sector = 512 bytes
//! @param media        Media ID to write.
//! @param addr         Sector address to start write
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_usb_write_10(U8 media, U32 addr, U16 nb_sector)
{
  Media *pMed = &medias[media];
  U32 medAddr, medLen;
  //printf("M%dW.%x ", media, addr);
  if (pMed->blockSize < MEDIA_SECTOR_SIZE)
  {
    medAddr = addr      * (MEDIA_SECTOR_SIZE/pMed->blockSize);
    medLen  = nb_sector * (MEDIA_SECTOR_SIZE/pMed->blockSize);
  }
  else
  {
    medAddr = addr      * MEDIA_SECTOR_SIZE / pMed->blockSize;
    medLen  = nb_sector * MEDIA_SECTOR_SIZE / pMed->blockSize;
  }
  if ((medAddr > pMed->size)
  ||  (medAddr + medLen > pMed->size))
    return CTRL_FAIL;

  media_access_signal_on();

  if ( !media_mem_check(media) )
  {
    media_access_signal_off();
    return CTRL_NO_PRESENT;
  }
  if ( pMed->mappedRD )
  {
    U8 *ptr_cram = (U8*)((pMed->baseAddress + medAddr) * pMed->blockSize);
    if (!udi_msc_trans_block( false, ptr_cram, medLen * pMed->blockSize, NULL))
    {
      media_access_signal_off();
      return CTRL_FAIL;
    }
  }
  else
  {
#ifndef USB_FIFO_MODE
    while(medLen)
    {
      U32 wrSize = min(medLen, MEDIA_BUFFER_NB_BYTES()/pMed->blockSize);
      //printf("%d*%d ", wrSize, pMed->blockSize);
      //printf("u ");
      /* Read USB */
      if (!udi_msc_trans_block( false, (uint8_t*)media_mem_buffer, wrSize * pMed->blockSize, NULL))
      {
        media_access_signal_off();
        return CTRL_FAIL; // transfert aborted
      }
      //printf("m ");
      /* Write media */
      if (MED_STATUS_SUCCESS != MED_Write(pMed, medAddr, media_mem_buffer, wrSize, NULL, NULL))
      {
        //MED_Flush(pMed);
        media_access_signal_off();
        return CTRL_FAIL; // transfer aborted
      }
      //printf("n ");
      /* Next */
      medLen  -= wrSize;
      medAddr += wrSize;
    }
#else
    U32 medBlkSiz = FIFO_BLOCK_NB_BYTES()/pMed->blockSize;
    U8  medSta = BLOCK_IDLE, usbSta = BLOCK_RX;
    U8 *pBuffer = (U8*)media_mem_buffer, *ioBuffer = NULL;
    U32  medIndex = 0, usbIndex = 0; // buffer byte index
    U32  medTotal = 0, usbTotal = 0;
    U32  medWSize = 0, usbRSize = 0;
    Bool fifoFull = false, fifoNull = true;
    while(medTotal < medLen)
    {
      /* USB receive */
      switch(usbSta)
      {
        case BLOCK_RX:
            //printf("uRX ");
            ioBuffer = &pBuffer[usbIndex];
            usbRSize = min(medLen - usbTotal, medBlkSiz);
          #ifdef USB_FIFO
            udi_access_done[media] = false;
            if (!USB_RX(media, ioBuffer, usbRSize * pMed->blockSize))
            {
              media_access_signal_off();
              return CTRL_FAIL; // transfer aborted
            }
            usbSta = BLOCK_RX_WAIT;
          #else
            if (!USB_RX(ioBuffer, usbRSize * pMed->blockSize))
            {
              media_access_signal_off();
              return CTRL_FAIL; // transfer aborted
            }
            usbSta = BLOCK_RX_NEXT;
          #endif
            break;

       #ifdef USB_FIFO
        case BLOCK_RX_WAIT:
            if (udi_access_done[media])
              usbSta = BLOCK_RX_NEXT;
            break;
       #endif

        case BLOCK_RX_NEXT:
            fifoNull = false;
            usbTotal += usbRSize;
            usbIndex  = (usbIndex + usbRSize * pMed->blockSize) % MEDIA_BUFFER_NB_BYTES();
            if (usbTotal >= medTotal)
            {
              //printf("uRE ");
              usbSta = BLOCK_IDLE;
            }
            else if (usbIndex == medIndex)
            {
              //printf("uPR ");
              fifoFull = true;
              usbSta = BLOCK_IDLE;
            }
            else
            {
              //printf("uNR ");
              usbSta = BLOCK_RX;
            }
            break;

        case BLOCK_IDLE:
            if (usbTotal < medLen && !fifoFull)
            {
              //printf("uSR ");
              usbSta = BLOCK_RX;
            }
        default:
            break;
      }
      /* Media write */
      switch(medSta)
      {
        case BLOCK_WR:
            //printf("mWR ");
            ioBuffer = &pBuffer[medIndex];
            medWSize = min(medLen - medTotal, medBlkSiz);
          #ifdef MED_FIFO
            med_access_done[media] = false;
            if (MED_STATUS_SUCCESS != MED_WR(pMed, medAddr, ioBuffer, medWSize, med_access_done[media]))
            {
              media_access_signal_off();
              return CTRL_FAIL; // transfer aborted
            }
            medSta = BLOCK_WR_WAIT;
          #else
            if (MED_STATUS_SUCCESS != MED_WR(pMed, medAddr, ioBuffer, medWSize))
            {
              media_access_signal_off();
              return CTRL_FAIL; // transfer aborted
            }
            medSta = BLOCK_WR_NEXT;
          #endif
            break;

       #ifdef MED_FIFO
        case BLOCK_WR_WAIT:
            if (med_access_done[media])
              medSta = BLOCK_WR_NEXT;
            break;
       #endif
        
        case BLOCK_WR_NEXT:
            fifoFull = false;
            medAddr  += medWSize;
            medTotal += medWSize;
            medIndex  = (medIndex + medWSize * pMed->blockSize) % MEDIA_BUFFER_NB_BYTES();
            if (medIndex == usbIndex)
            {
              //printf("mPW ");
              fifoNull = true;
              medSta = BLOCK_IDLE;
            }
            else
            {
              //printf("mNW ");
              medSta = BLOCK_WR;
            }
            break;
        
        case BLOCK_IDLE:
            if (medTotal < usbTotal)
            {
              //printf("mWS ");
              medSta = BLOCK_WR;
            }
        default:
            break;
      }
    }
#endif
  }
  //MED_Flush(pMed);
  //printf("w ");
  media_access_signal_off();
  return CTRL_GOOD;
}

//! This function transfers the USB data (programmed in sbc_write_10) directly to the memory interface
//! sector = 512 bytes
//! @param addr         Sector address to start write
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_usb_write_10_0(U32 addr, U16 nb_sector)
{
  return media_usb_write_10(0, addr, nb_sector);
}

//! This function transfers the USB data (programmed in sbc_write_10) directly to the memory interface
//! sector = 512 bytes
//! @param addr         Sector address to start write
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_usb_write_10_1(U32 addr, U16 nb_sector)
{
  return media_usb_write_10(1, addr, nb_sector);
}

//! This function transfers the USB data (programmed in sbc_write_10) directly to the memory interface
//! sector = 512 bytes
//! @param addr         Sector address to start write
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_usb_write_10_2(U32 addr, U16 nb_sector)
{
  return media_usb_write_10(2, addr, nb_sector);
}


#endif  // ACCESS_USB == ENABLED


//------------ SPECIFIC FUNCTIONS FOR TRANSFER BY RAM --------------------------

#if ACCESS_MEM_TO_RAM == ENABLED

#include <string.h>

//! This function tranfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param media        Media ID to access.
//! @param addr         Sector address to start read
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_mem_2_ram(U8 media, U32 addr, void *ram)
{
  Media *pMed = &medias[media];
  U32 medAddr, medLen, nb_sector = 1;
  if (pMed->blockSize < MEDIA_SECTOR_SIZE)
  {
    medAddr = addr      * (MEDIA_SECTOR_SIZE/pMed->blockSize);
    medLen  = nb_sector * (MEDIA_SECTOR_SIZE/pMed->blockSize);
  }
  else
  {
    medAddr = addr      * MEDIA_SECTOR_SIZE / pMed->blockSize;
    medLen  = nb_sector * MEDIA_SECTOR_SIZE / pMed->blockSize;
  }
  if ((medAddr > pMed->size)
  ||  (medAddr + medLen > pMed->size))
    return CTRL_FAIL;
  
  media_access_signal_on();
  
  if ( !media_mem_check(media) )
  {
    media_access_signal_off();
    return CTRL_NO_PRESENT;
  }
  MED_Flush(pMed);
  if ( pMed->mappedRD )
  {
    U8 *ptr_cram = (U8*)((pMed->baseAddress + medAddr) * pMed->blockSize);
    memcpy(ram, ptr_cram, medLen * pMed->blockSize);
  }
  else
  {
    if (MED_STATUS_SUCCESS != MED_Read(pMed, medAddr, ram, medLen, NULL, NULL))
    {
      media_access_signal_off();
      return CTRL_FAIL;
    }
  }
  media_access_signal_off();
  return CTRL_GOOD;
}

//! This function tranfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param addr         Sector address to start read
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_mem_2_ram_0(U32 addr, void *ram)
{
  return media_mem_2_ram(0, addr, ram);
}

//! This function tranfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param addr         Sector address to start read
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_mem_2_ram_1(U32 addr, void *ram)
{
  return media_mem_2_ram(1, addr, ram);
}

//! This function tranfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param addr         Sector address to start read
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_mem_2_ram_2(U32 addr, void *ram)
{
  return media_mem_2_ram(2, addr, ram);
}


//! This function tranfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param media        Media ID to access.
//! @param addr         Sector address to start write
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_ram_2_mem(U8 media, U32 addr, const void *ram)
{
  Media *pMed = &medias[media];
  U32 medAddr, medLen, nb_sector = 1;
  if (pMed->blockSize < MEDIA_SECTOR_SIZE)
  {
    medAddr = addr      * (MEDIA_SECTOR_SIZE/pMed->blockSize);
    medLen  = nb_sector * (MEDIA_SECTOR_SIZE/pMed->blockSize);
  }
  else
  {
    medAddr = addr      * MEDIA_SECTOR_SIZE / pMed->blockSize;
    medLen  = nb_sector * MEDIA_SECTOR_SIZE / pMed->blockSize;
  }
  if ((medAddr > pMed->size)
  ||  (medAddr + medLen > pMed->size))
    return CTRL_FAIL;
  
  media_access_signal_on();
  
  if ( !media_mem_check(media) )
  {
    media_access_signal_off();
    return CTRL_NO_PRESENT;
  }
  if ( pMed->mappedRD )
  {
    U8 *ptr_cram = (U8*)((pMed->baseAddress + medAddr) * pMed->blockSize);
    memcpy(ptr_cram, ram, medLen * pMed->blockSize);
  }
  else
  {
    if (MED_STATUS_SUCCESS != MED_Write(pMed, medAddr, ram, medLen, NULL, NULL))
    {
      //MED_Flush(pMed);
      media_access_signal_off();
      return CTRL_FAIL;
    }
  }
  //MED_Flush(pMed);
  media_access_signal_off();
  return CTRL_GOOD;
}

//! This function tranfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param addr         Sector address to start write
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_ram_2_mem_0(U32 addr, const void *ram)
{
  return media_ram_2_mem(0, addr, ram);
}

//! This function tranfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param addr         Sector address to start write
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_ram_2_mem_1(U32 addr, const void *ram)
{
  return media_ram_2_mem(1, addr, ram);
}

//! This function tranfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param addr         Sector address to start write
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status media_ram_2_mem_2(U32 addr, const void *ram)
{
  return media_ram_2_mem(2, addr, ram);
}


#endif  // ACCESS_MEM_TO_RAM == ENABLED


#endif  // MEDIA_MEM == ENABLE
