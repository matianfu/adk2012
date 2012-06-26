#ifndef _BOARD_ADC_
#define _BOARD_ADC_

/**
 * \file 
 * ADC
 * - \ref PIN_ADC0_AD1
 * - \ref PINS_ADC
 *
 */

/** ADC_AD1 pin definition. */
#define PIN_ADC0_AD1 {PIO_PA3X1_AD1_WKUP1, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}

/** Pins ADC */
#define PINS_ADC PIN_ADC0_AD1

#endif /* _BOARD_ADC_ */
