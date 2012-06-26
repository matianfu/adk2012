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

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"

#include "uip.h"
#include "uip_arp.h"

#include "tapdev.h"

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

#ifdef __ICCARM__          /* IAR */
#define __attribute__(...) /* IAR */
#endif                     /* IAR */

/* Number of buffer for RX */
#define RX_BUFFERS  16
/* Number of buffer for TX */
#define TX_BUFFERS  8

/*----------------------------------------------------------------------------
 *        Variables
 *----------------------------------------------------------------------------*/

/* The PINs for EMAC */
static const Pin gEmacPins[] = {BOARD_EMAC_PINS};

/* The EMAC driver instance */
static sEmacd gEmacd;

/* The MACB driver instance */
static Macb gMacb;

/* TX descriptors list */
#ifdef __ICCARM__          /* IAR */
#pragma data_alignment=8   /* IAR */
#endif                     /* IAR */
static sEmacTxDescriptor gTxDs[TX_BUFFERS];
/* TX callbacks list */
static fEmacdTransferCallback gTxCbs[TX_BUFFERS];
/* RX descriptors list */
#ifdef __ICCARM__          /* IAR */
#pragma data_alignment=8   /* IAR */
#endif                     /* IAR */
static sEmacRxDescriptor gRxDs[RX_BUFFERS];
/* Send Buffer */
/* Section 3.6 of AMBA 2.0 spec states that burst should not cross 1K Boundaries.
   Receive buffer manager writes are burst of 2 words => 3 lsb bits of the address
   shall be set to 0 */
#ifdef __ICCARM__          /* IAR */
#pragma data_alignment=8   /* IAR */
#endif                     /* IAR */
static uint8_t gpTxBuffer[TX_BUFFERS * EMAC_TX_UNITSIZE] __attribute__((aligned(8)));

#ifdef __ICCARM__          /* IAR */
#pragma data_alignment=8   /* IAR */
#endif                     /* IAR */
/* Receive Buffer */
static uint8_t gpRxBuffer[RX_BUFFERS * EMAC_RX_UNITSIZE] __attribute__((aligned(8)));

/* MAC address used for demo */
uint8_t gMacAddress[6] = {0x00, 0x45, 0x56, 0x78, 0x9a, 0xbc};

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * Emac interrupt handler
 */
void EMAC_IrqHandler(void)
{
    EMACD_Handler(&gEmacd);
}

/**
 * Set the MAC address of the system.
 * Should only be called before tapdev_init is called.
 */
void tapdev_setmac(u8_t *addr)
{
    gMacAddress[0] = addr[0];
    gMacAddress[1] = addr[1];
    gMacAddress[2] = addr[2];
    gMacAddress[3] = addr[3];
    gMacAddress[4] = addr[4];
    gMacAddress[5] = addr[5];
}

/**
 * Initialization for EMAC device.
 * Should be called at the beginning of the program to set up the
 * network interface.
 */
void tapdev_init(void)
{
    sEmacd    *pEmacd = &gEmacd;
    Macb      *pMacb = &gMacb;
    uint32_t  dwErrCount = 0 ;
    volatile uint32_t  dwDly;

    /* Reset PHY */
    RSTC_SetExtResetLength(RSTC, 13); /* (2^(13+1))/32768 */
    RSTC_ExtReset(RSTC);
    while(RSTC_GetNrstLevel(RSTC) == 0) {}
    /* Wait for PHY to be ready (CAT811: Max400ms) */
    dwDly = BOARD_MCK / 1000 / 3 * 400;
    while(dwDly--);

    /* Init EMAC driver structure */
    EMACD_Init(pEmacd, EMAC, ID_EMAC, 0, 0);
    EMACD_InitTransfer(pEmacd,
      gpRxBuffer, gRxDs, RX_BUFFERS,
      gpTxBuffer, gTxDs, gTxCbs, TX_BUFFERS);
    EMAC_SetAddress(gEmacd.pHw, 0, gMacAddress);

    /* Enable Interrupt */
    NVIC_EnableIRQ( EMAC_IRQn ) ;

    /* Init MACB driver */
    MACB_Init(pMacb, pEmacd, BOARD_EMAC_PHY_ADDR);

    /* PHY initialize */
    if (!MACB_InitPhy(pMacb, BOARD_MCK,
                    gEmacPins, PIO_LISTSIZE(gEmacPins)))
    {
      printf( "P: PHY Initialize ERROR!\n\r" ) ;
      return;
    }

    /* Auto Negotiate, work in RMII mode */
    if (!MACB_AutoNegotiate(pMacb, BOARD_EMAC_MODE_RMII))
    {

      printf( "P: Auto Negotiate ERROR!\n\r" ) ;
      return;
    }

    while ( MACB_GetLinkSpeed( pMacb, 1 ) == 0 )
    {
      dwErrCount++;
    }
    printf( "P: Link detected \n\r" ) ;
}

/**
 * Read for EMAC device.
 */
unsigned int tapdev_read( void )
{
    unsigned int pkt_len = 0 ;

    if ( EMACD_OK != EMACD_Poll( &gEmacd, (unsigned char*)uip_buf, UIP_CONF_BUFFER_SIZE, &pkt_len) )
    {
        pkt_len = 0 ;
    }

    return pkt_len ;
}

/**
 * Send to EMAC device
 */
void tapdev_send( void )
{
    unsigned char emac_rc ;

    emac_rc = EMACD_Send( &gEmacd, (void*)uip_buf, uip_len, NULL) ;

    if ( emac_rc != EMACD_OK )
    {
        TRACE_ERROR( "E: Send, rc 0x%x\n\r", emac_rc ) ;
    }
}

