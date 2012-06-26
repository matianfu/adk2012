/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2010, Atmel Corporation
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
 * MappedNandFlash layer will do operations on logical blocks of nandflash, it is called by
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "memories.h"

#include <string.h>
#include <assert.h>


/*----------------------------------------------------------------------------
 *        Local macros
 *----------------------------------------------------------------------------*/

#define min( a, b ) (((a) < (b)) ? (a) : (b))

/*----------------------------------------------------------------------------
 *        Internal definitions
 *----------------------------------------------------------------------------*/

/** Casts */
#define MANAGED(mapped) ((struct ManagedNandFlash *) mapped)
#define ECC(mapped)     ((struct EccNandFlash *) mapped)
#define RAW(mapped)     ((struct RawNandFlash *) mapped)
#define MODEL(mapped)   ((struct NandFlashModel *) mapped)

/** Logical block mapping pattern */
#define PATTERN(i)      ((i << 1) & 0x73)

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/**
 * \brief  Scans a mapped nandflash to find an existing logical block mapping. If a
 * block contains the mapping, its index is stored in the provided variable (if
 * pointer is not 0).
 *
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param logicalMappingBlock  Pointer to a variable for storing the block number.
 * \return  0 if mapping has been found; otherwise returns
 * NandCommon_ERROR_NOMAPPING if no mapping exists, or another NandCommon_ERROR_xxx code.
 */
static uint8_t FindLogicalMappingBlock(
    const struct MappedNandFlash *mapped,
    int16_t *logicalMappingBlock)
{
    uint16_t block;
    uint8_t found;
    uint16_t numBlocks = ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
    uint16_t pageDataSize = NandFlashModel_GetPageDataSize(MODEL(mapped));
    uint8_t error;
    uint8_t data[NandCommon_MAXPAGEDATASIZE];
    uint32_t i;

    TRACE_INFO("FindLogicalMappingBlock()~%d\n\r", numBlocks);

    /* Search each LIVE block */
    found = 0;
    block = 0;
    while (!found && (block < numBlocks)) {

        /* Check that block is LIVE*/
        if (MANAGED(mapped)->blockStatuses[block].status == NandBlockStatus_LIVE) {

            /* Read block*/
            TRACE_INFO("Checking LIVE block #%d\n\r", block);
            error = ManagedNandFlash_ReadPage(MANAGED(mapped), block, 0, data, 0);
            if (!error) {

                /* Compare data with logical mapping pattern*/
                i = 0;
                found = 1;
                while ((i < pageDataSize) && found) {

                    if (data[i] != PATTERN(i)) {

                        found = 0;
                    }
                    i++;
                }

                /* If this is the mapping, stop looking*/
                if (found) {

                    TRACE_WARNING_WP("-I- Logical mapping in block #%d\n\r",
                                     block);
                    if (logicalMappingBlock) {

                        *logicalMappingBlock = block;
                    }
                    return 0;
                }
            }
            else if (error != NandCommon_ERROR_WRONGSTATUS) {

                TRACE_ERROR(
                          "FindLogicalMappingBlock: Failed to scan block #%d\n\r",
                          block);
                return error;
            }
        }

        block++;
    }

    TRACE_WARNING("No logical mapping found in device\n\r");
    return NandCommon_ERROR_NOMAPPING;
}

/**
 * \brief  Loads the logical mapping contained in the given physical block.
 * block contains the mapping, its index is stored in the provided variable (if
 * pointer is not 0).
 *
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param physicalBlock  Physical block number.
 * \return  0 if successful; otherwise, returns a NandCommon_ERROR code.
 */
