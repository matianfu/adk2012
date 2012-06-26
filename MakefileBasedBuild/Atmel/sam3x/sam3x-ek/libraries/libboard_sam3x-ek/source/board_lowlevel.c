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
 *
 * Provides the low-level initialization function that called on chip startup.
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/
//#define OSC_12MHZ             12000000
//
///// MCFR Min and Max values with 32kHz RC
//#define MCFR_32RC_MIN(freq)   ((freq * 16)/44000)
//#define MCFR_32RC_MAX(freq)   ((freq * 16)/20000)

/* Define clock dwTimeout */
#define CLOCK_TIMEOUT    0xFFFFFFFF

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Performs the low-level initialization of the chip.
 * This includes EFC and master clock configuration.
 * It also enable a low level on the pin NRST triggers a user reset.
 */
extern WEAK void LowLevelInit( void )
{
    uint32_t dwTimeout;

    /* Set 3 WS for Embedded Flash Access for 84 MHz */
    EFC0->EEFC_FMR = EEFC_FMR_FWS( 3 );
    EFC1->EEFC_FMR = EEFC_FMR_FWS( 3 );

    /* Initialize main oscillator */
    if ( !(PMC->CKGR_MOR & CKGR_MOR_MOSCSEL) )  /* Main Oscillator Selection */
    {
        /* The Main Crystal Oscillator is enabled */
        PMC->CKGR_MOR = CKGR_MOR_KEY(0x37)
                      | CKGR_MOR_MOSCXTST(0x8) /* Main Crystal Oscillator Start-up Time: 1.4ms(datasheet) */
                      /* Specifies the number of Slow Clock cycles multiplied by 8 for the Main Crystal Oscillator start-up time */
                      | CKGR_MOR_MOSCRCEN      /* Main On-Chip RC Oscillator Enable */
                      | CKGR_MOR_MOSCXTEN;     /* Main Crystal Oscillator Enable */
                      /* MOSCSEL: The Main On-Chip RC Oscillator is selected */
                      /* MOSCRCF: The Fast RC Oscillator Frequency is at 4 MHz (default) */
                      /* CFDEN:  The Clock Failure Detector is disabled. */
        dwTimeout = 0;
        while ( !(PMC->PMC_SR & PMC_SR_MOSCXTS) && (dwTimeout++ < CLOCK_TIMEOUT) );
    }

    /* Switch to 3-20MHz Xtal oscillator */
    /* The Main Crystal Oscillator is enabled */
    PMC->CKGR_MOR = CKGR_MOR_KEY(0x37)
                  | CKGR_MOR_MOSCXTST(0x8)  /* Main Crystal Oscillator Start-up Time: 1.4ms(datasheet) */
                  /* Specifies the number of Slow Clock cycles multiplied by 8 for the Main Crystal Oscillator start-up time */
                  | CKGR_MOR_MOSCRCEN       /* Main On-Chip RC Oscillator Enable */
                  | CKGR_MOR_MOSCXTEN       /* Main Crystal Oscillator Enable */
                  | CKGR_MOR_MOSCSEL;       /* The Main Crystal Oscillator is selected */
                  /* MOSCRCF: The Fast RC Oscillator Frequency is at 4 MHz (default) */
                  /* CFDEN:  The Clock Failure Detector is disabled. */
    dwTimeout = 0;
    /* Wait Main XTAL Oscillator Status */
    while (!(PMC->PMC_SR & PMC_SR_MOSCXTS) && (dwTimeout++ < CLOCK_TIMEOUT));

    /* configure PLLA to 168 MHz */
    PMC->CKGR_PLLAR = CKGR_PLLAR_STUCKTO1
                    | CKGR_PLLAR_MULA(13)     /* PLLA Multiplier 12 MHz x (13+1) = 168 MHz */
                    | CKGR_PLLAR_PLLACOUNT(2) /* PLLA Counter 200µs(datasheet) */
                    | CKGR_PLLAR_DIVA(1);     /* Divider bypassed */
    dwTimeout = 0;
    /* Wait PLL A Lock Status */
    while (!(PMC->PMC_SR & PMC_SR_LOCKA) && (dwTimeout++ < CLOCK_TIMEOUT));

    PMC->PMC_MCKR = PMC_MCKR_PRES_CLK_2        /* Selected clock divided by 2 => 168/2 = 84 MHz  */
                  | PMC_MCKR_CSS_MAIN_CLK;     /* Main Clock is selected */
    dwTimeout = 0;
    /* Wait Master Clock Status */
    while (!(PMC->PMC_SR & PMC_SR_MCKRDY) && (dwTimeout++ < CLOCK_TIMEOUT));

    PMC->PMC_MCKR = PMC_MCKR_PRES_CLK_2        /* Selected clock divided by 2 => 168/2 = 84 MHz  */
                  | PMC_MCKR_CSS_PLLA_CLK;   /* PLLA Clock is selected */
    dwTimeout = 0;
    /* Wait Master Clock Status */
    while (!(PMC->PMC_SR & PMC_SR_MCKRDY) && (dwTimeout++ < CLOCK_TIMEOUT));

}
