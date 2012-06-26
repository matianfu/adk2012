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
   *  Implementation of the PIO low level handlers.
   */


/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

#include <assert.h>

/*----------------------------------------------------------------------------
 *        Global Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Parallel IO Controller A interrupt handler
 * \Redefined PIOA interrupt handler for NVIC interrupt table.
 */
extern void PIOA_IrqHandler( void )
{
    PioInterruptHandler( ID_PIOA, PIOA ) ;
}

/**
 * \brief Parallel IO Controller B interrupt handler
 * \Redefined PIOB interrupt handler for NVIC interrupt table.
 */
extern void PIOB_IrqHandler( void )
{
    PioInterruptHandler( ID_PIOB, PIOB ) ;
}

/**
 * \brief Parallel IO Controller C interrupt handler
 * \Redefined PIOC interrupt handler for NVIC interrupt table.
 */
extern void PIOC_IrqHandler( void )
{
    PioInterruptHandler( ID_PIOC, PIOC ) ;
}

/**
 * \brief Parallel IO Controller D interrupt handler
 * \Redefined PIOD interrupt handler for NVIC interrupt table.
 */
extern void PIOD_IrqHandler( void )
{
    PioInterruptHandler( ID_PIOD, PIOD ) ;
}

/**
 * \brief Parallel IO Controller E interrupt handler
 * \Redefined PIOE interrupt handler for NVIC interrupt table.
 */
extern void PIOE_IrqHandler( void )
{
    PioInterruptHandler( ID_PIOE, PIOE ) ;
}

/**
 * \brief Parallel IO Controller F interrupt handler
 * \Redefined PIOF interrupt handler for NVIC interrupt table.
 */
extern void PIOF_IrqHandler( void )
{
    PioInterruptHandler( ID_PIOF, PIOF ) ;
}


/**
 * \brief Initializes the PIO interrupt management logic
 *
 * The desired priority of PIO interrupts must be provided.
 * Calling this function multiple times result in the reset of currently
 * configured interrupts.
 *
 * \param priority  PIO controller interrupts priority.
 */
extern void PIO_InitializeInterrupts( uint32_t dwPriority )
{
    TRACE_DEBUG( "PIO_Initialize()\n\r" ) ;

    /* Reset sources */
//    _dwNumSources = 0 ;

    /* Configure PIO interrupt sources */
    TRACE_DEBUG( "PIO_Initialize: Configuring PIOA\n\r" ) ;
    PMC_EnablePeripheral( ID_PIOA ) ;
    PIOA->PIO_ISR ;
    PIOA->PIO_IDR = 0xFFFFFFFF ;
    NVIC_DisableIRQ( PIOA_IRQn ) ;
    NVIC_ClearPendingIRQ( PIOA_IRQn ) ;
    NVIC_SetPriority( PIOA_IRQn, dwPriority ) ;
    NVIC_EnableIRQ( PIOA_IRQn ) ;

    TRACE_DEBUG( "PIO_Initialize: Configuring PIOB\n\r" ) ;
    PMC_EnablePeripheral( ID_PIOB ) ;
    PIOB->PIO_ISR ;
    PIOB->PIO_IDR = 0xFFFFFFFF ;
    NVIC_DisableIRQ( PIOB_IRQn ) ;
    NVIC_ClearPendingIRQ( PIOB_IRQn ) ;
    NVIC_SetPriority( PIOB_IRQn, dwPriority ) ;
    NVIC_EnableIRQ( PIOB_IRQn ) ;

    TRACE_DEBUG( "PIO_Initialize: Configuring PIOC\n\r" ) ;
    PMC_EnablePeripheral( ID_PIOC ) ;
    PIOC->PIO_ISR ;
    PIOC->PIO_IDR = 0xFFFFFFFF ;
    NVIC_DisableIRQ( PIOC_IRQn ) ;
    NVIC_ClearPendingIRQ( PIOC_IRQn ) ;
    NVIC_SetPriority( PIOC_IRQn, dwPriority ) ;
    NVIC_EnableIRQ( PIOC_IRQn ) ;
	
    TRACE_DEBUG( "PIO_Initialize: Configuring PIOD\n\r" ) ;
    PMC_EnablePeripheral( ID_PIOD ) ;
    PIOD->PIO_ISR ;
    PIOD->PIO_IDR = 0xFFFFFFFF ;
    NVIC_DisableIRQ( PIOD_IRQn ) ;
    NVIC_ClearPendingIRQ( PIOD_IRQn ) ;
    NVIC_SetPriority( PIOD_IRQn, dwPriority ) ;
    NVIC_EnableIRQ( PIOD_IRQn ) ;

    TRACE_DEBUG( "PIO_Initialize: Configuring PIOE\n\r" ) ;
    PMC_EnablePeripheral( ID_PIOE ) ;
    PIOE->PIO_ISR ;
    PIOE->PIO_IDR = 0xFFFFFFFF ;
    NVIC_DisableIRQ( PIOE_IRQn ) ;
    NVIC_ClearPendingIRQ( PIOE_IRQn ) ;
    NVIC_SetPriority( PIOE_IRQn, dwPriority ) ;
    NVIC_EnableIRQ( PIOE_IRQn ) ;

    TRACE_DEBUG( "PIO_Initialize: Configuring PIOF\n\r" ) ;
    PMC_EnablePeripheral( ID_PIOF ) ;
    PIOF->PIO_ISR ;
    PIOF->PIO_IDR = 0xFFFFFFFF ;
    NVIC_DisableIRQ( PIOF_IRQn ) ;
    NVIC_ClearPendingIRQ( PIOF_IRQn ) ;
    NVIC_SetPriority( PIOF_IRQn, dwPriority ) ;
    NVIC_EnableIRQ( PIOF_IRQn ) ;	
}

