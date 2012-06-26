#ifndef _BOARD_MCI_
#define _BOARD_MCI_

/**
 * \file 
 * MCI
 * - \ref PINS_MCI
 *
 */
 
// ----------------------------------------------------------------------------------------------------------
// MCI
// ----------------------------------------------------------------------------------------------------------
/** MCI pins definition. */
#define PINS_MCI   { PIO_PA20A_MCCDA | PIO_PA19A_MCCK  | PIO_PA21A_MCDA0\
                   | PIO_PA22A_MCDA1 | PIO_PA23A_MCDA2 | PIO_PA24A_MCDA3, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP },\
                   { PIO_PD0B_MCDA4 | PIO_PD1B_MCDA5 | PIO_PD2B_MCDA6 | PIO_PD3B_MCDA7,\
                     PIOD, ID_PIOD, PIO_PERIPH_B, PIO_PULLUP},\
                   { PIO_PE20B_MCCDB | PIO_PE22B_MCDB0 | PIO_PE24B_MCDB1 | PIO_PE26B_MCDB2 | PIO_PE27B_MCDB3,\
                     PIOE, ID_PIOE, PIO_PERIPH_B, PIO_PULLUP }
///** MCI pin Card Detect. */
#define PIN_MCI_CD {PIO_PE6, PIOE, ID_PIOE, PIO_INPUT, PIO_PULLUP}


#endif /* _BOARD_MCI_ */
