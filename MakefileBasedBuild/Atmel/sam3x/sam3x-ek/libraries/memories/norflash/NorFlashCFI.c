/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
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
  *  \file
  *
  *  Implementation of norflash common flash interface functions.
  */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "memories.h"

#include <string.h>

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/

#define DUMP_CFI

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Dump the Common Flash Interface Definition Table.
 *
 * \param pNorFlashCFI  Pointer to an NorFlashCFI instance.
 */
static void NorFlash_CFI_DumpConfiguration( NorFlashCFI *pNorFlashCFI )
{
    uint8_t i ;

    TRACE_DEBUG("Common Flash Interface Definition Table\n\r");
    TRACE_DEBUG("Addr. Data   Description \n\r");
    TRACE_DEBUG("0x10  %04Xh  Query Unique ASCII string\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.queryUniqueString[0]);
    TRACE_DEBUG("0x11  %04Xh  \n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.queryUniqueString[1]);
    TRACE_DEBUG("0x12  %04Xh  \n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.queryUniqueString[2]);
    TRACE_DEBUG("0x13  %04Xh  Primary OEM Command Set\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.primaryCode);
    TRACE_DEBUG("0x15  %04Xh  Address for Primary Extended Table\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.primaryAddr);
    TRACE_DEBUG("0x17  %04Xh  Alternate OEM Command Set\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.alternateCode);
    TRACE_DEBUG("0x19  %04Xh  Address for Alternate OEM Extended Table\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.alternateAddr);
    TRACE_DEBUG("0x1B  %04Xh  VCC min write/erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minVcc);
    TRACE_DEBUG("0x1C  %04Xh  VCC max write/erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxVcc);
    TRACE_DEBUG("0x1D  %04Xh  VPP min voltage\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minVpp);
    TRACE_DEBUG("0x1E  %04Xh  VPP max voltage\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxVpp);
    TRACE_DEBUG("0x1F  %04Xh  Typical timeout per single word write\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minTimeOutWrite);
    TRACE_DEBUG("0x20  %04Xh  Typical timeout for Min. size buffer write\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minTimeOutBuffer);
    TRACE_DEBUG("0x21  %04Xh  Typical timeout per individual block erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minTimeOutBlockErase);
    TRACE_DEBUG("0x22  %04Xh  Typical timeout for full chip erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minTimeOutChipErase);
    TRACE_DEBUG("0x23  %04Xh  Max. timeout for word write\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxTimeOutWrite);
    TRACE_DEBUG("0x24  %04Xh  Max. timeout for buffer write\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxTimeOutBuffer);
    TRACE_DEBUG("0x25  %04Xh  Max. timeout per individual block erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxTimeOutBlockErase);
    TRACE_DEBUG("0x26  %04Xh  Max. timeout for full chip erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxTimeOutChipErase);

    TRACE_DEBUG("0x27  %04Xh  Device Size = 2N byte\n\r",
    pNorFlashCFI->norFlashCfiDeviceGeometry.deviceSize);
    TRACE_DEBUG("0x28  %04Xh  Flash Device Interface description\n\r",
    pNorFlashCFI->norFlashCfiDeviceGeometry.deviceInterface);
    TRACE_DEBUG("0x2A  %04Xh  Max. number of byte in multi-byte write\n\r",
    pNorFlashCFI->norFlashCfiDeviceGeometry.numMultiWrite);
    TRACE_DEBUG("0x2C  %04Xh  Number of Erase Block Regions within device\n\r",
    pNorFlashCFI->norFlashCfiDeviceGeometry.numEraseRegion);

    for(i = 0; i < pNorFlashCFI->norFlashCfiDeviceGeometry.numEraseRegion; i++)
    {
        TRACE_DEBUG("0x%2X  %04Xh  Number of Erase Blocks of identical size within region %x \n\r",
            0x2D + i * 4, pNorFlashCFI->norFlashCfiDeviceGeometry.eraseRegionInfo[i].Y, i );
        TRACE_DEBUG("0x%2X  %04Xh  (z) times 256 bytes within region %x \n\r",
            0x2E + i * 4, pNorFlashCFI->norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z, i );
    }
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Returns the numbers of block in all Norflash regions.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 */
uint32_t NorFlash_GetDeviceNumOfBlocks( NorFlashInfo *pNorFlashInfo )
{
    uint8_t i;
    uint32_t blocks = 0;
    uint16_t numBlockRegion;

    numBlockRegion = pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;

    for (i = 0; i < numBlockRegion; i++)
    {
        blocks += (pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i]).Y + 1;
    }

    return blocks;
}

/**
 * \brief Returns the minimun block size in all Norflash regions.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 */
uint32_t NorFlash_GetDeviceMinBlockSize( NorFlashInfo *pNorFlashInfo )
{
    uint8_t i;
    uint16_t numBlockRegion;
    uint32_t size ;
    uint32_t dwTemp ;

    numBlockRegion = pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;

    size = (pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[0].Z) * 256;

    for ( i = 1; i < numBlockRegion; i++ )
    {
        dwTemp=(pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256 ;

        if (size > dwTemp )
        {
            size = dwTemp ;
        }
    }

    return size;
}

/**
 * \brief Returns the maximun block size in all Norflash regions.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 */
uint32_t NorFlash_GetDeviceMaxBlockSize( NorFlashInfo* pNorFlashInfo )
{
    uint8_t i;
    uint16_t numBlockRegion;
    uint32_t size ;
    uint32_t dwTemp ;

    numBlockRegion = pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;

    size = (pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[0].Z) * 256;

    for (i = 1; i < numBlockRegion; i++)
    {
        dwTemp=(pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256 ;

        if (size < dwTemp )
        {
            size = dwTemp ;
        }
    }
    return size;
}

/**
 * \brief Returns the block size in giving block number.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 * \param sector  Sector number.
 */
uint32_t NorFlash_GetDeviceBlockSize( NorFlashInfo* pNorFlashInfo, uint32_t sector )
{
    uint16_t i;
    uint16_t j;
    uint16_t numBlockRegion ;
    uint16_t numBlockPerRegion ;
    uint32_t block = 0;

    numBlockRegion = pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;

    for (i = 0; i < numBlockRegion; i++)
    {
        numBlockPerRegion = (pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i]).Y + 1;

        for (j = 0; j < numBlockPerRegion; j++)
        {
            if (block == sector)
            {
                return (pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256 ;
            }
            block++;
        }
    }
    return 0;
}

/**
 * \brief Returns secort number on specified memory offset.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 * \param memoryOffset  Memory offset.
 */
uint16_t NorFlash_GetDeviceSectorInRegion( NorFlashInfo *pNorFlashInfo, uint32_t memoryOffset )
{
    uint16_t numBlockRegion,numBlockPerRegion;
    uint16_t sectorId = 0;
    uint32_t size = 0;
    uint8_t done = 0;
    uint16_t i , j;

    numBlockRegion = pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;

    for ( i=0 ; (i < numBlockRegion) && (done == 0) ; i++ )
    {
        numBlockPerRegion = (pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i]).Y + 1;

        for (j = 0; j < numBlockPerRegion; j++)
        {
            size+= (pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256 ;

            if ( size > memoryOffset )
            {
                done = 1;
                break;
            }
            sectorId++;
        }
    }

    return sectorId;
}

/**
 * \brief Returns start address of specified sector number.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 * \param sector  Sector number.
 */
uint32_t NorFlash_GetDeviceSectorAddress( NorFlashInfo *pNorFlashInfo, uint32_t sector )
{
    uint16_t numBlockRegion,numBlockPerRegion;
    uint16_t sectorId = 0;
    uint32_t address = 0;
    uint8_t done = 0;
    uint16_t i , j;

    numBlockRegion = pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;

    for ( i = 0 ; (i < numBlockRegion) && (done == 0) ; i++ )
    {
        numBlockPerRegion = (pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i]).Y + 1;

        for ( j = 0; j < numBlockPerRegion; j++ )
        {
            if (sector == sectorId)
            {
                done = 1;
                break;
            }

            address+= (pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256 ;
            sectorId++;
        }
    }

    return address;
}

/**
 * \brief Convert address to byte addressing.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 * \Param offset Address offset
 */
uint32_t NorFlash_GetByteAddress( NorFlashInfo *pNorFlashInfo, uint32_t offset )
{
    return (offset * pNorFlashInfo-> deviceChipWidth);
}

/**
 * \brief Convert address to byte addressing and return the address in chip.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 * \Param offset Address offset
 */
uint32_t NorFlash_GetByteAddressInChip( NorFlashInfo *pNorFlashInfo, uint32_t offset )
{
    return (pNorFlashInfo->baseAddress + (offset * pNorFlashInfo-> deviceChipWidth));
}

/**
 * \brief Returns the address in chip.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 * \Param offset Address offset
 */
uint32_t NorFlash_GetAddressInChip( NorFlashInfo *pNorFlashInfo, uint32_t offset )
{
    return (pNorFlashInfo->baseAddress + offset);
}

/**
 * \brief Returns bus width in bits of giving device.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 */
uint8_t NorFlash_GetDataBusWidth( NorFlashInfo *pNorFlashInfo )
{
    return (pNorFlashInfo->deviceChipWidth * 8);
}

/**
 * \brief Returns the size of the whole device in bytes.
 *
 * \param pNorFlashInfo  Pointer to a NorFlashInfo instance.
 */
uint32_t NorFlash_GetDeviceSizeInBytes( NorFlashInfo *pNorFlashInfo )
{
    return ((uint32_t) 2 << ((pNorFlashInfo->cfiDescription.norFlashCfiDeviceGeometry.deviceSize) - 1));
}

/**
 * \brief Looks for query struct in Norflash common flash interface.
 * If found, the model variable is filled with the correct values.
 * This function returns 0 if a matching model has been found; otherwise it
 * returns NorCommon_ERROR_UNKNOWNMODEL.
 */
uint8_t NorFlash_CFI_Detect( NorFlash *pNorFlash, uint8_t hardwareBusWidth )
{
    uint8_t i;
    uint8_t *pCfi = (uint8_t*)(&(pNorFlash->norFlashInfo.cfiDescription));
    uint32_t address;

    pNorFlash->norFlashInfo.cfiCompatible = 0;
    pNorFlash->norFlashInfo.deviceChipWidth = hardwareBusWidth;
    address = CFI_QUERY_OFFSET;

    for ( i = 0; i< sizeof( NorFlashInfo) ; i++)
    {
        WriteCommand(8, NorFlash_GetByteAddressInChip(&(pNorFlash->norFlashInfo), CFI_QUERY_ADDRESS), CFI_QUERY_COMMAND);
        ReadRawData(8, NorFlash_GetByteAddressInChip(&(pNorFlash->norFlashInfo), address), pCfi);
        address++;
        pCfi++;
    }

    /* Check the query-unique ASCII string "QRY" */
    if ( (pNorFlash->norFlashInfo.cfiDescription.norFlashCfiQueryInfo.queryUniqueString[0] != 'Q' )
        || (pNorFlash->norFlashInfo.cfiDescription.norFlashCfiQueryInfo.queryUniqueString[1] != 'R')
        || (pNorFlash->norFlashInfo.cfiDescription.norFlashCfiQueryInfo.queryUniqueString[2] != 'Y') )
    {
        return NorCommon_ERROR_UNKNOWNMODEL;
    }

#ifdef DUMP_CFI
    NorFlash_CFI_DumpConfiguration( &(pNorFlash->norFlashInfo.cfiDescription) ) ;
#endif

    if (pNorFlash->norFlashInfo.cfiDescription.norFlashCfiQueryInfo.primaryCode == CMD_SET_AMD)
    {
        pNorFlash->pOperations = &amdOperations;
    }
    else if ((pNorFlash->norFlashInfo.cfiDescription.norFlashCfiQueryInfo.primaryCode == CMD_SET_INTEL_EXT)
     || (pNorFlash->norFlashInfo.cfiDescription.norFlashCfiQueryInfo.primaryCode == CMD_SET_INTEL))
    {
        pNorFlash->pOperations = &intelOperations;
    }
    else
    {
        return NorCommon_ERROR_UNKNOWNMODEL;
    }

    pNorFlash->norFlashInfo.cfiCompatible = 1;
    NORFLASH_Reset(pNorFlash, 0);

    return 0;
}

