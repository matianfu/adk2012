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

/**
 * \file
 *
 * Implementation of TWI device driver.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "board.h"

#include <assert.h>

/*----------------------------------------------------------------------------
 *        Definition
 *----------------------------------------------------------------------------*/
#define TWITIMEOUTMAX 50000

/*----------------------------------------------------------------------------
 *        Types
 *----------------------------------------------------------------------------*/

/** TWI driver callback function.*/
typedef void (*TwiCallback)(Async *);

/** \brief TWI asynchronous transfer descriptor.*/
typedef struct _AsyncTwi {

    /** Asynchronous transfer status. */
    volatile uint8_t status;
    // Callback function to invoke when transfer completes or fails.*/
    TwiCallback callback;
    /** Pointer to the data buffer.*/
    uint8_t *pData;
    /** Total number of bytes to transfer.*/
    uint32_t num;
    /** Number of already transferred bytes.*/
    uint32_t transferred;
} AsyncTwi;
/*------------------------------------------------------------------------------
 *        Exported functions
 *------------------------------------------------------------------------------*/
#ifdef USE_TWI_DMA
/**
 *  \brief Start DMA reading data.
 * \param pTwid  Pointer to the Twid instance to initialize.
 * \param pBuffer Pointer to the data buffer.
 * \param wSize  Receive byte size.  
 */
void TWID_DMARead(
    Twid *pTwid,
    void* pBuffer, 
    uint16_t wSize)
{
    sDmad *pDmad = pTwid->pDmad;
    sDmaTransferDescriptor td;

    pTwid->ucRxDone = 0;

    td.dwSrcAddr = (uint32_t) &(pTwid->pTwi)->TWI_RHR;
    td.dwDstAddr = (uint32_t) pBuffer;
    td.dwCtrlA   = DMAC_CTRLA_BTSIZE(wSize)
                    | DMAC_CTRLA_SRC_WIDTH_BYTE
                    | DMAC_CTRLA_DST_WIDTH_BYTE;
    td.dwCtrlB   = DMAC_CTRLB_SRC_DSCR | DMAC_CTRLB_DST_DSCR
                    | DMAC_CTRLB_FC_PER2MEM_DMA_FC
                    | DMAC_CTRLB_SRC_INCR_FIXED
                    | DMAC_CTRLB_DST_INCR_INCREMENTING;

    td.dwDscAddr = 0;

    DMAD_PrepareSingleTransfer(pDmad, pTwid->dwTwidDmaRxChannel, &td);

    DMAD_StartTransfer(pDmad, pTwid->dwTwidDmaRxChannel); 

    TWI_StartRead(pTwid->pTwi, pTwid->address, pTwid->iaddress, pTwid->isize);

}

/**
 *  \brief Start DMA sending data.
 * \param pTwid  Pointer to the Twid instance to initialize.
 * \param pBuffer Pointer to the data buffer.
 * \param wSize  Transmit byte size. 
 */
void TWID_DMAWrite(
    Twid *pTwid,
    void* pBuffer, 
    uint16_t wSize)
{
    sDmad *pDmad = pTwid->pDmad;
    sDmaTransferDescriptor td;

    pTwid->ucTxDone=0;

    td.dwSrcAddr = (uint32_t) pBuffer;
    td.dwDstAddr = (uint32_t) &(pTwid->pTwi)->TWI_THR;
    td.dwCtrlA   = DMAC_CTRLA_BTSIZE(wSize)
                    | DMAC_CTRLA_SRC_WIDTH_BYTE
                    | DMAC_CTRLA_DST_WIDTH_BYTE;
    td.dwCtrlB   = DMAC_CTRLB_SRC_DSCR | DMAC_CTRLB_DST_DSCR
                    | DMAC_CTRLB_FC_MEM2PER_DMA_FC
                    | DMAC_CTRLB_SRC_INCR_INCREMENTING
                    | DMAC_CTRLB_DST_INCR_FIXED;

    td.dwDscAddr = 0;

    DMAD_PrepareSingleTransfer(pDmad, pTwid->dwTwidDmaTxChannel, &td);

    DMAD_StartTransfer(pDmad, pTwid->dwTwidDmaTxChannel); 
}


