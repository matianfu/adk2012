/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief CTRL_ACCESS interface for SD/MMC card.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with an MCI module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

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


#if (SD_MMC_MCI_0_MEM == ENABLE) || (SD_MMC_MCI_1_MEM == ENABLE)

#include "conf_sd_mmc_mci.h"
//#include "sd_mmc_mci.h"
#include "sd_mmc_mci_mem.h"

#define MCI_NR_SLOTS    2
#define MCI_LAST_SLOTS  0
#define SD_MMC_SECTOR_SIZE 512

//_____ D E F I N I T I O N S ______________________________________________

#define sd_mmc_mci_mem_check(slot) false
#define is_sd_mmc_mci_card_protected(slot) false
#define sd_mmc_mci_dma_read_open(slot, addr, NULL, nb_sector) true
#define sd_mmc_mci_read_multiple_sector(slot, nb_sector) true
#define sd_mmc_mci_read_close(slot) true
#define sd_mmc_mci_dma_write_open(slot, addr, NULL, nb_sector) true
#define sd_mmc_mci_write_multiple_sector(slot, nb_sector) true
#define sd_mmc_mci_write_close(slot) true

#define Sd_mmc_mci_access_signal_on()
#define Sd_mmc_mci_access_signal_off()

//! Initialization sequence status per Slot.
/*extern*/  Bool sd_mmc_mci_init_done[MCI_NR_SLOTS];
//! SD/MMC Card Size per Slot.
/*extern*/  U32  g_u32_card_size[MCI_NR_SLOTS];
//! SD/MMC Card Presence status per Slot.
static  U8   sd_mmc_mci_presence_status[MCI_NR_SLOTS] = {SD_MMC_INSERTED, SD_MMC_INSERTED};

/*! \name Control Interface
 */
//! @{


Ctrl_status sd_mmc_mci_test_unit_ready(U8 slot)
{
  if (slot > MCI_LAST_SLOTS) return CTRL_FAIL;

  Sd_mmc_mci_access_signal_on();
  switch (sd_mmc_mci_presence_status[slot])
  {
    case SD_MMC_REMOVED:
      sd_mmc_mci_init_done[slot] = FALSE;
      if (OK == sd_mmc_mci_mem_check(slot))
      {
        sd_mmc_mci_presence_status[slot] = SD_MMC_INSERTED;
        Sd_mmc_mci_access_signal_off();
        return CTRL_BUSY;
      }
      Sd_mmc_mci_access_signal_off();
      return CTRL_NO_PRESENT;

    case SD_MMC_INSERTED:
      if (OK != sd_mmc_mci_mem_check(slot))
      {
        sd_mmc_mci_presence_status[slot] = SD_MMC_REMOVING;
        sd_mmc_mci_init_done[slot] = FALSE;
        Sd_mmc_mci_access_signal_off();
        return CTRL_BUSY;
      }
      Sd_mmc_mci_access_signal_off();
      return CTRL_GOOD;

    case SD_MMC_REMOVING:
      sd_mmc_mci_presence_status[slot] = SD_MMC_REMOVED;
      Sd_mmc_mci_access_signal_off();
      return CTRL_NO_PRESENT;

    default:
      sd_mmc_mci_presence_status[slot] = SD_MMC_REMOVED;
      Sd_mmc_mci_access_signal_off();
      return CTRL_BUSY;
  }
}


Ctrl_status sd_mmc_mci_test_unit_ready_0(void)
{
   return sd_mmc_mci_test_unit_ready(0);
}


Ctrl_status sd_mmc_mci_test_unit_ready_1(void)
{
   return sd_mmc_mci_test_unit_ready(1);
}


Ctrl_status sd_mmc_mci_read_capacity(U8 slot, U32 *u32_nb_sector)
{
   Sd_mmc_mci_access_signal_on();

   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }
   *u32_nb_sector = g_u32_card_size[slot]-1;
   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_read_capacity_0(U32 *u32_nb_sector)
{
  return sd_mmc_mci_read_capacity(0, u32_nb_sector);
}


Ctrl_status sd_mmc_mci_read_capacity_1(U32 *u32_nb_sector)
{
  return sd_mmc_mci_read_capacity(1, u32_nb_sector);
}


