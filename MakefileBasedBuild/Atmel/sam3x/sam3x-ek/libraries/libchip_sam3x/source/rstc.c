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
 *  \file
 *
 *  Implementation of the RSTC module.
 */

/*-----------------------------------------------------------------------------
 *         Headers
 *-----------------------------------------------------------------------------*/

#include "chip.h"

/*-----------------------------------------------------------------------------
 *         Exported functions
 *-----------------------------------------------------------------------------*/

/**
 * \brief Configure the mode of the RSTC peripheral.
 * The configuration is computed by the lib (AT91C_RSTC_*).
 * \param rmr Desired mode configuration.
 */
extern void RSTC_ConfigureMode( Rstc* pRstc, uint32_t dwValue )
{
    dwValue &= ~RSTC_MR_KEY_Msk ;
    pRstc->RSTC_MR = dwValue | RSTC_MR_KEY( 0xA5u ) ;
}

/**
 * \brief Enable/Disable the detection of a low level on the pin NRST as User Reset
 *
 * \param enable 1 to enable & 0 to disable.
 */
extern void RSTC_SetUserResetEnable( Rstc* pRstc, uint32_t dwEnable )
{
    uint32_t dw = pRstc->RSTC_MR & (~RSTC_MR_KEY_Msk) ;

    if ( dwEnable )
    {
        dw |=  RSTC_MR_URSTEN ;
    }
    else
    {
        dw &= ~RSTC_MR_URSTEN ;
    }

    pRstc->RSTC_MR = dw | RSTC_MR_KEY( 0xA5u ) ;
}
/**
 * \brief Enable/Disable the interrupt of a User Reset (USRTS bit in RSTC_RST).
 *
 * \param enable 1 to enable & 0 to disable.
 */
extern void RSTC_SetUserResetInterruptEnable( Rstc* pRstc, uint32_t dwEnable )
{
    uint32_t dw = pRstc->RSTC_MR & (~RSTC_MR_KEY_Msk) ;

    if ( dwEnable )
    {
        dw |=  RSTC_MR_URSTIEN ;
    }
    else
    {
        dw &= ~RSTC_MR_URSTIEN ;
    }
    pRstc->RSTC_MR = dw | RSTC_MR_KEY( 0xA5u ) ;
}
/**
 * \brief Setup the external reset length. The length is asserted during a time of
 * pow(2, powl+1) Slow Clock(32KHz). The duration is between 60us and 2s.
 * \param enable 1 to enable & 0 to disable.
 */
extern void RSTC_SetExtResetLength( Rstc* pRstc, uint32_t dwPowerLength )
{
    uint32_t dw = pRstc->RSTC_MR ;

    dw &= ~(RSTC_MR_KEY_Msk | RSTC_MR_ERSTL_Msk) ;
    dw |=  RSTC_MR_ERSTL( dwPowerLength ) ;
    pRstc->RSTC_MR = dw | RSTC_MR_KEY( 0xA5u ) ;
}

/**
 * \brief Resets the processor.
 */
extern void RSTC_ProcessorReset( Rstc* pRstc )
{
    pRstc->RSTC_CR = RSTC_CR_PROCRST | RSTC_CR_KEY( 0xA5u ) ;
}
/**
 * \brief Resets the peripherals.
 */
extern void RSTC_PeripheralReset( Rstc* pRstc )
{
    pRstc->RSTC_CR = RSTC_CR_PERRST | RSTC_CR_KEY( 0xA5u ) ;
}

/**
 * \brief Asserts the NRST pin for external resets.
 */
extern void RSTC_ExtReset( Rstc* pRstc )
{
    pRstc->RSTC_CR = RSTC_CR_EXTRST | RSTC_CR_KEY( 0xA5u ) ;
}
/**
 * \brief Return NRST pin level ( 1 or 0 ).
 */
extern uint32_t RSTC_GetNrstLevel( Rstc* pRstc )
{
    if ( pRstc->RSTC_SR & RSTC_SR_NRSTL )
    {
        return 1 ;
    }

    return 0 ;
}
/**
 * \brief Check the user had trigged the reset
 *
 * \return Returns 1 if at least one high-to-low transition of NRST (User Reset) has
 * been detected since the last read of RSTC_SR.
 */
extern uint32_t RSTC_IsUserResetDetected( Rstc* pRstc )
{
    if ( pRstc->RSTC_SR & RSTC_SR_URSTS )
    {
        return 1 ;
    }

    return 0 ;
}
/**
 * \brief Check the RSTC if busy
 *
 * \return Return 1 if a software reset command is being performed by the reset
 * controller. The reset controller is busy.
 */
extern uint32_t RSTC_IsBusy( Rstc* pRstc )
{
    if ( pRstc->RSTC_SR & RSTC_SR_SRCMP )
    {
        return 1 ;
    }

    return 0 ;
}
/**
 * \brief Get the status
 */
extern uint32_t RSTC_GetStatus( Rstc* pRstc )
{
    return (pRstc->RSTC_SR) ;
}