static uint8_t LoadLogicalMapping(
    struct MappedNandFlash *mapped,
    uint16_t physicalBlock)
{
    uint8_t error;
    uint8_t data[NandCommon_MAXPAGEDATASIZE];
    uint16_t pageDataSize =
                    NandFlashModel_GetPageDataSize(MODEL(mapped));
    uint16_t numBlocks =
                    ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
    uint32_t remainingSize;
    uint8_t *currentBuffer;
    uint16_t currentPage;
    uint32_t readSize;
    uint32_t i;
    uint8_t status;
    int16_t logicalBlock;
    /*int16_t firstBlock, lastBlock;*/

    TRACE_INFO("LoadLogicalMapping(B#%d)\n\r", physicalBlock);

    /* Load mapping from pages #1 - #XXX of block*/
    currentBuffer = (uint8_t *) mapped->logicalMapping;
    remainingSize = sizeof(mapped->logicalMapping);
    currentPage = 1;
    while (remainingSize > 0) {

        /* Read page*/
        readSize = min(remainingSize, pageDataSize);
        error = ManagedNandFlash_ReadPage(MANAGED(mapped),
                                          physicalBlock,
                                          currentPage,
                                          data,
                                          0);
        if (error) {

            TRACE_ERROR(
                      "LoadLogicalMapping: Failed to load mapping\n\r");
            return error;
        }

        /* Copy page info*/
        memcpy(currentBuffer, data, readSize);

        currentBuffer += readSize;
        remainingSize -= readSize;
        currentPage++;
    }

    /* Store mapping block index*/
    mapped->logicalMappingBlock = physicalBlock;

    /* Power-loss recovery*/
    for (i=0; i < numBlocks; i++) {

        /* Check that this is not the logical mapping block*/
        if (i != physicalBlock) {

            status = mapped->managed.blockStatuses[i].status;
            logicalBlock = MappedNandFlash_PhysicalToLogical(mapped, i);

            /* Block is LIVE*/
            if (status == NandBlockStatus_LIVE) {

                /* Block is not mapped -> release it*/
                if (logicalBlock == -1) {

                    TRACE_WARNING_WP("-I- Release unmapped LIVE #%u\n\r",
                                     i);
                    ManagedNandFlash_ReleaseBlock(MANAGED(mapped), i);
                }
            }
            /* Block is DIRTY*/
            else if (status == NandBlockStatus_DIRTY) {

                /* Block is mapped -> fake it as live*/
                if (logicalBlock != -1) {

                    TRACE_WARNING_WP("-I- Mark mapped DIRTY #%u -> LIVE\n\r",
                                     i);
                    mapped->managed.blockStatuses[i].status =
                                                    NandBlockStatus_LIVE;
                }
            }
            /* Block is FREE or BAD*/
            else {

                /* Block is mapped -> remove it from mapping*/
                if (logicalBlock != -1) {

                    TRACE_WARNING_WP("-I- Unmap FREE or BAD #%u\n\r", i);
                    mapped->logicalMapping[logicalBlock] = -1;
                }
            }
        }
    }

    TRACE_WARNING_WP("-I- Mapping loaded from block #%d\n\r", physicalBlock);

    return 0;
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
/**
 * \brief  Initializes a MappedNandFlash instance. Scans the device to look for and
 * existing logical block mapping; otherwise starts from scratch (no block
 * mapped).
 *
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param model  Pointer to the underlying nand chip model. Can be 0.
 * \param commandAddress  Address at which commands are sent.
 * \param addressAddress  Address at which addresses are sent.
 * \param dataAddress  Address at which data is sent.
 * \param pinChipEnable  Pin controlling the CE signal of the NandFlash.
 * \param pinReadyBusy  Pin used to monitor the ready/busy signal of the Nand.
 * \return 0 if successful; otherwise returns a NandCommon_ERROR_xxx code.
 */
uint8_t MappedNandFlash_Initialize(
    struct MappedNandFlash *mapped,
    const struct NandFlashModel *model,
    uint32_t commandAddress,
    uint32_t addressAddress,
    uint32_t dataAddress,
    const Pin pinChipEnable,
    const Pin pinReadyBusy,
    uint16_t baseBlock,
    uint16_t sizeInBlocks)
{
    uint8_t error;
    uint16_t numBlocks;
    uint16_t block;
    int16_t logicalMappingBlock = 0;

    TRACE_INFO("MappedNandFlash_Initialize()\n\r");

    /* Initialize ManagedNandFlash*/
    error = ManagedNandFlash_Initialize(MANAGED(mapped),
                                        model,
                                        commandAddress,
                                        addressAddress,
                                        dataAddress,
                                        pinChipEnable,
                                        pinReadyBusy,
                                        baseBlock,
                                        sizeInBlocks);
    if (error) {

        return error;
    }

    /* Scan to find logical mapping*/
    mapped->mappingModified = 0;
    error = FindLogicalMappingBlock(mapped, &logicalMappingBlock);
    if (!error) {

        /* Extract mapping from block*/
        mapped->logicalMappingBlock = logicalMappingBlock;
        return LoadLogicalMapping(mapped, logicalMappingBlock);
    }
    else if (error == NandCommon_ERROR_NOMAPPING) {

        /* Start with no block mapped*/
        mapped->logicalMappingBlock = -1;
        numBlocks = ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
        for (block=0; block < numBlocks; block++) {

            mapped->logicalMapping[block] = -1;
        }
    }
    else {

        TRACE_ERROR("MappedNandFlash_Initialize: Initialize device\n\r");
        return error;
    }

    return 0;
}

/**
 * \brief  Reads the data and/or spare area of a page in a mapped logical block.
 * the data is valid using the ECC information contained in the spare. If one
 * buffer pointer is 0, the corresponding area is not saved.
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param block  Number of block to read from.
 * \param page  Number of page to read inside given block.
 * \param data  Data area buffer, can be 0.
 * \param spare  Spare area buffer, can be 0.
 * \return 0 if successful; otherwise, returns NandCommon_ERROR_BLOCKNOTMAPPED
 * if the block is not mapped, or a NandCommon_ERROR_xxx code.
 */
uint8_t MappedNandFlash_ReadPage(
    const struct MappedNandFlash *mapped,
    uint16_t block,
    uint16_t page,
    void *data,
    void *spare)
{
    int16_t physicalBlock;

    TRACE_INFO("MappedNandFlash_ReadPage(LB#%d:P#%d)\n\r", block, page);

    /* Check if block is mapped*/
    physicalBlock = mapped->logicalMapping[block];
    if (physicalBlock == -1) {

        TRACE_INFO( "MappedNandFlash_ReadPage: Block %d not mapped\n\r", block);
        return NandCommon_ERROR_BLOCKNOTMAPPED;
    }

    /* Read page from corresponding physical block*/
    return ManagedNandFlash_ReadPage(MANAGED(mapped),
                                     physicalBlock,
                                     page,
                                     data,
                                     spare);
}

/**
 * \brief  Writes  the data and/or spare area of a page in a mapped logical block.
 * the data is valid using the ECC information contained in the spare. If one
 * buffer pointer is 0, the corresponding area is not saved.
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param block  Number of block to read from.
 * \param page  Number of page to write inside given block.
 * \param data  Data area buffer, can be 0.
 * \param spare  Spare area buffer, can be 0.
 * \return 0 if successful; otherwise, returns NandCommon_ERROR_BLOCKNOTMAPPED
 * if the block is not mapped, or a NandCommon_ERROR_xxx code.
 */
uint8_t MappedNandFlash_WritePage(
    const struct MappedNandFlash *mapped,
    uint16_t block,
    uint16_t page,
    void *data,
    void *spare)
{
    int16_t physicalBlock;

    TRACE_INFO("MappedNandFlash_WritePage(LB#%d:P#%d)\n\r", block, page);

    /* Check if block is mapped*/
    physicalBlock = mapped->logicalMapping[block];
    if (physicalBlock == -1) {

        TRACE_ERROR("MappedNandFlash_WritePage: Block must be mapped\n\r");
        return NandCommon_ERROR_BLOCKNOTMAPPED;
    }

    /* Write page on physical block*/
    return ManagedNandFlash_WritePage(MANAGED(mapped),
                                      physicalBlock,
                                      page,
                                      data,
                                      spare);
}

/**
 * \brief  Maps a logical block number to an actual physical block. This allocates
 * the physical block (meaning it must be FREE), and releases the previous
 * block being replaced (if any).
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param logicalBlock  Logical block number to map.
 * \param physicalBlock  Physical block to map to the logical one.
 * \return  0 if successful; otherwise returns a NandCommon_ERROR_xxx code.
 */
uint8_t MappedNandFlash_Map(
    struct MappedNandFlash *mapped,
    uint16_t logicalBlock,
    uint16_t physicalBlock)
{
    uint8_t error;
    int16_t oldPhysicalBlock;

    TRACE_INFO("MappedNandFlash_Map(LB#%d -> PB#%d)\n\r",
               logicalBlock, physicalBlock);
    assert( logicalBlock < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped)) ) ; /* "MappedNandFlash_Map: logicalBlock out-of-range\n\r" */
    assert( physicalBlock < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped)) ) ; /* "MappedNandFlash_Map: physicalBlock out-of-range\n\r" */

    /* Allocate physical block*/
    error = ManagedNandFlash_AllocateBlock(MANAGED(mapped), physicalBlock);
    if ( error )
    {
        return error;
    }

    /* Release currently mapped block (if any)*/
    oldPhysicalBlock = mapped->logicalMapping[logicalBlock];
    if (oldPhysicalBlock != -1)
    {
        error = ManagedNandFlash_ReleaseBlock(MANAGED(mapped), oldPhysicalBlock);
        if ( error )
        {
            return error;
        }
    }

    /* Set mapping*/
    mapped->logicalMapping[logicalBlock] = physicalBlock;
    mapped->mappingModified = 1;

    return 0;
}

