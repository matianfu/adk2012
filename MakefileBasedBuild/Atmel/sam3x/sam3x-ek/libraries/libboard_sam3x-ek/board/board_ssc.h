#ifndef _BOARD_SSC_
#define _BOARD_SSC_

/**
 * \file
 * SSC
 * - \ref PIN_SSC_TD
 * - \ref PIN_SSC_TK
 * - \ref PIN_SSC_TF
 * - \ref PINS_SSC_CODEC
 *
 */
 
// ----------------------------------------------------------------------------------------------------------
// SSC
// checked - tvd
// ----------------------------------------------------------------------------------------------------------
/** SSC pin Transmitter Data (TD) */
#define PIN_SSC_TD      {PIO_PA16B_TD, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
/** SSC pin Transmitter Clock (TK) */
#define PIN_SSC_TK      {PIO_PA14B_TK, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
/** SSC pin Transmitter FrameSync (TF) */
#define PIN_SSC_TF      {PIO_PA15B_TF, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}

/** SSC pin Receiver Data (RD) */
#define PIN_SSC_RD      {PIO_PB18A_RD, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** SSC pin Receiver Clock (RK) */
#define PIN_SSC_RK      {PIO_PB19A_RK, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/** SSC pin Receiver FrameSync (RF) */
#define PIN_SSC_RF      {PIO_PB17A_RF, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}

/** SSC pins definition for codec. */
#define PINS_SSC_CODEC  PIN_SSC_TD, PIN_SSC_TK, PIN_SSC_TF, PIN_SSC_RD, PIN_SSC_RK, PIN_SSC_RF

#endif /* _BOARD_SSC_ */
