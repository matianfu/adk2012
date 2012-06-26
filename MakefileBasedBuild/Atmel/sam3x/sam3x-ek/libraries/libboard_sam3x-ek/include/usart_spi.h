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
 * \par Purpose
 *
 * This module provides several definitions and methods for using an USART
 * peripheral.
 *
 * \par Usage
 *
 * -# Enable the USART peripheral clock in the PMC.
 * -# Enable the required USART PIOs (see pio.h).
 * -# Configure the UART by calling USART_Configure.
 * -# Enable the transmitter and/or the receiver of the USART using
 *    USART_SetTransmitterEnabled and USART_SetReceiverEnabled.
 * -# Send data through the USART using the USART_Write and
 *    USART_WriteBuffer methods.
 * -# Receive data from the USART using the USART_Read and
 *    USART_ReadBuffer functions; the availability of data can be polled
 *    with USART_IsDataAvailable.
 * -# Disable the transmitter and/or the receiver of the USART with
 *    USART_SetTransmitterEnabled and USART_SetReceiverEnabled.
 */

#ifndef _USART_SPI_
#define _USART_SPI_

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/

#include "chip.h"

#include <stdint.h>

/*------------------------------------------------------------------------------
 *         Definitions
 *------------------------------------------------------------------------------*/
#ifdef __cplusplus
 extern "C" {
#endif


/** USART transfer complete callback. */
typedef void (*UsartdCallback)( uint8_t, void* ) ;

/** Usart transfer type */
typedef enum
{
  USART_PDC_TRANS_TYPE = 0,  /* default transfer with PDC */
  USART_FIFO_TRANS_TYPE,   /* transfer without PDC */
  USART_MAX_TRANS_TYPE = 0xff
}usart_trans_type;

/** \brief usart Transfer Request prepared by the application upper layer.
 *
 * This structure is sent to the Usart_SPISendCommand function to start the transfer.
 * At the end of the transfer, the callback is invoked by the interrupt handler.
 */
typedef struct _UsartdCmd
{
    /** Pointer to the command data. */
    uint8_t *pCmd;
    /** Command size in bytes. */
    uint8_t cmdSize;
    /** Pointer to the data to be sent. */
    uint8_t *pData;
    /** Data size in bytes. */
    uint16_t dataSize;
    /** Callback function invoked at the end of transfer. */
    UsartdCallback callback;
    /** Callback arguments. */
    void *pArgument;
} UsartdCmd ;

/** Constant structure associated with usart port. This structure prevents
    client applications to have access in the same time. */
typedef struct _Usartd
{
    /** Pointer to Usart Hardware registers */
    Usart* pUsartHw ;
    /* Usart ID*/
    uint32_t dwUsartID;
    /* Usart IRQ num*/
    IRQn_Type eUsartIRQn;
    /** Current usartCommand being processed */
    SpidCmd *pCurrentCommand ;
    /** Mutual exclusion semaphore. */
    volatile uint8_t ucSemaphore ;
    /** Usart transfer type */
    usart_trans_type eTransferType;
    /** Usart transfer end signal */
    uint8_t ucUsartTransEndSig;
} Usartd ;

/*------------------------------------------------------------------------------*/
/*         Exported functions                                                   */
/*------------------------------------------------------------------------------*/
extern uint8_t  USART_SPIDSendCommand(Usartd *pUsartd, SpidCmd *pCommand);
extern void     USART_SPIDHandler(Usartd *pUsartd);
extern uint8_t  USART_SPIDIsBusy(const Usartd *pUsartd);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _USART_ */