/**
 * \brief DMA driver configuration
 * \param pTwid  Pointer to the Twid instance to initialize.
 * \param RxCb  DMA receive callback function.
 * \param TxCb  DMA transfer callback function.
 */
 int32_t TWID_DMAConfigure( 
    Twid* pTwid, 
    DmadTransferCallback RxCb, 
    DmadTransferCallback TxCb )
{
    sDmad *pDmad = pTwid->pDmad;
    uint32_t dwCfg;
    uint8_t iController;
    uint8_t ucTWIID;

    /* Get the TWI ID*/
    ucTWIID = ((pTwid->pTwi) == TWI0) ? ID_TWI0 : ID_TWI1;

    /* Allocate channels */
    pTwid->dwTwidDmaRxChannel = DMAD_AllocateChannel( pDmad,
                                            ucTWIID, DMA_TRANSFER_MEMORY);

    pTwid->dwTwidDmaTxChannel = DMAD_AllocateChannel( pDmad,
                                            DMA_TRANSFER_MEMORY, ucTWIID);

    if (   pTwid->dwTwidDmaRxChannel == DMA_ALLOC_FAILED 
        || pTwid->dwTwidDmaTxChannel == DMA_ALLOC_FAILED )
    {
        return DMAD_ERROR;
    }
    /* Set RX callback */
    DMAD_SetCallback(pDmad, pTwid->dwTwidDmaRxChannel, RxCb, (void *)pTwid);
    /* Set TX callback */
    DMAD_SetCallback(pDmad, pTwid->dwTwidDmaTxChannel, TxCb, (void *)pTwid);
    /* Configure DMA RX channel */
    iController = (pTwid->dwTwidDmaRxChannel >> DMAC_CHANNEL_NUM);
    dwCfg = 0
           | DMAC_CFG_SRC_PER(
              DMAIF_GetChannelNumber( iController, ucTWIID, DMA_TRANSFER_RX ))
           | DMAC_CFG_DST_PER(
              DMAIF_GetChannelNumber( iController, ucTWIID, DMA_TRANSFER_RX ))
           | DMAC_CFG_SRC_H2SEL
           | DMAC_CFG_SOD
           | DMAC_CFG_FIFOCFG_ALAP_CFG;
    if (DMAD_PrepareChannel( pDmad, pTwid->dwTwidDmaRxChannel, dwCfg ))
        return DMAD_ERROR;
    /* Configure DMA TX channel */
    iController = (pTwid->dwTwidDmaTxChannel >> DMAC_CHANNEL_NUM);
    dwCfg = 0
           | DMAC_CFG_DST_PER(
              DMAIF_GetChannelNumber( iController, ucTWIID, DMA_TRANSFER_TX ))
           | DMAC_CFG_SRC_PER(
              DMAIF_GetChannelNumber( iController, ucTWIID, DMA_TRANSFER_TX ))
           | DMAC_CFG_DST_H2SEL
           | DMAC_CFG_SOD
           | DMAC_CFG_FIFOCFG_ALAP_CFG;
    if (DMAD_PrepareChannel( pDmad, pTwid->dwTwidDmaTxChannel, dwCfg ))
        return DMAD_ERROR;

    return 0;
}
#endif


/*----------------------------------------------------------------------------
 *        Global functions
 *----------------------------------------------------------------------------*/
/**
 * \brief Initializes a TWI driver instance, using the given TWI peripheral.
 * \note The peripheral must have been initialized properly before calling this function.
 * \param pTwid  Pointer to the Twid instance to initialize.
 * \param pTwi  Pointer to the TWI peripheral to use.
 * \param address  TWI slave address.
 * \param iaddress  Optional slave internal address.
 * \param isize  Internal address size in bytes. 
 */
void TWID_Initialize(
    Twid *pTwid, 
    Twi *pTwi,    
    uint8_t address,
    uint32_t iaddress,
    uint8_t isize,
    uint8_t transferType)
{
    TRACE_DEBUG( "TWID_Initialize()\n\r" ) ;
    assert( pTwid != NULL ) ;
    assert( pTwi != NULL ) ;

    /* Initialize driver. */
    pTwid->pTwi = pTwi;
    pTwid->pTransfer = 0;
    pTwid->address = address;
    pTwid->iaddress = iaddress;
    pTwid->isize = isize;
    pTwid->transferType = transferType;
}


