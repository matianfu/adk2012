#ifndef _BOARD_BUTTONS_
#define _BOARD_BUTTONS_

/**
 * \file
 * Push buttons
 * - \ref PIN_PB_LEFT_CLICK
 * - \ref PIN_PB_RIGHT_CLICK
 * - \ref PINS_PUSHBUTTONS
 * - \ref PUSHBUTTON_BP1
 * - \ref PUSHBUTTON_BP2
 *
 */

// ----------------------------------------------------------------------------------------------------------
// PUSHBUTTONS
// checked - tvd
// ----------------------------------------------------------------------------------------------------------
/** Push button LEFT CLICK definition. Attributes = pull-up + debounce + interrupt on falling edge. */
#define PIN_PB_LEFT_CLICK    {PIO_PE7, PIOE, ID_PIOE, PIO_INPUT, PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_FALL_EDGE}
/** Push button RIGHT CLICK definition. Attributes = pull-up + debounce + interrupt on falling edge. */
#define PIN_PB_RIGHT_CLICK    {PIO_PB23, PIOB, ID_PIOB, PIO_INPUT, PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_FALL_EDGE}

/** List of all push button definitions. */
#define PINS_PUSHBUTTONS    PIN_PB_LEFT_CLICK, PIN_PB_RIGHT_CLICK

/** Push button #1 index. */
#define PUSHBUTTON_BP1   0
/** Push button #2 index. */
#define PUSHBUTTON_BP2   1

/** Push button LEFT CLICK index. */
#define PUSHBUTTON_LEFT     0
/** Push button RIGHT CLICK index. */
#define PUSHBUTTON_RIGHT    1

#endif /* _BOARD_BUTTONS_ */
