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
 * \file
 * This file contains the default exception handlers.
 *
 * \note
 * The exception handler has weak aliases.
 * As they are weak aliases, any function with the same name will override
 * this definition.
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Default interrupt handler for not used irq.
 */
void IrqHandlerNotUsed( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default NMI interrupt handler.
 */
WEAK void NMI_Handler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default HardFault interrupt handler.
 */
WEAK void HardFault_Handler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default MemManage interrupt handler.
 */
WEAK void MemManage_Handler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default BusFault interrupt handler.
 */
WEAK void BusFault_Handler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default UsageFault interrupt handler.
 */
WEAK void UsageFault_Handler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default SVC interrupt handler.
 */
WEAK void SVC_Handler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default DebugMon interrupt handler.
 */
WEAK void DebugMon_Handler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default PendSV interrupt handler.
 */
WEAK void PendSV_Handler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default SysTick interrupt handler.
 */
WEAK void SysTick_Handler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for Supply Controller.
 */
WEAK void SUPC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for Reset Controller.
 */
WEAK void RSTC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for Real Time Clock.
 */
WEAK void RTC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for Real Time Timer.
 */
WEAK void RTT_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for Watchdog Timer.
 */
WEAK void WDT_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for PMC.
 */
WEAK void PMC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for EFC 0.
 */
WEAK void EFC0_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for EFC 0.
 */
WEAK void EFC1_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for UART.
 */
WEAK void UART_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for SMC.
 */
WEAK void SMC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for PIOA Controller.
 */
WEAK void PIOA_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for PIOB Controller.
 */
WEAK void PIOB_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for PIOC Controller.
 */
WEAK void PIOC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for PIOD Controller.
 */
WEAK void PIOD_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for PIOE Controller.
 */
WEAK void PIOE_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for PIOF Controller.
 */
WEAK void PIOF_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for USART0.
 */
WEAK void USART0_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for USART1.
 */
WEAK void USART1_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for USART2.
 */
WEAK void USART2_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for USART3.
 */
WEAK void USART3_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for MCI.
 */
WEAK void HSMCI_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TWI0.
 */
WEAK void TWI0_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TWI1.
 */
WEAK void TWI1_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for SPI 0.
 */
WEAK void SPI0_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for SPI 1.
 */
WEAK void SPI1_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for SSC.
 */
WEAK void SSC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TC0.
 */
WEAK void TC0_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TC1.
 */
WEAK void TC1_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TC2.
 */
WEAK void TC2_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TC3.
 */
WEAK void TC3_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TC4.
 */
WEAK void TC4_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TC5.
 */
WEAK void TC5_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TC6.
 */
WEAK void TC6_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TC7.
 */
WEAK void TC7_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TC8.
 */
WEAK void TC8_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for ADC.
 */
WEAK void ADC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for DAC.
 */
WEAK void DACC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for PWM.
 */
WEAK void PWM_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for UOTGHS.
 */
WEAK void UOTGHS_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for CAN 0.
 */
WEAK void CAN0_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for CAN 1.
 */
WEAK void CAN1_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for TRNG.
 */
WEAK void TRNG_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for EMAC.
 */
WEAK void EMAC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for DMAC.
 */
WEAK void DMAC_IrqHandler( void )
{
    while ( 1 ) ;
}

/**
 * \brief Default interrupt handler for SDRAMC.
 */
WEAK void SDRAMC_IrqHandler( void )
{
    while ( 1 ) ;
}