/**
 * \brief  Unmaps a logical block by releasing the corresponding physical block (if
 * any).
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param logicalBlock  Number of logical block to unmap.
  * \return  0 if successful; otherwise returns a NandCommon_ERROR_xxx code.
 */
uint8_t MappedNandFlash_Unmap(
    struct MappedNandFlash *mapped,
    uint16_t logicalBlock)
{
    int16_t physicalBlock = mapped->logicalMapping[logicalBlock];
    uint8_t error;

    TRACE_INFO("MappedNandFlash_Unmap(LB#%d)\n\r", logicalBlock);
    assert( logicalBlock < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped)) ) ; /* "MappedNandFlash_Unmap: logicalBlock out-of-range\n\r" */

    if (physicalBlock != -1) {

        error = ManagedNandFlash_ReleaseBlock(MANAGED(mapped), physicalBlock);
        if (error) {

            return error;
        }
    }
    mapped->logicalMapping[logicalBlock] = -1;
    mapped->mappingModified = 1;

    return 0;
}

/**
 * \brief  Returns the physical block mapped with the given logical block, or -1 if it
 * is not mapped.
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param logicalBlock  Logical block number.
  * \return  the physical block, or -1 if it is not mapped.
 */
int16_t MappedNandFlash_LogicalToPhysical(
    const struct MappedNandFlash *mapped,
    uint16_t logicalBlock)
{
    assert( logicalBlock < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped)) ) ; /* "MappedNandFlash_LogicalToPhysical: logicalBlock out-of-range\n\r" */

    return mapped->logicalMapping[logicalBlock];
}