Bool sd_mmc_mci_wr_protect(U8 slot)
{
  return is_sd_mmc_mci_card_protected(slot);
}


Bool sd_mmc_mci_wr_protect_0(void)
{
  return sd_mmc_mci_wr_protect(0);
}


Bool sd_mmc_mci_wr_protect_1(void)
{
  return sd_mmc_mci_wr_protect(1);
}


Bool sd_mmc_mci_removal(U8 slot)
{
  return FALSE;
}


Bool sd_mmc_mci_removal_0(void)
{
  return sd_mmc_mci_removal(0);
}


Bool sd_mmc_mci_removal_1(void)
{
  return sd_mmc_mci_removal(1);
}

//! @}

#if ACCESS_USB == ENABLED

#include "conf_usb.h"
#ifdef USB_DEVICE_VENDOR_ID
  // USB Device Stack V2
#include "udi_msc.h"
#else
  // USB Device Stack V1
#include "usb_drv.h"
#include "scsi_decoder.h"
#endif

/*! \name MEM <-> USB Interface
 */
//! @{


Ctrl_status sd_mmc_mci_usb_read_10(U8 slot, U32 addr, U16 nb_sector)
{
   Sd_mmc_mci_access_signal_on();

   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_read_open(slot, addr, NULL, nb_sector) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_multiple_sector(slot, nb_sector) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_usb_read_10_0(U32 addr, U16 nb_sector)
{
  return sd_mmc_mci_usb_read_10(0, addr, nb_sector);
}


Ctrl_status sd_mmc_mci_usb_read_10_1(U32 addr, U16 nb_sector)
{
  return sd_mmc_mci_usb_read_10(1, addr, nb_sector);
}

#if 0
void sd_mmc_mci_read_multiple_sector_callback(const void *psector)
{
#ifdef USB_DEVICE_VENDOR_ID
   // USB Device Stack V2
   udi_msc_trans_block( true, (uint8_t*)psector, SD_MMC_SECTOR_SIZE, NULL);
#else
  // USB Device Stack V1
  U16 data_to_transfer = SD_MMC_SECTOR_SIZE;

  // Transfer read sector to the USB interface.
  while (data_to_transfer)
  {
    while (!Is_usb_in_ready(g_scsi_ep_ms_in))
    {
      if(!Is_usb_endpoint_enabled(g_scsi_ep_ms_in))
         return; // USB Reset
    }         

    Usb_reset_endpoint_fifo_access(g_scsi_ep_ms_in);
    data_to_transfer = usb_write_ep_txpacket(g_scsi_ep_ms_in, psector,
                                             data_to_transfer, &psector);
    Usb_ack_in_ready_send(g_scsi_ep_ms_in);
  }
#endif
}
#endif

Ctrl_status sd_mmc_mci_usb_write_10(U8 slot,U32 addr, U16 nb_sector)
{
   Sd_mmc_mci_access_signal_on();

   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_write_open(slot, addr, NULL, nb_sector) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_multiple_sector(slot, nb_sector) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;

}


Ctrl_status sd_mmc_mci_usb_write_10_0(U32 addr, U16 nb_sector)
{
  return sd_mmc_mci_usb_write_10(0, addr, nb_sector);
}


Ctrl_status sd_mmc_mci_usb_write_10_1(U32 addr, U16 nb_sector)
{
  return sd_mmc_mci_usb_write_10(1, addr, nb_sector);
}

#if 0
void sd_mmc_mci_write_multiple_sector_callback(void *psector)
{
#ifdef USB_DEVICE_VENDOR_ID
   // USB Device Stack V2
   udi_msc_trans_block( false, (uint8_t*)psector, SD_MMC_SECTOR_SIZE, NULL);
#else
  // USB Device Stack V1
  U16 data_to_transfer = SD_MMC_SECTOR_SIZE;

  // Transfer sector to write from the USB interface.
  while (data_to_transfer)
  {
    while (!Is_usb_out_received(g_scsi_ep_ms_out))
    {
      if(!Is_usb_endpoint_enabled(g_scsi_ep_ms_out))
         return; // USB Reset
    }         

    Usb_reset_endpoint_fifo_access(g_scsi_ep_ms_out);
    data_to_transfer = usb_read_ep_rxpacket(g_scsi_ep_ms_out, psector,
                                            data_to_transfer, &psector);
    Usb_ack_out_received_free(g_scsi_ep_ms_out);
  }
#endif
}
#endif

//! @}

#endif  // ACCESS_USB == ENABLED


#if ACCESS_MEM_TO_RAM == ENABLED

/*! \name MEM <-> RAM Interface
 */
//! @{


Ctrl_status sd_mmc_mci_mem_2_ram(U8 slot, U32 addr, void *ram)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_read_open(slot, addr, 1) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_sector_2_ram(slot, ram) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;

}


