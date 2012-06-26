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

/** \addtogroup ssc_module Working with SSC
 * The SSC driver provides the interface to configure and use the SSC
 * peripheral.
 *
 * !Usage
 *
 * -# Enable the SSC interface pins.
 * -# Configure the SSC to operate at a specific frequency by calling
 *    SSC_Configure(). This function enables the peripheral clock of the SSC,
 *    but not its PIOs.
 * -# Configure the transmitter and/or the receiver using the
 *    SSC_ConfigureTransmitter() and SSC_ConfigureEmitter() functions.
 * -# Enable the PIOs or the transmitter and/or the received.
 * -# Enable the transmitter and/or the receiver using SSC_EnableTransmitter()
 *    and SSC_EnableReceiver()
 * -# Send data through the transmitter using SSC_Write() 
 * -# Receive data from the receiver using SSC_Read() 
 * -# Disable the transmitter and/or the receiver using SSC_DisableTransmitter()
 *    and SSC_DisableReceiver()
 *
 * For more accurate information, please look at the RTC section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref ssc.c\n
 * \ref ssc.h.\n
*/
/*@{*/
/*@}*/


/**
 * \file
 *
 * Implementation of Synchronous Serial (SSC) controller.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

/*----------------------------------------------------------------------------
 *       Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Configures a SSC peripheral.If the divided clock is not used, the master
 * clock frequency can be set to 0.
 * \note The emitter and transmitter are disabled by this function.
 * \param bitRate  bit rate.
 * \param masterClock  master clock.
 */
extern void SSC_Configure( Ssc* pSsc, uint32_t dwBitRate, uint32_t dwMasterClock )
{
    /* Enable SSC peripheral clock */
    PMC_EnablePeripheral( ID_SSC ) ;

    /* Reset, disable receiver & transmitter */
    pSsc->SSC_CR = SSC_CR_RXDIS | SSC_CR_TXDIS | SSC_CR_SWRST ;

//    pSsc->SSC_PTCR = SSC_PTCR_RXTDIS | SSC_PTCR_TXTDIS ;

    /* Configure clock frequency */
    if ( dwBitRate != 0 )
    {
        pSsc->SSC_CMR = dwMasterClock / (dwBitRate<<1) ;
    }
    else
    {
        pSsc->SSC_CMR = 0 ;
    }
}

/**
 * \brief Configures the transmitter of a SSC peripheral.
 * \param tcmr Transmit Clock Mode Register value.
 * \param tfmr Transmit Frame Mode Register value.
 */
extern void SSC_ConfigureTransmitter( Ssc* pSsc, uint32_t dwTCMR, uint32_t dwTFMR )
{
    pSsc->SSC_TCMR = dwTCMR ;
    pSsc->SSC_TFMR = dwTFMR ;
}

/**
 * \brief Configures the receiver of a SSC peripheral.
 * \param rcmr Receive Clock Mode Register value.
 * \param rfmr Receive Frame Mode Register value.
 */
extern void SSC_ConfigureReceiver( Ssc* pSsc, uint32_t dwRCMR, uint32_t dwRFMR )
{
    pSsc->SSC_RCMR = dwRCMR ;
    pSsc->SSC_RFMR = dwRFMR ;
}

/**
 * \brief Enables the transmitter of a SSC peripheral.
 */
extern void SSC_EnableTransmitter( Ssc* pSsc )
{
    pSsc->SSC_CR = SSC_CR_TXEN ;
}

/**
 * \brief Disables the transmitter of a SSC peripheral.
 */
extern void SSC_DisableTransmitter( Ssc* pSsc )
{
    pSsc->SSC_CR = SSC_CR_TXDIS ;
}

/**
 * \brief Enables the receiver of a SSC peripheral.
 */
extern void SSC_EnableReceiver( Ssc* pSsc )
{
    pSsc->SSC_CR = SSC_CR_RXEN ;
}

/**
 * \brief Disables the receiver of a SSC peripheral.
 */
extern void SSC_DisableReceiver( Ssc* pSsc )
{
    pSsc->SSC_CR = SSC_CR_RXDIS ;
}

/**
 * \brief Enables one or more interrupt sources of a SSC peripheral.
 * \param sources Bitwise OR of selected interrupt sources.
 */
extern void SSC_EnableInterrupts( Ssc* pSsc, uint32_t dwSources )
{
    pSsc->SSC_IER = dwSources ;
}

/**
 * \brief Disables one or more interrupt sources of a SSC peripheral.
 * \param sources Bitwise OR of selected interrupt sources.
 */
extern void SSC_DisableInterrupts( Ssc* pSsc, uint32_t dwSources )
{
    pSsc->SSC_IDR = dwSources ;
}

/**
 * \brief Sends one data frame through a SSC peripheral. If another frame is currently
 * being sent, this function waits for the previous transfer to complete.
 * \param frame Data frame to send.
 */
extern void SSC_Write( Ssc* pSsc, uint32_t dwFrame )
{
    while ( (pSsc->SSC_SR & SSC_SR_TXRDY) == 0 ) ;
    pSsc->SSC_THR = dwFrame ;
}

/**
 * \brief Waits until one frame is received on a SSC peripheral, and returns it.
 */
extern uint32_t SSC_Read( Ssc* pSsc )
{
    while ( (pSsc->SSC_SR & SSC_SR_RXRDY) == 0 ) ;

    return pSsc->SSC_RHR ;
}
