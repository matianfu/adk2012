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
 * \file
 *
 * Interface for Synchronous Serial (SSC) controller.
 *
 */

#ifndef _SSC_
#define _SSC_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "chip.h"

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
extern void SSC_Configure( Ssc* pSsc, uint32_t dwBitRate, uint32_t dwMasterClock ) ;

extern void SSC_ConfigureTransmitter( Ssc* pSsc, uint32_t dwTCMR, uint32_t dwTFMR ) ;
extern void SSC_ConfigureReceiver( Ssc* pSsc, uint32_t dwRCMR, uint32_t dwRFMR ) ;

extern void SSC_EnableTransmitter( Ssc* pSsc ) ;
extern void SSC_DisableTransmitter( Ssc* pSsc ) ;
extern void SSC_EnableReceiver( Ssc* pSsc ) ;
extern void SSC_DisableReceiver( Ssc* pSsc );

extern void SSC_EnableInterrupts( Ssc* pSsc, uint32_t dwSources ) ;
extern void SSC_DisableInterrupts( Ssc* pSsc, uint32_t dwSources ) ;

extern void SSC_Write( Ssc* pSsc, uint32_t dwFrame ) ;
extern uint32_t SSC_Read( Ssc* pSsc ) ;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _SSC_ */

