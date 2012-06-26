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
#ifndef _BOARD_EMAC_
#define _BOARD_EMAC_

/**
 * \file 
 * EMAC
 *
 * - BOARD_EMAC_EREFCK
 * - BOARD_EMAC_ETXEN
 * - BOARD_EMAC_ETX0
 * - BOARD_EMAC_ETX1
 * - BOARD_EMAC_ECRSDV
 * - BOARD_EMAC_ERX0
 * - BOARD_EMAC_ERX1
 * - BOARD_EMAC_ERXER
 * - BOARD_EMAC_EMDC
 * - BOARD_EMAC_EMDIO
 *
 * - BOARD_EMAC_PINS: One single define for all MACB pins
 * - BOARD_EMAC_PHY_ADDR: Phy MAC address
 * - BOARD_EMAC_MODE_RMII: Enable RMII connection with the PHY
 */

/** EMAC pin EREFCK */
#define BOARD_EMAC_EREFCK  {PIO_PB0, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** EMAC pin ETXEN */
#define BOARD_EMAC_ETXEN   {PIO_PB1A_ETXEN, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** EMAC pin ETX0 */
#define BOARD_EMAC_ETX0    {PIO_PB2A_ETX0, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** EMAC pin ETX1 */
#define BOARD_EMAC_ETX1    {PIO_PB3A_ETX1, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** EMAC pin ECRSDV */
#define BOARD_EMAC_ECRSDV  {PIO_PB4A_ECRSDV_ERXDV, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** EMAC pin ERX0 */
#define BOARD_EMAC_ERX0    {PIO_PB5A_ERX0, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** EMAC pin ERX1 */
#define BOARD_EMAC_ERX1    {PIO_PB6A_ERX1, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** EMAC pin ERXER */
#define BOARD_EMAC_ERXER   {PIO_PB7A_ERXER, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** EMAC pin EMDC */
#define BOARD_EMAC_EMDC    {PIO_PB8A_EMDC, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** EMAC pin EMDIO */
#define BOARD_EMAC_EMDIO   {PIO_PB9A_EMDIO, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}

/** EMAC pins */
#define BOARD_EMAC_PINS     \
                            BOARD_EMAC_EREFCK, \
                            BOARD_EMAC_ETX0, \
                            BOARD_EMAC_ETX1, \
                            BOARD_EMAC_ETXEN, \
                            BOARD_EMAC_ECRSDV, \
                            BOARD_EMAC_ERX0, \
                            BOARD_EMAC_ERX1, \
                            BOARD_EMAC_ERXER, \
                            BOARD_EMAC_EMDC, \
                            BOARD_EMAC_EMDIO

/** EMAC PHY address */
#define BOARD_EMAC_PHY_ADDR  0
/** EMAC RMII mode */
#define BOARD_EMAC_MODE_RMII 1

#endif /* _BOARD_EMAC_ */
