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
 *  Include Defines & macros for the mapped nand flash layer.
 */

#ifndef MAPPEDNANDFLASH_H
#define MAPPEDNANDFLASH_H

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "ManagedNandFlash.h"

/*----------------------------------------------------------------------------
 *        Type
 *----------------------------------------------------------------------------*/

struct MappedNandFlash {

    struct ManagedNandFlash managed;
    int16_t logicalMapping[NandCommon_MAXNUMBLOCKS];
    int16_t logicalMappingBlock;
    uint8_t mappingModified;
    uint8_t reserved;
};

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

extern uint8_t MappedNandFlash_Initialize(
    struct MappedNandFlash *mapped,
    const struct NandFlashModel *model,
    uint32_t commandAddress,
    uint32_t addressAddress,
    uint32_t dataAddress,
    const Pin pinChipEnable,
    const Pin pinReadyBusy,
    uint16_t baseBlock,
    uint16_t sizeInBlocks);

extern uint8_t MappedNandFlash_ReadPage(
    const struct MappedNandFlash *mapped,
    uint16_t block,
    uint16_t page,
    void *data,
    void *spare);

extern uint8_t MappedNandFlash_WritePage(
    const struct MappedNandFlash *mapped,
    uint16_t block,
    uint16_t page,
    void *data,
    void *spare);

extern uint8_t MappedNandFlash_Map(
    struct MappedNandFlash *mapped,
    uint16_t logicalBlock,
    uint16_t physicalBlock);

extern uint8_t MappedNandFlash_Unmap(
    struct MappedNandFlash *mapped,
    uint16_t logicalBlock);

extern int16_t MappedNandFlash_LogicalToPhysical(
    const struct MappedNandFlash *mapped,
    uint16_t logicalBlock);

extern int16_t MappedNandFlash_PhysicalToLogical(
    const struct MappedNandFlash *mapped,
    uint16_t physicalBlock);

extern uint8_t MappedNandFlash_SaveLogicalMapping(
    struct MappedNandFlash *mapped,
    uint16_t physicalBlock);

extern uint8_t MappedNandFlash_EraseAll(
    struct MappedNandFlash *mapped,
    uint8_t level);

#endif /* #ifndef MAPPEDNANDFLASH_H*/

