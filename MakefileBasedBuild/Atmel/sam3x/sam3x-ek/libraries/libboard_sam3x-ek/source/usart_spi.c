/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
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

/** \addtogroup usart_module Working with USART
 * The USART SPI driver provides the interface to configure and use the USART 
 * in SPI mode.\n
*/

/**
 * \file
 *
 * Implementation of USART (Universal Synchronous Asynchronous Receiver Transmitter)
 * controller.
 *
 */
/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/
#include "board.h"
#include <assert.h>
#include <string.h>

/*------------------------------------------------------------------------------
 *         Local definitions
 *------------------------------------------------------------------------------*/
#define DELAY_TIME 500

/*------------------------------------------------------------------------------
 *         Exported functions
 *------------------------------------------------------------------------------*/
/**
 * Starts a Usart master transfer. This is a non blocking function. It will
 * return as soon as the transfer is started.
 * Returns 0 if the transfer has been started successfully; otherwise returns
 * 1 is the driver is in use
 * \param pUsartd  Pointer to a usart instance.
 * \param pCommand Pointer to the uSART command to execute.
 */
uint8_t USART_SPIDSendCommand(Usartd *pUsartd, SpidCmd *pCommand)
{
    Usart *pUsart = pUsartd->pUsartHw;

    // Try to get the dataflash semaphore
    if (pUsartd->ucSemaphore == 0)
    {
        return 1;
    }

    pUsartd->ucSemaphore--;

    // Setup Force CS signal as dataflash required
    pUsart->US_CR |= US_CR_FCS;

    if (pUsartd->eTransferType == USART_PDC_TRANS_TYPE)
    {
        pUsartd->ucUsartTransEndSig = 0;

        pUsart->US_TPR = (uint32_t) pCommand->pCmd;
        pUsart->US_TCR = pCommand->cmdSize;

        pUsart->US_RPR = (uint32_t) pCommand->pCmd;
        pUsart->US_RCR = pCommand->cmdSize;

        pUsart->US_TNPR = (uint32_t) pCommand->pData;
        pUsart->US_TNCR = pCommand->dataSize;

        pUsart->US_RNPR = (uint32_t) pCommand->pData;
        pUsart->US_RNCR = pCommand->dataSize;

        // Enable PDC transfer
        pUsart->US_PTCR = US_PTCR_RXTEN | US_PTCR_TXTEN;

        // Enable buffer complete interrupt
        USART_EnableIt(pUsart, US_IER_RXBUFF);

        // Wait till command done
        while (!pUsartd->ucUsartTransEndSig);
    }
    else
    {
        uint32_t dw;
        /* Send dataflash command */
        for (dw=0; dw < pCommand->cmdSize; dw++ )
        {
            USART_Write(pUsart, pCommand->pCmd[dw], DELAY_TIME);
            USART_Read(pUsart, DELAY_TIME);
        }

        /* Send/receive the data */
        for (dw=0; dw < pCommand->dataSize; dw++ )
        {
            USART_Write(pUsart, (uint16_t)pCommand->pData[dw], DELAY_TIME);
            pCommand->pData[dw] = (uint8_t)USART_Read(pUsart, DELAY_TIME);
        }
    }

    return 0;
}

/**
 * The USART_SPIDHandler must be called by the Usart Interrupt Service Routine with the
 * corresponding Spi instance.
 * The USART_SPIDHandler will unlock the Spi semaphore and invoke the upper application 
 * callback.
 * \param pUsartd  Pointer to a pUsartd instance.
 */
void USART_SPIDHandler(Usartd *pUsartd)
{
    SpidCmd *pUsartCmd = pUsartd->pCurrentCommand;
    Usart *pUsart = pUsartd->pUsartHw;
    volatile uint32_t dwStatus;

    dwStatus = USART_GetStatus(pUsart);

    // Check the status register
    if ((dwStatus & US_CSR_RXBUFF) || (pUsartd->eTransferType == USART_FIFO_TRANS_TYPE))
    {
        // Disable buffer complete interrupt
        USART_DisableIt(pUsart, US_IDR_RXBUFF);

        // Release the chip select signal
        pUsart->US_CR |= US_CR_RCS;

        // Release the dataflash semaphore
        pUsartd->ucSemaphore++;

        // Invoke the callback associated with the current command
        if (pUsartCmd && pUsartCmd->callback) {

            pUsartCmd->callback(0, pUsartCmd->pArgument);
        }
    }
}

/**
 * Returns 1 if the Usart driver is currently busy executing a command; otherwise
 * returns 0.
 * \param pUsartd  Pointer to a Usart driver instance.
 */
uint8_t  USART_SPIDIsBusy(const Usartd *pUsartd)
{
    if (pUsartd->ucSemaphore == 0) {

        return 1;
    }
    else {

        return 0;
    }
}