/**
 * \brief  Returns the logical block mapped with the given logical block, or -1 if it
 * is not mapped.
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param physicalBlock  Physical block number.
  * \return  the logical block, or -1 if it is not mapped.
 */
int16_t MappedNandFlash_PhysicalToLogical(
    const struct MappedNandFlash *mapped,
    uint16_t physicalBlock)
{
    uint16_t numBlocks =
                    ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
    int16_t logicalBlock;

    assert( physicalBlock < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped)) ) ; /* "MappedNandFlash_PhysicalToLogical: physicalBlock out-of-range\n\r" */

    /* Search the mapping for the desired physical block*/
    for ( logicalBlock=0; logicalBlock < numBlocks; logicalBlock++ )
    {
        if ( mapped->logicalMapping[logicalBlock] == physicalBlock )
        {
            return logicalBlock ;
        }
    }

    return -1;
}

/**
 * \brief  Saves the logical mapping on a FREE, unmapped physical block. Allocates the
 * new block, releases the previous one (if any) and save the mapping.
 *
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param physicalBlock  Physical block number.
 * \return  0 if successful; otherwise, returns NandCommon_ERROR_WRONGSTATUS
 * if the block is not LIVE, or a NandCommon_ERROR code.
 */
uint8_t MappedNandFlash_SaveLogicalMapping(
    struct MappedNandFlash *mapped,
    uint16_t physicalBlock)
{
    uint8_t error;
    uint8_t data[NandCommon_MAXPAGEDATASIZE];
    uint16_t pageDataSize =
                    NandFlashModel_GetPageDataSize(MODEL(mapped));
    /*uint16_t numBlocks =
                    ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));*/
    uint32_t i;
    uint32_t remainingSize;
    uint8_t *currentBuffer;
    uint16_t currentPage;
    uint32_t writeSize;
    int16_t previousPhysicalBlock;

    TRACE_INFO("MappedNandFlash_SaveLogicalMapping(B#%d)\n\r", physicalBlock);

    /* If mapping has not been modified, do nothing*/
    if (!mapped->mappingModified) {

        return 0;
    }

    /* Allocate new block*/
    error = ManagedNandFlash_AllocateBlock(MANAGED(mapped), physicalBlock);
    if (error) {

        return error;
    }

    /* Save mapping*/
    previousPhysicalBlock = mapped->logicalMappingBlock;
    mapped->logicalMappingBlock = physicalBlock;

    /* Save actual mapping in pages #1-#XXX*/
    currentBuffer = (uint8_t *) mapped->logicalMapping;
    remainingSize = sizeof(mapped->logicalMapping);
    currentPage = 1;
    while (remainingSize > 0) {

        writeSize = min(remainingSize, pageDataSize);
        memset(data, 0xFF, pageDataSize);
        memcpy(data, currentBuffer, writeSize);
        error = ManagedNandFlash_WritePage(MANAGED(mapped),
                                           physicalBlock,
                                           currentPage,
                                           data,
                                           0);
        if (error) {

            TRACE_ERROR(
             "MappedNandFlash_SaveLogicalMapping: Failed to write mapping\n\r");
            return error;
        }

        currentBuffer += writeSize;
        remainingSize -= writeSize;
        currentPage++;
    }

    /* Mark page #0 of block with a distinguishible pattern, so the mapping can
       be retrieved at startup*/
    for (i=0; i < pageDataSize; i++) {

        data[i] = PATTERN(i);
    }
    error = ManagedNandFlash_WritePage(MANAGED(mapped),
                                       physicalBlock, 0,
                                       data, 0);
    if (error) {

        TRACE_ERROR(
            "MappedNandFlash_SaveLogicalMapping: Failed to write pattern\n\r");
        return error;
    }

    /* Mapping is not modified anymore*/
    mapped->mappingModified = 0;

    /* Release previous block (if any)*/
    if (previousPhysicalBlock != -1) {

        TRACE_DEBUG("Previous physical block was #%d\n\r",
                    previousPhysicalBlock);
        error = ManagedNandFlash_ReleaseBlock(MANAGED(mapped),
                                              previousPhysicalBlock);
        if (error) {

            return error;
        }
    }

    TRACE_INFO("Mapping saved on block #%d\n\r", physicalBlock);

    return 0;
}

/**
 * \brief  Erase all blocks in the mapped area of nand flash.
 *
 * \param mapped  Pointer to a MappedNandFlash instance.
 * \param level   Erase level.
 * \return 0.
 */
uint8_t MappedNandFlash_EraseAll(
    struct MappedNandFlash *mapped,
    uint8_t level)
{
    uint32_t block;
    ManagedNandFlash_EraseAll(MANAGED(mapped), level);
    /* Reset to no block mapped*/
    if (level > NandEraseDIRTY) {
        mapped->logicalMappingBlock = -1;
        mapped->mappingModified = 0;
        for (block=0;
             block < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
             block++) {
            mapped->logicalMapping[block] = -1;
        }
    }
    return 0;
}
