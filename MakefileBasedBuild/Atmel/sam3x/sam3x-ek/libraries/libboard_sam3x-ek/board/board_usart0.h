#ifndef _BOARD_USART0_
#define _BOARD_USART0_

/**
 * \file
 * USART0
 * - \ref PIN_USART0_RXD
 * - \ref PIN_USART0_TXD
 * - \ref PIN_USART0_CTS
 * - \ref PIN_USART0_RTS
 * - \ref PIN_USART0_SCK
 *
 * - \ref PIN_USART0_EN
 */

// ----------------------------------------------------------------------------------------------------------
// USART0
// Checked Tvd
// ----------------------------------------------------------------------------------------------------------
/** USART0 pin RX */
#define PIN_USART0_RXD    {PIO_PA10A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART0 pin TX */
#define PIN_USART0_TXD    {PIO_PA11A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART0 pin CTS */
#define PIN_USART0_CTS    {PIO_PB26A_CTS0, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** USART0 pin RTS */
#define PIN_USART0_RTS    {PIO_PB25A_RTS0, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** USART0 pin SCK */
#define PIN_USART0_SCK    {PIO_PA17B_SCK0, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}

/** USART0 pin ENABLE */
#define PIN_USART0_EN     {PIO_PE14, PIOE, ID_PIOE, PIO_OUTPUT_0, PIO_DEFAULT}


#endif /* _BOARD_USART0_ */
