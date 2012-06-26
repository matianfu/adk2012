#ifndef _BOARD_LEDS_
#define _BOARD_LEDS_

/**
 * \file 
 * LEDs
 * - \ref PIN_USER_LED1
 * - \ref PIN_USER_LED2
 * - \ref PIN_USER_LED3
 * - \ref PIN_POWER_LED
 * - \ref PINS_LEDS
 *
 */

// ----------------------------------------------------------------------------------------------------------
// LEDS
// checked - tvd jcb
// ----------------------------------------------------------------------------------------------------------
/** LED #0 pin definition (GREEN). D4 */
#define PIN_USER_LED1   {PIO_PB13, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
/** LED #1 pin definition (AMBER). D3 */
#define PIN_USER_LED2   {PIO_PB12, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
/** LED #1 pin definition (BLUE).  D2 */
#define PIN_USER_LED3   {PIO_PA12, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
/** LED #2 pin definition (RED).   D5 */
#define PIN_POWER_LED   {PIO_PA13, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}

#define LED_GREEN     0
#define LED_AMBER     1
#define LED_BLUE      2
#define LED_RED       3

/** List of all LEDs definitions. */
#define PINS_LEDS   PIN_USER_LED1, PIN_USER_LED2, PIN_USER_LED3, PIN_POWER_LED

#endif /* _BOARD_LEDS_ */
