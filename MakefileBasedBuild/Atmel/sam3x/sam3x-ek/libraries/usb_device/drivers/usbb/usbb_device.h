/**
 * \file
 *
 * \brief USBB Device Driver header file.
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef _USBB_DEVICE_H_
#define _USBB_DEVICE_H_

#include "compiler.h"
//#include "preprocessor.h"

//! \ingroup usb_device_group
//! \defgroup udd_group USB Device Driver (UDD)
//! USBB low-level driver for USB device mode
//!
//! @warning Bit-masks are used instead of bit-fields because PB registers
//! require 32-bit write accesses while AVR32-GCC 4.0.2 builds 8-bit
//! accesses even when volatile unsigned int bit-fields are specified.
//! @{

//! @name USBB Device IP properties
//! These macros give access to IP properties
//! @{
  //! Get maximal number of endpoints
#define  UDD_get_endpoint_max_nbr()             (6) // ep without dma not implemented
//#define  UDD_get_endpoint_max_nbr()             (9)
  //! Get maximal number of banks of endpoints
#define  UDD_get_endpoint_bank_max_nbr(ep)      ((ep==0)?1:((ep<=2)?3:2))
  //! Get DMA support of endpoints (always)
//#define  Is_udd_endpoint_dma_supported(ep)      (false) // ep with dma not implemented
//#define  Is_udd_endpoint_dma_supported(ep)      (true) // ep without dma not implemented
#define  Is_udd_endpoint_dma_supported(ep)      ((((ep)>=1)&&((ep)<=6))?true:false)
  //! Get High Band Width support of endpoints
#define  Is_udd_endpoint_high_bw_supported(ep)  (((ep)>=2)?true:false)
//! @}

//! @name USBB Device speeds management
//! @{
  //! Enable/disable device low-speed mode
#define  udd_low_speed_enable()              (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_LS))
#define  udd_low_speed_disable()             (Clr_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_LS))
  //! Test if device low-speed mode is forced
#define  Is_udd_low_speed_enable()           (Tst_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_LS))

#ifdef UOTGHS_DEVCTRL_SPDCONF_HIGH_SPEED
  //! Enable high speed mode
#  define   udd_high_speed_enable()          (Wr_bitfield(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_SPDCONF_Msk, 0))
  //! Disable high speed mode
#  define   udd_high_speed_disable()         (Wr_bitfield(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_SPDCONF_Msk, 3))
  //! Test if controller is in full speed mode
#  define   Is_udd_full_speed_mode()         (Rd_bitfield(UOTGHS->UOTGHS_SR, UOTGHS_SR_SPEED_Msk) == UOTGHS_SR_SPEED_FULL_SPEED)
#else
#  define   udd_high_speed_enable()          do { } while (0)
#  define   udd_high_speed_disable()         do { } while (0)
#  define   Is_udd_full_speed_mode()         TRUE
#endif
//! @}

//! @name USBB Device HS test mode management
//! @{
#ifdef UOTGHS_DEVCTRL_SPDCONF_HIGH_SPEED
  //! Enable high speed test mode
#  define   udd_enable_hs_test_mode()        (Wr_bitfield(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_SPDCONF_Msk, 2))
#  define   udd_enable_hs_test_mode_j()      (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_TSTJ))
#  define   udd_enable_hs_test_mode_k()      (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_TSTK))
#  define   udd_enable_hs_test_mode_packet() (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_TSTPCKT))
#endif
//! @}

//! @name USBB Device vbus management
//! @{
#define  udd_enable_vbus_interrupt()         (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_VBUSTE))
#define  udd_disable_vbus_interrupt()        (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_VBUSTE))
#define  Is_udd_vbus_interrupt_enabled()     (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_VBUSTE))
#define  Is_udd_vbus_high()                  (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_VBUS))
#define  Is_udd_vbus_low()                   (!Is_udd_vbus_high())
#define  udd_ack_vbus_transition()           (UOTGHS->UOTGHS_SCR = UOTGHS_SCR_VBUSTIC)
#define  udd_raise_vbus_transition()         (UOTGHS->UOTGHS_SFR = UOTGHS_SFR_VBUSTIS)
#define  Is_udd_vbus_transition()            (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_VBUSTI))
//! @}


//! @name USBB device attach control
//! These macros manage the USBB Device attach.
//! @{
  //! detaches from USB bus
#define  udd_detach_device()                 (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_DETACH))
  //! attaches to USB bus
#define  udd_attach_device()                 (Clr_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_DETACH))
  //! test if the device is detached
#define  Is_udd_detached()                   (Tst_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_DETACH))
//! @}


//! @name USBB device bus events control
//! These macros manage the USBB Device bus events.
//! @{

//! Initiates a remote wake-up event
//! @{
#define  udd_initiate_remote_wake_up()       (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_RMWKUP))
#define  Is_udd_pending_remote_wake_up()     (Tst_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_RMWKUP))
//! @}

//! Manage upstream resume event (=remote wakeup)
//! The USB driver sends a resume signal called "Upstream Resume"
//! @{
#define  udd_enable_remote_wake_up_interrupt()     (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_UPRSMES)
#define  udd_disable_remote_wake_up_interrupt()    (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_UPRSMEC)
#define  Is_udd_remote_wake_up_interrupt_enabled() (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_UPRSME))
#define  udd_ack_remote_wake_up_start()            (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_UPRSMC)
#define  udd_raise_remote_wake_up_start()          (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_UPRSMS)
#define  Is_udd_remote_wake_up_start()             (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_UPRSM))
//! @}

//! Manage end of resume event (=remote wakeup)
//! The USB controller detects a valid "End of Resume" signal initiated by the host
//! @{
#define  udd_enable_resume_interrupt()             (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_EORSMES)
#define  udd_disable_resume_interrupt()            (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_EORSMEC)
#define  Is_udd_resume_interrupt_enabled()         (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_EORSME))
#define  udd_ack_resume()                          (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_EORSMC)
#define  udd_raise_resume()                        (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_EORSMS)
#define  Is_udd_resume()                           (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_EORSM))
//! @}

//! Manage wake-up event (=usb line activity)
//! The USB controller is reactivated by a filtered non-idle signal from the lines
//! @{
#define  udd_enable_wake_up_interrupt()            (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_WAKEUPES)
#define  udd_disable_wake_up_interrupt()           (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_WAKEUPEC)
#define  Is_udd_wake_up_interrupt_enabled()        (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_WAKEUPE))
#define  udd_ack_wake_up()                         (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_WAKEUPC)
#define  udd_raise_wake_up()                       (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_WAKEUPS)
#define  Is_udd_wake_up()                          (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_WAKEUP))
//! @}

//! Manage reset event
//! Set when a USB "End of Reset" has been detected
//! @{
#define  udd_enable_reset_interrupt()              (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_EORSTES)
#define  udd_disable_reset_interrupt()             (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_EORSTEC)
#define  Is_udd_reset_interrupt_enabled()          (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_EORSTE))
#define  udd_ack_reset()                           (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_EORSTC)
#define  udd_raise_reset()                         (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_EORSTS)
#define  Is_udd_reset()                            (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_EORST))
//! @}

//! Manage sart of frame event
//! @{
#define  udd_enable_sof_interrupt()                (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_SOFES)
#define  udd_disable_sof_interrupt()               (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_SOFEC)
#define  Is_udd_sof_interrupt_enabled()            (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_SOFE))
#define  udd_ack_sof()                             (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_SOFC)
#define  udd_raise_sof()                           (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_SOFS)
#define  Is_udd_sof()                              (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_SOF))
#define  udd_frame_number()                        (Rd_bitfield(UOTGHS->UOTGHS_DEVFNUM, UOTGHS_DEVFNUM_FNUM_Msk))
#define  Is_udd_frame_number_crc_error()           (Tst_bits(UOTGHS->UOTGHS_DEVFNUM, UOTGHS_DEVFNUM_FNCERR))
//! @}

//! Manage suspend event
//! @{
#define  udd_enable_suspend_interrupt()            (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_SUSPES)
#define  udd_disable_suspend_interrupt()           (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_SUSPEC)
#define  Is_udd_suspend_interrupt_enabled()        (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_SUSPE))
#define  udd_ack_suspend()                         (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_SUSPC)
#define  udd_raise_suspend()                       (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_SUSPS)
#define  Is_udd_suspend()                          (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_SUSP))
//! @}

//! @}

//! @name USBB device address control
//! These macros manage the USBB Device address.
//! @{
  //! enables USB device address
#define  udd_enable_address()                      (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_ADDEN))
  //! disables USB device address
#define  udd_disable_address()                     (Clr_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_ADDEN))
#define  Is_udd_address_enabled()                  (Tst_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_ADDEN))
  //! configures the USB device address
#define  udd_configure_address(addr)               (Wr_bitfield(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_UADD_Msk, addr))
  //! gets the currently configured USB device address
#define  udd_get_configured_address()              (Rd_bitfield(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_UADD_Msk))
//! @}


//! @name USBB Device endpoint drivers
//! These macros manage the common features of the endpoints.
//! @{

//! Generic macro for USBB registers that can be arrayed
//! @{
#define USBB_ARRAY(reg,index)              ((&(UOTGHS->reg))[(index)])
//! @}

//! @name USBB Device endpoint configguration
//! @{
  //! enables the selected endpoint
#define  udd_enable_endpoint(ep)                   (Set_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPEN0 << (ep)))
  //! disables the selected endpoint
#define  udd_disable_endpoint(ep)                  (Clr_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPEN0 << (ep)))
  //! tests if the selected endpoint is enabled
#define  Is_udd_endpoint_enabled(ep)               (Tst_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPEN0 << (ep)))
  //! resets the selected endpoint
#define  udd_reset_endpoint(ep)                    (Set_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPRST0 << (ep)),\
                                                   Clr_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPRST0 << (ep)))
  //! tests if the selected endpoint is being reset
#define  Is_udd_resetting_endpoint(ep)             (Tst_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPRST0 << (ep)))

  //! configures the selected endpoint type
#define  udd_configure_endpoint_type(ep, type)     (Wr_bitfield(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_EPTYPE_Msk, type))
  //! gets the configured selected endpoint type
#define  udd_get_endpoint_type(ep)                 (Rd_bitfield(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_EPTYPE_Msk))
  //! enables the bank autoswitch for the selected endpoint
#define  udd_enable_endpoint_bank_autoswitch(ep)   (Set_bits(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_AUTOSW))
  //! disables the bank autoswitch for the selected endpoint
#define  udd_disable_endpoint_bank_autoswitch(ep)   (Clr_bits(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_AUTOSW))
#define  Is_udd_endpoint_bank_autoswitch_enabled(ep) (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_AUTOSW))
  //! configures the selected endpoint direction
#define  udd_configure_endpoint_direction(ep, dir) (Wr_bitfield(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_EPDIR, dir))
  //! gets the configured selected endpoint direction
#define  udd_get_endpoint_direction(ep)            (Rd_bitfield(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_EPDIR))
#define  Is_udd_endpoint_in(ep)                    (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_EPDIR))
  //! Bounds given integer size to allowed range and rounds it up to the nearest
  //! available greater size, then applies register format of USBB controller
  //! for endpoint size bit-field.
#define  udd_format_endpoint_size(size)            (32 - clz(((U32)min(max(size, 8), 1024) << 1) - 1) - 1 - 3)
  //! configures the selected endpoint size
#define  udd_configure_endpoint_size(ep, size)     (Wr_bitfield(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_EPSIZE_Msk, udd_format_endpoint_size(size)))
  //! gets the configured selected endpoint size
#define  udd_get_endpoint_size(ep)                 (8 << Rd_bitfield(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_EPSIZE_Msk))
  //! configures the selected endpoint number of banks
#define  udd_configure_endpoint_bank(ep, bank)     (Wr_bitfield(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_EPBK_Msk, bank))
  //! gets the configured selected endpoint number of banks
#define  udd_get_endpoint_bank(ep)                 (Rd_bitfield(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_EPBK_Msk)+1)
  //! allocates the configuration selected endpoint in DPRAM memory
#define  udd_allocate_memory(ep)                   (Set_bits(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_ALLOC))
  //! un-allocates the configuration selected endpoint in DPRAM memory
#define  udd_unallocate_memory(ep)                 (Clr_bits(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_ALLOC))
#define  Is_udd_memory_allocated(ep)               (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_ALLOC))

  //! configures selected endpoint in one step
#define  udd_configure_endpoint(ep, type, dir, size, bank) \
(\
   Wr_bits(USBB_ARRAY(UOTGHS_DEVEPTCFG[0],ep), UOTGHS_DEVEPTCFG_EPTYPE_Msk |\
                                  UOTGHS_DEVEPTCFG_EPDIR  |\
                                  UOTGHS_DEVEPTCFG_EPSIZE_Msk |\
                                  UOTGHS_DEVEPTCFG_EPBK_Msk ,   \
            (((U32)(type) << UOTGHS_DEVEPTCFG_EPTYPE_Pos) & UOTGHS_DEVEPTCFG_EPTYPE_Msk) |\
            (((U32)(dir ) << UOTGHS_DEVEPTCFG_EPDIR_Pos ) & UOTGHS_DEVEPTCFG_EPDIR) |\
            ( (U32)udd_format_endpoint_size(size) << UOTGHS_DEVEPTCFG_EPSIZE_Pos) |\
            (((U32)(bank) << UOTGHS_DEVEPTCFG_EPBK_Pos) & UOTGHS_DEVEPTCFG_EPBK_Msk))\
)
#define UOTGHS_DEVEPTCFG_EPDIR_Pos 8
  //! tests if current endpoint is configured
#define  Is_udd_endpoint_configured(ep)            (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_CFGOK))
  //! returns the control direction
#define  udd_control_direction()                   (Rd_bitfield(USBB_ARRAY(UOTGHS_DEVEPTISR[0], EP_CONTROL), UOTGHS_DEVEPTISR_CTRLDIR))

  //! resets the data toggle sequence
#define  udd_reset_data_toggle(ep)                 (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_RSTDTS)
  //! tests if the data toggle sequence is being reset
#define  Is_udd_data_toggle_reset(ep)              (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_RSTDT))
  //! returns data toggle
#define  udd_data_toggle(ep)                       (Rd_bitfield(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_DTSEQ_Msk))
//! @}


//! @name USBB Device control endpoint
//! These macros contorl the endpoints.
//! @{

//! @name USBB Device control endpoint interrupts
//! These macros control the endpoints interrupts.
//! @{
  //! enables the selected endpoint interrupt
#define  udd_enable_endpoint_interrupt(ep)         (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_PEP_0 << (ep))
  //! disables the selected endpoint interrupt
#define  udd_disable_endpoint_interrupt(ep)        (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_PEP_0 << (ep))
  //! tests if the selected endpoint interrupt is enabled
#define  Is_udd_endpoint_interrupt_enabled(ep)     (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_PEP_0 << (ep)))
  //! tests if an interrupt is triggered by the selected endpoint
#define  Is_udd_endpoint_interrupt(ep)             (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_PEP_0 << (ep)))
  //! returns the lowest endpoint number generating an endpoint interrupt or MAX_PEP_NB if none
#define  udd_get_interrupt_endpoint_number()       (ctz(((UOTGHS->UOTGHS_DEVISR >> UOTGHS_DEVISR_PEP_Pos) &\
                                                   (UOTGHS->UOTGHS_DEVIMR >> UOTGHS_DEVIMR_PEP_Pos)) |\
                                                   (1 << MAX_PEP_NB)))
#define UOTGHS_DEVISR_PEP_Pos   12
#define UOTGHS_DEVIMR_PEP_Pos   12
//! @}

//! @name USBB Device control endpoint errors
//! These macros control the endpoint errors.
//! @{
  //! enables the STALL handshake
#define  udd_enable_stall_handshake(ep)            (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_STALLRQS)
  //! disables the STALL handshake
#define  udd_disable_stall_handshake(ep)           (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_STALLRQC)
  //! tests if STALL handshake request is running
#define  Is_udd_endpoint_stall_requested(ep)       (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_STALLRQ))
  //! tests if STALL sent
#define  Is_udd_stall(ep)                          (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_STALLEDI))
  //! acks STALL sent
#define  udd_ack_stall(ep)                         (USBB_ARRAY(UOTGHS_DEVEPTICR[0],ep) = UOTGHS_DEVEPTICR_STALLEDIC)
  //! raises STALL sent
#define  udd_raise_stall(ep)                       (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_STALLEDIS)
  //! enables STALL sent interrupt
#define  udd_enable_stall_interrupt(ep)            (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_STALLEDES)
  //! disables STALL sent interrupt
#define  udd_disable_stall_interrupt(ep)           (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_STALLEDEC)
  //! tests if STALL sent interrupt is enabled
#define  Is_udd_stall_interrupt_enabled(ep)        (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_STALLEDE))

  //! tests if NAK OUT received
#define  Is_udd_nak_out(ep)                        (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_NAKOUTI))
  //! acks NAK OUT received
#define  udd_ack_nak_out(ep)                       (USBB_ARRAY(UOTGHS_DEVEPTICR[0],ep) = UOTGHS_DEVEPTICR_NAKOUTIC)
  //! raises NAK OUT received
#define  udd_raise_nak_out(ep)                     (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_NAKOUTIS)
  //! enables NAK OUT interrupt
#define  udd_enable_nak_out_interrupt(ep)          (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_NAKOUTES)
  //! disables NAK OUT interrupt
#define  udd_disable_nak_out_interrupt(ep)         (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_NAKOUTEC)
  //! tests if NAK OUT interrupt is enabled
#define  Is_udd_nak_out_interrupt_enabled(ep)      (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_NAKOUTE))

  //! tests if NAK IN received
#define  Is_udd_nak_in(ep)                         (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_NAKINI))
  //! acks NAK IN received
#define  udd_ack_nak_in(ep)                        (USBB_ARRAY(UOTGHS_DEVEPTICR[0],ep) = UOTGHS_DEVEPTICR_NAKINIC)
  //! raises NAK IN received
#define  udd_raise_nak_in(ep)                      (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_NAKINIS)
  //! enables NAK IN interrupt
#define  udd_enable_nak_in_interrupt(ep)           (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_NAKINES)
  //! disables NAK IN interrupt
#define  udd_disable_nak_in_interrupt(ep)          (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_NAKINEC)
  //! tests if NAK IN interrupt is enabled
#define  Is_udd_nak_in_interrupt_enabled(ep)       (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_NAKINE))

  //! acks endpoint isochronous overflow interrupt
#define  udd_ack_overflow_interrupt(ep)            (USBB_ARRAY(UOTGHS_DEVEPTICR[0],ep) = UOTGHS_DEVEPTICR_OVERFIC)
  //! raises endpoint isochronous overflow interrupt
#define  udd_raise_overflow_interrupt(ep)          (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_OVERFIS)
  //! tests if an overflow occurs
#define  Is_udd_overflow(ep)                       (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_OVERFI))
  //! enables overflow interrupt
#define  udd_enable_overflow_interrupt(ep)         (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_OVERFES)
  //! disables overflow interrupt
#define  udd_disable_overflow_interrupt(ep)        (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_OVERFEC)
  //! tests if overflow interrupt is enabled
#define  Is_udd_overflow_interrupt_enabled(ep)     (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_OVERFE))

  //! acks endpoint isochronous underflow interrupt
#define  udd_ack_underflow_interrupt(ep)           (USBB_ARRAY(UOTGHS_DEVEPTICR[0],ep) = UOTGHS_DEVEPTICR_UNDERFIC)
  //! raises endpoint isochronous underflow interrupt
#define  udd_raise_underflow_interrupt(ep)         (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_UNDERFIS)
  //! tests if an underflow occurs
#define  Is_udd_underflow(ep)                      (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_UNDERFI))
  //! enables underflow interrupt
#define  udd_enable_underflow_interrupt(ep)        (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_UNDERFES)
  //! disables underflow interrupt
#define  udd_disable_underflow_interrupt(ep)       (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_UNDERFEC)
  //! tests if underflow interrupt is enabled
#define  Is_udd_underflow_interrupt_enabled(ep)    (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_UNDERFE))

  //! tests if CRC ERROR ISO OUT detected
#define  Is_udd_crc_error(ep)                      (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_CRCERRI))
  //! acks CRC ERROR ISO OUT detected
#define  udd_ack_crc_error(ep)                     (USBB_ARRAY(UOTGHS_DEVEPTICR[0],ep) = UOTGHS_DEVEPTICR_CRCERRIC)
  //! raises CRC ERROR ISO OUT detected
#define  udd_raise_crc_error(ep)                   (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_CRCERRIS)
  //! enables CRC ERROR ISO OUT detected interrupt
#define  udd_enable_crc_error_interrupt(ep)        (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_CRCERRES)
  //! disables CRC ERROR ISO OUT detected interrupt
#define  udd_disable_crc_error_interrupt(ep)       (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_CRCERREC)
  //! tests if CRC ERROR ISO OUT detected interrupt is enabled
#define  Is_udd_crc_error_interrupt_enabled(ep)    (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_CRCERRE))
//! @}

//! @name USBB Device control endpoint errors
//! These macros control the endpoint errors.
//! @{

  //! tests if endpoint read allowed
#define  Is_udd_read_enabled(ep)                   (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_RWALL))
  //! tests if endpoint write allowed
#define  Is_udd_write_enabled(ep)                  (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_RWALL))

  //! returns the byte count
#define  udd_byte_count(ep)                        (Rd_bitfield(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_BYCT_Msk))
  //! clears FIFOCON bit
#define  udd_ack_fifocon(ep)                       (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_FIFOCONC)
  //! tests if FIFOCON bit set
#define  Is_udd_fifocon(ep)                        (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_FIFOCON))

  //! returns the number of busy banks
#define  udd_nb_busy_bank(ep)                      (Rd_bitfield(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_NBUSYBK_Msk))
  //! returns the number of the current bank
#define  udd_current_bank(ep)                      (Rd_bitfield(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_CURRBK_Msk))
  //! kills last bank
#define  udd_kill_last_in_bank(ep)                 (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_KILLBKS)
  //! tests if last bank killed
#define  Is_udd_last_in_bank_killed(ep)            (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_KILLBK))
  //! forces all banks full (OUT) or free (IN) interrupt
#define  udd_force_bank_interrupt(ep)              (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_NBUSYBKS)
  //! unforces all banks full (OUT) or free (IN) interrupt
#define  udd_unforce_bank_interrupt(ep)            (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_NBUSYBKS)
  //! enables all banks full (OUT) or free (IN) interrupt
#define  udd_enable_bank_interrupt(ep)             (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_NBUSYBKES)
  //! disables all banks full (OUT) or free (IN) interrupt
#define  udd_disable_bank_interrupt(ep)            (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_NBUSYBKEC)
  //! tests if all banks full (OUT) or free (IN) interrupt enabled
#define  Is_udd_bank_interrupt_enabled(ep)         (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_NBUSYBKE))

  //! tests if SHORT PACKET received
#define  Is_udd_short_packet(ep)                   (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_SHORTPACKET))
  //! acks SHORT PACKET received
#define  udd_ack_short_packet(ep)                  (USBB_ARRAY(UOTGHS_DEVEPTICR[0],ep) = UOTGHS_DEVEPTICR_SHORTPACKETC)
  //! raises SHORT PACKET received
#define  udd_raise_short_packet(ep)                (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_SHORTPACKETS)
  //! enables SHORT PACKET received interrupt
#define  udd_enable_short_packet_interrupt(ep)     (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_SHORTPACKETES)
  //! disables SHORT PACKET received interrupt
#define  udd_disable_short_packet_interrupt(ep)    (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_SHORTPACKETEC)
  //! tests if SHORT PACKET received interrupt is enabled
#define  Is_udd_short_packet_interrupt_enabled(ep) (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_SHORTPACKETE))

  //! Get 64-, 32-, 16- or 8-bit access to FIFO data register of selected endpoint.
  //! @param ep     Endpoint of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @return       Volatile 64-, 32-, 16- or 8-bit data pointer to FIFO data register
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
#define  udd_get_endpoint_fifo_access(ep, scale) \
          (((volatile TPASTE2(U, scale) (*)[0x8000 / ((scale) / 8)])UOTGHS_RAM_ADDR)[(ep)])

//! @name USBB endpoint DMA drivers
//! These macros manage the common features of the endpoint DMA channels.
//! @{
  //! enables the disabling of HDMA requests by endpoint interrupts
#define  udd_enable_endpoint_int_dis_hdma_req(ep)     (USBB_ARRAY(UOTGHS_DEVEPTIER[0](ep) = UOTGHS_DEVEPTIER_EPDISHDMAS)
  //! disables the disabling of HDMA requests by endpoint interrupts
#define  udd_disable_endpoint_int_dis_hdma_req(ep)    (USBB_ARRAY(UOTGHS_DEVEPTIDR[0](ep) = UOTGHS_DEVEPTIDR_EPDISHDMAC)
  //! tests if the disabling of HDMA requests by endpoint interrupts is enabled
#define  Is_udd_endpoint_int_dis_hdma_req_enabled(ep) (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0](ep), UOTGHS_DEVEPTIMR_EPDISHDMA))

  //! raises the selected endpoint DMA channel interrupt
#define  udd_raise_endpoint_dma_interrupt(ep)         (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_DMA_1 << ((ep) - 1))
  //! tests if an interrupt is triggered by the selected endpoint DMA channel
#define  Is_udd_endpoint_dma_interrupt(ep)            (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_DMA_1 << ((ep) - 1)))
  //! enables the selected endpoint DMA channel interrupt
#define  udd_enable_endpoint_dma_interrupt(ep)        (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_DMA_1 << ((ep) - 1))
  //! disables the selected endpoint DMA channel interrupt
#define  udd_disable_endpoint_dma_interrupt(ep)       (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_DMA_1 << ((ep) - 1))
  //! tests if the selected endpoint DMA channel interrupt is enabled
#define  Is_udd_endpoint_dma_interrupt_enabled(ep)    (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_DMA_1 << ((ep) - 1)))

  //! Access points to the USBB device DMA memory map with arrayed registers
  //! @{
      //! Structure for DMA registers
typedef struct {
    union {
        unsigned long nextdesc;
        //usbb_uddma1_nextdesc_t NEXTDESC;
    };
    unsigned long addr;
    union {
        unsigned long control;
        //usbb_uddma1_control_t CONTROL;
    };
    union {
        unsigned long status;
        //usbb_uddma1_status_t STATUS;
    };
} avr32_usbb_uxdmax_t, usbb_uxdmax_t;
      //! Structure for DMA registers
#define  USBB_UDDMA_ARRAY(ep)                (((volatile usbb_uxdmax_t *)UOTGHS->UOTGHS_DEVDMA)[(ep) - 1])

      //! Set control desc to selected endpoint DMA channel
#define  udd_endpoint_dma_set_control(ep,desc)     (USBB_UDDMA_ARRAY(ep).control=desc)
      //! Get control desc to selected endpoint DMA channel
#define  udd_endpoint_dma_get_control(ep)          (USBB_UDDMA_ARRAY(ep).control)
      //! Set RAM address to selected endpoint DMA channel
#define  udd_endpoint_dma_set_addr(ep,add)         (USBB_UDDMA_ARRAY(ep).addr=add)
      //! Get status to selected endpoint DMA channel
#define  udd_endpoint_dma_get_status(ep)           (USBB_UDDMA_ARRAY(ep).status)
   //! @}    
//! @}    

//! @}

//! @name USBB Device control endpoint errors
//! These macros control the endpoint errors.
//! @{

  //! tests if SETUP received
#define  Is_udd_setup_received(ep)                    (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_RXSTPI))
  //! acks SETUP received                            
#define  udd_ack_setup_received(ep)                   (USBB_ARRAY(UOTGHS_DEVEPTICR[0],ep) = UOTGHS_DEVEPTICR_RXSTPIC)
  //! raises SETUP received                          
#define  udd_raise_setup_received(ep)                 (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_RXSTPIS)
  //! enables SETUP received interrupt               
#define  udd_enable_setup_received_interrupt(ep)      (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_RXSTPES)
  //! disables SETUP received interrupt              
#define  udd_disable_setup_received_interrupt()       (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],EP_CONTROL) = UOTGHS_DEVEPTIDR_RXSTPEC)
  //! tests if SETUP received interrupt is enabled
#define  Is_udd_setup_received_interrupt_enabled(ep)  (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_RXSTPE))

  //! tests if OUT received
#define  Is_udd_out_received(ep)                   (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_RXOUTI))
  //! acks OUT received
#define  udd_ack_out_received(ep)                  (USBB_ARRAY(UOTGHS_DEVEPTICR[0],ep) = UOTGHS_DEVEPTICR_RXOUTIC)
  //! raises OUT received
#define  udd_raise_out_received(ep)                (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_RXOUTIS)
  //! enables OUT received interrupt
#define  udd_enable_out_received_interrupt(ep)     (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_RXOUTES)
  //! disables OUT received interrupt
#define  udd_disable_out_received_interrupt(ep)    (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_RXOUTEC)
  //! tests if OUT received interrupt is enabled
#define  Is_udd_out_received_interrupt_enabled(ep) (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_RXOUTE))

  //! tests if IN sending
#define  Is_udd_in_send(ep)                        (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTISR[0],ep), UOTGHS_DEVEPTISR_TXINI))
  //! acks IN sending                             
#define  udd_ack_in_send(ep)                       (USBB_ARRAY(UOTGHS_DEVEPTICR[0],ep) = UOTGHS_DEVEPTICR_TXINIC)
  //! raises IN sending                           
#define  udd_raise_in_send(ep)                     (USBB_ARRAY(UOTGHS_DEVEPTIFR[0],ep) = UOTGHS_DEVEPTIFR_TXINIS)
  //! enables IN sending interrupt                
#define  udd_enable_in_send_interrupt(ep)          (USBB_ARRAY(UOTGHS_DEVEPTIER[0],ep) = UOTGHS_DEVEPTIER_TXINES)
  //! disables IN sending interrupt               
#define  udd_disable_in_send_interrupt(ep)         (USBB_ARRAY(UOTGHS_DEVEPTIDR[0],ep) = UOTGHS_DEVEPTIDR_TXINEC)
  //! tests if IN sending interrupt is enabled    
#define  Is_udd_in_send_interrupt_enabled(ep)      (Tst_bits(USBB_ARRAY(UOTGHS_DEVEPTIMR[0],ep), UOTGHS_DEVEPTIMR_TXINE))
//! @}          

//! @}

#endif // _USBB_DEVICE_H_
