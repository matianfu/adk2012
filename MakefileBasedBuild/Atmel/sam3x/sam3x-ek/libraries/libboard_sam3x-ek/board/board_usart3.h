#ifndef _BOARD_USART3_
#define _BOARD_USART3_

/**
 * \file
 * USART3
 * - \ref PIN_USART3_RXD
 * - \ref PIN_USART3_TXD
 * - \ref PIN_USART3_CTS
 * - \ref PIN_USART3_RTS
 * - \ref PIN_USART3_SCK
 *
 */

// ----------------------------------------------------------------------------------------------------------
// USART3
// ----------------------------------------------------------------------------------------------------------
/** USART1 pin RX */
#define PIN_USART3_RXD    {PIO_PD5B_RXD3, PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT}
/** USART1 pin TX */
#define PIN_USART3_TXD    {PIO_PD4B_TXD3, PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT}
/** USART1 pin CTS */
#define PIN_USART3_CTS    {PIO_PF4A_CTS3, PIOF, ID_PIOF, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin RTS */
#define PIN_USART3_RTS    {PIO_PF5A_RTS3, PIOF, ID_PIOF, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin SCK */
#define PIN_USART3_SCK    {PIO_PE16B_SCK3, PIOE, ID_PIOE, PIO_PERIPH_B, PIO_DEFAULT}

#endif /* _BOARD_USART3_ */
