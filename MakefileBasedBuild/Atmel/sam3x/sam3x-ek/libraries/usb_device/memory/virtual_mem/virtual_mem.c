/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Management of the virtual memory.
 *
 * This file manages the virtual memory.
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
#include "conf_virtual_mem.h"


#if VIRTUAL_MEM == ENABLE

#include "virtual_mem.h"
#include <string.h>


#ifndef VMEM_NB_SECTOR
#  error Define VMEM_NB_SECTOR in conf_virtual_mem.h file
#endif

//_____ M A C R O S ________________________________________________________

#define VIRTUAL_MEM_TEST_CHANGE_STATE     DISABLED//ENABLED


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________


//_____ D E F I N I T I O N S ______________________________________________

#if (defined __GNUC__) && (defined __AVR32__)
  __attribute__((__aligned__(4)))
#elif (defined __ICCAVR32__)
  #pragma data_alignment = 4
#endif
static uint8_t vmem_data[VMEM_NB_SECTOR * VMEM_SECTOR_SIZE];

//static Bool cram_init = FALSE;
#if (ACCESS_USB == ENABLED || ACCESS_MEM_TO_RAM == ENABLED) && VIRTUAL_MEM_TEST_CHANGE_STATE == ENABLED
static volatile Bool s_b_data_modify = FALSE;
#endif


//_____ D E C L A R A T I O N S ____________________________________________

//! This function tests memory state, and starts memory initialization
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status virtual_test_unit_ready(void)
{
  return CTRL_GOOD;
}


//! This function returns the address of the last valid sector
//! @param u32_nb_sector  Pointer to number of sectors (sector = 512 bytes)
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status virtual_read_capacity(U32 *u32_nb_sector)
{
   if (VMEM_NB_SECTOR<8)
      *u32_nb_sector = 8-1;
   else
      *u32_nb_sector = VMEM_NB_SECTOR- 1;
   return CTRL_GOOD;
}


//! This function returns the write-protected mode
//! Only used by memory removal with a HARDWARE-SPECIFIC write-protected detection
//! @warning The customer must unplug the memory to change this write-protected mode.
//! @return TRUE if the memory is protected
Bool virtual_wr_protect(void)
{
  return FALSE;
}


//! This function informs about the memory type
//! @return TRUE if the memory is removable
Bool virtual_removal(void)
{
  return FALSE;
}


//------------ SPECIFIC FUNCTIONS FOR TRANSFER BY USB --------------------------

#if ACCESS_USB == ENABLED

#include "udi_msc.h"

//! This function transfers the memory data (programmed in sbc_read_10) directly to the USB interface
//! sector = 512 bytes
//! @param addr         Sector address to start read
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status virtual_usb_read_10(U32 addr, U16 nb_sector)
{
  U8 *ptr_cram;

  if ((addr > VMEM_NB_SECTOR)
  ||  (addr + nb_sector > VMEM_NB_SECTOR))
     return CTRL_FAIL;

  while (nb_sector--)
  {
    // If overflow (possible with size virtual mem < 8 sectors) then read the last sector
    addr = min(addr, VMEM_NB_SECTOR - 1);
    ptr_cram = &vmem_data[addr++ * VMEM_SECTOR_SIZE];
    if (!udi_msc_trans_block( true, ptr_cram, VMEM_SECTOR_SIZE, NULL))
       return CTRL_FAIL; // transfert aborted
  }

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
Ctrl_status virtual_usb_write_10(U32 addr, U16 nb_sector)
{
  U8 *ptr_cram;

  if ((addr > VMEM_NB_SECTOR)
  ||  (addr + nb_sector > VMEM_NB_SECTOR))
     return CTRL_FAIL;

  while (nb_sector--)
  {
    // If overflow (possible with size virtual mem < 8 sectors) then read the last sector
    addr = min(addr, VMEM_NB_SECTOR - 1);
    ptr_cram = &vmem_data[addr++ * VMEM_SECTOR_SIZE];
    if (!udi_msc_trans_block( false, ptr_cram, VMEM_SECTOR_SIZE, NULL))
       return CTRL_FAIL; // transfert aborted
  }

  return CTRL_GOOD;
}

#endif  // ACCESS_USB == ENABLED


//------------ SPECIFIC FUNCTIONS FOR TRANSFER BY RAM --------------------------

#if ACCESS_MEM_TO_RAM == ENABLED

#include <string.h>

//! This function tranfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param addr         Sector address to start read
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status virtual_mem_2_ram(U32 addr, void *ram)
{
  if (addr + 1 > Max(VMEM_NB_SECTOR, 8)) return CTRL_FAIL;

  // If overflow (possible with size virtual mem < 8 sectors) then read the last sector
  addr = min(addr, VMEM_NB_SECTOR - 1);

  memcpy(ram, &vmem_data[addr * VMEM_SECTOR_SIZE], VMEM_SECTOR_SIZE);

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
Ctrl_status virtual_ram_2_mem(U32 addr, const void *ram)
{
  if (addr + 1 > VMEM_NB_SECTOR) return CTRL_FAIL;

#if VIRTUAL_MEM_TEST_CHANGE_STATE == ENABLED
  if (addr + 1 > FILE_SECTOR && addr <= FILE_SECTOR)
    s_b_data_modify = TRUE;
#endif

  memcpy(&vmem_data[addr * VMEM_SECTOR_SIZE], ram, VMEM_SECTOR_SIZE);

  return CTRL_GOOD;
}

#endif  // ACCESS_MEM_TO_RAM == ENABLED


#endif  // VIRTUAL_MEM == ENABLE
