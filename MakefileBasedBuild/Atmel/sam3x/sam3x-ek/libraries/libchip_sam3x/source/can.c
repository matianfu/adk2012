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
   *  \file
   *
   *  Implementation of the CAN low level functions.
   */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/* CAN state */
#define CAN_DISABLED              0
#define CAN_HALTED                1
#define CAN_IDLE                  2
#define CAN_SENDING               3
#define CAN_RECEIVING             4

/* MOT: Mailbox Object Type */
#define CAN_MOT_DISABLE           0 /* Mailbox is disabled */
#define CAN_MOT_RECEPT            1 /* Reception Mailbox */
#define CAN_MOT_RECEPT_OW         2 /* Reception mailbox with overwrite */
#define CAN_MOT_TRANSMIT          3 /* Transmit mailbox */
#define CAN_MOT_CONSUMER          4 /* Consumer mailbox */
#define CAN_MOT_PRODUCER          5 /* Producer mailbox */

/* CAN synchronisation state */
#define CAN_SYNC_NOK              0
#define CAN_SYNC_OK               1
/* CAN timeout for sync */
#define CAN_TIMEOUT               100000

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/**
 *  CAN Error Detection
 *
 *  \param pCan       Can peripheral
 *  \param dwStatus   Status register content
 */
static void CAN_ErrorHandling( Can* pCan, uint32_t dwStatus )
{
    if ( (dwStatus & CAN_SR_ERRA) ==  CAN_SR_ERRA )
    {
        TRACE_ERROR( "(CAN) CAN is in Error Active mode\n\r" ) ;
    }
    else
    {
        if ( (dwStatus & CAN_SR_ERRP) ==  CAN_SR_ERRP )
        {
            TRACE_ERROR( "(CAN) CAN is in Error Passive mode\n\r" ) ;
        }
        else
        {
            if ( (dwStatus & CAN_SR_BOFF) ==  CAN_SR_BOFF )
            {
                TRACE_ERROR( "(CAN) CAN is in Buff Off mode\n\r" ) ;
                /* CAN reset */
                TRACE_ERROR( "(CAN) CAN reset\n\r" ) ;
                /* CAN Controller Disable */
                pCan->CAN_MR &= ~CAN_MR_CANEN ;
                /* CAN Controller Enable */
                pCan->CAN_MR |= CAN_MR_CANEN ;
            }
        }
    }

    /* Error for Frame dataframe */
    /* CRC error */
    if ( (dwStatus & CAN_SR_CERR) ==  CAN_SR_CERR )
    {
        TRACE_ERROR( "(CAN) CRC Error\n\r" ) ;
    }
    else
    {
        /* Bit-stuffing error */
        if ( (dwStatus & CAN_SR_SERR) ==  CAN_SR_SERR )
        {
            TRACE_ERROR( "(CAN) Stuffing Error\n\r" ) ;
        }
        else
        {
            /* Bit error */
            if ( (dwStatus & CAN_SR_BERR) ==  CAN_SR_BERR )
            {
                TRACE_ERROR( "(CAN) Bit Error\n\r" ) ;
            }
            else
            {
                /* Form error */
                if ( (dwStatus & CAN_SR_FERR) ==  CAN_SR_FERR )
                {
                    TRACE_ERROR( "(CAN) Form Error\n\r" ) ;
                }
                else
                {
                    /* Acknowledgment error */
                    if ( (dwStatus & CAN_SR_AERR) ==  CAN_SR_AERR )
                    {
                        TRACE_ERROR( "(CAN) Acknowledgment Error\n\r" ) ;
                    }
                }
            }
        }
    }

    /* Error interrupt handler
     * Represent the current status of the CAN bus and are not latched.
     * See CAN, par. Error Interrupt Handler
     * CAN_SR_WARN
     * CAN_SR_ERRA
     */
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * Generic CAN Interrupt handler
 *
 * \param pCan      CAN peripheral
 * \param pTransfer Xfer parameters
 */
extern void CAN_Handler( Can* pCan, SCanTransfer* pTransfer )
{
    uint32_t dwStatus ;
    uint32_t dwMSR ;
    uint32_t dwMid ;
    uint32_t dwMessageMode ;
    uint32_t dwMailbox ;
    uint8_t  ucState = CAN_DISABLED ;

    ucState = pTransfer->state ;

    dwStatus = (pCan->CAN_SR) ;
    dwStatus &= (pCan->CAN_IMR) ;
    pCan->CAN_IDR = dwStatus ;

    TRACE_DEBUG( "CAN status=0x%X\n\r", status ) ;
    if ( dwStatus & CAN_IDR_WAKEUP )
    {
        pTransfer->sync_can = CAN_SYNC_OK ;
        pTransfer->state = CAN_IDLE ;
    }
    /* Mailbox event ? */
    else
    {
        if ( (dwStatus&0x0000FFFF) != 0 )
        {
            TRACE_DEBUG( "Mailbox event\n\r" ) ;

            /* Handle Mailbox interrupts */
            for ( dwMailbox = 0 ; dwMailbox < CAN_MB_NUMBER ; dwMailbox++ )
            {
                dwMSR = pCan->CAN_MB[dwMailbox].CAN_MSR ;

                if ( (dwMSR & CAN_MSR_MRDY) == CAN_MSR_MRDY )
                {
                    /* Mailbox object type */
                    dwMessageMode = (pCan->CAN_MB[dwMailbox].CAN_MMR & CAN_MMR_MOT_Msk) >> CAN_MMR_MOT_Pos ;
                    TRACE_DEBUG( "message_mode 0x%X\n\r", dwMessageMode ) ;
                    TRACE_DEBUG( "dwMailbox 0x%X\n\r", dwMailbox ) ;

                    if ( dwMessageMode == 0 )
                    {
                        TRACE_ERROR( "Error in MOT\n\r" ) ;
                    }
                    else
                    {
                        if ( ( dwMessageMode == CAN_MOT_RECEPT ) || ( dwMessageMode == CAN_MOT_RECEPT_OW ) || ( dwMessageMode == CAN_MOT_PRODUCER ) )
                        {
                            TRACE_DEBUG( "Mailbox is in RECEPTION\n\r" ) ;
                            TRACE_DEBUG( "Length 0x%X\n\r", (can_msr>>16)&0xF ) ;
                            TRACE_DEBUG( "CAN Mailbox ID 0x%X\n\r", (pCan->CAN_MB[dwMailbox].dwMid & CAN_MID_MIDvA_Msk) >> CAN_MID_MIDvA_Pos ) ;

                            /* Read CAN data */
                            pTransfer->data_low_reg = pCan->CAN_MB[dwMailbox].CAN_MDL ;
                            pTransfer->data_high_reg = pCan->CAN_MB[dwMailbox].CAN_MDH ;
                            pTransfer->size = (pCan->CAN_MB[dwMailbox].CAN_MSR&CAN_MSR_MDLC_Msk)>>CAN_MSR_MDLC_Pos ;
                            pTransfer->mailbox_number = dwMailbox ;
                            dwMid = pCan->CAN_MB[dwMailbox].CAN_MID ;
                            if( (dwMid & CAN_MID_MIDE) == CAN_MID_MIDE )
                            {
                                pTransfer->identifier = dwMid & CAN_MID_MIDvA_Msk & CAN_MID_MIDvB_Msk;
                            }
                            else
                            {
                                pTransfer->identifier = (dwMid & CAN_MID_MIDvA_Msk) >> 18 ;
                            }
                            ucState = CAN_IDLE ;

                            /* Message Data has been received */
                            pCan->CAN_MB[dwMailbox].CAN_MCR = CAN_MCR_MTCR ;
                        }
                        else
                        {
                            TRACE_DEBUG( "Mailbox is in TRANSMIT\n\r" ) ;
                            TRACE_DEBUG( "Length 0x%X\n\r", (can_msr>>16)&0xF ) ;
                            ucState = CAN_IDLE ;
                        }
                    }
                }
            }

            pTransfer->state = ucState ;
        }
    }

    if ( (dwStatus & 0xFFCF0000) != 0 )
    {
        CAN_ErrorHandling( pCan, dwStatus ) ;
    }
}

/**
 *  Configure the corresponding mailbox
 *  \param pCan      CAN peripheral
 *  \param pTransfer can transfer structure
 */
extern void CAN_InitMailboxRegisters( Can* pCan, SCanTransfer* pTransfer )
{
    CanMb*   pCAN_Mailbox ;
    uint32_t dwTemp;

    pCAN_Mailbox = pCan->CAN_MB + pTransfer->mailbox_number ;

    dwTemp = 1 << (pTransfer->mailbox_number) ;
    pTransfer->mailbox_in_use |= dwTemp;

    /* MailBox Control Register */
    pCAN_Mailbox->CAN_MCR = 0x0 ;
    /* MailBox Mode Register */
    pCAN_Mailbox->CAN_MMR = 0x00 ;
    /* CAN Message Acceptance Mask Register */
    pCAN_Mailbox->CAN_MAM = pTransfer->acceptance_mask_reg ;

    /* MailBox ID Register */
    /* Disable the mailbox before writing to CAN_MIDx registers */
    if( (pTransfer->identifier & CAN_MAM_MIDE) == CAN_MAM_MIDE )
    {
        /* Extended */
        pCAN_Mailbox->CAN_MAM |= CAN_MAM_MIDE ;
    }
    else
    {
        pCAN_Mailbox->CAN_MAM &= ~CAN_MAM_MIDE ;
    }
    pCAN_Mailbox->CAN_MID = pTransfer->identifier;

    /* MailBox Mode Register */
    pCAN_Mailbox->CAN_MMR = pTransfer->mode_reg ;
    /* MailBox Data Low Register */
    pCAN_Mailbox->CAN_MDL = pTransfer->data_low_reg ;
    /* MailBox Data High Register */
    pCAN_Mailbox->CAN_MDH = pTransfer->data_high_reg ;
    /* MailBox Control Register */
    pCAN_Mailbox->CAN_MCR = pTransfer->control_reg ;
}

/**
 *  Reset the MBx
 *  \param pCan      CAN peripheral
 *  \param pTransfer can transfer structure
 */
extern void CAN_ResetAllMailbox( Can* pCan, SCanTransfer* pTransfer)
{
    uint32_t dw ;

    if ( pTransfer != NULL )
    {
        CAN_TransferReset( pCan, pTransfer ) ;

        for ( dw = 0 ; dw < CAN_MB_NUMBER ; dw++ )
        {
            pTransfer->mailbox_number = dw ;
            pTransfer->mode_reg = CAN_MMR_MOT_MB_DISABLED ;
            pTransfer->acceptance_mask_reg = 0x00000000 ;
            pTransfer->identifier = 0x00000000 ;
            pTransfer->data_low_reg = 0x00000000 ;
            pTransfer->data_high_reg = 0x00000000 ;
            pTransfer->control_reg = 0x00000000 ;

            CAN_InitMailboxRegisters( pCan, pTransfer ) ;
        }
    }
}

/**
 *  CAN reset Transfer descriptor
 *  \param pCan      CAN peripheral
 *  \param pTransfer can transfer structure
 */
extern void CAN_TransferReset( Can* pCan, SCanTransfer* pTransfer )
{
    pCan = pCan; /* stop warning */

    pTransfer->state = CAN_IDLE ;
    pTransfer->mailbox_number = 0 ;
    pTransfer->sync_can = 0 ;
    pTransfer->mode_reg = 0 ;
    pTransfer->acceptance_mask_reg = 0 ;
    pTransfer->identifier = 0 ;
    pTransfer->data_low_reg = 0x00000000 ;
    pTransfer->data_high_reg = 0x00000000 ;
    pTransfer->control_reg = 0x00000000 ;
    pTransfer->mailbox_in_use = 0 ;
    pTransfer->size = 0 ;
}

/**
 *  Wait for CAN synchronisation
 *  \param pCan      CAN peripheral
 *  \param pTransfer can transfer structure
 *
 *  \return 1 for good initialisation, otherwise 0
 */

static uint32_t CAN_Synchronisation( Can* pCan, SCanTransfer *pTransfer )
{
    uint32_t dwTick = 0 ;

    TRACE_INFO( "CAN_Synchronisation\n\r" ) ;

    pTransfer->sync_can = CAN_SYNC_NOK ;

    /* Enable CAN and Wait for WakeUp Interrupt */
    pCan->CAN_IER = CAN_IER_WAKEUP ;
    /* CAN Controller Enable */
    pCan->CAN_MR = CAN_MR_CANEN ;
    /* Enable Autobaud/Listen mode */
    /* dangerous, CAN not answer in this mode */

    while( (pTransfer->sync_can != CAN_SYNC_OK) && (dwTick < CAN_TIMEOUT) )
    {
        dwTick++ ;
    }

    if ( dwTick == CAN_TIMEOUT )
    {
        TRACE_ERROR( "CAN Initialisations FAILED\n\r" ) ;
        return 0 ;
    }
    else
    {
        TRACE_INFO( "CAN Initialisations Completed\n\r" ) ;
    }

    return 1 ;
}

/**
 *  Write a CAN transfer
 *  \param pCan      CAN peripheral
 *  \param pTransfer can transfer structure
 *
 *  \return return CAN_STATUS_SUCCESS if command passed, otherwise
 *          return CAN_STATUS_LOCKED
 */
extern uint32_t CAN_Write( Can* pCan, SCanTransfer *pTransfer )
{
    if ( pTransfer->state == CAN_RECEIVING )
    {
        pTransfer->state = CAN_IDLE ;
    }

    if ( pTransfer->state != CAN_IDLE )
    {
        return CAN_STATUS_LOCKED ;
    }

    TRACE_DEBUG( "CAN_Write\n\r" ) ;
    pTransfer->state = CAN_SENDING ;

    pCan->CAN_TCR = pTransfer->mailbox_in_use ;
    pCan->CAN_IER = pTransfer->mailbox_in_use ;

    return CAN_STATUS_SUCCESS ;
}

/**
 *  Read a CAN transfer
 *  \param pCan      CAN peripheral
 *  \param pTransfer can transfer structure
 *
 *  \return return CAN_STATUS_SUCCESS if command passed, otherwise
 *          return CAN_STATUS_LOCKED
 */
extern uint32_t CAN_Read( Can* pCan, SCanTransfer *pTransfer )
{
    if ( pTransfer->state != CAN_IDLE )
    {
        return CAN_STATUS_LOCKED ;
    }

    TRACE_DEBUG( "CAN_Read\n\r" ) ;
    pTransfer->state = CAN_RECEIVING ;

    /* Enable interrupt */
    pCan->CAN_IER = pTransfer->mailbox_in_use ;

    return CAN_STATUS_SUCCESS ;
}

/**
 *  Test if CAN is in IDLE state
 *
 *  \param pTransfer can transfer structure
 *
 *  \return return 0 if CAN is in IDLE, otherwise return 1
 */
extern uint32_t CAN_IsInIdle( Can* pCan, SCanTransfer* pTransfer )
{
  pCan = pCan; /* stop warning */
  return ( pTransfer->state != CAN_IDLE ) ;
}

/**
 *  Disable CAN and enter in low power
 */
extern void CAN_Disable( Can* pCan )
{
    /* Disable the interrupt on the interrupt controller */
    if ( pCan == CAN0 )
    {
        NVIC_DisableIRQ( CAN0_IRQn ) ;
    }
    else
    {
        NVIC_DisableIRQ( CAN1_IRQn ) ;
    }

    /* Disable all IT */
    pCan->CAN_IDR = 0x1FFFFFFF ;

    /* Enable Low Power mode */
    pCan->CAN_MR |= CAN_MR_LPM ;

    /* Disable the CAN controller peripheral clock */
    if ( pCan == CAN0 )
    {
        PMC_DisablePeripheral( ID_CAN0 ) ;
    }
    else
    {
        PMC_DisablePeripheral( ID_CAN1 ) ;
    }
}

/**
 *  Baudrate processing
 *
 *  \param pCan CAN base address
 *  \param baudrate Baudrate value (kB/s)
 *                  allowed values: 1000, 800, 500, 250, 125, 50, 25, 10
 *
 *  \return return 1 in success, otherwise return 0
 */
static uint32_t CAN_BaudRateCalculate( Can* pCan, uint32_t dwMCK, uint32_t dwBaudrate )
{
    uint32_t BRP ;
    uint32_t PROPAG ;
    uint32_t PHASE1;
    uint32_t PHASE2 ;
    uint32_t SJW ;
    uint32_t t1t2 ;
    uint32_t dwTimeQuanta ;

    pCan->CAN_BR = 0 ;

    if ( dwBaudrate == 1000 )
    {
        dwTimeQuanta = 8 ;
    }
    else
    {
        dwTimeQuanta = 16 ;
    }

    BRP = (dwMCK / (dwBaudrate*1000*dwTimeQuanta))-1 ;
    /*TRACE_DEBUG("BRP = 0x%X\n\r", BRP); */
    /* timing Delay:
     * Delay Bus Driver: 50 ns
     * Delay Receiver:   30 ns
     * Delay Bus Line (20m):  110 ns
     */
    if ( (dwTimeQuanta*dwBaudrate*2*(50+30+110)/1000000) >= 1 )
    {
        PROPAG = (dwTimeQuanta*dwBaudrate*2*(50+30+110)/1000000)-1 ;
    }
    else
    {
        PROPAG = 0 ;
    }
    /*TRACE_DEBUG("PROPAG = 0x%X\n\r", PROPAG); */

    t1t2 = dwTimeQuanta-1-(PROPAG+1) ;
    /*TRACE_DEBUG("t1t2 = 0x%X\n\r", t1t2); */

    if ( (t1t2 & 0x01) == 0x01 )
    {
        /* ODD */
        /*TRACE_DEBUG("ODD\n\r"); */
        PHASE1 = ((t1t2-1)/2)-1 ;
        PHASE2 = PHASE1+1 ;
    }
    else
    {
        /* EVEN */
        /*TRACE_DEBUG("EVEN\n\r"); */
        PHASE1 = (t1t2/2)-1 ;
        PHASE2 = PHASE1 ;
    }
    /*TRACE_DEBUG("PHASE1 = 0x%X\n\r", PHASE1); */
    /*TRACE_DEBUG("PHASE2 = 0x%X\n\r", PHASE2); */

    if ( 1 > (4/(PHASE1+1)) )
    {
        /*TRACE_DEBUG("4*Tcsc\n\r"); */
        SJW = 3 ;
    }
    else
    {
        /*TRACE_DEBUG("Tphs1\n\r"); */
        SJW = PHASE1 ;
    }
    /*TRACE_DEBUG("SJW = 0x%X\n\r", SJW); */
    /* Verif */
    if ( BRP == 0 )
    {
        TRACE_DEBUG( "BRP = 0 is not authorized\n\r" ) ;

        return 0 ;
    }

    if ( (PROPAG + PHASE1 + PHASE2) != (dwTimeQuanta-4) )
    {
        TRACE_DEBUG( "Pb (PROPAG + PHASE1 + PHASE2) = %d\n\r", PROPAG + PHASE1 + PHASE2 ) ;
        TRACE_DEBUG( "with TimeQuanta-4 = %d\n\r", dwTimeQuanta-4 ) ;

        return 0 ;
    }

    pCan->CAN_BR = CAN_BR_PHASE2( PHASE2 ) | CAN_BR_PHASE1( PHASE1 ) | CAN_BR_PROPAG( PROPAG ) | CAN_BR_SJW( SJW ) | CAN_BR_BRP( BRP ) ;

    return 1 ;

}

/**
 *  Init of the CAN peripheral
 *  \param baudrate Baudrate value (kB/s)
 *                  allowed values: 1000, 800, 500, 250, 125, 50, 25, 10
 *  \param canTransfer0 CAN0 structure transfer
 *  \param canTransfer1 CAN1 structure transfer
 *  \return return 1 if CAN has good baudrate and CAN is synchronized,
 *          otherwise return 0
 */
extern uint32_t CAN_Init( Can* pCan, uint32_t dwMCK, uint32_t dwBaudrate, SCanTransfer *pTransfer)
{
    /* Enable the CAN controller peripheral clock */
    if ( pCan == CAN0 )
    {
        PMC_EnablePeripheral( ID_CAN0 ) ;
    }
    else
    {
        PMC_EnablePeripheral( ID_CAN1 ) ;
    }

    /* disable all IT */
    pCan->CAN_IDR = 0x1FFFFFFF ;

    /* Enable the interrupt on the interrupt controller */
    NVIC_EnableIRQ( (pCan == CAN0) ? CAN0_IRQn : CAN1_IRQn ) ;

    if ( CAN_BaudRateCalculate( pCan, dwMCK, dwBaudrate ) == 0 )
    {
        /* Baudrate problem */
        TRACE_DEBUG( "Baudrate problem\r\n" ) ;

        return 0 ;
    }

    CAN_ResetAllMailbox( pCan, pTransfer);

    /* Enable the interrupt with all error cases */
    pCan->CAN_IER = CAN_IER_CERR |   /* (CAN) CRC Error */
                    CAN_IER_SERR |   /* (CAN) Stuffing Error */
                    CAN_IER_BERR |   /* (CAN) Bit Error */
                    CAN_IER_FERR |   /* (CAN) Form Error */
                    CAN_IER_AERR ;   /* (CAN) Acknowledgment Error */

    /* Wait for CAN synchronisation */
    if ( CAN_Synchronisation( pCan, pTransfer ) == 1 )
    {
        return 1u ;
    }
    else
    {
        return 0u ;
    }
}