/**
 * \brief Interrupt handler for a TWI peripheral. Manages asynchronous transfer
 * occuring on the bus. This function MUST be called by the interrupt service
 * routine of the TWI peripheral if asynchronous read/write are needed.
  * \param pTwid  Pointer to a Twid instance.
 */
void TWID_Handler( Twid *pTwid )
{
    uint8_t status;
    AsyncTwi *pTransfer ;
    Twi *pTwi ;

    assert( pTwid != NULL ) ;

    pTransfer = (AsyncTwi*)pTwid->pTransfer ;
    assert( pTransfer != NULL ) ;
    pTwi = pTwid->pTwi ;
    assert( pTwi != NULL ) ;

    /* Retrieve interrupt status */
    status = TWI_GetMaskedStatus(pTwi);

    /* Byte received */
    if (TWI_STATUS_RXRDY(status)) {

        pTransfer->pData[pTransfer->transferred] = TWI_ReadByte(pTwi);
        pTransfer->transferred++;

        /* check for transfer finish */
        if (pTransfer->transferred == pTransfer->num) {

            TWI_DisableIt(pTwi, TWI_IDR_RXRDY);
            TWI_EnableIt(pTwi, TWI_IER_TXCOMP);
        }
        /* Last byte? */
        else if (pTransfer->transferred == (pTransfer->num - 1)) {

            TWI_Stop(pTwi);
        }
    }
    /* Byte sent*/
    else if (TWI_STATUS_TXRDY(status)) {

        /* Transfer finished ? */
        if (pTransfer->transferred == pTransfer->num) {

            TWI_DisableIt(pTwi, TWI_IDR_TXRDY);
            TWI_EnableIt(pTwi, TWI_IER_TXCOMP);
            TWI_SendSTOPCondition(pTwi);
        }
        /* Bytes remaining */
        else {

            TWI_WriteByte(pTwi, pTransfer->pData[pTransfer->transferred]);
            pTransfer->transferred++;
        }
    }
    /* Transfer complete*/
    else if (TWI_STATUS_TXCOMP(status)) {

        TWI_DisableIt(pTwi, TWI_IDR_TXCOMP);
        pTransfer->status = 0;
        if (pTransfer->callback) {

            pTransfer->callback((Async *) pTransfer);
        }
        pTwid->pTransfer = 0;
    }
}

/**
 * \brief Asynchronously reads data from a slave on the TWI bus. An optional
 * callback function is triggered when the transfer is complete.
 * \param pTwid  Pointer to a Twid instance.
 * \param pData  Data buffer for storing received bytes.
 * \param num  Number of bytes to read.
 * \param pAsync  Asynchronous transfer descriptor.
 * \return 0 if the transfer has been started; otherwise returns a TWI error code.
 */
