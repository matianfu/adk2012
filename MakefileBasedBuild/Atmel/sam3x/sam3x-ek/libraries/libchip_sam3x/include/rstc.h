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
 *  \file
 *
 *  Function prototypes for RSTC.
 */

#ifndef _RSTC_H
#define _RSTC_H

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

extern void RSTC_ConfigureMode( Rstc* pRstc, uint32_t dwValue ) ;

extern void RSTC_SetUserResetEnable( Rstc* pRstc, uint32_t dwEnable ) ;
extern void RSTC_SetUserResetInterruptEnable( Rstc* pRstc, uint32_t dwEnable ) ;

extern void RSTC_SetExtResetLength( Rstc* pRstc, uint32_t dwPowerLength ) ;

extern void RSTC_ProcessorReset( Rstc* pRstc ) ;
extern void RSTC_PeripheralReset( Rstc* pRstc ) ;
extern void RSTC_ExtReset( Rstc* pRstc ) ;

extern uint32_t RSTC_GetNrstLevel( Rstc* pRstc ) ;
extern uint32_t RSTC_IsUserResetDetected( Rstc* pRstc ) ;
extern uint32_t RSTC_IsBusy( Rstc* pRstc ) ;

extern uint32_t RSTC_GetStatus( Rstc* pRstc ) ;


#endif // #ifndef _RSTC_H