Ctrl_status sd_mmc_mci_dma_mem_2_ram(U8 slot, U32 addr, void *ram)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_read_open(slot, addr, ram, 1) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_dma_read_sector_2_ram(slot,ram) )
     return CTRL_FAIL;

   if(! sd_mmc_mci_read_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;

}


Ctrl_status sd_mmc_mci_mem_2_ram_0(U32 addr, void *ram)
{
  return sd_mmc_mci_mem_2_ram(0, addr, ram);
}


Ctrl_status sd_mmc_mci_mem_2_ram_1(U32 addr, void *ram)
{
  return sd_mmc_mci_mem_2_ram(1, addr, ram);
}


Ctrl_status sd_mmc_mci_multiple_mem_2_ram(U8 slot, U32 addr, void *ram, U32 nb_sectors)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_read_open(slot, addr, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_multiple_sector_2_ram(slot, ram, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_dma_multiple_mem_2_ram(U8 slot, U32 addr, void *ram, U32 nb_sectors)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_read_open(slot, addr, ram, nb_sectors ) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_dma_read_multiple_sector_2_ram(slot, ram, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_multiple_mem_2_ram_0(U32 addr, void *ram, U32 nb_sectors)
{
  return sd_mmc_mci_multiple_mem_2_ram(0, addr, ram, nb_sectors);
}


Ctrl_status sd_mmc_mci_multiple_mem_2_ram_1(U32 addr, void *ram, U32 nb_sectors)
{
  return sd_mmc_mci_multiple_mem_2_ram(1, addr, ram, nb_sectors);
}


Ctrl_status sd_mmc_mci_ram_2_mem(U8 slot, U32 addr, const void *ram)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_write_open(slot, addr, 1) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_sector_from_ram(slot,ram) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_dma_ram_2_mem(U8 slot, U32 addr, const void *ram)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_write_open(slot, addr, ram, 1) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_dma_write_sector_from_ram(slot, ram) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_ram_2_mem_0(U32 addr, const void *ram)
{
  return sd_mmc_mci_ram_2_mem(0, addr, ram);
}


Ctrl_status sd_mmc_mci_ram_2_mem_1(U32 addr, const void *ram)
{
  return sd_mmc_mci_ram_2_mem(1, addr, ram);
}


Ctrl_status sd_mmc_mci_multiple_ram_2_mem(U8 slot, U32 addr, const void *ram, U32 nb_sectors)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_write_open(slot, addr, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_multiple_sector_from_ram(slot, ram, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_dma_multiple_ram_2_mem(U8 slot, U32 addr, const void *ram, U32 nb_sectors)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_write_open(slot, addr, ram, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_dma_write_multiple_sector_from_ram(slot, ram, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_multiple_ram_2_mem_0(U32 addr, const void *ram, U32 nb_sectors)
{
  return sd_mmc_mci_multiple_ram_2_mem(0, addr, ram, nb_sectors);
}


Ctrl_status sd_mmc_mci_multiple_ram_2_mem_1(U32 addr, const void *ram, U32 nb_sectors)
{
  return sd_mmc_mci_multiple_ram_2_mem(1, addr, ram, nb_sectors);
}


//! @}

#endif  // ACCESS_MEM_TO_RAM == ENABLED


#endif  // SD_MMC_MCI_0_MEM == ENABLE || SD_MMC_MCI_1_MEM == ENABLE