uint8_t TWID_Read(
    Twid *pTwid,
    uint8_t *pData,
    uint32_t num,
    Async *pAsync)
{
    Twi *pTwi;
    AsyncTwi *pTransfer;
    uint32_t timeout;
    volatile uint32_t dw=0;

    assert( pTwid != NULL ) ;
    pTwi = pTwid->pTwi;
    pTransfer = (AsyncTwi *) pTwid->pTransfer;

    assert( (pTwid->address & 0x80) == 0 ) ;
    assert( (pTwid->iaddress & 0xFF000000) == 0 ) ;
    assert( pTwid->isize < 4 ) ;

    /* Check that no transfer is already pending*/
    if (pTransfer) 
    {
        TRACE_ERROR("TWID_Read: A transfer is already pending\n\r");
        return TWID_ERROR_BUSY;
    }

    /* Set STOP signal if only one byte is sent*/
    if (num == 1) 
    {
        TWI_Stop(pTwi);
    }

    /* Asynchronous transfer*/
    if (pAsync) {

        /* Update the transfer descriptor */
        pTwid->pTransfer = pAsync;
        pTransfer = (AsyncTwi *) pAsync;
        pTransfer->status = ASYNC_STATUS_PENDING;
        pTransfer->pData = pData;
        pTransfer->num = num;
        pTransfer->transferred = 0;

        /* Enable read interrupt and start the transfer */
        TWI_EnableIt(pTwi, TWI_IER_RXRDY);
        TWI_StartRead(pTwi, pTwid->address, pTwid->iaddress, pTwid->isize); }
    /* Synchronous transfer*/
    else
    {
        switch (pTwid->transferType)
        {
           case TWID_TRANSFER_TYPE_NORMAL :
                TWI_StartRead(pTwi, pTwid->address, pTwid->iaddress, pTwid->isize);

                /* Read all bytes, setting STOP before the last byte*/
                while (num > 0) 
                {
                    /* Last byte ?*/
                    if (num == 1) 
                    {
                        TWI_Stop(pTwi);
                    }

                    /* Wait for byte then read and store it*/
                    timeout = 0;
                    while( !TWI_ByteReceived(pTwi) && (++timeout<TWITIMEOUTMAX) );

                    if (timeout == TWITIMEOUTMAX) 
                    {
                        TRACE_ERROR("TWID Timeout BR\n\r");
                    }

                    *pData++ = TWI_ReadByte(pTwi);

                    num--;
                }
            break;

            case TWID_TRANSFER_TYPE_DMA :
                if (num > 2)
                {
                    /* Read num-1 bytes to hanlde the last byte signal */
                    TWID_DMARead(pTwid, (void*)pData, num-1);

                    /* Wait for read all the bytes */
                    while (!pTwid->ucRxDone);
                }
                /*  setting STOP before the last byte */
                TWI_Stop(pTwi);

                /* Wait till last byte ready */
                while (!TWI_ByteReceived(pTwi)) ;

                /* Read last byte */
                pData[num-1] = TWI_ReadByte(pTwi);
            break;

            case TWID_TRANSFER_TYPE_PDC :
                TWI_StartRead(pTwi, pTwid->address, pTwid->iaddress, pTwid->isize);

                /* receive data from TWI pdc channel.*/
                TWI_PDC_read(pTwi, pData, num-1);

                /* Wait for the pdc transfer is completed */
                while ( !( TWI_PDCReceiveComplete( pTwi ) ) );

                /* Disable the pdc */
                TWI_PDC_Disable( pTwi );

                /* Send a stop condition before last byte is received*/
                TWI_Stop(pTwi);

                /* Receive last byte */
                TWI_PDC_read(pTwi, &pData[num-1], 1);

                /* Wait for the pdc transfer is completed */
                while ( !( TWI_PDCReceiveComplete( pTwi ) ) );

                 /* Disable the pdc */
                TWI_PDC_Disable( pTwi );
            break;

            default :
                TWI_StartRead(pTwi, pTwid->address, pTwid->iaddress, pTwid->isize);

                /* Read all bytes, setting STOP before the last byte*/
                while (num > 0) 
                {
                    /* Last byte ?*/
                    if (num == 1) 
                    {
                        TWI_Stop(pTwi);
                    }

                    /* Wait for byte then read and store it*/
                    timeout = 0;
                    while( !TWI_ByteReceived(pTwi) && (++timeout<TWITIMEOUTMAX) );

                    if (timeout == TWITIMEOUTMAX) 
                    {
                        TRACE_ERROR("TWID Timeout BR\n\r");
                    }

                    *pData++ = TWI_ReadByte(pTwi);

                    num--;
                }
            break;
        }

        /* Wait for transfer to be complete */
        timeout = 0;
        while( !TWI_TransferComplete(pTwi) && (++timeout<TWITIMEOUTMAX) );
        if (timeout == TWITIMEOUTMAX) {
            TRACE_ERROR("TWID Timeout TC\n\r");
        }
    }

    return 0;
}

/**
 * \brief Asynchronously sends data to a slave on the TWI bus. An optional callback
 * function is invoked whenever the transfer is complete.
 * \param pTwid  Pointer to a Twid instance.
 * \param pData  Data buffer for storing received bytes.
 * \param num  Data buffer to send.
 * \param pAsync  Asynchronous transfer descriptor.
 * \return 0 if the transfer has been started; otherwise returns a TWI error code.
 */
