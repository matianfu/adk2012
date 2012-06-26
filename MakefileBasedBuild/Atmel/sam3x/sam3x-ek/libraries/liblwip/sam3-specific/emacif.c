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
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "emacif" to replace it with
 * something that better describes your network interface.
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"

#include <string.h>

#include "lwip/opt.h"

#include "emacif.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "netif/etharp.h"

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

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

static struct emacif Emacif_config;

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

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/* Forward declarations. */
static void  emacif_input(struct netif *netif);
static err_t emacif_output(struct netif *netif, struct pbuf *p,
             struct ip_addr *ipaddr);


static void
low_level_init(struct netif *netif)
{
  struct emacif *emacif = netif->state;

  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;
  /* set MAC hardware address */
  netif->hwaddr[0] = emacif->ethaddr.addr[0];
  netif->hwaddr[1] = emacif->ethaddr.addr[1];
  netif->hwaddr[2] = emacif->ethaddr.addr[2];
  netif->hwaddr[3] = emacif->ethaddr.addr[3];
  netif->hwaddr[4] = emacif->ethaddr.addr[4];
  netif->hwaddr[5] = emacif->ethaddr.addr[5];
  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  netif->flags = NETIF_FLAG_BROADCAST;

  /* Do whatever else is needed to initialize interface. */
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
    EMAC_SetAddress(gEmacd.pHw, 0, Emacif_config.ethaddr.addr);

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
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this emacif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct emacif *emacif = netif->state;
  struct pbuf *q;
  char buf[1514];
  char *bufptr = &buf[0];
  unsigned char rc;

#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE);    /* drop the padding word */
#endif

  for(q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */

    /* send data from(q->payload, q->len); */
    memcpy(bufptr, q->payload, q->len);
    bufptr += q->len;
  }

  /* signal that packet should be sent(); */
  rc = EMACD_Send(&gEmacd, buf, p->tot_len, NULL);
  if (rc != EMACD_OK) {
    return ERR_BUF;
  }

#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE);     /* reclaim the padding word */
#endif

  LINK_STATS_INC(link.xmit);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this emacif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct emacif *emacif = netif->state;
  struct pbuf *p, *q;
  u16_t len;
  unsigned char buf[1514];
  char *bufptr = &buf[0];

  unsigned int frmlen;
  unsigned char rc;

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  rc = EMACD_Poll(&gEmacd, buf, sizeof(buf), &frmlen);
  if (rc != EMACD_OK)
    return NULL;

  len = frmlen;

#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE;      /* allow room for Ethernet padding */
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if (p != NULL) {

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE);          /* drop the padding word */
#endif

    /* We iterate over the pbuf chain until we have read the entire
     * packet into the pbuf. */
    for(q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
       * available data in the pbuf is given by the q->len
       * variable. */
      /* read data into(q->payload, q->len); */
      memcpy(q->payload, bufptr, q->len);
      bufptr += q->len;
    }
    /* acknowledge that packet has been read(); */

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE);           /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.recv);
  } else {
    /* drop packet(); */
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
  }

  return p;
}

/**
 * This function is called by the TCP/IP stack when an IP packet
 * should be sent. It calls the function called low_level_output() to
 * do the actual transmission of the packet.
 *
 */

static err_t
emacif_output(struct netif *netif, struct pbuf *p,
      struct ip_addr *ipaddr)
{

 /* resolve hardware address, then send (or queue) packet */
  return etharp_output(netif, p, ipaddr);
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this emacif
 */

static void
emacif_input(struct netif *netif)
{
  struct emacif *emacif;
  struct eth_hdr *ethhdr;
  struct pbuf *p;

  emacif = netif->state;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);
  /* no packet could be read, silently ignore this */
  if (p == NULL) return;
  /* points to packet payload, which starts with an Ethernet header */
  ethhdr = p->payload;

  switch (htons(ethhdr->type)) {
  /* IP packet? */
  case ETHTYPE_IP:
    /* skip Ethernet header */
    pbuf_header(p, -sizeof(struct eth_hdr));
    /* pass to network layer */
    netif->input(p, netif);
    break;

  case ETHTYPE_ARP:
    /* pass p to ARP module  */
    etharp_arp_input(netif, &emacif->ethaddr, p);
    break;
  default:
    pbuf_free(p);
    p = NULL;
    break;
  }
}

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
 * Should only be called before emacif_init is called.
 * The stack calls emacif_init after the user calls netif_add
 *
 */

void
emacif_setmac(u8_t *addr)
{
    Emacif_config.ethaddr.addr[0] = addr[0];
    Emacif_config.ethaddr.addr[1] = addr[1];
    Emacif_config.ethaddr.addr[2] = addr[2];
    Emacif_config.ethaddr.addr[3] = addr[3];
    Emacif_config.ethaddr.addr[4] = addr[4];
    Emacif_config.ethaddr.addr[5] = addr[5];
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */

err_t
emacif_init(struct netif *netif)
{
  struct emacif *emacif;

  emacif = &Emacif_config;

  if (emacif == NULL)
  {
      LWIP_DEBUGF(NETIF_DEBUG, ("emacif_init: out of memory\n"));
      return ERR_MEM;
  }

  netif->state = emacif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = emacif_output;
  netif->linkoutput = low_level_output;

  low_level_init(netif);

  etharp_init();

  return ERR_OK;
}

/**
 * Polling task
 * Should be called periodically
 *
 */
void emacif_poll(struct netif *netif)
{
    emacif_input(netif);
}

