#ifndef _BOARD_CAN_
#define _BOARD_CAN_

/**
 * \file 
 * CAN
 * \ref PIN_CAN0_TRANSCEIVER_RXEN
 * \ref PIN_CAN0_TRANSCEIVER_RS
 * \ref PIN_CAN0_TXD
 * \ref PIN_CAN0_RXD
 * \ref PINS_CAN0
 *
 * \ref PIN_CAN1_TRANSCEIVER_RXEN
 * \ref PIN_CAN1_TRANSCEIVER_RS
 * \ref PIN_CAN1_TXD
 * \ref PIN_CAN1_RXD
 * \ref PINS_CAN1
 */

// ----------------------------------------------------------------------------------------------------------
// CAN
// checked - tvd
// ----------------------------------------------------------------------------------------------------------

/** CAN0 RXEN: Select input for high speed mode or ultra low current sleep mode */
#define PIN_CAN0_TRANSCEIVER_RXEN { PIO_PB21, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT }

/** CAN0 RS: Select input for high speed mode or low-current standby mode */
#define PIN_CAN0_TRANSCEIVER_RS   { PIO_PB20, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT }

/** CAN0 TXD: Transmit data input */
#define PIN_CAN0_TXD { PIO_PA0A_CANTX0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }

/** CAN0 RXD: Receive data output */
#define PIN_CAN0_RXD { PIO_PA1A_CANRX0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT }

/** List of all CAN0 definitions. */
#define PINS_CAN0    PIN_CAN0_TXD, PIN_CAN0_RXD

/** CAN1 RXEN: Select input for high speed mode or ultra low current sleep mode */
#define PIN_CAN1_TRANSCEIVER_RXEN { PIO_PE16, PIOE, ID_PIOE, PIO_OUTPUT_1, PIO_DEFAULT }

/** CAN1 RS: Select input for high speed mode or low-current standby mode */
#define PIN_CAN1_TRANSCEIVER_RS   { PIO_PE15, PIOE, ID_PIOE, PIO_OUTPUT_0, PIO_DEFAULT }

/** CAN1 TXD: Transmit data input */
#define PIN_CAN1_TXD { PIO_PB14A_CANTX1, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT }

/** CAN1 RXD: Receive data output */
#define PIN_CAN1_RXD { PIO_PB15A_CANRX1, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT }

/** List of all CAN1 definitions. */
#define PINS_CAN1    PIN_CAN1_TXD, PIN_CAN1_RXD

#endif /* _BOARD_CAN_ */