uint8_t TWID_Write(
    Twid *pTwid,
    uint8_t *pData,
    uint32_t num,
    Async *pAsync)
{
    Twi *pTwi = pTwid->pTwi;
    AsyncTwi *pTransfer = (AsyncTwi *) pTwid->pTransfer;
    uint32_t timeout;

    assert( pTwi != NULL ) ;
    assert( (pTwid->address & 0x80) == 0 ) ;
    assert( (pTwid->iaddress & 0xFF000000) == 0 ) ;
    assert( pTwid->isize < 4 ) ;

    /* Check that no transfer is already pending */
    if (pTransfer) {

        TRACE_ERROR("TWI_Write: A transfer is already pending\n\r");
        return TWID_ERROR_BUSY;
    }

    /* Asynchronous transfer */
    if (pAsync) {

        /* Update the transfer descriptor */
        pTwid->pTransfer = pAsync;
        pTransfer = (AsyncTwi *) pAsync;
        pTransfer->status = ASYNC_STATUS_PENDING;
        pTransfer->pData = pData;
        pTransfer->num = num;
        pTransfer->transferred = 0;

        /* Enable write interrupt and start the transfer */
        TWI_StartWrite(pTwi, pTwid->address, pTwid->iaddress, pTwid->isize);

        TWI_EnableIt(pTwi, TWI_IER_TXRDY);
    }
    /* Synchronous transfer*/
    else 
    {
      switch (pTwid->transferType)
      {
           case TWID_TRANSFER_TYPE_NORMAL :
                TWI_StartWrite(pTwi, pTwid->address, pTwid->iaddress, pTwid->isize);

                /* Send all bytes */
                while (num > 0)
                {
                    /* Wait before sending the next byte */
                    timeout = 0;
                    while( !TWI_ByteSent(pTwi) && (++timeout<TWITIMEOUTMAX) );
                    if (timeout == TWITIMEOUTMAX) 
                    {
                        TRACE_ERROR("TWID Timeout BS\n\r");
                    }

                    TWI_WriteByte(pTwi, *pData++);
                    num--;
                };
            break;

            case TWID_TRANSFER_TYPE_DMA :
            {
                TWI_StartWrite(pTwi, pTwid->address, pTwid->iaddress, pTwid->isize);

                /* Send data to TWI dma channel.*/
                TWID_DMAWrite(pTwid, (void*)pData, num);

                while (!pTwid->ucTxDone);
            }
            break;

            case TWID_TRANSFER_TYPE_PDC : 
                TWI_StartWrite(pTwi, pTwid->address, pTwid->iaddress, pTwid->isize);

                /* Send data to TWI pdc channel.*/
                TWI_PDC_write(pTwi, pData, num);

                /* Wait for the pdc transfer is completed */
                while ( !TWI_PDCTransferComplete(pTwi) );

                /* Disable the pdc */
                TWI_PDC_Disable( pTwi ) ;
            break;

            default :
                TWI_StartWrite(pTwi, pTwid->address, pTwid->iaddress, pTwid->isize);

                /* Send all bytes */
                while (num > 0) 
                {
                    /* Wait before sending the next byte */
                    timeout = 0;
                    while( !TWI_ByteSent(pTwi) && (++timeout<TWITIMEOUTMAX) );
                    if (timeout == TWITIMEOUTMAX) 
                    {
                        TRACE_ERROR("TWID Timeout BS\n\r");
                    }

                    TWI_WriteByte(pTwi, *pData++);
                    num--;
                };
            break;

      }

      /* Wait for actual end of transfer */
      timeout = 0;

      /* Send a STOP condition */
      TWI_SendSTOPCondition(pTwi);

      while( !TWI_TransferComplete(pTwi) && (++timeout<TWITIMEOUTMAX) );
      if (timeout == TWITIMEOUTMAX) 
      {
          TRACE_ERROR("TWID Timeout TC2\n\r");
      }
    }

    return 0;
}
