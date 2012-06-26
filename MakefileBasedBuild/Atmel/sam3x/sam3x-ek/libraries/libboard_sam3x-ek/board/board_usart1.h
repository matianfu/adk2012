#ifndef _BOARD_USART1_
#define _BOARD_USART1_

/**
 * \file
 * USART1
 * - \ref PIN_USART1_RXD
 * - \ref PIN_USART1_TXD
 * - \ref PIN_USART1_CTS
 * - \ref PIN_USART1_RTS
 * - \ref PIN_USART1_SCK
 *
 */

// ----------------------------------------------------------------------------------------------------------
// USART1
// ----------------------------------------------------------------------------------------------------------
/** USART1 pin RX */
#define PIN_USART1_RXD    {PIO_PA12A_RXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin TX */
#define PIN_USART1_TXD    {PIO_PA13A_TXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin CTS */
#define PIN_USART1_CTS    {PIO_PA15A_CTS1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin RTS */
#define PIN_USART1_RTS    {PIO_PA14A_RTS1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin SCK */
#define PIN_USART1_SCK    {PIO_PA16A_SCK1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

#endif /* _BOARD_USART1_ */
