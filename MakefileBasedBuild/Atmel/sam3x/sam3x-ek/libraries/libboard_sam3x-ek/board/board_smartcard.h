#ifndef _BOARD_SMARTCARD_
#define _BOARD_SMARTCARD_

/**
 * \file
 * SmartCard
 * - \ref SMARTCARD_CONNECT_PIN
 * - \ref PIN_ISO7816_RSTMC
 * - \ref PINS_ISO7816
 *
 */

// ----------------------------------------------------------------------------------------------------------
// SMARTCARD
// ----------------------------------------------------------------------------------------------------------
/// Smartcard detection pin
//#define SMARTCARD_CONNECT_PIN {1 << 13, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}

/// PIN used for reset the smartcard
#define PIN_ISO7816_RSTMC       {1 << 11, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
/// Pins used for connect the smartcard
#define PINS_ISO7816            PIN_USART1_TXD, PIN_USART1_SCK, PIN_ISO7816_RSTMC

#endif /* _BOARD_SMARTCARD_ */
